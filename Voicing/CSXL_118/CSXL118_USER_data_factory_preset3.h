// Created Mon Jul 04 17:44:47 2016

// RUgly Version 2.0.032, June 20 2016
// Device: Martin CSXL118
// Device Version: 0.10.4.0
// XML Version: 18

#include "OLYspeaker1_map.h"

#if (OLYspeaker1_XML_VERSION != 18)
#error xml version out of date
#else

const uint32_t USER_Voicing3[]=
{
	 0x00000000 , // 0	NULL 0
	 0x000003E8 , // 1	USER_INPUT_ROUTING 1000
	 0x00000001 , // 2	USER_INPUT_FUNCGENPOST 1
	 0x00000001 , // 3	USER_INPUT_FUNCGEN_ENABLE 1
	 0xC2F00000 , // 4	USER_INPUT_FUNCGEN_FADER -120.0000000000
	 0x00000002 , // 5	USER_INPUT_FUNCGEN_TYPE 2
	 0x447A0000 , // 6	USER_INPUT_FUNCGEN_FREQ 1000.0000000000
	 0xC0533333 , // 7	USER_SPEAKER_FADER -3.2999999523
	 0x00000000 , // 8	USER_SPEAKER_MUTE 0
	 0x00000001 , // 9	USER_SPEAKER_INVERT 1
	 0x00000001 , // 10	USER_SPEAKER_PEQ_ENABLE 1
	 0x00000002 , // 11	USER_SPEAKER_PEQ_BAND1_TYPE 2
	 0x3F800000 , // 12	USER_SPEAKER_PEQ_BAND1_GAIN 1.0000000000
	 0x42700000 , // 13	USER_SPEAKER_PEQ_BAND1_FREQ 60.0000000000
	 0x40800000 , // 14	USER_SPEAKER_PEQ_BAND1_Q 4.0000000000
	 0x00000002 , // 15	USER_SPEAKER_PEQ_BAND2_TYPE 2
	 0xBF800000 , // 16	USER_SPEAKER_PEQ_BAND2_GAIN -1.0000000000
	 0x42B40000 , // 17	USER_SPEAKER_PEQ_BAND2_FREQ 90.0000000000
	 0x40400000 , // 18	USER_SPEAKER_PEQ_BAND2_Q 3.0000000000
	 0x00000004 , // 19	USER_SPEAKER_PEQ_BAND3_TYPE 4
	 0xC0A00000 , // 20	USER_SPEAKER_PEQ_BAND3_GAIN -5.0000000000
	 0x42C80000 , // 21	USER_SPEAKER_PEQ_BAND3_FREQ 100.0000000000
	 0x3F4CCCCD , // 22	USER_SPEAKER_PEQ_BAND3_Q 0.8000000119
	 0x00000004 , // 23	USER_SPEAKER_PEQ_BAND4_TYPE 4
	 0xC1400000 , // 24	USER_SPEAKER_PEQ_BAND4_GAIN -12.0000000000
	 0x43AF0000 , // 25	USER_SPEAKER_PEQ_BAND4_FREQ 350.0000000000
	 0x3F35C28F , // 26	USER_SPEAKER_PEQ_BAND4_Q 0.7099999785
	 0x00000002 , // 27	USER_SPEAKER_PEQ_BAND5_TYPE 2
	 0x00000000 , // 28	USER_SPEAKER_PEQ_BAND5_GAIN 0.0000000000e+000
	 0x447A0000 , // 29	USER_SPEAKER_PEQ_BAND5_FREQ 1000.0000000000
	 0x40000000 , // 30	USER_SPEAKER_PEQ_BAND5_Q 2.0000000000
	 0x00000002 , // 31	USER_SPEAKER_PEQ_BAND6_TYPE 2
	 0x00000000 , // 32	USER_SPEAKER_PEQ_BAND6_GAIN 0.0000000000e+000
	 0x447A0000 , // 33	USER_SPEAKER_PEQ_BAND6_FREQ 1000.0000000000
	 0x40000000 , // 34	USER_SPEAKER_PEQ_BAND6_Q 2.0000000000
	 0x00000002 , // 35	USER_SPEAKER_PEQ_BAND7_TYPE 2
	 0x00000000 , // 36	USER_SPEAKER_PEQ_BAND7_GAIN 0.0000000000e+000
	 0x447A0000 , // 37	USER_SPEAKER_PEQ_BAND7_FREQ 1000.0000000000
	 0x40000000 , // 38	USER_SPEAKER_PEQ_BAND7_Q 2.0000000000
	 0x00000002 , // 39	USER_SPEAKER_PEQ_BAND8_TYPE 2
	 0x00000000 , // 40	USER_SPEAKER_PEQ_BAND8_GAIN 0.0000000000e+000
	 0x447A0000 , // 41	USER_SPEAKER_PEQ_BAND8_FREQ 1000.0000000000
	 0x40000000 , // 42	USER_SPEAKER_PEQ_BAND8_Q 2.0000000000
	 0x00000001 , // 43	USER_SPEAKER_DELAY_ENABLE 1
	 0x40066666 , // 44	USER_SPEAKER_DELAY_TIME 2.0999999046
	 0x40800000 , // 45	USER_GROUP_FADER 4.0000000000
	 0x00000000 , // 46	USER_GROUP_MUTE 0
	 0x00000001 , // 47	USER_GROUP_PEQ_ENABLE 1
	 0x0000000D , // 48	USER_GROUP_PEQ_BAND1_TYPE 13
	 0x00000000 , // 49	USER_GROUP_PEQ_BAND1_GAIN 0.0000000000e+000
	 0x42B40000 , // 50	USER_GROUP_PEQ_BAND1_FREQ 90.0000000000
	 0x3FA73D74 , // 51	USER_GROUP_PEQ_BAND1_Q 1.3065629005
	 0x0000000D , // 52	USER_GROUP_PEQ_BAND2_TYPE 13
	 0x00000000 , // 53	USER_GROUP_PEQ_BAND2_GAIN 0.0000000000e+000
	 0x42B40000 , // 54	USER_GROUP_PEQ_BAND2_FREQ 90.0000000000
	 0x3F0A8BD4 , // 55	USER_GROUP_PEQ_BAND2_Q 0.5411961079
	 0x0000000D , // 56	USER_GROUP_PEQ_BAND3_TYPE 13
	 0x00000000 , // 57	USER_GROUP_PEQ_BAND3_GAIN 0.0000000000e+000
	 0x42B40000 , // 58	USER_GROUP_PEQ_BAND3_FREQ 90.0000000000
	 0x3FA73D74 , // 59	USER_GROUP_PEQ_BAND3_Q 1.3065629005
	 0x0000000D , // 60	USER_GROUP_PEQ_BAND4_TYPE 13
	 0x00000000 , // 61	USER_GROUP_PEQ_BAND4_GAIN 0.0000000000e+000
	 0x42B40000 , // 62	USER_GROUP_PEQ_BAND4_FREQ 90.0000000000
	 0x3F0A8BD4 , // 63	USER_GROUP_PEQ_BAND4_Q 0.5411961079
	 0x0000000E , // 64	USER_GROUP_PEQ_BAND5_TYPE 14
	 0x00000000 , // 65	USER_GROUP_PEQ_BAND5_GAIN 0.0000000000e+000
	 0x41F00000 , // 66	USER_GROUP_PEQ_BAND5_FREQ 30.0000000000
	 0x3FA73D74 , // 67	USER_GROUP_PEQ_BAND5_Q 1.3065629005
	 0x0000000E , // 68	USER_GROUP_PEQ_BAND6_TYPE 14
	 0x00000000 , // 69	USER_GROUP_PEQ_BAND6_GAIN 0.0000000000e+000
	 0x41F00000 , // 70	USER_GROUP_PEQ_BAND6_FREQ 30.0000000000
	 0x3F0A8BD4 , // 71	USER_GROUP_PEQ_BAND6_Q 0.5411961079
	 0x0000000E , // 72	USER_GROUP_PEQ_BAND7_TYPE 14
	 0x00000000 , // 73	USER_GROUP_PEQ_BAND7_GAIN 0.0000000000e+000
	 0x41F00000 , // 74	USER_GROUP_PEQ_BAND7_FREQ 30.0000000000
	 0x3FA73D74 , // 75	USER_GROUP_PEQ_BAND7_Q 1.3065629005
	 0x0000000E , // 76	USER_GROUP_PEQ_BAND8_TYPE 14
	 0x00000000 , // 77	USER_GROUP_PEQ_BAND8_GAIN 0.0000000000e+000
	 0x41F00000 , // 78	USER_GROUP_PEQ_BAND8_FREQ 30.0000000000
	 0x3F0A8BD4 , // 79	USER_GROUP_PEQ_BAND8_Q 0.5411961079
	 0x00000001 , // 80	USER_GROUP_DELAY_ENABLE 1
	 0x00000000 , // 81	USER_GROUP_DELAY_TIME 0.0000000000e+000
	 0x00000000 , // 82	USER_GROUP_SOFTLIMIT_ENABLE 0
	 0x00000000 , // 83	USER_GROUP_SOFTLIMIT_GAIN 0.0000000000e+000
	 0x00000000 , // 84	USER_GROUP_SOFTLIMIT_THRESH 0.0000000000e+000
	 0x3DCCCCCD , // 85	USER_GROUP_SOFTLIMIT_ATTACK 0.1000000015
	 0x437A0000 , // 86	USER_GROUP_SOFTLIMIT_RELEASE 250.0000000000
	 0x40000000 , // 87	USER_GROUP_SOFTLIMIT_RATIO 2.0000000000
	 0x00000001 , // 88	USER_GROUP_SOFTLIMIT_KNEE 1
};
#endif
