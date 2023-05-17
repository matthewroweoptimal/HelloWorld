
/* Model Type = CSXL_118 */
#include "Model_Info.h"

/* Include paths to files that include the preset tables referenced below */
#include "CSXL118_USER_data_factory_preset1.h"		
#include "CSXL118_USER_data_factory_preset2.h"
#include "CSXL118_USER_data_factory_preset3.h"
#include "CSXL118_USER_data_factory_preset4.h"
#include "CSXL118_PROFILE_data_factory_preset1.h"		
#include "CSXL118_PROFILE_data_factory_preset2.h"
#include "CSXL118_PROFILE_data_factory_preset3.h"
#include "CSXL118_PROFILE_data_factory_preset4.h"
#include "CSXL118_VOICING_data_factory_preset1.h"
#include "CSXL118_THROW_data_factory_preset1.h"
#include "CSXL118_XOVER_data_factory_preset1.h"
#include "CSXL118_FIR_data_factory_preset1.h" 

/* Pointers to preset values in external files.  NULL loads defaults from _map.c file */
/* Non "editable" presets MUST have external table */
//const uint32_t * User_Presets[USER_PRESETS] = {NULL, NULL, NULL, NULL, NULL};
const uint32_t * User_Presets[USER_PRESETS] = {NULL, NULL, USER_Voicing1, USER_Voicing2, USER_Voicing3, USER_Voicing4}; 
const uint32_t * Profile_Presets[PROFILE_PRESETS] = {NULL, NULL, PROFILE_Voicing1, PROFILE_Voicing2, PROFILE_Voicing3, PROFILE_Voicing4};
const uint32_t * Throw_Presets[THROW_PRESETS] = {THROW_Voicing1};
const uint32_t * Voicing_Presets[VOICING_PRESETS] = {VOICING_Voicing1};
const uint32_t * FIR_Presets[FIR_PRESETS] = {FIR_Voicing1};
const uint32_t * Xover_Presets[XOVER_PRESETS] = {XOVER_Voicing1};
