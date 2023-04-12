#include "OLYSpeaker1_ParamTable.h"
#include "OLYspeaker1_map.h"

tParameterDesc* gOLYspeaker1ParameterTables[eTARGET_FENCE] = {
	 0,			// eTARGET_CURRENT_STATE
	0,   	// eTARGET_METERS
	(tParameterDesc*)gOLYspeaker1DeviceParameterTable,   	// eTARGET_DEVICE,
	(tParameterDesc*)gOLYspeaker1StatusParameterTable,   	// eTARGET_STATUS,
	(tParameterDesc*)gOLYspeaker1UserParameterTable,   		// eTARGET_USER,
	(tParameterDesc*)gOLYspeaker1ProfileParameterTable,   	// eTARGET_PROFILE,
	(tParameterDesc*)gOLYspeaker1ThrowParameterTable,   	// eTARGET_THROW,
	(tParameterDesc*)gOLYspeaker1VoicingParameterTable,   	// eTARGET_VOICING,
	(tParameterDesc*)gOLYspeaker1FirParameterTable,   		// eTARGET_FIR,
	(tParameterDesc*)gOLYspeaker1XoverParameterTable,   	// eTARGET_XOVER,
	(tParameterDesc*)gOLYspeaker1AirlossParameterTable,   	// eTARGET_AIRLOSS,
	(tParameterDesc*)gOLYspeaker1NoisegateParameterTable,  	// eTARGET_NOISEGATE,
};

int nOLYspeaker1ParameterTablesMax[eTARGET_FENCE] = {
	 0,			// eTARGET_CURENT_STATE
	0, 	// eTARGET_METER
	OLYspeaker1_DEVICE_PARAMETER_MAX,   	// eTARGET_DEVICE,
	OLYspeaker1_STATUS_PARAMETER_MAX,  		// eTARGET_STATUS,
	OLYspeaker1_USER_PARAMETER_MAX,   		// eTARGET_USER,
	OLYspeaker1_PROFILE_PARAMETER_MAX,   	// eTARGET_PROFILE,
	OLYspeaker1_THROW_PARAMETER_MAX,   		// eTARGET_THROW,
	OLYspeaker1_VOICING_PARAMETER_MAX,   	// eTARGET_VOICING,
	OLYspeaker1_FIR_PARAMETER_MAX,   		// eTARGET_FIR,
	OLYspeaker1_XOVER_PARAMETER_MAX,   		// eTARGET_XOVER,
	OLYspeaker1_AIRLOSS_PARAMETER_MAX, 		// eTARGET_AIRLOSS,
	OLYspeaker1_NOISEGATE_PARAMETER_MAX, 	// eTARGET_NOISEGATE,
};
