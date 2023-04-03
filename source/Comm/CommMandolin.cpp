/*
 * CommMandolin.c
 *
 *  Created on: Jul 1, 2015
 *      Author: Kris.Simonsen
 */

#include "CommMandolin.h"
#include <stdio.h>
#include <string.h>	// For memcpy()

#include "os_tasks.h"

static mandolin_message 	tmpMsg;

static uint8_t	__attribute__ ((section(".external_ram")))	tmpMsgPayload[MANDOLIN_MAX_PAYLOAD_DATA_WORDS*MANDOLIN_BYTES_PER_WORD];

//static uint8_t tmpMsgPayload[MANDOLIN_MAX_PAYLOAD_DATA_WORDS*MANDOLIN_BYTES_PER_WORD];

mandolin_fifo	UartVoiceRxFifo;
static uint8_t	UartVoiceRxFifo_bytes[UART_VOICE_MANDOLIN_FIFO_SIZE];

_pool_id	message_pool;

const char * comm_types[] = {
	"COMM_UNKNOWN",
	"COMM_RS232_UART",
	"COMM_RS232_USB",
	"COMM_ETHERNET_TCP",
	"COMM_SPI",
	"COMM_IRDA",
	"COMM_SYSTEM"
};

static uint8_t g_sequence = 1;

void init_message_pool()
{
	message_pool = _msgpool_create(sizeof(mandolin_mqx_message), MAX_MSG_COUNT, 0, MAX_MSG_COUNT);	//	NUM_CLIENTS

	if (message_pool == MSGPOOL_NULL_POOL_ID) {
		_task_block();
	}
}

uint32_t ReadParameterFlags(mandolin_message * pMsg){
	return *((uint32_t*)pMsg->payload + 1);
}

mandolin_message * GetBlankMessage()
{
	uint32_t *pPayload = (uint32_t*)tmpMsgPayload;
	tmpMsg.payload = pPayload;

	return &tmpMsg;
}

mandolin_message * GetAckResponse(mandolin_message * pMsg)
{
	uint32_t *pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload = pPayload;
	tmpMsg.id = pMsg->id;
	tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
	tmpMsg.length = 0;

	MANDOLIN_MSG_pack(&tmpMsg, pMsg->sequence);

	return &tmpMsg;
}

mandolin_message * GetSysEventResponseWithData(mandolin_message * pMsg, uint8_t *pResponse, uint32_t uiBytes)
{
	uint8_t *pPayload = (uint8_t*)tmpMsgPayload;

	memcpy(tmpMsgPayload,pMsg->payload,4);	// copy the message ID

	if (pResponse)
		memcpy(tmpMsgPayload+4, pResponse, uiBytes);

	tmpMsg.payload = pPayload;
	tmpMsg.id = pMsg->id;
	tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
	tmpMsg.length = 1 + (uiBytes+3)/4;	//	looking for words

	MANDOLIN_MSG_pack(&tmpMsg, pMsg->sequence);

	return &tmpMsg;
}


mandolin_message * GetAckResponseWithData(mandolin_message * pMsg, uint8_t *pResponse, uint32_t uiBytes)
{
	uint8_t *pPayload = (uint8_t*)tmpMsgPayload;

	if (pResponse)
		memcpy(tmpMsgPayload, pResponse, uiBytes);
	tmpMsg.payload = pPayload;
	tmpMsg.id = pMsg->id;
	tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
	tmpMsg.length = (uiBytes+3)/4;	//	looking for words

	MANDOLIN_MSG_pack(&tmpMsg, pMsg->sequence);

	return &tmpMsg;
}

mandolin_message * GetErrorResponse(mandolin_error_code error, mandolin_message * pMsg)
{
	uint32_t *pPayload = (uint32_t*)tmpMsgPayload;

		tmpMsg.payload = pPayload;
		tmpMsg.id = pMsg->id;
		tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY | MANDOLIN_TRANSPORT_NACK;
		tmpMsg.length = 1;

		pPayload[0] = error;

		MANDOLIN_MSG_pack(&tmpMsg, pMsg->sequence);

		return &tmpMsg;
}

mandolin_message * GetParameterBlockResponse(OLY_target Group, int Instance, uint32_t PID1, uint32_t * Values, uint32_t numParams, uint8_t sequenceID)
{
    uint32_t idx = 0;
    uint32 i = 0;
    uint32_t pid = PID1;

    uint32_t *pPayload  = (uint32_t*)tmpMsgPayload;

    //uint32_t flags 		= (Group <<  MANDOLIN_EDIT_TARGET_SHIFT) | (Instance << MANDOLIN_WILDCARD_MAP_SHIFT);
    uint32_t flags 		= (Group <<  MANDOLIN_EDIT_TARGET_SHIFT) | (Instance << MANDOLIN_WILDCARD_MAP_SHIFT) | (numParams << MANDOLIN_WILDCARD_COUNT_SHIFT);
    tmpMsg.payload   	= pPayload;
    tmpMsg.id        	= MANDOLIN_MSG_GET_APPLICATION_PARAMETER;
    tmpMsg.transport 	= MANDOLIN_TRANSPORT_REPLY;
    tmpMsg.length 		= 1 + 1 + numParams;      // flags + PID1 + num parameters

    pPayload[idx++] = pid;     		// PID
    pPayload[idx++] = flags;  		// Flags
    pPayload[idx++] = Values[pid++];  // Value

    for (i=pid;i<(pid+numParams-1);i++){
        pPayload[idx++] = Values[i];  // Value
    }

    MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

    return &tmpMsg;
}

mandolin_message * GetParametersResponse(OLY_target Group, int Instance, uint32_t * PIDs, uint32_t * Values, uint32_t numParams, uint8_t sequenceID)
{
    uint32_t * pPayload = (uint32_t*)tmpMsgPayload;
    uint16_t i = 0;
    uint32_t idx = 0;

    tmpMsg.payload  = pPayload;
    tmpMsg.id       = MANDOLIN_MSG_GET_APPLICATION_PARAMETER;
    tmpMsg.transport= MANDOLIN_TRANSPORT_REPLY;
    tmpMsg.length   = 1 + (numParams * 2);                  // Flags + count(PID + VALUE)

    pPayload[idx++] = PIDs[0];         // PID
    pPayload[idx++] = (Group<<MANDOLIN_EDIT_TARGET_SHIFT) | (MANDOLIN_TYPE_MULTIPARAMETER<<MANDOLIN_WILDCARD_TYPE_SHIFT) | (Instance<<MANDOLIN_WILDCARD_MAP_SHIFT);
    pPayload[idx++] = Values[0];       // Value

    for (i=1;i<numParams;i++){
        pPayload[idx++] = PIDs[i];     // PID
        pPayload[idx++] = Values[i];   // Value
    }

    MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

    return &tmpMsg;
}

mandolin_message * GetParameterResponse(OLY_target Group, int Instance, uint32_t PID, uint32_t Value, uint8_t sequenceID)
{
	uint32_t *pPayload = (uint32_t*)tmpMsgPayload;
	uint32_t flag = ((Group <<  MANDOLIN_EDIT_TARGET_SHIFT) & MANDOLIN_EDIT_TARGET_MASK) |
			((MANDOLIN_DIRECTION_NEXT <<  MANDOLIN_WILDCARD_DIRECTION_SHIFT) & MANDOLIN_WILDCARD_DIRECTION_MASK) |
			((MANDOLIN_TYPE_NEIGHBORHOOD <<  MANDOLIN_WILDCARD_TYPE_SHIFT) & MANDOLIN_WILDCARD_TYPE_MASK) |
			(Instance<<MANDOLIN_WILDCARD_MAP_SHIFT);

	tmpMsg.payload   = pPayload;
	tmpMsg.id		 = MANDOLIN_MSG_GET_APPLICATION_PARAMETER;
	tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
	tmpMsg.length 	 = 3; 						// Flags + count(PID + VALUE)

	pPayload[0] = PID;
	pPayload[1] = flag;
	pPayload[2] = Value;

	MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	return &tmpMsg;
}

mandolin_message * GetSoftwareInfoResponse(uint32_t dante_fw_ver, uint32_t dante_cap_ver, uint32_t dante_cap_build, uint8_t sequenceID)
{
    uint32_t *pPayload = (uint32_t*)tmpMsgPayload;
    uint32_t version = ((MANDOLIN_MAJOR_VERSION & 0x0ff) << 24) | ((MANDOLIN_MINOR_VERSION & 0x0ff) << 16) | ((OLYspeaker1_XML_VERSION & 0x0ffff) << 0);

    volatile oly_version_t oly_version;
#ifdef _SECONDARY_BOOT
    oly_version.u32 = OLY_BOOT_VERSION;
#else
    oly_version.u32 = OLY_FW_VERSION;
#endif

    tmpMsg.payload   = pPayload;
    tmpMsg.id        = MANDOLIN_MSG_GET_SOFTWARE_INFO;
    tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
	tmpMsg.length    = 9;

	pPayload[0] = version;
#ifdef _SECONDARY_BOOT
	pPayload[1] = OLY_APPID_BOOTLOADER;
#elif MFG_TEST_EAW || MFG_TEST_MARTIN
	pPayload[1] = OLY_APPID_MFG_TEST;
#else	//	_SECONDARY_BOOT
	pPayload[1] = OLY_APPID_MAIN;
#endif	//	_SECONDARY_BOOT
	pPayload[2] = (oly_version.major << 24) | (oly_version.minor << 16) | (oly_version.sub << 8) | (oly_version.build);

	pPayload[3] = OLY_FIRM_DANTE_APP;         // Ultimo2 OS version
	pPayload[4] = dante_fw_ver;
	pPayload[5] = OLY_FIRM_DANTE_CAP;         // Ultimo2 Capabilities version
	pPayload[6] = dante_cap_ver;
	pPayload[7] = OLY_FIRM_DANTE_BUILD;       // Ultimo2 Capabilities build version
	pPayload[8] = dante_cap_build;

    MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	printf("Connection: Sent GetSoftwareInfo reply.\n");

	return &tmpMsg;
}

mandolin_message * GetHardwareInfoResponse(uint8_t brand, uint16_t model, uint8 mac_addr[6], uint8_t sequenceID)
{
    uint32_t *pPayload = (uint32_t*)tmpMsgPayload;

    tmpMsg.payload   = pPayload;
    tmpMsg.id        = MANDOLIN_MSG_GET_HARDWARE_INFO;
    tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
    tmpMsg.length    = 2;

    pPayload[0] = ((brand & 0xF) << 28) | ((model & 0xFFF) << 16) | ((mac_addr[0] & 0xFF) << 8) | ((mac_addr[1] & 0xFF));
    pPayload[1] = ((mac_addr[2] & 0xFF) << 24) | ((mac_addr[3] & 0xFF) << 16) | ((mac_addr[4] & 0xFF) << 8) | (mac_addr[5] & 0xFF);

    MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	printf("Connection: Sent GetHardwareInfo reply.\n");

    return &tmpMsg;
}

mandolin_message * GetStatusAllResponse(uint32_t * pValues, uint8_t sequenceID)
{
	uint32_t *pPayload = (uint32_t*)tmpMsgPayload;
	uint32_t idx = 0;

	tmpMsg.payload   = pPayload;
	tmpMsg.id        = MANDOLIN_MSG_TEST;
	tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
	tmpMsg.length    = 2;

	pPayload[idx++] = TEST_CMD_GET_STATUS_ALL;

	int i;
	for (i=1; i<ePID_OLYspeaker1_STATUS_FENCE; i++ )
	{
		pPayload[idx++] = pValues[i];
	}

	tmpMsg.length = ePID_OLYspeaker1_STATUS_FENCE;

	MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	printf("Connection: Sent GetStatusAll reply.\n");
	return &tmpMsg;
}

mandolin_message * GetStaticAllResponse(uint32_t ip, uint32_t gateway, uint32_t mask, uint8_t sequenceID)
{
    uint32_t *pPayload = (uint32_t*)tmpMsgPayload;

    tmpMsg.payload   = pPayload;
    tmpMsg.id        = MANDOLIN_MSG_TEST;
    tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
    tmpMsg.length    = 4;

    pPayload[0] = TEST_CMD_GET_STATIC_ALL;
    pPayload[1] = ip;
    pPayload[2] = gateway;
    pPayload[3] = mask;

    MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	printf("Connection: Sent GetStaticAll reply.\n");
    return &tmpMsg;
}

mandolin_message * GetSerialNumberResponse(uint32_t uiSerialNumber, uint8_t sequenceID)
{
    uint32_t *pPayload = (uint32_t*)tmpMsgPayload;

    tmpMsg.payload   = pPayload;
    tmpMsg.id        = MANDOLIN_MSG_TEST;
    tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
    tmpMsg.length    = 2;

    pPayload[0] = TEST_CMD_GET_SERIAL_NUMBER;
    pPayload[1] = uiSerialNumber;

    MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	printf("Connection: Sent GetSerialNumber reply.\n");
    return &tmpMsg;
}


/* HOST COMMANDS */

mandolin_message * SetParameter(OLY_target Group, int Instance, uint32_t PID, uint32_t Value)
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_SET_APPLICATION_PARAMETER;
	tmpMsg.transport= MANDOLIN_TRANSPORT_NULL;
	tmpMsg.length 	= 3; 						// Flags + count(PID + VALUE)

	pPayload[0] = PID;
	pPayload[1] = (Group<<MANDOLIN_EDIT_TARGET_SHIFT) | (Instance << MANDOLIN_WILDCARD_MAP_SHIFT) | (1 << MANDOLIN_WILDCARD_COUNT_SHIFT);	// Flags
	pPayload[2] = Value;  								// Value

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	return &tmpMsg;
}

mandolin_message * SetParameters(OLY_target Group, int Instance, uint32_t * PIDs, uint32_t * Values, uint32_t numParams)
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;
	uint16_t i = 0;
	uint32_t idx = 0;
	uint32_t count = numParams;

	if (numParams > 0x7F)
		count = 0;	// Only 7 bits in flag

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_SET_APPLICATION_PARAMETER;
	tmpMsg.transport= MANDOLIN_TRANSPORT_NOACK;
	tmpMsg.length 	= 1 + (numParams * 2); 						// Flags + count(PID + VALUE)

	pPayload[idx++] = PIDs[0];
	pPayload[idx++] = (Group<<MANDOLIN_EDIT_TARGET_SHIFT) | (MANDOLIN_TYPE_MULTIPARAMETER<<MANDOLIN_WILDCARD_TYPE_SHIFT) | (Instance<<MANDOLIN_WILDCARD_MAP_SHIFT) | (count << MANDOLIN_WILDCARD_COUNT_SHIFT);
	pPayload[idx++] = Values[0];  		// Value

	for (i=1;i<numParams;i++){
		pPayload[idx++] = PIDs[i];     // PID
		pPayload[idx++] = Values[i]; 	// Value
	}

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	return &tmpMsg;
}

mandolin_message * SetParametersList(OLY_target Group, uint32_t Instance, uint32_t listID, bool noAck, uint32_t * PIDs, mandolin_parameter_value * Values, uint32_t numParams)
{
	mandolin_parameter_value * pPayload = (mandolin_parameter_value *)tmpMsgPayload;
	uint32_t idx = 0;
	uint32_t pid, i;
	uint32_t count = numParams;

	if (numParams > 0x7F)
		count = 0;	// Only 7 bits in flag

	tmpMsg.payload  	= pPayload;
	tmpMsg.id			= MANDOLIN_MSG_SET_APPLICATION_PARAMETER;
	tmpMsg.transport	= noAck ? MANDOLIN_TRANSPORT_NOACK:0;
	tmpMsg.length 		= 1 + 1 + numParams; 	// listID + flags + values

	pPayload[idx++].u = listID;
	pPayload[idx++].u = (Group << MANDOLIN_EDIT_TARGET_SHIFT) |
					(MANDOLIN_TYPE_LIST << MANDOLIN_WILDCARD_TYPE_SHIFT) |
					(Instance<<MANDOLIN_WILDCARD_MAP_SHIFT) |
					(count << MANDOLIN_WILDCARD_COUNT_SHIFT);

	for (i=0;i<numParams;i++){
		pid = PIDs[i];
		pPayload[idx++] = Values[pid];
	}

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	return &tmpMsg;
}

mandolin_message * SetMeters(int flags, bool bNoAck, uint32_t * PIDs, mandolin_parameter_value * values, mandolin_parameter_value * lastValues, uint32_t numParams, uint32_t fence)
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;
	uint16_t i = 0;
	uint32_t idx = 0;
	uint32_t pid;
	int pidCount = 0;

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_SET_APPLICATION_PARAMETER;
	tmpMsg.transport= bNoAck?MANDOLIN_TRANSPORT_NOACK:0;
	tmpMsg.length 	= 1 + (numParams * 2); 						// Flags + count(PID + VALUE)

	for (i=0;i<numParams;i++){
		pid = PIDs[i];
		if ((pid<1) || (pid>=fence))
				pid = 1;

		if ((!lastValues) ||  (lastValues[pid].u!=values[pid].u))
		{
			pPayload[idx++] = pid;     // PID
			if (!pidCount)
				pPayload[idx++] = flags;
			pPayload[idx++] = values[pid].u; 	// Value
			pidCount++;
			if (lastValues)
				lastValues[pid].u = values[pid].u;
		}
	}

	tmpMsg.length 	= 1 + (pidCount * 2); 						// Flags + count(PID + VALUE)

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	if (pidCount)
		return &tmpMsg;
	else
		return(NULL);
}

mandolin_message * SetParameterBlock(OLY_target Group, int Instance, uint32_t PID1, uint32_t * Values, uint32_t numParams)
{
	uint32_t i = 0;
	uint32_t idx = 0;
	uint32_t flags,pid;
	uint32_t count = numParams;

	if (numParams > 0x7F)
		count = 0;	// Only 7 bits in flag


	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_SET_APPLICATION_PARAMETER;
	tmpMsg.transport= MANDOLIN_TRANSPORT_NULL;
	tmpMsg.length = 1 + 1 + numParams;		// flags + PID1 + num parameters

	flags =	(Group << MANDOLIN_EDIT_TARGET_SHIFT) | (Instance<<MANDOLIN_WILDCARD_MAP_SHIFT) | (count << MANDOLIN_WILDCARD_COUNT_SHIFT);

    pPayload[idx++] = PID1;
	pPayload[idx++] = flags;
    pPayload[idx++] = Values[PID1];  // Value

    pid = PID1+1;

    for (i=pid;i<(numParams + PID1);i++){
    	pPayload[idx++] = Values[i];  // Value
    }

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	return &tmpMsg;
}

mandolin_message * RequestMeter(OLY_target Group, uint32_t PID, uint32_t instance, uint8_t sequenceID)
{
	static mandolin_message 	tmpMeterRequestMsg;
	static uint8_t				tmpMeterRequestMsgPayload[256];
	uint32_t * pPayload = (uint32_t*)tmpMeterRequestMsgPayload;

	tmpMeterRequestMsg.payload  = pPayload;
	tmpMeterRequestMsg.id		= MANDOLIN_MSG_GET_APPLICATION_PARAMETER;
	tmpMeterRequestMsg.transport= MANDOLIN_TRANSPORT_NULL;
	tmpMeterRequestMsg.length 	= 2; 												// PID + Flags

	pPayload[0] = PID;
	pPayload[1] = (((instance<<MANDOLIN_EDIT_TARGET_SHIFT) + Group ) << MANDOLIN_EDIT_TARGET_SHIFT) & 0x007fff00;	// Flags

	MANDOLIN_MSG_pack(&tmpMeterRequestMsg, sequenceID);

	return &tmpMeterRequestMsg;
}

mandolin_message * RequestMeterResponse(OLY_target Group, uint32_t * meterPayload, uint8_t sequenceID)
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;
	uint8_t ix;

	tmpMsg.length 	= eMID_OLYspeaker1_FENCE + 1; 												// Meter ID + Flags + 26*MeterValues
	tmpMsg.transport= MANDOLIN_TRANSPORT_REPLY + MANDOLIN_TRANSPORT_NOACK;
	tmpMsg.id		= MANDOLIN_MSG_GET_APPLICATION_PARAMETER;
	tmpMsg.payload  = pPayload;

	pPayload[0] = 1;
	pPayload[1] = ((eMID_OLYspeaker1_FENCE<<16)&0x7F) | (Group<<MANDOLIN_EDIT_TARGET_SHIFT);	// Flags
	for(ix=0; ix<eMID_OLYspeaker1_FENCE-1; ix++)	pPayload[2+ix] = *meterPayload++;

	MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	return &tmpMsg;
}

mandolin_message * Mfg_TestValueResponse(uint32_t PID, uint32_t Value, uint8_t sequenceID) {
	uint32_t *pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload   = pPayload;
	tmpMsg.id		 = MANDOLIN_MSG_TEST;
	tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
	tmpMsg.length 	 = 3; 						// Flags + count(PID + VALUE)

	pPayload[0] = PID;
	pPayload[1] = Value;

	MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	return &tmpMsg;
}

mandolin_message * Mfg_TestValueIndexedResponse(uint32_t PID, uint32_t VID, uint32_t Value, uint8_t sequenceID) {
	uint32_t *pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload   = pPayload;
	tmpMsg.id		 = MANDOLIN_MSG_TEST;
	tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
	tmpMsg.length 	 = 3; 						// Flags + count(PID + VALUE)

	pPayload[0] = PID;
	pPayload[1] = VID;
	pPayload[2] = Value;

	MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	return &tmpMsg;
}

mandolin_message * CreateGetHardwareInfoMsg()
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_GET_HARDWARE_INFO;
	tmpMsg.transport= MANDOLIN_TRANSPORT_NULL;
	tmpMsg.length 	= 0; 												// PID + Flags

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	printf("Connection: Sent GetHardwareInfo command.\n");
	return &tmpMsg;
}

mandolin_message * CreateGetMaxMsgMsg()
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_GET_SYSTEM_STATUS;
	tmpMsg.transport= MANDOLIN_TRANSPORT_NULL;
	tmpMsg.length 	= 1; 												// PID + Flags

	pPayload[0] = MANDOLIN_SS_MAX_MSG_WORDS;

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	printf("Connection: Sent GetMaxMsgMsg command.\n");
	return &tmpMsg;
}

mandolin_message * CreateConnectMsg(bool fast_connect)
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	tmpMsg.transport= MANDOLIN_TRANSPORT_NULL;
	tmpMsg.length 	= 2; 												// PID + Flags

	pPayload[0] = MANDOLIN_SE_CONNECT;
	pPayload[1] = fast_connect;

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	printf("Connection: Sent Connect command.\n");
	return &tmpMsg;
}

mandolin_message * CreateSyncOkMsg()
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	tmpMsg.transport= MANDOLIN_TRANSPORT_NULL;
	tmpMsg.length 	= 1; 												// PID + Flags

	pPayload[0] = MANDOLIN_SE_SYNC_OK;

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	printf("Sync: Sent SyncOk command.\n");
	return &tmpMsg;
}


mandolin_message * GetSyncMessage(mandolin_system_event nEvent)
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT;
	tmpMsg.transport= MANDOLIN_TRANSPORT_NULL;
	tmpMsg.length 	= 1; 												// PID + Flags

	pPayload[0] = nEvent;

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	return &tmpMsg;
}

mandolin_message * SetApplicationString(OLYspeaker1_DEVICE_lid LID, char * str_data)
{
	uint32_t * pPayload = (uint32_t*)tmpMsgPayload;
	uint32_t str_len = strlen(str_data);

	tmpMsg.payload  = pPayload;
	tmpMsg.id		= MANDOLIN_MSG_SET_APPLICATION_STRING;
	tmpMsg.transport= MANDOLIN_TRANSPORT_NULL;
	tmpMsg.length = 2 + ((str_len+1+3)/4);	// round up to nearest word

	pPayload[0] = LID;
	pPayload[1] = (eTARGET_DEVICE <<  MANDOLIN_EDIT_TARGET_SHIFT) | (1 << MANDOLIN_WILDCARD_COUNT_SHIFT);

	if 	(str_data)
//		memcpy(&pPayload[2], str_data, str_len+1);	// offset for pid + flag, add NULL
		strcpy((char*)&pPayload[2], str_data);	// offset for pid + flag, add NULL

	MANDOLIN_MSG_pack(&tmpMsg, GetNextSequence());

	return &tmpMsg;
}

mandolin_message * GetApplicationStringResponse(OLYspeaker1_DEVICE_lid LID, char * str_data, uint32_t str_len, uint8_t sequenceID)
{
	uint32_t *pPayload = (uint32_t*)tmpMsgPayload;

	if (str_data)
		memcpy(&pPayload[2], str_data, str_len+1);	// offset for pid + flag, add NULL

	tmpMsg.payload = pPayload;
	tmpMsg.id = MANDOLIN_MSG_GET_APPLICATION_STRING;
	tmpMsg.transport = MANDOLIN_TRANSPORT_REPLY;
	tmpMsg.length = 2 + ((str_len+1+3)/4);	//	looking for words

	pPayload[0] = LID;
	pPayload[1] = (eTARGET_DEVICE <<  MANDOLIN_EDIT_TARGET_SHIFT) | (1 << MANDOLIN_WILDCARD_COUNT_SHIFT); 		// Max of one reply at a time

	MANDOLIN_MSG_pack(&tmpMsg, sequenceID);

	return &tmpMsg;
}

LOUD_routingsrc AudioSrc2Routing(oly_audio_source_t source)
{
	LOUD_routingsrc routing = eROUTINGSRC_NONE;
	switch (source) {
		case(src_none):
			routing = eROUTINGSRC_NONE;
			//printf("Audio: No Source Selected\n");
			break;
		case(src_analog):
			routing = eROUTINGSRC_MICPRE;
			//printf("Audio: Analog Source Selected\n");
			break;
		case(src_dante):
			routing = eROUTINGSRC_DANTE;
			//printf("Audio: Dante Source Selected\n");
			break;
		case(src_pinknoise):
			routing = eROUTINGSRC_GENERATOR;
			//printf("Audio: Internal Pink Noise Source Selected\n");
			break;
		default:
			break;
	}
	return routing;
}

int gMsgPoolAllocErrorCount = 0; // TODO: What is this

void ParseMandolinMQX(_queue_id source_qid, comm_method comm_type, mandolin_fifo * pFifo, int nRetries, int nTaskId)
{
	// if nRetries  < 0 waits for ever

	uint32_t 	len;
	_queue_id	uart_qid;
	int nCount = 0;
	int nMsgs  = 0;

	mandolin_parse_error error;
	mandolin_mqx_message_ptr rx_msg = NULL;

	while ((len = MANDOLIN_FIFO_parse(pFifo, &error)) && ((nRetries < 0) || (nCount <= nRetries)))		// not sure we want to wait for ever.. maybe have another task to process in backgroud.
	{
		// Complete message found, send as mqx message

		//	Verify there is at least two messages free so we don't deny a transmit operation which pulls from same pool.
		TASKDEBUG_POS(nTaskId,49)
		int nMsgs = MSGQ_GET_COUNT(_msgq_get_id(0,CONFIG_QUEUE));
		TASKDEBUG_POS(nTaskId,50)
		if ((nMsgs+2)<MAX_MSG_COUNT)

			rx_msg = (mandolin_mqx_message_ptr) MSG_ALLOC(message_pool);

		else
			rx_msg = NULL;
		if (rx_msg)
		{
			nCount = 0; // got a pool, reset counter

			rx_msg->mqx_header.SOURCE_QID = source_qid;
			rx_msg->mqx_header.TARGET_QID = _msgq_get_id(0,CONFIG_QUEUE);
			rx_msg->mqx_header.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(comm_method) + 8 + (len * MANDOLIN_BYTES_PER_WORD);
			TASKDEBUG_POS(nTaskId,51)

			if (rx_msg->length = MANDOLIN_MSG_read(pFifo, &rx_msg->message, rx_msg->payload))
			{
				rx_msg->sender_type = comm_type;
				TASKDEBUG_POS(nTaskId,52)

				if(rx_msg->length > MAX_MANDOLIN_MSG_SIZE)
				{
					printf("Network MSGQ error: 0x%lx\n", _task_errno);
					_task_set_error(MQX_OK); // Reset error so not blocked when overfull
					TASKDEBUG_POS(nTaskId,62)

					MSG_FREE(_msgq_get_id(0,CONFIG_QUEUE), rx_msg);
				}
				else if (!MSGQ_SEND(_msgq_get_id(0,CONFIG_QUEUE), rx_msg))
				{
					printf("Network MSGQ error: 0x%lx\n", _task_errno);
					_task_set_error(MQX_OK); // Reset error so not blocked when overfull

					TASKDEBUG_POS(nTaskId,63)
					MSG_FREE(_msgq_get_id(0,CONFIG_QUEUE), rx_msg);
				}
				else if (comm_type != COMM_SPI)	// don't print meters
				{
//					printf("MSG length %d, Rx on %s, sent to CONFIG MSG Q\n", rx_msg->length, comm_types[comm_type]);
				}
				TASKDEBUG_POS(nTaskId,53)
			}
		}
		else
		{
			// Give the competing task some additional time to process the queue - EXPERIMENTAL
			TASKDEBUG_POS(nTaskId,54)
			nCount++;		// this is in a mutex now, so it must return sometime.
			if ((nRetries >= 0) && (nCount > nRetries))
			{
				gMsgPoolAllocErrorCount++;
			}
			else
			{

				_time_delay(5);			// wait for something to free a message so alloc can give you one
			}


			//MANDOLIN_MSG_erase(pFifo);
//			printf("ERR: Out of messages, pending!\n");
		}
	}

	TASKDEBUG_POS(nTaskId,55)

	if (error.error) {
		if (comm_type != COMM_SPI)  // DSP errors are normal because of the SPI duplex
			printf("ERR: Mandolin Parse Error = %d!\n" , (int)error.error);
	}
}

void Fifo_Init(void)
{
	MANDOLIN_FIFO_init(&UartVoiceRxFifo, UART_VOICE_MANDOLIN_FIFO_SIZE, UartVoiceRxFifo_bytes);		// Set up RX buffer
}

void UartVoice_RxByte(uint8_t byte)
{
	if (!MANDOLIN_FIFO_is_full(&UartVoiceRxFifo)){
		MANDOLIN_FIFO_enqueue_byte(&UartVoiceRxFifo, byte);
	} else {
		// TODO: Proper overflow handling
		printf("Mandolin Voice UART overfull\n");
		//GPIO_DRV_SetPinOutput(TP14);	//debug
	}
}

int GetNextSequence()
{
	return(g_sequence++);
}

