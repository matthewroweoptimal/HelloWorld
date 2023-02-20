
/* Model Type = CDDL-15 */

#define CODEC_CH1_INVERT	0
#define CODEC_CH2_INVERT	1
#define CODEC_CH3_INVERT	0
#define CODEC_CH4_INVERT	0

/* Amp channels used */
#define HAS_HF				1
#define HAS_MF				0
#define HAS_LF				1
#define HAS_LF2				0
#define HAS_CURRENT_SENSE   1

#define AMP1_BRIDGED		1

/* Defines number of instances for each group. Instances above MAX_EDITABLE are read-only */
/* EDITABLE instances are stored in parameter flash */
/* There must be at least one instance for each group */

#define USER_PRESETS	 		5
//#define USER_PRESETS_STORED		2
#define USER_PRESETS_STORED		5

#define PROFILE_PRESETS			5
//#define PROFILE_PRESETS_STORED	2
#define PROFILE_PRESETS_STORED	5		// TODO: Update this once preset bug is fixed

#define THROW_PRESETS			1
#define THROW_PRESETS_STORED	1

#define VOICING_PRESETS			1
#define VOICING_PRESETS_STORED	1

#define FIR_PRESETS				1
#define FIR_PRESETS_STORED		1

#define XOVER_PRESETS			1	
#define XOVER_PRESETS_STORED	1

#define USE_CDD_NOISEGATE       1

/* UI Definitions */
#define USE_CDD_UI				1
