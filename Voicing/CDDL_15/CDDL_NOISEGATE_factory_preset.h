// Created Wed Jun 22 14:42:01 2016

// RUgly Version 2.0.030d, June 17 2016
// Device: Martin CDDL12
// Device Version: 0.9.10.0
// XML Version: 18

#include "OLYspeaker1_map.h"

#if (OLYspeaker1_XML_VERSION != 18)
#error xml version out of date
#else

    const uint32_t NOISEGATE_Preset_CDDL[] =
{    0x00000000 , // 0 NULL 0
     0x00000001 , // 1 NOISEGATE_INPUT_GATE_ENABLE 1
     0xC2B40000 , // 2 NOISEGATE_INPUT_GATE_THRESH -90.000000 
     0x3DCCCCCD , // 3 NOISEGATE_INPUT_GATE_ATTACK 0.100000 
     0x447A0000 , // 3 NOISEGATE_INPUT_GATE_RELEASE 1000.000000
     0x41700000 , // 3 NOISEGATE_INPUT_GATE_RANGE 15.000000
     0x44FA0000 , // 3 NOISEGATE_INPUT_GATE_HOLD 0.100000 
};

#endif
