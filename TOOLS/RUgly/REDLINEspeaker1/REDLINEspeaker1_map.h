// DO NOT EDIT THIS FILE
// Created by xmlerator on Tuesday, 02 June 2015 at 02:15:41 PM.
#define REDLINEspeaker1_XML_VERSION 7

#ifndef __REDLINEspeaker1_MAP_H__
#define __REDLINEspeaker1_MAP_H__

//#include "REDLINEspeaker1_arch.h"
#include "ProductMap.h"
#include "REDLINEspeaker1_pids.h"
///*
//#ifdef WIN32							
//#define SHOWNAMES 1					
//#else									
//#define SHOWNAMES 0					
//#endif	*/							

//typedef struct {						
//#if(SHOWNAMES)						
//	char             pName[50];			
//#endif								
//	int      typeEnum;			
//	unsigned short   typeInstance;		
//	int    effectEnum;		
//	unsigned short   effectInstance;	
//	int qualEnum;			
//	unsigned short   qualInstance;		
//	int paramEnum;			
//	unsigned int     linkedChannelPID;
//	unsigned int     linkedEffectPID;
//	unsigned short   format;			
//	unsigned int     minValue;			
//	unsigned int     maxValue;			
//	unsigned int     defValue;			
//} tParameterDesc;		

typedef struct {						
#if(SHOWNAMES)						
	char             pName[50];			
#endif								
	int      typeEnum;			
	unsigned short   typeInstance;		
	int    effectEnum;		
	unsigned short   effectInstance;	
	int qualEnum;			
	unsigned short   qualInstance;		
	int paramEnum;			
	unsigned int     linkedChannelPID;
	unsigned int     linkedEffectPID;
	unsigned short   format;			
	char             defString[63];		
} REDLINEspeaker1_label_descriptor;		

extern tParameterDesc gREDLINEspeaker1GlobalParameterTable[];
#define REDLINEspeaker1_GLOBAL_PARAMETER_MAX  ePID_REDLINEspeaker1_GLOBAL_FENCE

extern tParameterDesc gREDLINEspeaker1SnapshotParameterTable[];
#define REDLINEspeaker1_SNAPSHOT_PARAMETER_MAX  ePID_REDLINEspeaker1_SNAPSHOT_FENCE

extern tParameterDesc gREDLINEspeaker1FirParameterTable[];
#define REDLINEspeaker1_FIR_PARAMETER_MAX  ePID_REDLINEspeaker1_FIR_FENCE

extern REDLINEspeaker1_label_descriptor gREDLINEspeaker1GlobalLabelParameterTable[];
#define REDLINEspeaker1_GLOBAL_LABEL_PARAMETER_MAX  eLID_REDLINEspeaker1_GLOBAL_FENCE

extern REDLINEspeaker1_label_descriptor gREDLINEspeaker1SnapshotLabelParameterTable[];
#define REDLINEspeaker1_SNAPSHOT_LABEL_PARAMETER_MAX  eLID_REDLINEspeaker1_SNAPSHOT_FENCE



extern tParameterDesc* gREDLINEspeaker1ParameterTables[];
extern int nREDLINEspeaker1ParameterTablesMax[];

#endif	// __REDLINEspeaker1_MAP_H__
