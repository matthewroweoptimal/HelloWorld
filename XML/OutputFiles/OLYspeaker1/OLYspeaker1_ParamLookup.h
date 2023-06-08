// DO NOT EDIT THIS FILE
// Created by xmlerator on Tuesday, 23 August 2016 at 04:12:29 PM.
#define OLYspeaker1_XML_VERSION 18

#ifndef __OLYspeaker1_PARAMLOOKUP_H__
#define __OLYspeaker1_PARAMLOOKUP_H__


extern "C" {
#include "OLYspeaker1_map.h"
}
#include "OLYspeaker1_arch.h"
#include "OLYspeaker1_pids.h"

typedef struct {
	int				eTarget;
	int				eParam;
	char*			pcParam;
	char*			pcOldParam;
} tLookupEntry;

extern tLookupEntry ParameterLookupTable[];
extern const int nLookupTableItems;

extern tParameterDesc* pParameterTables[eTARGET_FENCE];

#endif	// __OLYspeaker1_PARAMLOOKUP_H__
