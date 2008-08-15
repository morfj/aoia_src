#ifndef _CTREE_INC
#define _CTREE_INC

#include <shared/UnicodeSupport.h>

typedef char TEXT, *pTEXT;
typedef unsigned char UTEXT, *pUTEXT;
typedef short COUNT, *pCOUNT;
typedef unsigned short UCOUNT, *pUCOUNT;
typedef long LONG, *pLONG;
typedef unsigned long ULONG, *pULONG;
typedef void *pVOID;
typedef ULONG VRLEN, *pVRLEN;

typedef struct iseg {
	COUNT soffset; /* segment position (offset) */ 
	COUNT slength; /* segment length */ 
	COUNT segmode; /* segment mode */ 
} ISEG, *pISEG;

typedef struct iidx {
	COUNT ikeylen; /* key length */ 
	COUNT ikeytyp; /* key type */ 
	COUNT ikeydup; /* duplicate flag */ 
	COUNT inulkey; /* NULL key flag */ 
	COUNT iempchr; /* empty character */ 
	COUNT inumseg; /* number of segments */ 
	pISEG seg; /* segment information */ 
	pTEXT ridxnam; /* r-tree symbolic name */ 
	pTEXT aidxnam; /* optional index file name */ 
	pCOUNT altseq; /* optional alternate sequence */ 
	pUTEXT pvbyte; /* optional pointer to pad byte */ 
} IIDX, *pIIDX;

typedef struct ifil {
	pTEXT pfilnam; /* file name (w/o ext) */ 
	COUNT dfilno; /* data file number */ 
	UCOUNT dreclen; /* data record length */ 
	UCOUNT dxtdsiz; /* data file ext size */ 
	COUNT dfilmod; /* data file mode */ 
	COUNT dnumidx; /* number of indices */ 
	UCOUNT ixtdsiz; /* index file ext size */ 
	COUNT ifilmod; /* index file mode */ 
	pIIDX ix; /* index information */ 
	pTEXT rfstfld; /* r-tree 1st fld name */ 
	pTEXT rlstfld; /* r-tree last fld name */ 
	COUNT tfilno; /* temporary file number */ 
} IFIL, *pIFIL;

#ifdef _CTREE_CPP
#define DECLARE
#else
#define DECLARE extern
#endif

DECLARE COUNT (__cdecl * InitISAM)(COUNT bufs, COUNT fils, COUNT sect);
DECLARE COUNT (__cdecl * CloseISAM)(); 
DECLARE COUNT (__cdecl * OpenIFile)(pIFIL ifilptr);
DECLARE COUNT (__cdecl * CloseIFile)(pIFIL ifilptr);
DECLARE COUNT (__cdecl * GetRecord)(COUNT keyno, pVOID target, pVOID recptr);
DECLARE COUNT (__cdecl * GetVRecord)(COUNT keyno, pVOID target, pVOID recptr, pVRLEN plen);
DECLARE COUNT (__cdecl * VRecordLength)(COUNT datno);
DECLARE COUNT (__cdecl * ReReadVRecord)(COUNT datno, pVOID recptr, VRLEN bufsiz);

DECLARE COUNT (__cdecl * FirstKey)(COUNT keyno, pVOID idxval);
DECLARE LONG (__cdecl * NextKey)(COUNT keyno, pVOID idxval);
DECLARE COUNT (__cdecl * GetGTEKey)(COUNT keyno, pVOID target, pVOID idxval);
DECLARE COUNT (__cdecl * NextVRecord)(short keyno, void* recptr, unsigned int* plen);
DECLARE COUNT (__cdecl * GetGTEVRecord)(short keyno, void* Target, void* recptr, unsigned int* plen);

DECLARE COUNT (__cdecl * ReWriteVRecord)(COUNT datno, pVOID recptr, VRLEN bufsiz);


DECLARE int CTreeStd_LinkDll(std::tstring const& DllPath);
DECLARE void CTreeStd_UnlinkDll();

#endif /* _CTREE_INC */
