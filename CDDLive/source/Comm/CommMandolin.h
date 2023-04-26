/*
 * CommMandolin.h
 *
 *  Created on: May 20, 2015
 *      Author: Kris.Simonsen
 */

#ifndef SOURCES_MANDOLIN_HANDLE_H_
#define SOURCES_MANDOLIN_HANDLE_H_

#include "oly.h"
#include "mandolin.h"
#include "MQX_to_FreeRTOS.h"
//#include <message.h>
#include "LOUD_defines.h"

// for SPI Flash Validation
#include "Drivers/spi_flash_nu.h"


#define ROUND(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))	// round float to nearest whole value

typedef enum {
    COMM_UNKNOWN,
    COMM_RS232_UART,
    COMM_RS232_USB,
    COMM_ETHERNET_TCP,
	COMM_SPI,
	COMM_IRDA_A,
	COMM_IRDA_B,
	COMM_IRDA_C,
	COMM_IRDA_D,
	COMM_SYSTEM
} comm_method;

typedef struct {
	MESSAGE_HEADER_STRUCT	mqx_header;
	comm_method				sender_type;
	uint32_t				sender_id;
	uint32_t				length;
	mandolin_message		message;
	uint8_t					payload[MANDOLIN_MAX_PAYLOAD_DATA_WORDS * MANDOLIN_BYTES_PER_WORD];
} mandolin_mqx_message, * mandolin_mqx_message_ptr;

typedef struct {
	MESSAGE_HEADER_STRUCT	mqx_header;
	comm_method				sender_type;
	uint32_t				sender_id;
	uint32_t				length;
	uint8_t					data[MANDOLIN_MAX_MESSAGE_WORDS * MANDOLIN_BYTES_PER_WORD];
} mandolin_mqx_msg_packed, * mandolin_mqx_msg_packed_ptr;

typedef enum {
	TEST_CMD_SET_STATIC_IP = 0x01,
	TEST_CMD_SET_STATIC_ALL = 0x02,
	TEST_CMD_SET_IDENTITY = 0x03,
	TEST_CMD_GET_MAC_ADDR = 0x04,
	TEST_CMD_GET_STATIC_ALL = 0x05,
	TEST_CMD_SET_MODEL = 0x06,
	TEST_CMD_SET_SERIAL_NUMBER = 0x07,
	TEST_CMD_GET_SERIAL_NUMBER = 0x08,
	TEST_CMD_RESTORE_DEFAULTS = 0x09,
	TEST_CMD_GET_STATUS_ALL = 0x0A,
	TEST_CMD_PRINT_MESSAGE = 0x0B,
	/* Manufacturing Test */
	TEST_CMD_SET_INPUT,
	TEST_CMD_SET_OUTPUT,
	TEST_CMD_SET_FUNC_GEN,
	TEST_CMD_GET_RMS_METER_LEVEL,
	TEST_CMD_GET_RMS_METER_RANGE,
	TEST_CMD_WRITE_IO,
	TEST_CMD_READ_IO,
	TEST_CMD_READ_AMP_TEMPERATURE,
	TEST_CMD_VALIDATE_SPI_FLASH,
	TEST_CMD_GET_IMON_VALUE,
	TEST_CMD_GET_HW_VER,
	TEST_DSP_ERROR = 0x30,
	TEST_CMD_FENCE,
} oly_test_command;

typedef enum {
	TEST_INPUT_ANALOG = 0x00,
	TEST_INPUT_DANTE,
	TEST_INPUT_PINK,
	TEST_INPUT_SINE,
} test_audio_input;

typedef enum {
	INPUT_EAW_AMP1_CH1_CLIP = 0x00,
	INPUT_EAW_AMP1_CH2_CLIP,
	INPUT_EAW_AMP1_PROTECT,
	INPUT_EAW_AMP2_CH1_CLIP,
	INPUT_EAW_AMP2_CH2_CLIP,
	INPUT_EAW_AMP2_PROTECT,
	INPUT_EAW_FENCE,
} test_pin_input_eaw;

typedef enum {
	OUTPUT_EAW_AMP1_DISABLE_MUTE = 0x00,
	OUTPUT_EAW_AMP1_BTL4R,
	OUTPUT_EAW_AMP1_TEMP_VAC_SEL_CONTROL,
	OUTPUT_EAW_AMP1_STANDBY,
	OUTPUT_EAW_AMP2_DISABLE_MUTE,
	OUTPUT_EAW_AMP2_BTL4R,
	OUTPUT_EAW_AMP2_TEMP_VAC_SEL_CONTROL,
	OUTPUT_EAW_AMP2_STANDBY,
	OUTPUT_EAW_DISPLAY_D0,
	OUTPUT_EAW_DISPLAY_D1,
	OUTPUT_EAW_DISPLAY_D2,
	OUTPUT_EAW_DISPLAY_D3,
	OUTPUT_EAW_DISPLAY_D4,
	OUTPUT_EAW_DISPLAY_D5,
	OUTPUT_EAW_DISPLAY_D6,
	OUTPUT_EAW_FAN_CONTROL,
	OUTPUT_EAW_LOGO_LED_AMBER,
	OUTPUT_EAW_LOGO_LED_WHITE,
	OUTPUT_EAW_LOGO_LED_RED,
	OUTPUT_EAW_FENCE,
} test_pin_output_eaw;

typedef enum {
	INPUT_MARTIN_AMP1_CH1_CLIP = 0x00,
	INPUT_MARTIN_AMP1_CH2_CLIP,
	INPUT_MARTIN_AMP1_PROTECT,
	INPUT_MARTIN_AMP2_CH1_CLIP,
	INPUT_MARTIN_AMP2_CH2_CLIP,
	INPUT_MARTIN_AMP2_PROTECT,
	INPUT_MARTIN_DISPLAY_D0,
	INPUT_MARTIN_DISPLAY_D1,
	INPUT_MARTIN_FENCE,
} test_pin_input_martin;

typedef enum {
	OUTPUT_MARTIN_AMP1_DISABLE_MUTE = 0x00,
	OUTPUT_MARTIN_AMP1_BTL4R,
	OUTPUT_MARTIN_AMP1_TEMP_VAC_SEL_CONTROL,
	OUTPUT_MARTIN_AMP1_STANDBY,
	OUTPUT_MARTIN_AMP2_DISABLE_MUTE,
	OUTPUT_MARTIN_AMP2_BTL4R,
	OUTPUT_MARTIN_AMP2_TEMP_VAC_SEL_CONTROL,
	OUTPUT_MARTIN_AMP2_STANDBY,
	OUTPUT_MARTIN_DISPLAY_D2,
	OUTPUT_MARTIN_DISPLAY_D3,
	OUTPUT_MARTIN_DISPLAY_D4,
	OUTPUT_MARTIN_DISPLAY_D5,
	OUTPUT_MARTIN_DISPLAY_D6,
	OUTPUT_MARTIN_FAN_CONTROL,
	OUTPUT_MARTIN_LOGO_LED_AMBER,
	OUTPUT_MARTIN_LOGO_LED_WHITE,
	OUTPUT_MARTIN_LOGO_LED_RED,
	OUTPUT_MARTIN_FENCE,
} test_pin_output_martin;

typedef enum {
	MANDOLIN_AE_STORE_USER_EDIT = 0x40,		// CDD "store current to user" function
	MANDOLIN_AE_CANCEL_USER_EDIT = 0x41,	// CDD "cancel edit and revert to previous" function
	MANDOLIN_AE_SET_AVAILABLE = 0x42,		// OLY "logo indicate available for Rig"
	MANDOLIN_AE_SET_SELF_TEST_MODE = 0x43,	// Request to set OLY self test mode
	MANDOLIN_AE_START_AUTO_OPTIMIZE = 0x44,	// OLY start the auto-optimize process
	MANDOLIN_AE_RESET_AUTO_OPTIMIZE = 0x45,	// OLY clear the auto-optimize settings
	MANDOLIN_AE_RESTORE_DEFAULTS = 0x46		// Restore factory defaults
} oly_application_event;

#define IDENTIFY_FOREVER	0xFFFFFFFF


/* Mandolin FIFO & buffers */
#define UART_VOICE_MANDOLIN_FIFO_SIZE 	4096
#define SHARC_MANDOLIN_FIFO_SIZE 		4096


extern mandolin_fifo 	UartVoiceRxFifo;extern mandolin_fifo 	SpiSharcRxFifo;

void Fifo_Init(void);
void UartVoice_RxByte(uint8_t byte);
int GetNextSequence(void);


/* Message Queues */
#define TCP_QUEUE			8
#define CONFIG_QUEUE		9
#define NET_TX_QUEUE		10
#define DSP_QUEUE			11
#define IRDA_A_QUEUE		12
#define IRDA_B_QUEUE		13
#define IRDA_C_QUEUE		14
#define IRDA_D_QUEUE		15
#define UART_QUEUE			16


#define MAX_MSG_COUNT		10	//IQ change to get ultimo comms working... this might screw up the initial
#define NUM_CLIENTS			6

extern _pool_id				message_pool;

/* General Mandolin create messages */
mandolin_message * CreateSyncOkMsg();
mandolin_message * CreateConnectMsg(bool fast_connect);
mandolin_message * CreateGetMaxMsgMsg();
mandolin_message * CreateGetHardwareInfoMsg();
uint32_t 		   ReadParameterFlags(mandolin_message * pMsg);
mandolin_message * Ping(mandolin_message * pMsg);
mandolin_message * GetPingResponse(mandolin_message * pMsg);
mandolin_message * GetBlankMessage();
mandolin_message * GetAckResponse(mandolin_message * pMsg);
mandolin_message * GetAckResponseWithData(mandolin_message * pMsg, uint8_t *pResponse, uint32_t uiBytes);
mandolin_message * GetSysEventResponseWithData(mandolin_message * pMsg, uint8_t *pResponse, uint32_t uiBytes);
mandolin_message * GetErrorResponse(mandolin_error_code error, mandolin_message * pMsg);
mandolin_message * GetParameterResponse(OLY_target Group, int Instance, uint32_t PID, uint32_t Value, uint8_t sequenceID);
mandolin_message * GetParametersResponse(OLY_target Group, int Instance, uint32_t * PIDs, uint32_t * Values, uint32_t numParams, uint8_t sequenceID);
mandolin_message * GetParameterBlockResponse(OLY_target Group, int Instance, uint32_t PID1, uint32_t * Values, uint32_t numParams, uint8_t sequenceID);
mandolin_message * SetParameter(OLY_target Group, int Instance, uint32_t PID, uint32_t Value);
mandolin_message * SetParameters(OLY_target Group, int Instance, uint32_t * PIDs, uint32_t * Values, uint32_t numParams);
mandolin_message * SetParameterBlock(OLY_target Group, int Instance, uint32_t PID1, uint32_t * Values, uint32_t numParams);
mandolin_message * GetSoftwareInfoResponse(uint32_t dante_fw_ver, uint32_t dante_cap_ver, uint32_t dante_cap_build, OLY_REGION_TYPE launchType, uint8_t sequenceID);
mandolin_message * GetHardwareInfoResponse(uint8_t brand, uint16_t model, uint8 mac_addr[6], uint8_t sequenceID);
mandolin_message * GetMacAddrResponse(uint8 mac_addr[6], uint8_t sequenceID);
mandolin_message * GetStatusAllResponse(uint32_t * pValues, uint8_t sequenceID);
mandolin_message * GetStaticAllResponse(uint32_t ip, uint32_t gateway, uint32_t mask, uint8_t sequenceID);
mandolin_message * RequestMeter(OLY_target Group, uint32_t PID, uint32_t instance, uint8_t sequenceID);
mandolin_message * RequestMeterResponse(OLY_target Group, uint32_t * meterPayload, uint8_t sequenceID);
mandolin_message * GetSerialNumberResponse(uint32_t uiSerialNumber, uint8_t sequenceID);
mandolin_message * GetSyncMessage(mandolin_system_event nEvent);
mandolin_message * SetMeters(int flags, bool bNoAck, uint32_t * PIDs, mandolin_parameter_value * values, mandolin_parameter_value * lastValues, uint32_t numParams, uint32_t fence);	// TODO: remove this, use set parameters list
mandolin_message * SetParametersList(OLY_target Group, uint32_t Instance, uint32_t listID, bool noAck, uint32_t * PIDs, mandolin_parameter_value * Values, uint32_t numParams);
mandolin_message * GetApplicationStringResponse(OLYspeaker1_DEVICE_lid LID, char * str_data, uint32_t str_len, uint8_t sequenceID);
mandolin_message * SetApplicationString(OLYspeaker1_DEVICE_lid LID, char * str_data);

LOUD_routingsrc    AudioSrc2Routing(oly_audio_source_t source);

// test
mandolin_message * Mfg_TestValueResponse(uint32_t PID, uint32_t Value, uint8_t sequenceID);
mandolin_message * Mfg_TestValueIndexedResponse(uint32_t PID, uint32_t VID, uint32_t Value, uint8_t sequenceID);

void ParseMandolinMQX(_queue_id source_qid, comm_method comm_type, mandolin_fifo * pFifo, int nRetries, int nTaskId);
void init_message_pool();



#endif /* SOURCES_MANDOLIN_HANDLE_H_ */
