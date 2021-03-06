// DO NOT EDIT THIS FILE
// Created by xmlerator on Tuesday, 23 August 2016 at 04:12:29 PM.
#define OLYspeaker1_XML_VERSION 18



- (IntegerParameter *)inputRouting:(NSInteger)input;
- (BooleanParameter *)inputFuncgenpost:(NSInteger)input;
- (BooleanParameter *)inputFuncgenEnable:(NSInteger)input;
- (FloatParameter *)inputFuncgenFader:(NSInteger)input;
- (IntegerParameter *)inputFuncgenType:(NSInteger)input;
- (FloatParameter *)inputFuncgenFreq:(NSInteger)input;
- (FloatParameter *)speakerFader:(NSInteger)speaker;
- (BooleanParameter *)speakerMute:(NSInteger)speaker;
- (BooleanParameter *)speakerInvert:(NSInteger)speaker;
- (BooleanParameter *)speakerPeqEnable:(NSInteger)speaker;
- (IntegerParameter *)speakerPeqBandType:(NSInteger)speaker band:(NSInteger)band;
- (FloatParameter *)speakerPeqBandGain:(NSInteger)speaker band:(NSInteger)band;
- (FloatParameter *)speakerPeqBandFreq:(NSInteger)speaker band:(NSInteger)band;
- (FloatParameter *)speakerPeqBandQ:(NSInteger)speaker band:(NSInteger)band;
- (BooleanParameter *)speakerDelayEnable:(NSInteger)speaker;
- (FloatParameter *)speakerDelayTime:(NSInteger)speaker;
- (FloatParameter *)groupFader:(NSInteger)group;
- (BooleanParameter *)groupMute:(NSInteger)group;
- (BooleanParameter *)groupPeqEnable:(NSInteger)group;
- (IntegerParameter *)groupPeqBandType:(NSInteger)group band:(NSInteger)band;
- (FloatParameter *)groupPeqBandGain:(NSInteger)group band:(NSInteger)band;
- (FloatParameter *)groupPeqBandFreq:(NSInteger)group band:(NSInteger)band;
- (FloatParameter *)groupPeqBandQ:(NSInteger)group band:(NSInteger)band;
- (BooleanParameter *)groupDelayEnable:(NSInteger)group;
- (FloatParameter *)groupDelayTime:(NSInteger)group;
- (BooleanParameter *)groupSoftlimitEnable:(NSInteger)group;
- (FloatParameter *)groupSoftlimitGain:(NSInteger)group;
- (FloatParameter *)groupSoftlimitThresh:(NSInteger)group;
- (FloatParameter *)groupSoftlimitAttack:(NSInteger)group;
- (FloatParameter *)groupSoftlimitRelease:(NSInteger)group;
- (FloatParameter *)groupSoftlimitRatio:(NSInteger)group;
- (IntegerParameter *)groupSoftlimitKnee:(NSInteger)group;

- (BooleanParameter *)profilePeqEnable:(NSInteger)profile;
- (IntegerParameter *)profilePeqBandType:(NSInteger)profile band:(NSInteger)band;
- (FloatParameter *)profilePeqBandGain:(NSInteger)profile band:(NSInteger)band;
- (FloatParameter *)profilePeqBandFreq:(NSInteger)profile band:(NSInteger)band;
- (FloatParameter *)profilePeqBandQ:(NSInteger)profile band:(NSInteger)band;

- (BooleanParameter *)throwPeqEnable:(NSInteger)throw;
- (IntegerParameter *)throwPeqBandType:(NSInteger)throw band:(NSInteger)band;
- (FloatParameter *)throwPeqBandGain:(NSInteger)throw band:(NSInteger)band;
- (FloatParameter *)throwPeqBandFreq:(NSInteger)throw band:(NSInteger)band;
- (FloatParameter *)throwPeqBandQ:(NSInteger)throw band:(NSInteger)band;

- (BooleanParameter *)globalPeqEnable:(NSInteger)global;
- (IntegerParameter *)globalPeqBandType:(NSInteger)global band:(NSInteger)band;
- (FloatParameter *)globalPeqBandGain:(NSInteger)global band:(NSInteger)band;
- (FloatParameter *)globalPeqBandFreq:(NSInteger)global band:(NSInteger)band;
- (FloatParameter *)globalPeqBandQ:(NSInteger)global band:(NSInteger)band;
- (FloatParameter *)hfFuncgenleveloffset:(NSInteger)hf;
- (FloatParameter *)hfFader:(NSInteger)hf;
- (BooleanParameter *)hfMute:(NSInteger)hf;
- (BooleanParameter *)hfInvert:(NSInteger)hf;
- (FloatParameter *)hfFader_post:(NSInteger)hf;
- (BooleanParameter *)hfMute_post:(NSInteger)hf;
- (BooleanParameter *)hfPeqEnable:(NSInteger)hf;
- (IntegerParameter *)hfPeqBandType:(NSInteger)hf band:(NSInteger)band;
- (FloatParameter *)hfPeqBandGain:(NSInteger)hf band:(NSInteger)band;
- (FloatParameter *)hfPeqBandFreq:(NSInteger)hf band:(NSInteger)band;
- (FloatParameter *)hfPeqBandQ:(NSInteger)hf band:(NSInteger)band;
- (BooleanParameter *)hfSoftlimitEnable:(NSInteger)hf;
- (FloatParameter *)hfSoftlimitGain:(NSInteger)hf;
- (FloatParameter *)hfSoftlimitThresh:(NSInteger)hf;
- (FloatParameter *)hfSoftlimitAttack:(NSInteger)hf;
- (FloatParameter *)hfSoftlimitRelease:(NSInteger)hf;
- (FloatParameter *)hfSoftlimitRatio:(NSInteger)hf;
- (IntegerParameter *)hfSoftlimitKnee:(NSInteger)hf;
- (BooleanParameter *)hfKlimiterPeakenable:(NSInteger)hf;
- (FloatParameter *)hfKlimiterPeakattack:(NSInteger)hf;
- (FloatParameter *)hfKlimiterGainrelease:(NSInteger)hf;
- (FloatParameter *)hfKlimiterLookahead:(NSInteger)hf;
- (FloatParameter *)hfKlimiterMaxpeak:(NSInteger)hf;
- (BooleanParameter *)hfKlimiterClipenable:(NSInteger)hf;
- (FloatParameter *)hfKlimiterClipattack:(NSInteger)hf;
- (FloatParameter *)hfKlimiterCliprelease:(NSInteger)hf;
- (FloatParameter *)hfKlimiterClipthreshold:(NSInteger)hf;
- (BooleanParameter *)hfPeq_postEnable:(NSInteger)hf;
- (IntegerParameter *)hfPeq_postBandType:(NSInteger)hf band:(NSInteger)band;
- (FloatParameter *)hfPeq_postBandGain:(NSInteger)hf band:(NSInteger)band;
- (FloatParameter *)hfPeq_postBandFreq:(NSInteger)hf band:(NSInteger)band;
- (FloatParameter *)hfPeq_postBandQ:(NSInteger)hf band:(NSInteger)band;
- (BooleanParameter *)hfDyneqDetecten:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (IntegerParameter *)hfDyneqDetecttype:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqDetectgain:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqDetectfreq:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqDetectq:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)hfDyneqAgcenable:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqAgcattack:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqAgcdecay:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)hfDyneqAgcactive:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqAgcthresh:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqAgcratio:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)hfDyneqResumen:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (IntegerParameter *)hfDyneqResumtype:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqResumgain:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqResumfreq:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (FloatParameter *)hfDyneqResumq:(NSInteger)hf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)hfDelayEnable:(NSInteger)hf;
- (FloatParameter *)hfDelayTime:(NSInteger)hf;
- (FloatParameter *)mfFuncgenleveloffset:(NSInteger)mf;
- (FloatParameter *)mfFader:(NSInteger)mf;
- (BooleanParameter *)mfMute:(NSInteger)mf;
- (BooleanParameter *)mfInvert:(NSInteger)mf;
- (FloatParameter *)mfFader_post:(NSInteger)mf;
- (BooleanParameter *)mfMute_post:(NSInteger)mf;
- (BooleanParameter *)mfPeqEnable:(NSInteger)mf;
- (IntegerParameter *)mfPeqBandType:(NSInteger)mf band:(NSInteger)band;
- (FloatParameter *)mfPeqBandGain:(NSInteger)mf band:(NSInteger)band;
- (FloatParameter *)mfPeqBandFreq:(NSInteger)mf band:(NSInteger)band;
- (FloatParameter *)mfPeqBandQ:(NSInteger)mf band:(NSInteger)band;
- (BooleanParameter *)mfSoftlimitEnable:(NSInteger)mf;
- (FloatParameter *)mfSoftlimitGain:(NSInteger)mf;
- (FloatParameter *)mfSoftlimitThresh:(NSInteger)mf;
- (FloatParameter *)mfSoftlimitAttack:(NSInteger)mf;
- (FloatParameter *)mfSoftlimitRelease:(NSInteger)mf;
- (FloatParameter *)mfSoftlimitRatio:(NSInteger)mf;
- (IntegerParameter *)mfSoftlimitKnee:(NSInteger)mf;
- (BooleanParameter *)mfKlimiterPeakenable:(NSInteger)mf;
- (FloatParameter *)mfKlimiterPeakattack:(NSInteger)mf;
- (FloatParameter *)mfKlimiterGainrelease:(NSInteger)mf;
- (FloatParameter *)mfKlimiterLookahead:(NSInteger)mf;
- (FloatParameter *)mfKlimiterMaxpeak:(NSInteger)mf;
- (BooleanParameter *)mfKlimiterClipenable:(NSInteger)mf;
- (FloatParameter *)mfKlimiterClipattack:(NSInteger)mf;
- (FloatParameter *)mfKlimiterCliprelease:(NSInteger)mf;
- (FloatParameter *)mfKlimiterClipthreshold:(NSInteger)mf;
- (BooleanParameter *)mfPeq_postEnable:(NSInteger)mf;
- (IntegerParameter *)mfPeq_postBandType:(NSInteger)mf band:(NSInteger)band;
- (FloatParameter *)mfPeq_postBandGain:(NSInteger)mf band:(NSInteger)band;
- (FloatParameter *)mfPeq_postBandFreq:(NSInteger)mf band:(NSInteger)band;
- (FloatParameter *)mfPeq_postBandQ:(NSInteger)mf band:(NSInteger)band;
- (BooleanParameter *)mfDyneqDetecten:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (IntegerParameter *)mfDyneqDetecttype:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqDetectgain:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqDetectfreq:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqDetectq:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)mfDyneqAgcenable:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqAgcattack:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqAgcdecay:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)mfDyneqAgcactive:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqAgcthresh:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqAgcratio:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)mfDyneqResumen:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (IntegerParameter *)mfDyneqResumtype:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqResumgain:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqResumfreq:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (FloatParameter *)mfDyneqResumq:(NSInteger)mf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)mfDelayEnable:(NSInteger)mf;
- (FloatParameter *)mfDelayTime:(NSInteger)mf;
- (FloatParameter *)lfFuncgenleveloffset:(NSInteger)lf;
- (FloatParameter *)lfFader:(NSInteger)lf;
- (BooleanParameter *)lfMute:(NSInteger)lf;
- (BooleanParameter *)lfInvert:(NSInteger)lf;
- (FloatParameter *)lfFader_post:(NSInteger)lf;
- (BooleanParameter *)lfMute_post:(NSInteger)lf;
- (BooleanParameter *)lfPeqEnable:(NSInteger)lf;
- (IntegerParameter *)lfPeqBandType:(NSInteger)lf band:(NSInteger)band;
- (FloatParameter *)lfPeqBandGain:(NSInteger)lf band:(NSInteger)band;
- (FloatParameter *)lfPeqBandFreq:(NSInteger)lf band:(NSInteger)band;
- (FloatParameter *)lfPeqBandQ:(NSInteger)lf band:(NSInteger)band;
- (BooleanParameter *)lfSoftlimitEnable:(NSInteger)lf;
- (FloatParameter *)lfSoftlimitGain:(NSInteger)lf;
- (FloatParameter *)lfSoftlimitThresh:(NSInteger)lf;
- (FloatParameter *)lfSoftlimitAttack:(NSInteger)lf;
- (FloatParameter *)lfSoftlimitRelease:(NSInteger)lf;
- (FloatParameter *)lfSoftlimitRatio:(NSInteger)lf;
- (IntegerParameter *)lfSoftlimitKnee:(NSInteger)lf;
- (BooleanParameter *)lfKlimiterPeakenable:(NSInteger)lf;
- (FloatParameter *)lfKlimiterPeakattack:(NSInteger)lf;
- (FloatParameter *)lfKlimiterGainrelease:(NSInteger)lf;
- (FloatParameter *)lfKlimiterLookahead:(NSInteger)lf;
- (FloatParameter *)lfKlimiterMaxpeak:(NSInteger)lf;
- (BooleanParameter *)lfKlimiterClipenable:(NSInteger)lf;
- (FloatParameter *)lfKlimiterClipattack:(NSInteger)lf;
- (FloatParameter *)lfKlimiterCliprelease:(NSInteger)lf;
- (FloatParameter *)lfKlimiterClipthreshold:(NSInteger)lf;
- (BooleanParameter *)lfPeq_postEnable:(NSInteger)lf;
- (IntegerParameter *)lfPeq_postBandType:(NSInteger)lf band:(NSInteger)band;
- (FloatParameter *)lfPeq_postBandGain:(NSInteger)lf band:(NSInteger)band;
- (FloatParameter *)lfPeq_postBandFreq:(NSInteger)lf band:(NSInteger)band;
- (FloatParameter *)lfPeq_postBandQ:(NSInteger)lf band:(NSInteger)band;
- (BooleanParameter *)lfDyneqDetecten:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (IntegerParameter *)lfDyneqDetecttype:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqDetectgain:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqDetectfreq:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqDetectq:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)lfDyneqAgcenable:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqAgcattack:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqAgcdecay:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)lfDyneqAgcactive:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqAgcthresh:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqAgcratio:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)lfDyneqResumen:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (IntegerParameter *)lfDyneqResumtype:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqResumgain:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqResumfreq:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (FloatParameter *)lfDyneqResumq:(NSInteger)lf dyneq:(NSInteger)dyneq;
- (BooleanParameter *)lfDelayEnable:(NSInteger)lf;
- (FloatParameter *)lfDelayTime:(NSInteger)lf;
- (IntegerParameter *)ampRouting:(NSInteger)amp;

- (BooleanParameter *)hfFirEnable:(NSInteger)hf;
- (BooleanParameter *)hfFirMute:(NSInteger)hf;
- (FloatParameter *)hfFirCoefValue:(NSInteger)hf coef:(NSInteger)coef;
- (BooleanParameter *)mfFirEnable:(NSInteger)mf;
- (BooleanParameter *)mfFirMute:(NSInteger)mf;
- (FloatParameter *)mfFirCoefValue:(NSInteger)mf coef:(NSInteger)coef;
- (BooleanParameter *)lfFirEnable:(NSInteger)lf;
- (BooleanParameter *)lfFirMute:(NSInteger)lf;
- (FloatParameter *)lfFirCoefValue:(NSInteger)lf coef:(NSInteger)coef;

- (IntegerParameter *)speakerXoverType:(NSInteger)speaker;
- (BooleanParameter *)speakerXoverEnable:(NSInteger)speaker;
- (FloatParameter *)speakerXoverFreq:(NSInteger)speaker;
- (IntegerParameter *)speakerXoverSlope:(NSInteger)speaker;
- (IntegerParameter *)speakerXoverResponse:(NSInteger)speaker;
- (BooleanParameter *)speakerXoverHpfoutenable:(NSInteger)speaker;
- (BooleanParameter *)speakerXoverOverride_fir_disable:(NSInteger)speaker;


- (BooleanParameter *)inputGateEnable:(NSInteger)input;
- (FloatParameter *)inputGateThresh:(NSInteger)input;
- (FloatParameter *)inputGateAttack:(NSInteger)input;
- (FloatParameter *)inputGateRelease:(NSInteger)input;
- (FloatParameter *)inputGateRange:(NSInteger)input;
- (FloatParameter *)inputGateHold:(NSInteger)input;

- (MeterParameter *)inputMeterIn:(NSInteger)input;
- (MeterParameter *)speakerMeterIn:(NSInteger)speaker;
- (MeterParameter *)groupMeterSoftlimitIn:(NSInteger)group;
- (MeterParameter *)groupMeterSoftlimitGainreduce:(NSInteger)group;
- (MeterParameter *)groupMeterSoftlimitOut:(NSInteger)group;
- (MeterParameter *)hfMeterOut:(NSInteger)hf;
- (MeterParameter *)hfMeterSoftlimitIn:(NSInteger)hf;
- (MeterParameter *)hfMeterSoftlimitGainreduce:(NSInteger)hf;
- (MeterParameter *)hfMeterSoftlimitOut:(NSInteger)hf;
- (MeterParameter *)hfMeterKlimiterIn:(NSInteger)hf;
- (MeterParameter *)hfMeterKlimiterGainreduce:(NSInteger)hf;
- (MeterParameter *)hfMeterKlimiterOut:(NSInteger)hf;
- (MeterParameter *)mfMeterOut:(NSInteger)mf;
- (MeterParameter *)mfMeterSoftlimitIn:(NSInteger)mf;
- (MeterParameter *)mfMeterSoftlimitGainreduce:(NSInteger)mf;
- (MeterParameter *)mfMeterSoftlimitOut:(NSInteger)mf;
- (MeterParameter *)mfMeterKlimiterIn:(NSInteger)mf;
- (MeterParameter *)mfMeterKlimiterGainreduce:(NSInteger)mf;
- (MeterParameter *)mfMeterKlimiterOut:(NSInteger)mf;
- (MeterParameter *)lfMeterOut:(NSInteger)lf;
- (MeterParameter *)lfMeterSoftlimitIn:(NSInteger)lf;
- (MeterParameter *)lfMeterSoftlimitGainreduce:(NSInteger)lf;
- (MeterParameter *)lfMeterSoftlimitOut:(NSInteger)lf;
- (MeterParameter *)lfMeterKlimiterIn:(NSInteger)lf;
- (MeterParameter *)lfMeterKlimiterGainreduce:(NSInteger)lf;
- (MeterParameter *)lfMeterKlimiterOut:(NSInteger)lf;
- (MeterParameter *)hf_dyneqMeterGain:(NSInteger)hf_dyneq;
- (MeterParameter *)mf_dyneqMeterGain:(NSInteger)mf_dyneq;
- (MeterParameter *)lf_dyneqMeterGain:(NSInteger)lf_dyneq;
- (MeterParameter *)inputMeterGateIn:(NSInteger)input;
- (MeterParameter *)inputMeterGateGainreduce:(NSInteger)input;
- (MeterParameter *)inputMeterGateOut:(NSInteger)input;


