
/* Model Type = CDDL-8 */
#include "Model_Info.h"

/* Include paths to files that include the preset tables referenced below */
#include "CDDL8_VOICING_data_factory_preset1.h"
#include "CDDL8_FIR_data_factory_preset1.h"

/* Pointers to preset values in external files.  NULL loads defaults from _map.c file */
/* Non "editable" presets MUST have external table */
//const uint32_t * User_Presets[USER_PRESETS] = {NULL, NULL, NULL, NULL, NULL};
const uint32_t * User_Presets[USER_PRESETS] = {NULL, NULL, NULL, NULL, NULL}; 
const uint32_t * Profile_Presets[PROFILE_PRESETS] = {NULL, NULL, NULL, NULL, NULL};
const uint32_t * Throw_Presets[THROW_PRESETS] = {NULL};
const uint32_t * Voicing_Presets[VOICING_PRESETS] = {VOICING_Voicing1};
const uint32_t  * FIR_Presets[FIR_PRESETS] = {FIR_Voicing1};
const uint32_t * Xover_Presets[XOVER_PRESETS] = {NULL};
