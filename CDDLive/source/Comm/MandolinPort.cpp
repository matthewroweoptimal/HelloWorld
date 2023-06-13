/*
 * MandolinPort.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: Kris.Simonsen
 */

#include <MandolinPort.h>
#include "ConfigManager.h"
#include <stdio.h>
#include <string.h>

extern "C" {
//#include "gpio_pins.h"
}

extern oly::Config *olyConfig;

namespace oly {

MandolinPort::MandolinPort(mandolin_fifo * rx_fifo) {
	RxFifo = rx_fifo;
	OpenPort(false);
	InitializeMeters();
	InitSync(false);
}

void MandolinPort::RxByte(uint8_t byte) {
	if (!MANDOLIN_FIFO_is_full(RxFifo)){
		MANDOLIN_FIFO_enqueue_byte(RxFifo,byte);
	} else {
//		GPIO_DRV_SetPinOutput(TP14);	//debug
	}
}

void MandolinPort::KickDog()
{
	m_uiIdleTimeElapsed = 0;
}

bool MandolinPort::IsPortOpen()
{
	return(m_bPortOpen);
}

void MandolinPort::OpenPort(bool bPortOpen)
{
	m_bPortOpen = bPortOpen;
	ChangeConnectState(mcsDisconnected);
	m_bForceClose = false;
	if (m_bPortOpen)
	{
		m_uiIdleTimeElapsed = 0;
		m_uiIdleTimeoutMs = MCS_DEFAULT_IDLE_TIMEOUT;
	}
	InitializeMeters();
}

bool MandolinPort::GetForceClose()
{
	return(m_bForceClose);
}

void MandolinPort::SetForceClose()
{
	m_bForceClose = true;
	printf("Forcing network port closure!\n");
}

void MandolinPort::Run(int nMs)
{
	switch (m_MandolinConnectState) {
	case mcsDisconnected:
		break;
	case mcsConnectReceived:
		//	Send Get Hardware Info message
		WriteMessage(CreateGetHardwareInfoMsg());
		ChangeConnectState(mcsGetHardInfoSent);
		break;
	case mcsGetHardInfoSent:
		break;
	case mcsGetHardInfoReplyReceived:
		WriteMessage(CreateGetMaxMsgMsg());
		ChangeConnectState(mcsGetMaxMsgSent);
		break;
	case mcsGetMaxMsgSent:
		break;
	case mcsGetMaxMsgReplyReceived:
		break;
	case mcsGetMaxMsgReceived:
		WriteMessage(CreateConnectMsg(false));
		ChangeConnectState(mcsConnected);
		break;
	case mcsConnected:
		break;
	}
	
	// Advance sync state machine (moved to allow offline sync)
	if (m_bSyncing)
	{
		if (!m_bSyncSentMsg)
			SendParameterSync();

	}

	m_uiStateTimeElapsed += nMs;
	m_uiIdleTimeElapsed += nMs;

}

//	Receipt of Connect command initiates connection process
mandolin_error_code MandolinPort::Connect(mandolin_message * pMsg)
{
	uint32_t *pPayload = (uint32_t *)pMsg->payload;
	uint32_t uiConnectType;
	uint32_t uiConnectMode;

	printf("Connection: Connect command received.\n");

	if (pMsg->length>=3)
		uint32_t uiConnectMode = pPayload[1];
	if (pMsg->length>=2)
	{
		uiConnectType = pPayload[0];
		ChangeConnectState(mcsConnectReceived);
		return(MANDOLIN_NO_ERROR);
	}

	return(MANDOLIN_ERROR_INVALID_DATA);
}

mandolin_error_code MandolinPort::GetHardwareInfoReply(mandolin_message * pMsg)
{
	mandolin_error_code error = MANDOLIN_NO_ERROR;
	
	printf("Connection: GetHardwareInfo reply received.\n");

	//	Send GetMaxMsg Message
	ChangeConnectState(mcsGetHardInfoReplyReceived);
	return(error);
}


//	Currently nothing is done with this info, but host has to be asked to advance its state machine
mandolin_error_code MandolinPort::GetMaxMsgWordsReply(mandolin_message * pMsg)
{
	mandolin_error_code error = MANDOLIN_NO_ERROR;
	uint32_t *pPayload = (uint32_t *)pMsg->payload;
	uint32_t uiMaxMsgWords;

	printf("Connection: GetMaxMsgWords reply received.\n");

	if (pPayload && (pMsg->length>1))
		uiMaxMsgWords = pPayload[0];

	ChangeConnectState(mcsGetMaxMsgReplyReceived);

	return(error);
}

//	receipt of this command is used as a trigger to complete connection process by senfing CONNECT message
mandolin_error_code MandolinPort::GetMaxMsgWords(mandolin_message * pMsg)
{
	mandolin_error_code error = MANDOLIN_NO_ERROR;

	printf("Connection: GetMaxMsgWords command received.\n");

	//	Send Connect Message
	ChangeConnectState(mcsGetMaxMsgReceived);

	return(error);
}

void MandolinPort::ChangeConnectState(MANDOLIN_CONNECT_STATE newState, uint32_t uiTimeoutMs)
{
	m_MandolinConnectState = newState;
	m_uiStateTimeElapsed = 0;
	m_uiStateTimeoutMs = uiTimeoutMs;
}

//	Syncing methods

//	Receipt of Sync command inititates sync process
mandolin_error_code MandolinPort::Sync(mandolin_message * pMsg)
{
	bool bValid = (mcsConnected==m_MandolinConnectState);
	mandolin_error_code error = bValid?MANDOLIN_NO_ERROR:MANDOLIN_ERROR;

	printf("Connection: Sync command received.\n");

	//	Send First Message
#if 0	// Enforce connection before sync
	if (bValid)
#endif
		InitSync(true);

	return(error);
}

void MandolinPort::InitSync(bool bEnable)
{
	m_syncTarget = eTARGET_DEVICE;
	m_nSyncParam = 1;
	m_nSyncParamMax = ePID_OLYspeaker1_DEVICE_FENCE-1;
	m_nSyncParams = 0;
	m_nMaxTxParams = MANDOLIN_MAX_PAYLOAD_DATA_WORDS-3;
	m_nInstance = 0;
	m_bSyncing = bEnable;
	m_bSyncSentMsg = false;
	m_bSyncLabels = false;
}

void MandolinPort::SendParameterSync()
{
	if (m_bSyncing)
	{
		//Note benh: should also check for negative here since dealing with ints?
		m_nSyncParams = m_nSyncParamMax-m_nSyncParam;
		if (m_nSyncParams>m_nMaxTxParams)
			m_nSyncParams = m_nMaxTxParams;
		oly_params_t *pParams = olyConfig->GetParamsDatabase();
		oly_flash_params_t *pStoredParams = olyConfig->GetStoredParamsDatabase();
		oly_status_t *pStatusParams = olyConfig->GetStatusParamsDatabase();

		switch (m_syncTarget) {
		case eTARGET_DEVICE:
			if (!m_bSyncLabels) {
				printf("Sync: Sent SetParameter command eTARGET_DEVICE %d to %d.\n", m_nSyncParam, m_nSyncParam+m_nSyncParams-1);
				WriteMessage(SetParameterBlock(m_syncTarget, 0, m_nSyncParam, pParams->Device->Values, m_nSyncParams));
				m_bSyncLabels = true;
			} else {
				printf("Sync: Sent SetStringParameter command eTARGET_DEVICE[%d].\n", m_nInstance);
				WriteMessage(SetApplicationString((OLYspeaker1_DEVICE_lid)m_nInstance, pStoredParams->Stored_Labels[m_nInstance]));
			}
			m_bSyncSentMsg = true;
			break;
		case eTARGET_USER:
			printf("Sync: Sent SetParameter command eTARGET_USER[%d] %d to %d.\n", m_nInstance, m_nSyncParam, m_nSyncParam+m_nSyncParams-1);
			WriteMessage(SetParameterBlock(m_syncTarget, m_nInstance, m_nSyncParam, pParams->User[m_nInstance], m_nSyncParams));
			m_bSyncSentMsg = true;
			break;
		case eTARGET_THROW:
			printf("Sync: Sent SetParameter command eTARGET_THROW[%d] %d to %d.\n", m_nInstance, m_nSyncParam, m_nSyncParam+m_nSyncParams-1);
			WriteMessage(SetParameterBlock(m_syncTarget, m_nInstance, m_nSyncParam, pParams->Throw[m_nInstance], m_nSyncParams));
			m_bSyncSentMsg = true;
			break;
		case eTARGET_PROFILE:
			printf("Sync: Sent SetParameter command eTARGET_PROFILE[%d] %d to %d.\n", m_nInstance, m_nSyncParam, m_nSyncParam+m_nSyncParams-1);
			WriteMessage(SetParameterBlock(m_syncTarget, m_nInstance, m_nSyncParam, pParams->Profile[m_nInstance], m_nSyncParams));
			m_bSyncSentMsg = true;
			break;
		case eTARGET_XOVER:
			printf("Sync: Sent SetParameter command eTARGET_XOVER[%d] %d to %d.\n", m_nInstance, m_nSyncParam, m_nSyncParam+m_nSyncParams-1);
			WriteMessage(SetParameterBlock(m_syncTarget, m_nInstance, m_nSyncParam, pParams->Xover[m_nInstance], m_nSyncParams));
			m_bSyncSentMsg = true;
			break;
		case eTARGET_AIRLOSS:
			printf("Sync: Sent SetParameter command eTARGET_AIRLOSS %d to %d.\n", m_nSyncParam, m_nSyncParam+m_nSyncParams-1);
			WriteMessage(SetParameterBlock(m_syncTarget, m_nInstance, m_nSyncParam, pParams->Airloss, m_nSyncParams));
			m_bSyncSentMsg = true;
			break;
		case eTARGET_STATUS:
			printf("Sync: Sent SetParameter command eTARGET_STATUS %d to %d.\n", m_nSyncParam, m_nSyncParam+m_nSyncParams-1);
			WriteMessage(SetParameterBlock(m_syncTarget, m_nInstance, m_nSyncParam, pStatusParams->Values, m_nSyncParams));
			m_bSyncSentMsg = true;
			break;
		default:
			InitSync(false);
			break;
		}
	}
}

//	match this reply sequence to sent message, if so send next SetParameter message of SYNC_OK
void MandolinPort::SetParameterReply(mandolin_message * pMsg)
{
	m_bSyncSentMsg = false;
	m_nSyncParam += m_nSyncParams;
	if (m_nSyncParam >= m_nSyncParamMax)
	{
		switch (m_syncTarget) {
		case eTARGET_DEVICE:
			if (m_bSyncLabels) {
				m_nInstance++;
				if (m_nInstance >= eLID_OLYspeaker1_DEVICE_FENCE)
				{
					m_syncTarget = eTARGET_USER;
					m_nSyncParam = 1;
					m_nInstance = 0;
					m_nSyncParamMax = ePID_OLYspeaker1_USER_FENCE;
					m_bSyncLabels = false;
				}
			}
			break;
		case eTARGET_USER:
			m_nInstance++;
			m_nSyncParam = 1;
			if (m_nInstance >= USER_PRESETS)
			{
				m_syncTarget = eTARGET_THROW;
				m_nSyncParamMax = ePID_OLYspeaker1_THROW_FENCE;
				m_nInstance = 0;
			}
			break;
		case eTARGET_THROW:
			m_nInstance++;
			m_nSyncParam = 1;
			if (m_nInstance >= THROW_PRESETS)
			{
				m_syncTarget = eTARGET_AIRLOSS;
				m_nSyncParamMax = ePID_OLYspeaker1_AIRLOSS_FENCE;
				m_nInstance = 0;
			}
			break;
		case eTARGET_AIRLOSS:
			m_nSyncParam = 1;
			m_syncTarget = eTARGET_STATUS;
			m_nSyncParamMax = ePID_OLYspeaker1_STATUS_FENCE;
			m_nInstance = 0;
			break;
		case eTARGET_STATUS:
			m_nSyncParam = 1;
			m_syncTarget = eTARGET_XOVER;
			m_nSyncParamMax = ePID_OLYspeaker1_XOVER_FENCE;
			m_nInstance = 0;
			break;
		case eTARGET_XOVER:
			m_nInstance++;
			m_nSyncParam = 1;
			if (m_nInstance >= XOVER_PRESETS)
			{
				m_syncTarget = eTARGET_PROFILE;
				m_nSyncParamMax = ePID_OLYspeaker1_PROFILE_FENCE;
				m_nInstance = 0;
			}
			break;
		case eTARGET_PROFILE:
			m_nInstance++;
			m_nSyncParam = 1;
			if (m_nInstance >= PROFILE_PRESETS)
			{
				InitSync(false);
				WriteMessage(CreateSyncOkMsg());
				return;
			}
			break;
		}
	}
	SendParameterSync();
}

//	Initialize meters - call on port connection
void MandolinPort::InitializeMeters()
{
	for(int nList = 0; nList<PORT_MAX_PARAMETER_LISTS ;nList++)
	{
		m_nParameterListNum[nList] = 0;
		m_nAutoMetersRate[nList] = 0;		// msec for AutoMeters
		m_bAutoMetersNoAck[nList] = true;
		m_bMetersSent = false;
		m_bStatusSent = false;
	}
}


//	Set up meters
void MandolinPort::ConfigureMeters(int nAutoMetersListId, int nAutoMetersRate, int nAutoMetersFlags, bool bAutoMetersNoAck)
{
	int nList = nAutoMetersListId - 1;
	if ((nList>=0) && (nList<=1))
	{
		if (nAutoMetersRate<0)
		{
			m_nAutoMetersRate[nList] = -nAutoMetersRate;
			m_bDontSendIfSame[nList] = true;
			int nTarget = MANDOLIN_EDIT_TARGET(m_nAutoMetersFlags[nList]);
			if (eTARGET_STATUS==nTarget)
				m_bStatusSent = false;
			else if (eTARGET_METERS==nTarget)
				m_bMetersSent = false;
		}
		else
		{
			m_nAutoMetersRate[nList] = nAutoMetersRate;
			m_bDontSendIfSame[nList] = false;
		}
		m_nAutoMetersFlags[nList] = nAutoMetersFlags;
		m_bAutoMetersNoAck[nList] = bAutoMetersNoAck;
	}
}

void MandolinPort::KillAllSubscriptions(void)
{
	for(int nList=0;nList<PORT_MAX_PARAMETER_LISTS;nList++)
	{
    	if (m_SubscriptionTimer[nList])
    	{
		    _timer_cancel(m_SubscriptionTimer[nList]);
		    m_SubscriptionTimer[nList] = 0;
	    }
    }
}

int MandolinPort::GetParameterListFlags(int listId)
{
	int nList = listId - 1;
	return(m_nAutoMetersFlags[nList]);
}

//	Change parameter ID in list
void MandolinPort::SetParameterListParameterId(int listId, int nIndex, int nId)
{
	int nList = listId - 1;
	if ((nList>=0) && (nList<PORT_MAX_PARAMETER_LISTS))
		m_nParameterListParameterId[nList][nIndex] = nId;
}

//	Change number of parameters in list
void MandolinPort::SetParameterListNum(int listId, int nNum)
{
	int nList = listId - 1;
	if ((nList>=0) && (nList<PORT_MAX_PARAMETER_LISTS))
		m_nParameterListNum[nList] = nNum;
}

// Send any meter messages needed
void MandolinPort::ReportMeters(int listId, mandolin_parameter_value * pStatusValues)
{
	int nList = listId - 1;
	int nTarget = MANDOLIN_EDIT_TARGET(m_nAutoMetersFlags[nList]);

	if (m_bPortOpen && ((nList>=0) && (nList<PORT_MAX_PARAMETER_LISTS)) /*&& (mcsConnected==m_MandolinConnectState)*/  && (m_nAutoMetersRate[nList] > 0) && (m_nParameterListNum[nList]>0))
	{
		//TODO: There's got to be a better way to do this.
		if (eTARGET_STATUS==nTarget)
		{
			bool bDontSendIfSame = m_bStatusSent && m_bDontSendIfSame[nList];
			mandolin_message *pMsg = SetMeters(m_nAutoMetersFlags[nList],
				m_bAutoMetersNoAck[nList],
				m_nParameterListParameterId[nList], 
				pStatusValues, 
				bDontSendIfSame?m_LastSentStatus:NULL,
				m_nParameterListNum[nList], 
				ePID_OLYspeaker1_STATUS_FENCE);
			if (pMsg)
			{
				WriteMessage(pMsg);
				if (!m_bStatusSent)
				{
					memcpy(m_LastSentStatus, pStatusValues, sizeof(m_LastSentStatus));
					m_bStatusSent = true;
				}
			}
		}
		else if (eTARGET_METERS==nTarget)
		{
			bool bDontSendIfSame = m_bMetersSent && m_bDontSendIfSame[nList];

			WriteMessage(SetParametersList(eTARGET_METERS, 0, listId, m_bAutoMetersNoAck[nList], m_nParameterListParameterId[nList], m_MeterBuffer, m_nParameterListNum[nList]));

			if (!m_bMetersSent)
			{
				memcpy(m_LastSentMeters, m_LastSentMeters, sizeof(m_LastSentMeters));
				m_bMetersSent = true;
			}

			for(int i=0; i<=eMID_OLYspeaker1_FENCE; i++ )
				m_MeterBuffer[i].f = FLOAT_NEG_INFINITY;	// Reset local peaks
		}

	}
}

void MandolinPort::HandleMetersResponse(mandolin_parameter_value * pMeterValues)
{
	int i;

	for( i=0; i<=eMID_OLYspeaker1_FENCE; i++) {
		if ((pMeterValues+i)->f > m_MeterBuffer[i].f)
			m_MeterBuffer[i].f = (pMeterValues+i)->f;
	}
}

void MandolinPort::GetMeterResponse(mandolin_message * pMsg)
{
	int i;

	WriteMessage(RequestMeterResponse(eTARGET_METERS, (uint32_t*) &m_MeterBuffer[1], pMsg->sequence));

	for( i=0; i<=eMID_OLYspeaker1_FENCE; i++ )
		m_MeterBuffer[i].f = FLOAT_NEG_INFINITY;	// Reset local peaks
}

void MandolinPort::SyncTarget(OLY_target Group, int Instance, uint32_t PID1, uint32_t * Values, uint32_t numParams)
{
	int nMaxIds = MANDOLIN_MAX_PAYLOAD_DATA_WORDS-3;
	uint32_t nStartParam = PID1;

	while (numParams) {
		if (nMaxIds > numParams)
			nMaxIds = numParams;
		mandolin_message *pMsg = SetParameterBlock(Group, Instance, nStartParam, Values, nMaxIds);
		if (pMsg)
			WriteMessage(pMsg);
		numParams -= nMaxIds;
		nStartParam += nMaxIds;
		_time_delay(10);
	}
}

void MandolinPort::Ping(mandolin_message * pMsg)
{
	if (pMsg->length)
	{
//		printf("Big Ping Rx\n");
		WriteMessage(GetAckResponseWithData(pMsg, (uint8_t *)pMsg->payload, pMsg->length*4));
	}
	else
	{
//		printf("Ping Rx\n");
		WriteMessage(GetAckResponse(pMsg));
	}
}


} /* namespace oly */




