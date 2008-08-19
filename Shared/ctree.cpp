#include "stdafx.h"

#define _CTREE_CPP
#include "ctree.h"

// Handle of the Ctree DLL.  NULL until it is loaded.
static HINSTANCE hDll = NULL;

#define DYNLINK(a,b) \
	if(!(fnDll = GetProcAddress(hDll, (b)))) goto exit_error;\
	memcpy(&(a), &fnDll, sizeof(void *));	

// Perform dynamic linking of ctreestd.dll functions
int CTreeStd_LinkDll(std::tstring const& DllPath)
{
	FARPROC fnDll;

	if (hDll) {
		// Already linked. Signal success.
		return 1;
	}

	// Load the dll file into memory
	hDll = LoadLibrary(DllPath.c_str());
	if (hDll == NULL) {
		// Unable to load library file. Signal failure.
		return 0;
	}
		
	// Look up functions in the dll and set their pointers
	// On failure, jump to exit_error.
	DYNLINK(InitISAM, "INTISAM");
	DYNLINK(CloseISAM, "CLISAM");
	DYNLINK(OpenIFile, "OPNIFIL");
	DYNLINK(CloseIFile, "CLIFIL");
	DYNLINK(GetRecord, "EQLREC");
	DYNLINK(GetVRecord, "EQLVREC");
	DYNLINK(VRecordLength, "GETVLEN");
	DYNLINK(ReReadVRecord, "REDVREC");

	DYNLINK(FirstKey, "FRSKEY");
	DYNLINK(NextKey, "NXTKEY");
	DYNLINK(GetGTEKey, "GTEKEY");
	DYNLINK(ReWriteVRecord, "RWTVREC");
	DYNLINK(NextVRecord, "NXTVREC" );
	DYNLINK(GetGTEVRecord, "GTEVREC" );
 
    DYNLINK(NbrOfKeyEntries, "IDXENT");
    DYNLINK(NbrOfKeysInRange, "RNGENT");

	return 1; // No Failures.  Signal success.

exit_error:
	// Function name not found in the dynamic link table by 
	// GetProcAddress.  Unload the dll and signal failure.
	FreeLibrary(hDll);
	hDll = NULL;
	return 0;
}


// Unlink ctreestd.dll functions and unload the dll
void CTreeStd_UnlinkDll() {
	if (!hDll) {
		// It isn't linked yet, nothing to do.
		return;
	}

	// Set all function pointers back to NULL so that
	// if the user tries to use any of these no longer
	// linked functions he'll get an exception error.
	InitISAM = NULL;
	CloseISAM = NULL;
	OpenIFile = NULL;
	CloseIFile = NULL;
	GetRecord = NULL;
	GetVRecord = NULL;
	VRecordLength = NULL;
	ReReadVRecord = NULL;

	// Unload the dll library from memory
	FreeLibrary(hDll);
	hDll = NULL;
}
