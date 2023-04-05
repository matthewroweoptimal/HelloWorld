#include "Product_arch.h"
#include "LOUD_enums.h"

//#include "Hal_types.h"

#include "ProductMap.h"


tParameterDesc** pProductParameterTables[ePRODUCT_FENCE] = {
	gOLYspeaker1ParameterTables, // ePRODUCT_OLYSPEAKER
	gREDLINEspeaker1ParameterTables, // ePRODUCT_REDLINESPEAKER1

};


int* nProductParameterTablesMax[ePRODUCT_FENCE] = {
	nOLYspeaker1ParameterTablesMax, // ePRODUCT_OLYSPEAKER
	nREDLINEspeaker1ParameterTablesMax, // ePRODUCT_REDLINESPEAKER1
	//	nOTTOParameterTablesMax, // ePRODUCT_OTTO
//	nANNAParameterTablesMax, // ePRODUCT_ANNA

};

int nProductXMLVersion[ePRODUCT_FENCE] = {
	OLYspeaker1_XML_VERSION,	// ePRODUCT_ANYA
	REDLINEspeaker1_XML_VERSION,	// ePRODUCT_REDLINESPEAKER1
//	OTTO_XML_VERSION,	// ePRODUCT_OTTO
//	ANNA_XML_VERSION,	// ePRODUCT_ANNA
};


int GetParameterDescTable(int nProduct, int nTarget, tParameterDesc** ppParameterDesc, int* pTableLength, int* nXMLVersion)
{
	*ppParameterDesc = 0;
	*pTableLength = 0;
	*nXMLVersion = 0;
	if (nProduct >= ePRODUCT_FENCE) return 0;
	if (nTarget >= eTARGET_FENCE) return 0;
	*ppParameterDesc = (pProductParameterTables[nProduct])[nTarget];
	*pTableLength = (nProductParameterTablesMax[nProduct])[nTarget];
	*nXMLVersion = nProductXMLVersion[nProduct];
	return *pTableLength;

}



tParameterDesc* GetParameterDesc(int nProductType, int nTarget, int nParamNum)
{
	tParameterDesc* pParameterDesc;
	int nTableLength;
	int nXMLVersion;

	GetParameterDescTable(nProductType,nTarget,&pParameterDesc,&nTableLength,&nXMLVersion);

	if (!pParameterDesc) return 0;

	if (nParamNum < 0)
	{
		return 0;
	}

	if (nParamNum >= nTableLength) return 0;

	return &pParameterDesc[nParamNum];


}