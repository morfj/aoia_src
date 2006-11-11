#include <windows.h>
#include <malloc.h>
#include <string>

#include "AODB.h"
#include "ctree.h"


/* Structures */
typedef unsigned __int64 UINT64;


/* Variables */
char strAODBpath[MAX_PATH]; 
IFIL udtAODBfile;
IIDX udtAODBindex;
ISEG udtAODBseg[2];


/* Initialise AO Database */
long InitAODatabase(const char *strAOFolder)
{
	COUNT iErrVal;

	/* Build full path/file */
	std::string AoDbPath(strAOFolder);
	AoDbPath += "\\cd_image\\data\\db\\ResourceDatabase";

	/* Link the Ctree DLL */
	if (!CTreeStd_LinkDll("ctreestd.dll"))
	{
		iErrVal = AODB_ERR_NODLL;
		goto InitAODatabase_Err_nodll;
	}
	
	/* Initialise Ctree */
	if (InitISAM(256,4,4))
	{ 
		iErrVal = AODB_ERR_NOINIT;
		goto InitAODatabase_Err_noinit;
	}

	/* Build file details */
	udtAODBfile.pfilnam = const_cast<char*>(AoDbPath.c_str());
	udtAODBfile.dfilno = -1;
	udtAODBfile.dreclen = 8;
	udtAODBfile.dxtdsiz = 4096;
	udtAODBfile.dfilmod = 0xC7;
	udtAODBfile.dnumidx = 1;
	udtAODBfile.ixtdsiz = 4096;
	udtAODBfile.ifilmod = 0x43;
	udtAODBfile.ix = &udtAODBindex;
	udtAODBfile.rfstfld = "Type";
	udtAODBfile.rlstfld = "Blob";
	udtAODBfile.tfilno = 0;
	udtAODBindex.ikeylen = 8;
	udtAODBindex.ikeytyp = 0;
	udtAODBindex.ikeydup = 0;
	udtAODBindex.inulkey = 0;
	udtAODBindex.iempchr = 0;
	udtAODBindex.inumseg = 2;
	udtAODBindex.seg = udtAODBseg;
	udtAODBindex.ridxnam = "ItemIdx";
	udtAODBindex.aidxnam = 0;
	udtAODBindex.altseq = 0;
	udtAODBindex.pvbyte = 0;
	udtAODBseg[0].soffset = 0;
	udtAODBseg[0].slength = 4;
	udtAODBseg[0].segmode = 1;
	udtAODBseg[1].soffset = 4;
	udtAODBseg[1].slength = 4;
	udtAODBseg[1].segmode = 1;

	/* Open the AO database */
	if (OpenIFile(&udtAODBfile))
	{
		iErrVal = AODB_ERR_NODB;
		goto InitAODatabase_Err_nodb;
	}

	return AODB_ERR_NOERROR;

InitAODatabase_Err_nodb:
	CloseISAM();
InitAODatabase_Err_noinit:
	CTreeStd_UnlinkDll();
InitAODatabase_Err_nodll:
	return iErrVal;
}

/* Decodes data - only required for items */
/*void DecodeDataChunk(BYTE *a_xData, unsigned long lDataLen)
{
	unsigned long lLoop;
	VRLEN lLength;
	UINT64 llSeed;
	BYTE *a_xDataPtr;

	a_xDataPtr = a_xData;
	lLength = lDataLen;
	llSeed = *((int *)(a_xData + 4));
	a_xDataPtr = a_xDataPtr + 0x18;
	for (lLoop = 0x18; lLoop < lLength; lLoop++) {
		llSeed = llSeed * (UINT64)(0x1012003);
		llSeed = llSeed % (UINT64)(0x4e512dc8f);
		*a_xDataPtr = *a_xDataPtr ^ (BYTE)(llSeed);
		a_xDataPtr++;
	}
}*/

/* Release the AO Database */
void ReleaseAODatabase(void)
{
	CloseIFile(&udtAODBfile);
	CloseISAM();
	CTreeStd_UnlinkDll();
}
