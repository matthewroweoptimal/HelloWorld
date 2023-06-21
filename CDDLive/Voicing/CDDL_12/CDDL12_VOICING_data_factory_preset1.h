// Created Wed Jun 22 14:42:01 2016

// RUgly Version 2.0.030d, June 17 2016
// Device: Martin CDDL12
// Device Version: 0.9.10.0
// XML Version: 18

#include "OLYspeaker1_map.h"

#if (OLYspeaker1_XML_VERSION != 18)
#error xml version out of date
#else

const uint32_t VOICING_Voicing1[]=
{
	 0x00000000 , // 0	NULL 0
	 0x00000001 , // 1	VOICING_GLOBAL_PEQ_ENABLE 1
	 0x00000002 , // 2	VOICING_GLOBAL_PEQ_BAND1_TYPE 2
	 0x00000000 , // 3	VOICING_GLOBAL_PEQ_BAND1_GAIN 0.0000000000e+000
	 0x42200000 , // 4	VOICING_GLOBAL_PEQ_BAND1_FREQ 40.0000000000
	 0x40240831 , // 5	VOICING_GLOBAL_PEQ_BAND1_Q 2.5629999638
	 0x00000002 , // 6	VOICING_GLOBAL_PEQ_BAND2_TYPE 2
	 0x00000000 , // 7	VOICING_GLOBAL_PEQ_BAND2_GAIN 0.0000000000e+000
	 0x42200000 , // 8	VOICING_GLOBAL_PEQ_BAND2_FREQ 40.0000000000
	 0x3F666666 , // 9	VOICING_GLOBAL_PEQ_BAND2_Q 0.8999999762
	 0x00000002 , // 10	VOICING_GLOBAL_PEQ_BAND3_TYPE 2
	 0x00000000 , // 11	VOICING_GLOBAL_PEQ_BAND3_GAIN 0.0000000000e+000
	 0x42200000 , // 12	VOICING_GLOBAL_PEQ_BAND3_FREQ 40.0000000000
	 0x3F19DB23 , // 13	VOICING_GLOBAL_PEQ_BAND3_Q 0.6010000110
	 0x00000002 , // 14	VOICING_GLOBAL_PEQ_BAND4_TYPE 2
	 0x00000000 , // 15	VOICING_GLOBAL_PEQ_BAND4_GAIN 0.0000000000e+000
	 0x42200000 , // 16	VOICING_GLOBAL_PEQ_BAND4_FREQ 40.0000000000
	 0x3F028F5C , // 17	VOICING_GLOBAL_PEQ_BAND4_Q 0.5099999905
	 0x00000002 , // 18	VOICING_GLOBAL_PEQ_BAND5_TYPE 2
	 0x00000000 , // 19	VOICING_GLOBAL_PEQ_BAND5_GAIN 0.0000000000e+000
	 0x428C0000 , // 20	VOICING_GLOBAL_PEQ_BAND5_FREQ 70.0000000000
	 0x3F8CCCCD , // 21	VOICING_GLOBAL_PEQ_BAND5_Q 1.1000000238
	 0x00000002 , // 22	VOICING_GLOBAL_PEQ_BAND6_TYPE 2
	 0x00000000 , // 23	VOICING_GLOBAL_PEQ_BAND6_GAIN 0.0000000000e+000
	 0x452F0000 , // 24	VOICING_GLOBAL_PEQ_BAND6_FREQ 2800.0000000000
	 0x40400000 , // 25	VOICING_GLOBAL_PEQ_BAND6_Q 3.0000000000
	 0x00000002 , // 26	VOICING_GLOBAL_PEQ_BAND7_TYPE 2
	 0x00000000 , // 27	VOICING_GLOBAL_PEQ_BAND7_GAIN 0.0000000000e+000
	 0x43960000 , // 28	VOICING_GLOBAL_PEQ_BAND7_FREQ 300.0000000000
	 0x40000000 , // 29	VOICING_GLOBAL_PEQ_BAND7_Q 2.0000000000
	 0x00000002 , // 30	VOICING_GLOBAL_PEQ_BAND8_TYPE 2
	 0x00000000 , // 31	VOICING_GLOBAL_PEQ_BAND8_GAIN 0.0000000000e+000
	 0x44EBC000 , // 32	VOICING_GLOBAL_PEQ_BAND8_FREQ 1886.0000000000
	 0x3F800000 , // 33	VOICING_GLOBAL_PEQ_BAND8_Q 1.0000000000
	 0x00000002 , // 34	VOICING_GLOBAL_PEQ_BAND9_TYPE 2
	 0x00000000 , // 35	VOICING_GLOBAL_PEQ_BAND9_GAIN 0.0000000000e+000
	 0x452BE000 , // 36	VOICING_GLOBAL_PEQ_BAND9_FREQ 2750.0000000000
	 0x40400000 , // 37	VOICING_GLOBAL_PEQ_BAND9_Q 3.0000000000
	 0x00000002 , // 38	VOICING_GLOBAL_PEQ_BAND10_TYPE 2
	 0x00000000 , // 39	VOICING_GLOBAL_PEQ_BAND10_GAIN 0.0000000000e+000
	 0x44BB8000 , // 40	VOICING_GLOBAL_PEQ_BAND10_FREQ 1500.0000000000
	 0x3FE66666 , // 41	VOICING_GLOBAL_PEQ_BAND10_Q 1.7999999523
	 0x00000002 , // 42	VOICING_GLOBAL_PEQ_BAND11_TYPE 2
	 0x00000000 , // 43	VOICING_GLOBAL_PEQ_BAND11_GAIN 0.0000000000e+000
	 0x456D8000 , // 44	VOICING_GLOBAL_PEQ_BAND11_FREQ 3800.0000000000
	 0x40000000 , // 45	VOICING_GLOBAL_PEQ_BAND11_Q 2.0000000000
	 0x00000002 , // 46	VOICING_GLOBAL_PEQ_BAND12_TYPE 2
	 0x00000000 , // 47	VOICING_GLOBAL_PEQ_BAND12_GAIN 0.0000000000e+000
	 0x45FA0000 , // 48	VOICING_GLOBAL_PEQ_BAND12_FREQ 8000.0000000000
	 0x3F34FDF4 , // 49	VOICING_GLOBAL_PEQ_BAND12_Q 0.7070000172
	 0x00000000 , // 50	VOICING_HF_FUNCGENLEVELOFFSET 0.0000000000e+000
	 0x411b3333 , // 51	VOICING_HF_FADER 6.0000000000 - IQ NU 6 + 3.7 = 9.7
	 0x00000000 , // 52	VOICING_HF_MUTE 0
	 0x00000000 , // 53	VOICING_HF_INVERT 0
	 0x40266666 , // 54	VOICING_HF_FADER_POST - IQ NU 2.6
	 0x00000000 , // 55	VOICING_HF_MUTE_POST 0
	 0x00000001 , // 56	VOICING_HF_PEQ_ENABLE 1
	 0x0000000E , // 57	VOICING_HF_PEQ_BAND1_TYPE 14
	 0x00000000 , // 58	VOICING_HF_PEQ_BAND1_GAIN 0.0000000000e+000
	 0x44FA0000 , // 59	VOICING_HF_PEQ_BAND1_FREQ 2000.0000000000
	 0x3FA74BC7 , // 60	VOICING_HF_PEQ_BAND1_Q 1.3070000410
	 0x0000000E , // 61	VOICING_HF_PEQ_BAND2_TYPE 14
	 0x00000000 , // 62	VOICING_HF_PEQ_BAND2_GAIN 0.0000000000e+000
	 0x44FA0000 , // 63	VOICING_HF_PEQ_BAND2_FREQ 2000.0000000000
	 0x3F0A7EFA , // 64	VOICING_HF_PEQ_BAND2_Q 0.5410000086
	 0x00000001 , // 65	VOICING_HF_PEQ_BAND3_TYPE 1
	 0x00000000 , // 66	VOICING_HF_PEQ_BAND3_GAIN 0.0000000000e+000
	 0x44FA0000 , // 67	VOICING_HF_PEQ_BAND3_FREQ 2000.0000000000
	 0x3FA74BC7 , // 68	VOICING_HF_PEQ_BAND3_Q 1.3070000410
	 0x0000000E , // 69	VOICING_HF_PEQ_BAND4_TYPE 14
	 0x00000000 , // 70	VOICING_HF_PEQ_BAND4_GAIN 0.0000000000e+000
	 0x44FA0000 , // 71	VOICING_HF_PEQ_BAND4_FREQ 2000.0000000000
	 0x3F0A7EFA , // 72	VOICING_HF_PEQ_BAND4_Q 0.5410000086
	 0x00000002 , // 73	VOICING_HF_PEQ_BAND5_TYPE 2
	 0x00000000 , // 74	VOICING_HF_PEQ_BAND5_GAIN 0.0000000000e+000
	 0x45433000 , // 75	VOICING_HF_PEQ_BAND5_FREQ 3123.0000000000
	 0x40400000 , // 76	VOICING_HF_PEQ_BAND5_Q 3.0000000000
	 0x00000002 , // 77	VOICING_HF_PEQ_BAND6_TYPE 2
	 0x00000000 , // 78	VOICING_HF_PEQ_BAND6_GAIN 0.0000000000e+000
	 0x45A5A000 , // 79	VOICING_HF_PEQ_BAND6_FREQ 5300.0000000000
	 0x40200000 , // 80	VOICING_HF_PEQ_BAND6_Q 2.5000000000
	 0x00000002 , // 81	VOICING_HF_PEQ_BAND7_TYPE 2
	 0x00000000 , // 82	VOICING_HF_PEQ_BAND7_GAIN 0.0000000000e+000
	 0x447A0000 , // 83	VOICING_HF_PEQ_BAND7_FREQ 1000.0000000000
	 0x40000000 , // 84	VOICING_HF_PEQ_BAND7_Q 2.0000000000
	 0x00000002 , // 85	VOICING_HF_PEQ_BAND8_TYPE 2
	 0x00000000 , // 86	VOICING_HF_PEQ_BAND8_GAIN 0.0000000000e+000
	 0x447A0000 , // 87	VOICING_HF_PEQ_BAND8_FREQ 1000.0000000000
	 0x40000000 , // 88	VOICING_HF_PEQ_BAND8_Q 2.0000000000
	 0x00000002 , // 89	VOICING_HF_PEQ_BAND9_TYPE 2
	 0x00000000 , // 90	VOICING_HF_PEQ_BAND9_GAIN 0.0000000000e+000
	 0x447A0000 , // 91	VOICING_HF_PEQ_BAND9_FREQ 1000.0000000000
	 0x40000000 , // 92	VOICING_HF_PEQ_BAND9_Q 2.0000000000
	 0x00000002 , // 93	VOICING_HF_PEQ_BAND10_TYPE 2
	 0x00000000 , // 94	VOICING_HF_PEQ_BAND10_GAIN 0.0000000000e+000
	 0x447A0000 , // 95	VOICING_HF_PEQ_BAND10_FREQ 1000.0000000000
	 0x40000000 , // 96	VOICING_HF_PEQ_BAND10_Q 2.0000000000
	 0x00000002 , // 97	VOICING_HF_PEQ_BAND11_TYPE 2
	 0x00000000 , // 98	VOICING_HF_PEQ_BAND11_GAIN 0.0000000000e+000
	 0x447A0000 , // 99	VOICING_HF_PEQ_BAND11_FREQ 1000.0000000000
	 0x40000000 , // 100	VOICING_HF_PEQ_BAND11_Q 2.0000000000
	 0x00000002 , // 101	VOICING_HF_PEQ_BAND12_TYPE 2
	 0x00000000 , // 102	VOICING_HF_PEQ_BAND12_GAIN 0.0000000000e+000
	 0x447A0000 , // 103	VOICING_HF_PEQ_BAND12_FREQ 1000.0000000000
	 0x40000000 , // 104	VOICING_HF_PEQ_BAND12_Q 2.0000000000
	 0x00000000 , // 105	VOICING_HF_SOFTLIMIT_ENABLE 0
	 0x00000000 , // 106	VOICING_HF_SOFTLIMIT_GAIN 0.0000000000e+000
	 0x00000000 , // 107	VOICING_HF_SOFTLIMIT_THRESH 0.0000000000e+000
	 0x3DCCCCCD , // 108	VOICING_HF_SOFTLIMIT_ATTACK 0.1000000015
	 0x437A0000 , // 109	VOICING_HF_SOFTLIMIT_RELEASE 250.0000000000
	 0x40000000 , // 110	VOICING_HF_SOFTLIMIT_RATIO 2.0000000000
	 0x00000001 , // 111	VOICING_HF_SOFTLIMIT_KNEE 1
	 0x00000001 , // 112	VOICING_HF_KLIMITER_PEAKENABLE 1
	 0x3F800000 , // 113	VOICING_HF_KLIMITER_PEAKATTACK 1.0000000000
	 0x41800000 , // 114	VOICING_HF_KLIMITER_GAINRELEASE 16.0000000000
	 0x00000000 , // 115	VOICING_HF_KLIMITER_LOOKAHEAD 0.0000000000e+000
	 0x3E428F5D , // 116	VOICING_HF_KLIMITER_MAXPEAK 0.1900000125
	 0x00000000 , // 117	VOICING_HF_KLIMITER_CLIPENABLE 0
	 0x3F800000 , // 118	VOICING_HF_KLIMITER_CLIPATTACK 1.0000000000
	 0x41F00000 , // 119	VOICING_HF_KLIMITER_CLIPRELEASE 30.0000000000
	 0x3F27C7A4 , // 120	VOICING_HF_KLIMITER_CLIPTHRESHOLD 0.6553900242
	 0x00000001 , // 121	VOICING_HF_PEQ_POST_ENABLE 1
	 0x00000002 , // 122	VOICING_HF_PEQ_POST_BAND1_TYPE 2
	 0x00000000 , // 123	VOICING_HF_PEQ_POST_BAND1_GAIN 0.0000000000e+000
	 0x447A0000 , // 124	VOICING_HF_PEQ_POST_BAND1_FREQ 1000.0000000000
	 0x40000000 , // 125	VOICING_HF_PEQ_POST_BAND1_Q 2.0000000000
	 0x00000002 , // 126	VOICING_HF_PEQ_POST_BAND2_TYPE 2
	 0x00000000 , // 127	VOICING_HF_PEQ_POST_BAND2_GAIN 0.0000000000e+000
	 0x447A0000 , // 128	VOICING_HF_PEQ_POST_BAND2_FREQ 1000.0000000000
	 0x40000000 , // 129	VOICING_HF_PEQ_POST_BAND2_Q 2.0000000000
	 0x00000002 , // 130	VOICING_HF_PEQ_POST_BAND3_TYPE 2
	 0x00000000 , // 131	VOICING_HF_PEQ_POST_BAND3_GAIN 0.0000000000e+000
	 0x447A0000 , // 132	VOICING_HF_PEQ_POST_BAND3_FREQ 1000.0000000000
	 0x40000000 , // 133	VOICING_HF_PEQ_POST_BAND3_Q 2.0000000000
	 0x00000002 , // 134	VOICING_HF_PEQ_POST_BAND4_TYPE 2
	 0x00000000 , // 135	VOICING_HF_PEQ_POST_BAND4_GAIN 0.0000000000e+000
	 0x447A0000 , // 136	VOICING_HF_PEQ_POST_BAND4_FREQ 1000.0000000000
	 0x40000000 , // 137	VOICING_HF_PEQ_POST_BAND4_Q 2.0000000000
	 0x00000001 , // 138	VOICING_HF_DYNEQ1_DETECTEN 1
	 0x00000008 , // 139	VOICING_HF_DYNEQ1_DETECTTYPE 8
	 0x41200000 , // 140	VOICING_HF_DYNEQ1_DETECTGAIN 10.0000000000
	 0x456A6000 , // 141	VOICING_HF_DYNEQ1_DETECTFREQ 3750.0000000000
	 0x40000000 , // 142	VOICING_HF_DYNEQ1_DETECTQ 2.0000000000
	 0x00000001 , // 143	VOICING_HF_DYNEQ1_AGCENABLE 1
	 0x40000000 , // 144	VOICING_HF_DYNEQ1_AGCATTACK 2.0000000000
	 0x42000000 , // 145	VOICING_HF_DYNEQ1_AGCDECAY 32.0000000000
	 0x00000000 , // 146	VOICING_HF_DYNEQ1_AGCACTIVE 0
	 0xC1A00000 , // 147	VOICING_HF_DYNEQ1_AGCTHRESH -20.0000000000
	 0x40000000 , // 148	VOICING_HF_DYNEQ1_AGCRATIO 2.0000000000
	 0x00000001 , // 149	VOICING_HF_DYNEQ1_RESUMEN 1
	 0x00000002 , // 150	VOICING_HF_DYNEQ1_RESUMTYPE 2
	 0xC0400000 , // 151	VOICING_HF_DYNEQ1_RESUMGAIN -3.0000000000
	 0x456A6000 , // 152	VOICING_HF_DYNEQ1_RESUMFREQ 3750.0000000000
	 0x40000000 , // 153	VOICING_HF_DYNEQ1_RESUMQ 2.0000000000
	 0x00000000 , // 154	VOICING_HF_DYNEQ2_DETECTEN 0
	 0x00000002 , // 155	VOICING_HF_DYNEQ2_DETECTTYPE 2
	 0x41700000 , // 156	VOICING_HF_DYNEQ2_DETECTGAIN 15.0000000000
	 0x41A00000 , // 157	VOICING_HF_DYNEQ2_DETECTFREQ 20.0000000000
	 0x40000000 , // 158	VOICING_HF_DYNEQ2_DETECTQ 2.0000000000
	 0x00000000 , // 159	VOICING_HF_DYNEQ2_AGCENABLE 0
	 0x3F000000 , // 160	VOICING_HF_DYNEQ2_AGCATTACK 0.5000000000
	 0x41A00000 , // 161	VOICING_HF_DYNEQ2_AGCDECAY 20.0000000000
	 0x00000000 , // 162	VOICING_HF_DYNEQ2_AGCACTIVE 0
	 0xC2200000 , // 163	VOICING_HF_DYNEQ2_AGCTHRESH -40.0000000000
	 0x3F800000 , // 164	VOICING_HF_DYNEQ2_AGCRATIO 1.0000000000
	 0x00000000 , // 165	VOICING_HF_DYNEQ2_RESUMEN 0
	 0x00000002 , // 166	VOICING_HF_DYNEQ2_RESUMTYPE 2
	 0x41700000 , // 167	VOICING_HF_DYNEQ2_RESUMGAIN 15.0000000000
	 0x41A00000 , // 168	VOICING_HF_DYNEQ2_RESUMFREQ 20.0000000000
	 0x40000000 , // 169	VOICING_HF_DYNEQ2_RESUMQ 2.0000000000
	 0x00000001 , // 170	VOICING_HF_DELAY_ENABLE 1
	 0x00000000 , // 171	VOICING_HF_DELAY_TIME 0.0000000000e+000
	 0x00000000 , // 172	VOICING_MF_FUNCGENLEVELOFFSET 0.0000000000e+000
	 0x00000000 , // 173	VOICING_MF_FADER 0.0000000000e+000
	 0x00000000 , // 174	VOICING_MF_MUTE 0
	 0x00000000 , // 175	VOICING_MF_INVERT 0
	 0x00000000 , // 176	VOICING_MF_FADER_POST 0.0000000000e+000
	 0x00000000 , // 177	VOICING_MF_MUTE_POST 0
	 0x00000001 , // 178	VOICING_MF_PEQ_ENABLE 1
	 0x00000002 , // 179	VOICING_MF_PEQ_BAND1_TYPE 2
	 0x00000000 , // 180	VOICING_MF_PEQ_BAND1_GAIN 0.0000000000e+000
	 0x447A0000 , // 181	VOICING_MF_PEQ_BAND1_FREQ 1000.0000000000
	 0x40000000 , // 182	VOICING_MF_PEQ_BAND1_Q 2.0000000000
	 0x00000002 , // 183	VOICING_MF_PEQ_BAND2_TYPE 2
	 0x00000000 , // 184	VOICING_MF_PEQ_BAND2_GAIN 0.0000000000e+000
	 0x447A0000 , // 185	VOICING_MF_PEQ_BAND2_FREQ 1000.0000000000
	 0x40000000 , // 186	VOICING_MF_PEQ_BAND2_Q 2.0000000000
	 0x00000002 , // 187	VOICING_MF_PEQ_BAND3_TYPE 2
	 0x00000000 , // 188	VOICING_MF_PEQ_BAND3_GAIN 0.0000000000e+000
	 0x447A0000 , // 189	VOICING_MF_PEQ_BAND3_FREQ 1000.0000000000
	 0x40000000 , // 190	VOICING_MF_PEQ_BAND3_Q 2.0000000000
	 0x00000002 , // 191	VOICING_MF_PEQ_BAND4_TYPE 2
	 0x00000000 , // 192	VOICING_MF_PEQ_BAND4_GAIN 0.0000000000e+000
	 0x447A0000 , // 193	VOICING_MF_PEQ_BAND4_FREQ 1000.0000000000
	 0x40000000 , // 194	VOICING_MF_PEQ_BAND4_Q 2.0000000000
	 0x00000002 , // 195	VOICING_MF_PEQ_BAND5_TYPE 2
	 0x00000000 , // 196	VOICING_MF_PEQ_BAND5_GAIN 0.0000000000e+000
	 0x447A0000 , // 197	VOICING_MF_PEQ_BAND5_FREQ 1000.0000000000
	 0x40000000 , // 198	VOICING_MF_PEQ_BAND5_Q 2.0000000000
	 0x00000002 , // 199	VOICING_MF_PEQ_BAND6_TYPE 2
	 0x00000000 , // 200	VOICING_MF_PEQ_BAND6_GAIN 0.0000000000e+000
	 0x447A0000 , // 201	VOICING_MF_PEQ_BAND6_FREQ 1000.0000000000
	 0x40000000 , // 202	VOICING_MF_PEQ_BAND6_Q 2.0000000000
	 0x00000002 , // 203	VOICING_MF_PEQ_BAND7_TYPE 2
	 0x00000000 , // 204	VOICING_MF_PEQ_BAND7_GAIN 0.0000000000e+000
	 0x447A0000 , // 205	VOICING_MF_PEQ_BAND7_FREQ 1000.0000000000
	 0x40000000 , // 206	VOICING_MF_PEQ_BAND7_Q 2.0000000000
	 0x00000002 , // 207	VOICING_MF_PEQ_BAND8_TYPE 2
	 0x00000000 , // 208	VOICING_MF_PEQ_BAND8_GAIN 0.0000000000e+000
	 0x447A0000 , // 209	VOICING_MF_PEQ_BAND8_FREQ 1000.0000000000
	 0x40000000 , // 210	VOICING_MF_PEQ_BAND8_Q 2.0000000000
	 0x00000002 , // 211	VOICING_MF_PEQ_BAND9_TYPE 2
	 0x00000000 , // 212	VOICING_MF_PEQ_BAND9_GAIN 0.0000000000e+000
	 0x447A0000 , // 213	VOICING_MF_PEQ_BAND9_FREQ 1000.0000000000
	 0x40000000 , // 214	VOICING_MF_PEQ_BAND9_Q 2.0000000000
	 0x00000002 , // 215	VOICING_MF_PEQ_BAND10_TYPE 2
	 0x00000000 , // 216	VOICING_MF_PEQ_BAND10_GAIN 0.0000000000e+000
	 0x447A0000 , // 217	VOICING_MF_PEQ_BAND10_FREQ 1000.0000000000
	 0x40000000 , // 218	VOICING_MF_PEQ_BAND10_Q 2.0000000000
	 0x00000002 , // 219	VOICING_MF_PEQ_BAND11_TYPE 2
	 0x00000000 , // 220	VOICING_MF_PEQ_BAND11_GAIN 0.0000000000e+000
	 0x447A0000 , // 221	VOICING_MF_PEQ_BAND11_FREQ 1000.0000000000
	 0x40000000 , // 222	VOICING_MF_PEQ_BAND11_Q 2.0000000000
	 0x00000002 , // 223	VOICING_MF_PEQ_BAND12_TYPE 2
	 0x00000000 , // 224	VOICING_MF_PEQ_BAND12_GAIN 0.0000000000e+000
	 0x447A0000 , // 225	VOICING_MF_PEQ_BAND12_FREQ 1000.0000000000
	 0x40000000 , // 226	VOICING_MF_PEQ_BAND12_Q 2.0000000000
	 0x00000000 , // 227	VOICING_MF_SOFTLIMIT_ENABLE 0
	 0x00000000 , // 228	VOICING_MF_SOFTLIMIT_GAIN 0.0000000000e+000
	 0x00000000 , // 229	VOICING_MF_SOFTLIMIT_THRESH 0.0000000000e+000
	 0x3DCCCCCD , // 230	VOICING_MF_SOFTLIMIT_ATTACK 0.1000000015
	 0x437A0000 , // 231	VOICING_MF_SOFTLIMIT_RELEASE 250.0000000000
	 0x40000000 , // 232	VOICING_MF_SOFTLIMIT_RATIO 2.0000000000
	 0x00000001 , // 233	VOICING_MF_SOFTLIMIT_KNEE 1
	 0x00000000 , // 234	VOICING_MF_KLIMITER_PEAKENABLE 0
	 0x41800000 , // 235	VOICING_MF_KLIMITER_PEAKATTACK 16.0000000000
	 0x42C80000 , // 236	VOICING_MF_KLIMITER_GAINRELEASE 100.0000000000
	 0x00000000 , // 237	VOICING_MF_KLIMITER_LOOKAHEAD 0.0000000000e+000
	 0x3F27C7A4 , // 238	VOICING_MF_KLIMITER_MAXPEAK 0.6553900242
	 0x00000000 , // 239	VOICING_MF_KLIMITER_CLIPENABLE 0
	 0x3F800000 , // 240	VOICING_MF_KLIMITER_CLIPATTACK 1.0000000000
	 0x41F00000 , // 241	VOICING_MF_KLIMITER_CLIPRELEASE 30.0000000000
	 0x3F27C7A4 , // 242	VOICING_MF_KLIMITER_CLIPTHRESHOLD 0.6553900242
	 0x00000001 , // 243	VOICING_MF_PEQ_POST_ENABLE 1
	 0x00000002 , // 244	VOICING_MF_PEQ_POST_BAND1_TYPE 2
	 0x00000000 , // 245	VOICING_MF_PEQ_POST_BAND1_GAIN 0.0000000000e+000
	 0x447A0000 , // 246	VOICING_MF_PEQ_POST_BAND1_FREQ 1000.0000000000
	 0x40000000 , // 247	VOICING_MF_PEQ_POST_BAND1_Q 2.0000000000
	 0x00000002 , // 248	VOICING_MF_PEQ_POST_BAND2_TYPE 2
	 0x00000000 , // 249	VOICING_MF_PEQ_POST_BAND2_GAIN 0.0000000000e+000
	 0x447A0000 , // 250	VOICING_MF_PEQ_POST_BAND2_FREQ 1000.0000000000
	 0x40000000 , // 251	VOICING_MF_PEQ_POST_BAND2_Q 2.0000000000
	 0x00000002 , // 252	VOICING_MF_PEQ_POST_BAND3_TYPE 2
	 0x00000000 , // 253	VOICING_MF_PEQ_POST_BAND3_GAIN 0.0000000000e+000
	 0x447A0000 , // 254	VOICING_MF_PEQ_POST_BAND3_FREQ 1000.0000000000
	 0x40000000 , // 255	VOICING_MF_PEQ_POST_BAND3_Q 2.0000000000
	 0x00000002 , // 256	VOICING_MF_PEQ_POST_BAND4_TYPE 2
	 0x00000000 , // 257	VOICING_MF_PEQ_POST_BAND4_GAIN 0.0000000000e+000
	 0x447A0000 , // 258	VOICING_MF_PEQ_POST_BAND4_FREQ 1000.0000000000
	 0x40000000 , // 259	VOICING_MF_PEQ_POST_BAND4_Q 2.0000000000
	 0x00000000 , // 260	VOICING_MF_DYNEQ1_DETECTEN 0
	 0x00000002 , // 261	VOICING_MF_DYNEQ1_DETECTTYPE 2
	 0x41700000 , // 262	VOICING_MF_DYNEQ1_DETECTGAIN 15.0000000000
	 0x41A00000 , // 263	VOICING_MF_DYNEQ1_DETECTFREQ 20.0000000000
	 0x40000000 , // 264	VOICING_MF_DYNEQ1_DETECTQ 2.0000000000
	 0x00000000 , // 265	VOICING_MF_DYNEQ1_AGCENABLE 0
	 0x3F000000 , // 266	VOICING_MF_DYNEQ1_AGCATTACK 0.5000000000
	 0x41A00000 , // 267	VOICING_MF_DYNEQ1_AGCDECAY 20.0000000000
	 0x00000000 , // 268	VOICING_MF_DYNEQ1_AGCACTIVE 0
	 0xC2200000 , // 269	VOICING_MF_DYNEQ1_AGCTHRESH -40.0000000000
	 0x3F800000 , // 270	VOICING_MF_DYNEQ1_AGCRATIO 1.0000000000
	 0x00000000 , // 271	VOICING_MF_DYNEQ1_RESUMEN 0
	 0x00000002 , // 272	VOICING_MF_DYNEQ1_RESUMTYPE 2
	 0x41700000 , // 273	VOICING_MF_DYNEQ1_RESUMGAIN 15.0000000000
	 0x41A00000 , // 274	VOICING_MF_DYNEQ1_RESUMFREQ 20.0000000000
	 0x40000000 , // 275	VOICING_MF_DYNEQ1_RESUMQ 2.0000000000
	 0x00000000 , // 276	VOICING_MF_DYNEQ2_DETECTEN 0
	 0x00000002 , // 277	VOICING_MF_DYNEQ2_DETECTTYPE 2
	 0x41700000 , // 278	VOICING_MF_DYNEQ2_DETECTGAIN 15.0000000000
	 0x41A00000 , // 279	VOICING_MF_DYNEQ2_DETECTFREQ 20.0000000000
	 0x40000000 , // 280	VOICING_MF_DYNEQ2_DETECTQ 2.0000000000
	 0x00000000 , // 281	VOICING_MF_DYNEQ2_AGCENABLE 0
	 0x3F000000 , // 282	VOICING_MF_DYNEQ2_AGCATTACK 0.5000000000
	 0x41A00000 , // 283	VOICING_MF_DYNEQ2_AGCDECAY 20.0000000000
	 0x00000000 , // 284	VOICING_MF_DYNEQ2_AGCACTIVE 0
	 0xC2200000 , // 285	VOICING_MF_DYNEQ2_AGCTHRESH -40.0000000000
	 0x3F800000 , // 286	VOICING_MF_DYNEQ2_AGCRATIO 1.0000000000
	 0x00000000 , // 287	VOICING_MF_DYNEQ2_RESUMEN 0
	 0x00000002 , // 288	VOICING_MF_DYNEQ2_RESUMTYPE 2
	 0x41700000 , // 289	VOICING_MF_DYNEQ2_RESUMGAIN 15.0000000000
	 0x41A00000 , // 290	VOICING_MF_DYNEQ2_RESUMFREQ 20.0000000000
	 0x40000000 , // 291	VOICING_MF_DYNEQ2_RESUMQ 2.0000000000
	 0x00000001 , // 292	VOICING_MF_DELAY_ENABLE 1
	 0x00000000 , // 293	VOICING_MF_DELAY_TIME 0.0000000000e+000
	 0x00000000 , // 294	VOICING_LF_FUNCGENLEVELOFFSET 0.0000000000e+000
	 0x406CCCCD , // 295	VOICING_LF_FADER 0.0000000000e+000 - IQ NU +3.7 = 3.7
	 0x00000000 , // 296	VOICING_LF_MUTE 0
	 0x00000000 , // 297	VOICING_LF_INVERT 0
	 0x40266666 , // 298	VOICING_LF_FADER_POST IQ NU 2.6
	 0x00000000 , // 299	VOICING_LF_MUTE_POST 0
	 0x00000001 , // 300	VOICING_LF_PEQ_ENABLE 1
	 0x0000000D , // 301	VOICING_LF_PEQ_BAND1_TYPE 13
	 0x00000000 , // 302	VOICING_LF_PEQ_BAND1_GAIN 0.0000000000e+000
	 0x44FA0000 , // 303	VOICING_LF_PEQ_BAND1_FREQ 2000.0000000000
	 0x3FA74BC7 , // 304	VOICING_LF_PEQ_BAND1_Q 1.3070000410
	 0x0000000D , // 305	VOICING_LF_PEQ_BAND2_TYPE 13
	 0x00000000 , // 306	VOICING_LF_PEQ_BAND2_GAIN 0.0000000000e+000
	 0x44FA0000 , // 307	VOICING_LF_PEQ_BAND2_FREQ 2000.0000000000
	 0x3F0A7EFA , // 308	VOICING_LF_PEQ_BAND2_Q 0.5410000086
	 0x0000000D , // 309	VOICING_LF_PEQ_BAND3_TYPE 13
	 0x00000000 , // 310	VOICING_LF_PEQ_BAND3_GAIN 0.0000000000e+000
	 0x44FA0000 , // 311	VOICING_LF_PEQ_BAND3_FREQ 2000.0000000000
	 0x3FA74BC7 , // 312	VOICING_LF_PEQ_BAND3_Q 1.3070000410
	 0x0000000D , // 313	VOICING_LF_PEQ_BAND4_TYPE 13
	 0x00000000 , // 314	VOICING_LF_PEQ_BAND4_GAIN 0.0000000000e+000
	 0x44FA0000 , // 315	VOICING_LF_PEQ_BAND4_FREQ 2000.0000000000
	 0x3F0A7EFA , // 316	VOICING_LF_PEQ_BAND4_Q 0.5410000086
	 0x00000002 , // 317	VOICING_LF_PEQ_BAND5_TYPE 2
	 0x00000000 , // 318	VOICING_LF_PEQ_BAND5_GAIN 0.0000000000e+000
	 0x43000000 , // 319	VOICING_LF_PEQ_BAND5_FREQ 128.0000000000
	 0x40C00000 , // 320	VOICING_LF_PEQ_BAND5_Q 6.0000000000
	 0x00000002 , // 321	VOICING_LF_PEQ_BAND6_TYPE 2
	 0x00000000 , // 322	VOICING_LF_PEQ_BAND6_GAIN 0.0000000000e+000
	 0x43580000 , // 323	VOICING_LF_PEQ_BAND6_FREQ 216.0000000000
	 0x40A00000 , // 324	VOICING_LF_PEQ_BAND6_Q 5.0000000000
	 0x00000002 , // 325	VOICING_LF_PEQ_BAND7_TYPE 2
	 0x00000000 , // 326	VOICING_LF_PEQ_BAND7_GAIN 0.0000000000e+000
	 0x43F90000 , // 327	VOICING_LF_PEQ_BAND7_FREQ 498.0000000000
	 0x3FE66666 , // 328	VOICING_LF_PEQ_BAND7_Q 1.7999999523
	 0x00000002 , // 329	VOICING_LF_PEQ_BAND8_TYPE 2
	 0x00000000 , // 330	VOICING_LF_PEQ_BAND8_GAIN 0.0000000000e+000
	 0x443B8000 , // 331	VOICING_LF_PEQ_BAND8_FREQ 750.0000000000
	 0x40400000 , // 332	VOICING_LF_PEQ_BAND8_Q 3.0000000000
	 0x00000002 , // 333	VOICING_LF_PEQ_BAND9_TYPE 2
	 0x00000000 , // 334	VOICING_LF_PEQ_BAND9_GAIN 0.0000000000e+000
	 0x44E10000 , // 335	VOICING_LF_PEQ_BAND9_FREQ 1800.0000000000
	 0x3F34FDF4 , // 336	VOICING_LF_PEQ_BAND9_Q 0.7070000172
	 0x00000002 , // 337	VOICING_LF_PEQ_BAND10_TYPE 2
	 0x00000000 , // 338	VOICING_LF_PEQ_BAND10_GAIN 0.0000000000e+000
	 0x44E10000 , // 339	VOICING_LF_PEQ_BAND10_FREQ 1800.0000000000
	 0x3F34FDF4 , // 340	VOICING_LF_PEQ_BAND10_Q 0.7070000172
	 0x00000002 , // 341	VOICING_LF_PEQ_BAND11_TYPE 2
	 0x00000000 , // 342	VOICING_LF_PEQ_BAND11_GAIN 0.0000000000e+000
	 0x44E10000 , // 343	VOICING_LF_PEQ_BAND11_FREQ 1800.0000000000
	 0x3F34FDF4 , // 344	VOICING_LF_PEQ_BAND11_Q 0.7070000172
	 0x00000002 , // 345	VOICING_LF_PEQ_BAND12_TYPE 2
	 0x00000000 , // 346	VOICING_LF_PEQ_BAND12_GAIN 0.0000000000e+000
	 0x44E10000 , // 347	VOICING_LF_PEQ_BAND12_FREQ 1800.0000000000
	 0x3F34FDF4 , // 348	VOICING_LF_PEQ_BAND12_Q 0.7070000172
	 0x00000000 , // 349	VOICING_LF_SOFTLIMIT_ENABLE 0
	 0x00000000 , // 350	VOICING_LF_SOFTLIMIT_GAIN 0.0000000000e+000
	 0x00000000 , // 351	VOICING_LF_SOFTLIMIT_THRESH 0.0000000000e+000
	 0x3DCCCCCD , // 352	VOICING_LF_SOFTLIMIT_ATTACK 0.1000000015
	 0x437A0000 , // 353	VOICING_LF_SOFTLIMIT_RELEASE 250.0000000000
	 0x40000000 , // 354	VOICING_LF_SOFTLIMIT_RATIO 2.0000000000
	 0x00000001 , // 355	VOICING_LF_SOFTLIMIT_KNEE 1
	 0x00000001 , // 356	VOICING_LF_KLIMITER_PEAKENABLE 1
	 0x41B80000 , // 357	VOICING_LF_KLIMITER_PEAKATTACK 23.0000000000
	 0x43B80000 , // 358	VOICING_LF_KLIMITER_GAINRELEASE 368.0000000000
	 0x00000000 , // 359	VOICING_LF_KLIMITER_LOOKAHEAD 0.0000000000e+000
	 0x3E75C28F , // 360	VOICING_LF_KLIMITER_MAXPEAK 0.2399999946
	 0x00000000 , // 361	VOICING_LF_KLIMITER_CLIPENABLE 0
	 0x3F800000 , // 362	VOICING_LF_KLIMITER_CLIPATTACK 1.0000000000
	 0x41F00000 , // 363	VOICING_LF_KLIMITER_CLIPRELEASE 30.0000000000
	 0x3F27C7A4 , // 364	VOICING_LF_KLIMITER_CLIPTHRESHOLD 0.6553900242
	 0x00000001 , // 365	VOICING_LF_PEQ_POST_ENABLE 1
	 0x00000002 , // 366	VOICING_LF_PEQ_POST_BAND1_TYPE 2
	 0x00000000 , // 367	VOICING_LF_PEQ_POST_BAND1_GAIN 0.0000000000e+000
	 0x457A0000 , // 368	VOICING_LF_PEQ_POST_BAND1_FREQ 4000.0000000000
	 0x3F35C28F , // 369	VOICING_LF_PEQ_POST_BAND1_Q 0.7099999785
	 0x00000002 , // 370	VOICING_LF_PEQ_POST_BAND2_TYPE 2
	 0x00000000 , // 371	VOICING_LF_PEQ_POST_BAND2_GAIN 0.0000000000e+000
	 0x447A0000 , // 372	VOICING_LF_PEQ_POST_BAND2_FREQ 1000.0000000000
	 0x40000000 , // 373	VOICING_LF_PEQ_POST_BAND2_Q 2.0000000000
	 0x00000002 , // 374	VOICING_LF_PEQ_POST_BAND3_TYPE 2
	 0x00000000 , // 375	VOICING_LF_PEQ_POST_BAND3_GAIN 0.0000000000e+000
	 0x447A0000 , // 376	VOICING_LF_PEQ_POST_BAND3_FREQ 1000.0000000000
	 0x40000000 , // 377	VOICING_LF_PEQ_POST_BAND3_Q 2.0000000000
	 0x00000002 , // 378	VOICING_LF_PEQ_POST_BAND4_TYPE 2
	 0x00000000 , // 379	VOICING_LF_PEQ_POST_BAND4_GAIN 0.0000000000e+000
	 0x447A0000 , // 380	VOICING_LF_PEQ_POST_BAND4_FREQ 1000.0000000000
	 0x40000000 , // 381	VOICING_LF_PEQ_POST_BAND4_Q 2.0000000000
	 0x00000000 , // 382	VOICING_LF_DYNEQ1_DETECTEN 0
	 0x00000008 , // 383	VOICING_LF_DYNEQ1_DETECTTYPE 8
	 0x40E00000 , // 384	VOICING_LF_DYNEQ1_DETECTGAIN 7.0000000000
	 0x428C0000 , // 385	VOICING_LF_DYNEQ1_DETECTFREQ 70.0000000000
	 0x3F8CCCCD , // 386	VOICING_LF_DYNEQ1_DETECTQ 1.1000000238
	 0x00000001 , // 387	VOICING_LF_DYNEQ1_AGCENABLE 1
	 0x41B80000 , // 388	VOICING_LF_DYNEQ1_AGCATTACK 23.0000000000
	 0x43B80000 , // 389	VOICING_LF_DYNEQ1_AGCDECAY 368.0000000000
	 0x00000000 , // 390	VOICING_LF_DYNEQ1_AGCACTIVE 0
	 0xC1700000 , // 391	VOICING_LF_DYNEQ1_AGCTHRESH -15.0000000000
	 0x3FB33333 , // 392	VOICING_LF_DYNEQ1_AGCRATIO 1.3999999762
	 0x00000000 , // 393	VOICING_LF_DYNEQ1_RESUMEN 0
	 0x00000002 , // 394	VOICING_LF_DYNEQ1_RESUMTYPE 2
	 0xC0E00000 , // 395	VOICING_LF_DYNEQ1_RESUMGAIN -7.0000000000
	 0x428C0000 , // 396	VOICING_LF_DYNEQ1_RESUMFREQ 70.0000000000
	 0x3F8CCCCD , // 397	VOICING_LF_DYNEQ1_RESUMQ 1.1000000238
	 0x00000000 , // 398	VOICING_LF_DYNEQ2_DETECTEN 0
	 0x00000008 , // 399	VOICING_LF_DYNEQ2_DETECTTYPE 8
	 0x40E00000 , // 400	VOICING_LF_DYNEQ2_DETECTGAIN 7.0000000000
	 0x428C0000 , // 401	VOICING_LF_DYNEQ2_DETECTFREQ 70.0000000000
	 0x3F8CCCCD , // 402	VOICING_LF_DYNEQ2_DETECTQ 1.1000000238
	 0x00000000 , // 403	VOICING_LF_DYNEQ2_AGCENABLE 0
	 0x41B80000 , // 404	VOICING_LF_DYNEQ2_AGCATTACK 23.0000000000
	 0x43B80000 , // 405	VOICING_LF_DYNEQ2_AGCDECAY 368.0000000000
	 0x00000000 , // 406	VOICING_LF_DYNEQ2_AGCACTIVE 0
	 0xC1600000 , // 407	VOICING_LF_DYNEQ2_AGCTHRESH -14.0000000000
	 0x3FB33333 , // 408	VOICING_LF_DYNEQ2_AGCRATIO 1.3999999762
	 0x00000000 , // 409	VOICING_LF_DYNEQ2_RESUMEN 0
	 0x00000002 , // 410	VOICING_LF_DYNEQ2_RESUMTYPE 2
	 0x40C00000 , // 411	VOICING_LF_DYNEQ2_RESUMGAIN 6.0000000000
	 0x42B40000 , // 412	VOICING_LF_DYNEQ2_RESUMFREQ 90.0000000000
	 0x3F99999A , // 413	VOICING_LF_DYNEQ2_RESUMQ 1.2000000477
	 0x00000001 , // 414	VOICING_LF_DELAY_ENABLE 1
	 0x3E570A3D , // 415	VOICING_LF_DELAY_TIME 0.2099999934
	 0x00000003 , // 416	VOICING_AMP1_ROUTING 3		//Always LF.
	 0x00000001 , // 417	VOICING_AMP2_ROUTING 1		//IQ edit for Nuvoton. 3 for sub. 1 for top box.
	 0x00000000 , // 418	VOICING_AMP3_ROUTING 0		//IQ edit for Nuvoton. Never used.
	 0x00000000 , // 419	VOICING_AMP4_ROUTING 0		//IQ edit for Nuvoton. Never used.
};
#endif
