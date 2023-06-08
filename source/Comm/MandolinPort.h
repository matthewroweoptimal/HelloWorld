/*
 * MandolinPort.h
 *
 *  Created on: Jul 1, 2015
 *      Author: Kris.Simonsen
 */

#ifndef SOURCES_MANDOLINPORT_H_
#define SOURCES_MANDOLINPORT_H_

#include "CommMandolin.h"

extern "C" {
#include <timer.h>
}
#include <list>

//	State machine to keep track of Mandolin connection progress
typedef enum enumMandolinConnectState {
	mcsDisconnected,			//	No CONNECT mesg received
	mcsConnectReceived,			//	CONNECT message received,
	mcsGetHardInfoSent,			//	GetHardInfo Sent, awaiting reply
	mcsGetHardInfoReplyReceived,//	Reply for Get Hardware Info received,
	mcsGetMaxMsgSent,			//	GetMaxMsg Sent, awaiting reply
	mcsGetMaxMsgReplyReceived,	//	GetMaxMsg reply received, awaiting GetMaxMsg command
	mcsGetMaxMsgReceived,		//	GetMaxMsg Received
	mcsConnected				//	Connect message sent, awaiting Sync
} MANDOLIN_CONNECT_STATE;

#define MCS_DEFAULT_IDLE_TIMEOUT	(10000)	//	default milliseconds to wait with no communication before closing port
#define MCS_DEFAULT_STATE_TIMEOUT	(3000)	//	default milliseconds to wait in state with no progress before aborting

#define PORT_MAX_PARAMETER_LISTS	(2)
#define PORT_PARAMETERLIST_MAX		(((int)eMID_OLYspeaker1_FENCE>(int)ePID_OLYspeaker1_STATUS_FENCE)?(int)eMID_OLYspeaker1_FENCE:(int)ePID_OLYspeaker1_STATUS_FENCE)

namespace oly {

class MandolinPort {
protected:
	mandolin_fifo *	RxFifo;
	_queue_id 		port_qid;
	bool			m_bPortOpen;	//	Different that mandolin "connected" state, indicates a communication channel is open
	bool 			m_bForceClose;

	//	Connection State machine
	MANDOLIN_CONNECT_STATE m_MandolinConnectState;
	uint32_t m_uiStateTimeElapsed;
	uint32_t m_uiStateTimeoutMs;
	uint32_t m_uiIdleTimeElapsed;
	uint32_t m_uiIdleTimeoutMs;

	void ChangeConnectState(MANDOLIN_CONNECT_STATE newState, uint32_t uiTimeoutMs=MCS_DEFAULT_STATE_TIMEOUT);


public:
	//	Connection State machine API
	virtual void Run(int nMs);
	mandolin_error_code Connect(mandolin_message * pMsg);
	mandolin_error_code GetHardwareInfoReply(mandolin_message * pMsg);
	mandolin_error_code GetMaxMsgWordsReply(mandolin_message * pMsg);
	mandolin_error_code GetMaxMsgWords(mandolin_message * pMsg);

	MandolinPort(mandolin_fifo * rx_fifo);
	virtual void WriteMessage(mandolin_message * pMsg, bool bRequireConnection=false)=0;	//	set bRequireConnection if mandolin connection process must have completed to send this message (only used for network port currently)
	void RxByte(uint8_t byte);
	bool GetConnected() { return(mcsConnected==m_MandolinConnectState);}
	bool IsPortOpen();
	void OpenPort(bool bPortOpen);
	bool GetForceClose();
	void KickDog();
	void Ping(mandolin_message * pMsg);
	void SetForceClose();

	//	Meters
	void InitializeMeters();
	int GetParameterListFlags(int listId);
	virtual void ConfigureMeters(int nAutoMetersListId, int nAutoMetersRate, int nAutoMetersFlags, bool bAutoMetersNoAck);
	void KillAllSubscriptions(void);
	void SetParameterListId(int nListId);
	void SetParameterListParameterId(int listId, int nIndex, int nId);
	void ReportMeters(int listId, mandolin_parameter_value *pStatusValues);
	void SetParameterListNum(int listId, int nNum);
	void HandleMetersResponse(mandolin_parameter_value * pMeterValues);
	void GetMeterResponse(mandolin_message * pMsg);

	int GetAutoMetersRate();
	int GetAutoMetersListId();

protected:
	_timer_id m_SubscriptionTimer[PORT_MAX_PARAMETER_LISTS];
	MQX_TICK_STRUCT m_SubscriptionTicks[PORT_MAX_PARAMETER_LISTS];
    int m_nAutoMetersRate[PORT_MAX_PARAMETER_LISTS]; // msec for AutoMeterss
    mandolin_parameter_value m_MeterBuffer[eMID_OLYspeaker1_FENCE+1];
	mandolin_parameter_value m_LastSentMeters[eMID_OLYspeaker1_FENCE+1];
	mandolin_parameter_value m_LastSentStatus[ePID_OLYspeaker1_STATUS_FENCE+1];
	bool m_bMetersSent;
	bool m_bStatusSent;
	bool m_bDontSendIfSame[PORT_MAX_PARAMETER_LISTS];

private:
	//	Sync State Machine
	OLY_target m_syncTarget;
	int m_nSyncParam;
	int m_nSyncParams;
	int m_nSyncParamMax;
	int m_nMaxTxParams;
	int m_nInstance;
	bool m_bSyncing;
	bool m_bSyncSentMsg;
	bool m_bSyncLabels;

	//	Meters
	int m_nParameterListNum[PORT_MAX_PARAMETER_LISTS];
	int m_nParameterListId[PORT_MAX_PARAMETER_LISTS]; // -1 none
    uint32_t m_nParameterListParameterId[PORT_MAX_PARAMETER_LISTS][PORT_PARAMETERLIST_MAX]; // parameter id list
    int m_nAutoMetersFlags[PORT_MAX_PARAMETER_LISTS]; // -1 none
    bool m_bAutoMetersNoAck[PORT_MAX_PARAMETER_LISTS]; // -1 none

	void InitSync(bool bEnable);
	void SendParameterSync();

public:
	//	Sync state machine API
	mandolin_error_code Sync(mandolin_message * pMsg);
	void SetParameterReply(mandolin_message * pMsg);
	void SyncTarget(OLY_target Group, int Instance, uint32_t PID1, uint32_t * Values, uint32_t numParams);

};

} /* namespace oly */

#endif /* SOURCES_MANDOLINPORT_H_ */
