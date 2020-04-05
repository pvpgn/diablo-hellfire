/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/ENGINE.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
void DrawCel (long, long, BYTE *, long, long);
void DrawCelP (BYTE *, BYTE *, long, long);
void DrawSlabCel (long, long, BYTE *, long, long, long, long);
void DrawSlabCelP (BYTE *, BYTE *, long, long, long, long);

void DrawCelL (long, long, BYTE *, long, long);
void DrawCelPL (BYTE *, BYTE *, long, long);
void DrawSlabCelL (long, long, BYTE *, long, long, long, long);
void DrawSlabCelPL (BYTE *, BYTE *, long, long, long, long);
void TDrawSlabCelPL (BYTE *, BYTE *, long, long, long, long);

void CDrawSlabCel (long, long, BYTE *, long, long, long, long);
void CDrawSlabCelP (BYTE *, BYTE *, long, long, long, long);
void CDrawSlabCelL (long, long, BYTE *, long, long, long, long);
void CDrawSlabCelPL (BYTE *, BYTE *, long, long, long, long);
void TCDrawSlabCelPL (BYTE *, BYTE *, long, long, long, long);

void DrawSlabCelI(long, long, BYTE *, long, long, long, long, char);
void CDrawSlabCelI(long, long, BYTE *, long, long, long, long, char);

void OutlineSlabCel(byte, long, long, BYTE *, long, long, long, long);
void COutlineSlabCel(byte, long, long, BYTE *, long, long, long, long);

void DrawBuffCel(BYTE *, long, long, long, BYTE *, long, long);

void DecodeFullCel (BYTE *, BYTE *, long, long);
void DecodeFullCelL (BYTE *, BYTE *, long, long);

void CDecodeFullCel (BYTE *, BYTE *, long, long);
void CDecodeFullCelL (BYTE *, BYTE *, long, long);

void TranslateCels(byte *, byte *, int);

int GetDirection(int, int, int, int);

long GetRndSeed();
void SetRndSeed(long);
long random(byte, long);

void DrawLine(int, int, int, int, byte);
void DrawPoint(int, int, byte);

void PlayInGameMovie(const char * pszMovie);

//******************************************************************
// memory management
//******************************************************************
#define DEBUG_MEM	1	// 0 in final
#ifdef NDEBUG
#undef DEBUG_MEM
#define DEBUG_MEM 0
#endif

// public memory management functions
BYTE * DiabloAllocPtr(DWORD dwBytes);
BYTE * DiabloAllocPtrSig(DWORD dwBytes,DWORD dwSig);

// NOTE: DiabloFreePtr behavior
//
// p MUST be an l-value
//
// if p is NULL:
//    no memory is freed
// if p is NOT NULL:
//    p is set to NULL before call to free, and then memory is freed
// GUARANTEES p == NULL immediately after DiabloFreePtr is called!
void DiabloFreePtr(void * p);


BYTE * LoadFileInMem(const char * pszName, DWORD * pdwFileLen);
BYTE * LoadFileInMemSig(const char * pszName, DWORD * pdwFileLen,DWORD dwSig);
DWORD LoadFileWithMem(const char * pszName, BYTE * pbMem);
void mem_cleanup(BOOL bNormalExit);


#if DEBUG_MEM

// internal functions
void mem_use_sig(DWORD dwSig,DWORD dwBytes);
void mem_unuse_sig(DWORD dwSig,DWORD dwBytes);
BYTE * mem_malloc_dbg(DWORD dwBytes,DWORD dwSig,DWORD dwLine,const TCHAR * pszFile);
void mem_free_dbg(void * p,DWORD dwLine,const TCHAR * pszFile);
BYTE * load_file_dbg(const char * pszName,DWORD * pdwFileLen,DWORD dwSig,DWORD dwLine,const TCHAR * pszFile);

// definitions of public functions
#define DiabloAllocPtr(dwBytes) mem_malloc_dbg(dwBytes,'NONE',__LINE__,__FILE__)
#define DiabloAllocPtrSig(dwBytes,dwSig) mem_malloc_dbg(dwBytes,dwSig,__LINE__,__FILE__)
#define LoadFileInMem(pszName,pdwFileLen) load_file_dbg(pszName,pdwFileLen,'NONE',__LINE__,__FILE__)
#define LoadFileInMemSig(pszName,pdwFileLen,dwSig) load_file_dbg(pszName,pdwFileLen,dwSig,__LINE__,__FILE__)

// free function -- set pointer to NULL before real call to free
#define DiabloFreePtr(p) {					\
	void * p__p = (void *) (p);				\
	(p) = NULL;								\
	mem_free_dbg(p__p,__LINE__,__FILE__);	\
}


#else

// internal functions
void mem_free_dbg(void * p);

// definitions of public functions
#define DiabloAllocPtrSig(dwBytes,dwSig) DiabloAllocPtr(dwBytes)
#define LoadFileInMemSig(pszName,pdwFileLen,dwSig) LoadFileInMem(pszName,pdwFileLen)

// free function -- set pointer to NULL before real call to free
#define DiabloFreePtr(p) {					\
	void * p__p = (void *) (p);				\
	(p) = NULL;								\
	mem_free_dbg(p__p);						\
}


#endif


//******************************************************************
// file manager
//******************************************************************
#ifdef _STORM_H_
void patSFileCloseFile(HSFILE handle);
DWORD patSFileGetFileSize(HSFILE handle,LPDWORD filesizehigh = NULL);
BOOL patSFileOpenFile(LPCTSTR filename,HSFILE *handle,BOOL bCanFail = FALSE);
void patSFileReadFile(HSFILE handle,LPVOID buffer,DWORD bytestoread);
DWORD patSFileSetFilePointer(HSFILE handle,LONG distancetomove,PLONG distancetomovehigh,DWORD movemethod);
#endif
