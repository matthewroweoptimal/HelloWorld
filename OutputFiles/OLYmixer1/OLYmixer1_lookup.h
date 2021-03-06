// DO NOT EDIT THIS FILE
// Created by xmlerator on Friday, 12 December 2014 at 06:05:53 PM.
#define OLYmixer1_XML_VERSION 7

#ifndef __OLYmixer1_LOOKUP_H__
#define __OLYmixer1_LOOKUP_H__

#include "OLYmixer1_pids.h"
#include "OLYmixer1_size.h"

#define GetParamID_InputSolo(input) ( ePID_OLYmixer1_GLOBAL_INPUT1_SOLO + GLOBAL_INPUT__PARAMS*((input)-1) )
#define GetParamID_FxreverbSolo(fxreverb) ( ePID_OLYmixer1_GLOBAL_FXREVERB_SOLO + GLOBAL_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxdelaySolo(fxdelay) ( ePID_OLYmixer1_GLOBAL_FXDELAY_SOLO + GLOBAL_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_AuxSolo(aux) ( ePID_OLYmixer1_GLOBAL_AUX1_SOLO + GLOBAL_AUX__PARAMS*((aux)-1) )
#define GetParamID_FxsendSolo(fxsend) ( ePID_OLYmixer1_GLOBAL_FXSEND1_SOLO + GLOBAL_FXSEND__PARAMS*((fxsend)-1) )
#define GetParamID_SoloFader(solo) ( ePID_OLYmixer1_GLOBAL_SOLO_FADER + GLOBAL_SOLO__PARAMS*((solo)-1) )

#define GetParamID_InputSafe(input) ( ePID_OLYmixer1_SHOW_INPUT1_SAFE + SHOW_INPUT__PARAMS*((input)-1) )
#define GetParamID_FxreverbSafe(fxreverb) ( ePID_OLYmixer1_SHOW_FXREVERB_SAFE + SHOW_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxdelaySafe(fxdelay) ( ePID_OLYmixer1_SHOW_FXDELAY_SAFE + SHOW_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_MainoutSafe(mainout) ( ePID_OLYmixer1_SHOW_MAINOUT_SAFE + SHOW_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_AuxSafe(aux) ( ePID_OLYmixer1_SHOW_AUX1_SAFE + SHOW_AUX__PARAMS*((aux)-1) )

#define GetParamID_SnapshotCrc(snapshot) ( ePID_OLYmixer1_INFO_SNAPSHOT1_CRC + INFO_SNAPSHOT__PARAMS*((snapshot)-1) )
#define GetParamID_ImageCrc(image) ( ePID_OLYmixer1_INFO_IMAGE1_CRC + INFO_IMAGE__PARAMS*((image)-1) )

#define GetParamID_InputRouting(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_ROUTING + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputTrim(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_TRIM + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputThumb(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_THUMB + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputColor(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_COLOR + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputInvert(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_INVERT + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputMute(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_MUTE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputFader(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_FADER + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPan(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PAN + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputLrassign(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_LRASSIGN + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputStlink(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_STLINK + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputHpfEnable(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_HPF_ENABLE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputHpfFreq(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_HPF_FREQ + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputHpfSlope(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_HPF_SLOPE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputGateModel(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_GATE_MODEL + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputGateEnable(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_GATE_ENABLE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputGateThresh(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_GATE_THRESH + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputGateAttack(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_GATE_ATTACK + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputGateRelease(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_GATE_RELEASE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputGateRange(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_GATE_RANGE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputGateHold(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_GATE_HOLD + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputCompModel(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_COMP_MODEL + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputCompEnable(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_COMP_ENABLE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputCompGain(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_COMP_GAIN + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputCompThresh(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_COMP_THRESH + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputCompAttack(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_COMP_ATTACK + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputCompRelease(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_COMP_RELEASE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputCompRatio(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_COMP_RATIO + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputCompKnee(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_COMP_KNEE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqModel(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_MODEL + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqEnable(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_ENABLE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqHighType(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_HIGH_TYPE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqHighGain(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_HIGH_GAIN + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqHighFreq(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_HIGH_FREQ + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqHighQ(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_HIGH_Q + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqHimidType(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_HIMID_TYPE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqHimidGain(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_HIMID_GAIN + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqHimidFreq(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_HIMID_FREQ + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqHimidQ(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_HIMID_Q + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqLomidType(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_LOMID_TYPE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqLomidGain(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_LOMID_GAIN + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqLomidFreq(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_LOMID_FREQ + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqLomidQ(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_LOMID_Q + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqLowType(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_LOW_TYPE + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqLowGain(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_LOW_GAIN + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqLowFreq(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_LOW_FREQ + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputPeqLowQ(input) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_PEQ_LOW_Q + SNAPSHOT_INPUT__PARAMS*((input)-1) )
#define GetParamID_InputAuxsendFader(input,auxsend) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_AUXSEND1_FADER + SNAPSHOT_INPUT__PARAMS*((input)-1) + SNAPSHOT_INPUT_AUXSEND__PARAMS*((auxsend)-1) )
#define GetParamID_InputAuxsendMute(input,auxsend) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_AUXSEND1_MUTE + SNAPSHOT_INPUT__PARAMS*((input)-1) + SNAPSHOT_INPUT_AUXSEND__PARAMS*((auxsend)-1) )
#define GetParamID_InputAuxsendPan(input,auxsend) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_AUXSEND1_PAN + SNAPSHOT_INPUT__PARAMS*((input)-1) + SNAPSHOT_INPUT_AUXSEND__PARAMS*((auxsend)-1) )
#define GetParamID_InputFxsendFader(input,fxsend) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_FXSEND1_FADER + SNAPSHOT_INPUT__PARAMS*((input)-1) + SNAPSHOT_INPUT_FXSEND__PARAMS*((fxsend)-1) )
#define GetParamID_InputFxsendMute(input,fxsend) ( ePID_OLYmixer1_SNAPSHOT_INPUT1_FXSEND1_MUTE + SNAPSHOT_INPUT__PARAMS*((input)-1) + SNAPSHOT_INPUT_FXSEND__PARAMS*((fxsend)-1) )
#define GetParamID_FxsendThumb(fxsend) ( ePID_OLYmixer1_SNAPSHOT_FXSEND1_THUMB + SNAPSHOT_FXSEND__PARAMS*((fxsend)-1) )
#define GetParamID_FxsendColor(fxsend) ( ePID_OLYmixer1_SNAPSHOT_FXSEND1_COLOR + SNAPSHOT_FXSEND__PARAMS*((fxsend)-1) )
#define GetParamID_FxsendMute(fxsend) ( ePID_OLYmixer1_SNAPSHOT_FXSEND1_MUTE + SNAPSHOT_FXSEND__PARAMS*((fxsend)-1) )
#define GetParamID_FxsendFader(fxsend) ( ePID_OLYmixer1_SNAPSHOT_FXSEND1_FADER + SNAPSHOT_FXSEND__PARAMS*((fxsend)-1) )
#define GetParamID_FxsendSource(fxsend) ( ePID_OLYmixer1_SNAPSHOT_FXSEND1_SOURCE + SNAPSHOT_FXSEND__PARAMS*((fxsend)-1) )
#define GetParamID_FxsendUselrmute(fxsend) ( ePID_OLYmixer1_SNAPSHOT_FXSEND1_USELRMUTE + SNAPSHOT_FXSEND__PARAMS*((fxsend)-1) )
#define GetParamID_FxreverbReverbModel(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_REVERB_MODEL + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbReverbType(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_REVERB_TYPE + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbReverbPredelay(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_REVERB_PREDELAY + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbReverbDecay(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_REVERB_DECAY + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbReverbDiffusion(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_REVERB_DIFFUSION + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbReverbDamping(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_REVERB_DAMPING + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbReverbRolloff(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_REVERB_ROLLOFF + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbReverbRelease(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_REVERB_RELEASE + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbThumb(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_THUMB + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbColor(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_COLOR + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbMute(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_MUTE + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbFader(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_FADER + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbBal(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_BAL + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbLrassign(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_LRASSIGN + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqModel(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_MODEL + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqEnable(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_ENABLE + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqHighType(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_HIGH_TYPE + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqHighGain(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_HIGH_GAIN + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqHighFreq(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_HIGH_FREQ + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqHighQ(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_HIGH_Q + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqHimidType(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_HIMID_TYPE + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqHimidGain(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_HIMID_GAIN + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqHimidFreq(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_HIMID_FREQ + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqHimidQ(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_HIMID_Q + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqLomidType(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_LOMID_TYPE + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqLomidGain(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_LOMID_GAIN + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqLomidFreq(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_LOMID_FREQ + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqLomidQ(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_LOMID_Q + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqLowType(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_LOW_TYPE + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqLowGain(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_LOW_GAIN + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqLowFreq(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_LOW_FREQ + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbPeqLowQ(fxreverb) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_PEQ_LOW_Q + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) )
#define GetParamID_FxreverbAuxsendFader(fxreverb,auxsend) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_AUXSEND1_FADER + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) + SNAPSHOT_FXREVERB_AUXSEND__PARAMS*((auxsend)-1) )
#define GetParamID_FxreverbAuxsendMute(fxreverb,auxsend) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_AUXSEND1_MUTE + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) + SNAPSHOT_FXREVERB_AUXSEND__PARAMS*((auxsend)-1) )
#define GetParamID_FxreverbAuxsendBal(fxreverb,auxsend) ( ePID_OLYmixer1_SNAPSHOT_FXREVERB_AUXSEND1_BAL + SNAPSHOT_FXREVERB__PARAMS*((fxreverb)-1) + SNAPSHOT_FXREVERB_AUXSEND__PARAMS*((auxsend)-1) )
#define GetParamID_FxdelayDelayModel(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_DELAY_MODEL + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayDelayType(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_DELAY_TYPE + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayDelayDelay(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_DELAY_DELAY + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayDelayDelay2(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_DELAY_DELAY2 + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayDelayFeedback(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_DELAY_FEEDBACK + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayDelayDamping(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_DELAY_DAMPING + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayThumb(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_THUMB + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayColor(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_COLOR + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayMute(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_MUTE + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayFader(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_FADER + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayBal(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_BAL + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayLrassign(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_LRASSIGN + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqModel(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_MODEL + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqEnable(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_ENABLE + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqHighType(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_HIGH_TYPE + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqHighGain(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_HIGH_GAIN + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqHighFreq(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_HIGH_FREQ + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqHighQ(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_HIGH_Q + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqHimidType(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_HIMID_TYPE + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqHimidGain(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_HIMID_GAIN + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqHimidFreq(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_HIMID_FREQ + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqHimidQ(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_HIMID_Q + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqLomidType(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_LOMID_TYPE + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqLomidGain(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_LOMID_GAIN + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqLomidFreq(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_LOMID_FREQ + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqLomidQ(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_LOMID_Q + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqLowType(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_LOW_TYPE + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqLowGain(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_LOW_GAIN + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqLowFreq(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_LOW_FREQ + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayPeqLowQ(fxdelay) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_PEQ_LOW_Q + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) )
#define GetParamID_FxdelayAuxsendFader(fxdelay,auxsend) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_AUXSEND1_FADER + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) + SNAPSHOT_FXDELAY_AUXSEND__PARAMS*((auxsend)-1) )
#define GetParamID_FxdelayAuxsendMute(fxdelay,auxsend) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_AUXSEND1_MUTE + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) + SNAPSHOT_FXDELAY_AUXSEND__PARAMS*((auxsend)-1) )
#define GetParamID_FxdelayAuxsendBal(fxdelay,auxsend) ( ePID_OLYmixer1_SNAPSHOT_FXDELAY_AUXSEND1_BAL + SNAPSHOT_FXDELAY__PARAMS*((fxdelay)-1) + SNAPSHOT_FXDELAY_AUXSEND__PARAMS*((auxsend)-1) )
#define GetParamID_RecordbusRouting(recordbus) ( ePID_OLYmixer1_SNAPSHOT_RECORDBUS1_ROUTING + SNAPSHOT_RECORDBUS__PARAMS*((recordbus)-1) )
#define GetParamID_RecordbusFader(recordbus) ( ePID_OLYmixer1_SNAPSHOT_RECORDBUS1_FADER + SNAPSHOT_RECORDBUS__PARAMS*((recordbus)-1) )
#define GetParamID_MainoutFader(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_FADER + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutMute(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_MUTE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutBal(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_BAL + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutThumb(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_THUMB + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutColor(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_COLOR + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutCompModel(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_COMP_MODEL + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutCompEnable(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_COMP_ENABLE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutCompGain(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_COMP_GAIN + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutCompThresh(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_COMP_THRESH + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutCompAttack(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_COMP_ATTACK + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutCompRelease(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_COMP_RELEASE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutCompRatio(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_COMP_RATIO + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutCompKnee(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_COMP_KNEE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutGeqModel(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_GEQ_MODEL + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutGeqEnable(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_GEQ_ENABLE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutGeqGain(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_GEQ_GAIN + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutGeqBandGain(mainout,band) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_GEQ_BAND1_GAIN + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) + SNAPSHOT_MAINOUT_GEQ_BAND__PARAMS*((band)-1) )
#define GetParamID_MainoutHpfEnable(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_HPF_ENABLE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutHpfFreq(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_HPF_FREQ + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutHpfSlope(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_HPF_SLOPE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutHpfResponse(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_HPF_RESPONSE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutLpfEnable(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_LPF_ENABLE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutLpfFreq(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_LPF_FREQ + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutLpfSlope(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_LPF_SLOPE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutLpfResponse(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_LPF_RESPONSE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqModel(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_MODEL + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqEnable(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_ENABLE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqHighType(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_HIGH_TYPE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqHighGain(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_HIGH_GAIN + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqHighFreq(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_HIGH_FREQ + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqHighQ(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_HIGH_Q + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqHimidType(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_HIMID_TYPE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqHimidGain(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_HIMID_GAIN + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqHimidFreq(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_HIMID_FREQ + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqHimidQ(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_HIMID_Q + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqLomidType(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_LOMID_TYPE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqLomidGain(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_LOMID_GAIN + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqLomidFreq(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_LOMID_FREQ + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqLomidQ(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_LOMID_Q + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqLowType(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_LOW_TYPE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqLowGain(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_LOW_GAIN + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqLowFreq(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_LOW_FREQ + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutPeqLowQ(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_PEQ_LOW_Q + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutDelayEnable(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_DELAY_ENABLE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )
#define GetParamID_MainoutDelayDistance(mainout) ( ePID_OLYmixer1_SNAPSHOT_MAINOUT_DELAY_DISTANCE + SNAPSHOT_MAINOUT__PARAMS*((mainout)-1) )

#define GetMeterID_RtaBand(band) ( eMID_OLYmixer1_RTA_BAND1((band)-1) )
#define GetMeterID_InputIn(input) ( eMID_OLYmixer1_INPUT1_IN + INPUT__METERS*((input)-1) )
#define GetMeterID_InputGateIn(input) ( eMID_OLYmixer1_INPUT1_GATE_IN + INPUT__METERS*((input)-1) )
#define GetMeterID_InputGateGainreduce(input) ( eMID_OLYmixer1_INPUT1_GATE_GAINREDUCE + INPUT__METERS*((input)-1) )
#define GetMeterID_InputGateOut(input) ( eMID_OLYmixer1_INPUT1_GATE_OUT + INPUT__METERS*((input)-1) )
#define GetMeterID_InputCompIn(input) ( eMID_OLYmixer1_INPUT1_COMP_IN + INPUT__METERS*((input)-1) )
#define GetMeterID_InputCompGainreduce(input) ( eMID_OLYmixer1_INPUT1_COMP_GAINREDUCE + INPUT__METERS*((input)-1) )
#define GetMeterID_InputCompOut(input) ( eMID_OLYmixer1_INPUT1_COMP_OUT + INPUT__METERS*((input)-1) )
#define GetMeterID_FxsendSend(fxsend) ( eMID_OLYmixer1_FXSEND1_SEND + FXSEND__METERS*((fxsend)-1) )
#define GetMeterID_FxreverbPrefader_l(fxreverb) ( eMID_OLYmixer1_FXREVERB_PREFADER_L + FXREVERB__METERS*((fxreverb)-1) )
#define GetMeterID_FxreverbPrefader_r(fxreverb) ( eMID_OLYmixer1_FXREVERB_PREFADER_R + FXREVERB__METERS*((fxreverb)-1) )
#define GetMeterID_FxdelayPrefader_l(fxdelay) ( eMID_OLYmixer1_FXDELAY_PREFADER_L + FXDELAY__METERS*((fxdelay)-1) )
#define GetMeterID_FxdelayPrefader_r(fxdelay) ( eMID_OLYmixer1_FXDELAY_PREFADER_R + FXDELAY__METERS*((fxdelay)-1) )
#define GetMeterID_MainoutOut_l(mainout) ( eMID_OLYmixer1_MAINOUT_OUT_L + MAINOUT__METERS*((mainout)-1) )
#define GetMeterID_MainoutOut_r(mainout) ( eMID_OLYmixer1_MAINOUT_OUT_R + MAINOUT__METERS*((mainout)-1) )
#define GetMeterID_MainoutCompIn_l(mainout) ( eMID_OLYmixer1_MAINOUT_COMP_IN_L + MAINOUT__METERS*((mainout)-1) )
#define GetMeterID_MainoutCompIn_r(mainout) ( eMID_OLYmixer1_MAINOUT_COMP_IN_R + MAINOUT__METERS*((mainout)-1) )
#define GetMeterID_MainoutCompGainreduce_l(mainout) ( eMID_OLYmixer1_MAINOUT_COMP_GAINREDUCE_L + MAINOUT__METERS*((mainout)-1) )
#define GetMeterID_MainoutCompGainreduce_r(mainout) ( eMID_OLYmixer1_MAINOUT_COMP_GAINREDUCE_R + MAINOUT__METERS*((mainout)-1) )
#define GetMeterID_MainoutCompOut_l(mainout) ( eMID_OLYmixer1_MAINOUT_COMP_OUT_L + MAINOUT__METERS*((mainout)-1) )
#define GetMeterID_MainoutCompOut_r(mainout) ( eMID_OLYmixer1_MAINOUT_COMP_OUT_R + MAINOUT__METERS*((mainout)-1) )

#define GetLabelID_SnapshotName(snapshot) ( eLID_OLYmixer1_SHOW_SNAPSHOT1_NAME + SHOW_SNAPSHOT__LABELS*((snapshot)-1) )
#define GetLabelID_InputName(input) ( eLID_OLYmixer1_SNAPSHOT_INPUT1_NAME + SNAPSHOT_INPUT__LABELS*((input)-1) )
#define GetLabelID_FxsendName(fxsend) ( eLID_OLYmixer1_SNAPSHOT_FXSEND1_NAME + SNAPSHOT_FXSEND__LABELS*((fxsend)-1) )
#define GetLabelID_FxreverbName(fxreverb) ( eLID_OLYmixer1_SNAPSHOT_FXREVERB_NAME + SNAPSHOT_FXREVERB__LABELS*((fxreverb)-1) )
#define GetLabelID_FxdelayName(fxdelay) ( eLID_OLYmixer1_SNAPSHOT_FXDELAY_NAME + SNAPSHOT_FXDELAY__LABELS*((fxdelay)-1) )
#define GetLabelID_MainoutName(mainout) ( eLID_OLYmixer1_SNAPSHOT_MAINOUT_NAME + SNAPSHOT_MAINOUT__LABELS*((mainout)-1) )

#endif	// __OLYmixer1_LOOKUP_H__
