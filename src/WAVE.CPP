//******************************************************************
// wave.cpp
// created 10.18.96
// written by Patrick Wyatt
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include "storm.h"
#include "sound.h"
#include "engine.h"


//******************************************************************
// externs
//******************************************************************
extern HSARCHIVE ghsMainArchive;
void FileErrorDlg(const char * pszName);
// pjw.patch1.start.1/13/97
BOOL InsertCDDlg();
// pjw.patch1.end.1/13/97


//******************************************************************
//******************************************************************
static void reinsert_cd(HSFILE hsFile,DWORD * pdwTryCount,const char * pszFile) {
	HSARCHIVE hsArchive;	

	// fatal if we've tried to many times
	if (*pdwTryCount >= 5) FileErrorDlg(pszFile);

	if (hsFile && SFileGetFileArchive(hsFile,&hsArchive) && (hsArchive != ghsMainArchive)) {
		// wait for transient disk error to disappear
		Sleep(20);
		*pdwTryCount += 1;
	}
	else {
		// pjw.patch1.start.1/13/97
		if (! InsertCDDlg()) FileErrorDlg(pszFile);
		// pjw.patch1.end.1/13/97
	}
}



//******************************************************************
//******************************************************************
void patSFileCloseFile(HSFILE handle) {
	SFileCloseFile(handle);
}


//******************************************************************
//******************************************************************
DWORD patSFileGetFileSize(HSFILE handle,LPDWORD filesizehigh) {
	DWORD dwSize;
	DWORD dwTryCount = 0;
	while (0 == (dwSize = SFileGetFileSize(handle,filesizehigh)))
		reinsert_cd(handle,&dwTryCount,NULL);
	return dwSize;
}


//******************************************************************
//******************************************************************
BOOL patSFileOpenFile(LPCTSTR filename,HSFILE *handle,BOOL bCanFail) {
	DWORD dwTryCount = 0;
	while (1) {
		if (SFileOpenFile(filename,handle)) return TRUE;
		if (bCanFail && GetLastError() == SFILE_ERROR_FILE_NOT_FOUND)
			return FALSE;
		reinsert_cd(NULL,&dwTryCount,filename);
	}
}


//******************************************************************
//******************************************************************
// pjw.patch1.start.1/13/97
void patSFileReadFile(HSFILE handle,LPVOID buffer,DWORD bytestoread) {

/*  --- old code ---
	BOOL bResult;
	DWORD dwTryCount = 0;
	while (bytestoread) {
		DWORD dwBytes;
		bResult = SFileReadFile(handle,buffer,bytestoread,&dwBytes,NULL);
		if (bResult) return;

		// hopefully we got some bytes
		buffer = (LPVOID) ((LPBYTE) buffer + dwBytes);
		bytestoread -= dwBytes;

		reinsert_cd(handle,&dwTryCount,NULL);
	}
*/
	DWORD dwBytes;
	DWORD dwTryCount = 0;
	DWORD dwPos = patSFileSetFilePointer(handle,0,NULL,FILE_CURRENT);
	while (! SFileReadFile(handle,buffer,bytestoread,&dwBytes,NULL)) {
		reinsert_cd(handle,&dwTryCount,NULL);
		patSFileSetFilePointer(handle,dwPos,NULL,FILE_BEGIN);
	}
	app_assert(bytestoread == dwBytes);
}
// pjw.patch1.end.1/13/97


//******************************************************************
//******************************************************************
DWORD patSFileSetFilePointer(HSFILE handle,LONG distancetomove,PLONG distancetomovehigh,DWORD movemethod) {
	DWORD dwTryCount = 0;
	while (1) {
		DWORD dwResult = SFileSetFilePointer(handle,distancetomove,distancetomovehigh,movemethod);
		if (dwResult != 0xffffffff)
			return dwResult;
		reinsert_cd(handle,&dwTryCount,NULL);
	}
}


//******************************************************************
//******************************************************************
typedef struct TMemFile {
	DWORD dwFileSize;		// total file size
	DWORD dwFileOffset;		// offset in file
	DWORD dwBufSize;		// buffer size
	DWORD dwBufPos;			// position in buffer
	DWORD dwBufLeft;		// bytes left in buffer
	LPBYTE lpBuf;			// buffer
	HSFILE hsFile;			// file handle
} TMemFile;
#define MIN_BUF_SIZE	4096


//******************************************************************
//******************************************************************
static void MemFileFillBuf(TMemFile * pMemFile) {
	app_assert(pMemFile);

	// move to current file position
	patSFileSetFilePointer(
		pMemFile->hsFile,
		pMemFile->dwFileOffset,
		0,
		FILE_BEGIN
	);

	// read min(buffer size,file bytes left)
	DWORD dwRead = pMemFile->dwFileSize - pMemFile->dwFileOffset;
	dwRead = min(pMemFile->dwBufSize,dwRead);
	if (dwRead) patSFileReadFile(pMemFile->hsFile,pMemFile->lpBuf,dwRead);

	// update buffer status
	pMemFile->dwBufLeft = dwRead;
	pMemFile->dwBufPos = 0;
}


//******************************************************************
//******************************************************************
static void MemFileLoad(HSFILE hsFile,TMemFile * pMemFile,DWORD dwBufSize = 0xffffffff) {
	// validate parameters
	app_assert(hsFile);
	app_assert(pMemFile);
	ZeroMemory(pMemFile,sizeof(*pMemFile));

	// create a buffer for file data no larger than total file size
	pMemFile->dwFileSize = patSFileGetFileSize(hsFile);
	pMemFile->dwBufSize = max(dwBufSize,MIN_BUF_SIZE);
	pMemFile->dwBufSize = min(pMemFile->dwBufSize,pMemFile->dwFileSize);
	pMemFile->lpBuf = DiabloAllocPtrSig(pMemFile->dwBufSize,'SNDt');
	pMemFile->hsFile = hsFile;
}


//******************************************************************
//******************************************************************
static void MemFileFree(TMemFile * pMemFile) {
	app_assert(pMemFile);
	DiabloFreePtr(pMemFile->lpBuf);
}


//******************************************************************
//******************************************************************
static BOOL MemFileRead(TMemFile * pMemFile,LPVOID lpBuf,DWORD dwBytes) {
	app_assert(pMemFile);
	app_assert(lpBuf);

	while (dwBytes) {
		// re-fill buffer if required
		if (! pMemFile->dwBufLeft)
			MemFileFillBuf(pMemFile);

		// copy bytes from buffer
		DWORD dwRead = min(dwBytes,pMemFile->dwBufLeft);
		if (! dwRead) return FALSE;
		CopyMemory(lpBuf,pMemFile->lpBuf + pMemFile->dwBufPos,dwRead);

		// update file
		pMemFile->dwFileOffset += dwRead;
		pMemFile->dwBufPos += dwRead;
		pMemFile->dwBufLeft -= dwRead;

		// update bytes to read
		dwBytes -= dwRead;
	}

	return TRUE;
}


//******************************************************************
//******************************************************************
static DWORD MemFileSeek(TMemFile * pMemFile,LONG lDist,DWORD dwMethod) {
	// @@ fix this stuff later if these routines get re-used
	app_assert(dwMethod == FILE_CURRENT);
	app_assert(lDist >= 0);

	if ((DWORD) lDist < pMemFile->dwBufLeft) {
		pMemFile->dwBufLeft -= lDist;
		pMemFile->dwBufPos += lDist;
	}
	else {
		pMemFile->dwBufLeft = 0;
	}

	pMemFile->dwFileOffset += lDist;
	return pMemFile->dwFileOffset;
}


//******************************************************************
//******************************************************************
static BOOL find_chunk(TMemFile * pMemFile,FOURCC ckID,CKINFO * pck) {
	struct {
		FOURCC ckID;
		DWORD dwSize;
	} ckHdr;

	while (1) {

		// read chunk header
		if (! MemFileRead(pMemFile,&ckHdr,sizeof(ckHdr)))
			return FALSE;

		// does it match?
		if (ckHdr.ckID == ckID)
			break;

		// skip over this chunk
		if (0xffffFFFF == MemFileSeek(pMemFile,ckHdr.dwSize,FILE_CURRENT))
			return FALSE;
	}

	// get chunk size and position
	pck->dwSize = ckHdr.dwSize;
	pck->dwOffset = MemFileSeek(pMemFile,0,FILE_CURRENT);
	return (pck->dwOffset != 0xffffFFFF);
}


//******************************************************************
//******************************************************************
static BOOL read_header(TMemFile * pMemFile,WAVEFORMATEX * pwfx,CKINFO * pWaveInfo) {
	app_assert(pMemFile);
	app_assert(pwfx);
	CKINFO fmtInfo;

	// read "RIFF" DWORD=len "WAVE"
	MMCKINFO mmck;
	if (! MemFileRead(pMemFile,&mmck,sizeof(FOURCC)*2 + sizeof(DWORD)))
		return FALSE;

    if ((mmck.ckid != FOURCC_RIFF) || (mmck.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
		return FALSE;

    // Search the input file for for the 'fmt ' chunk
	if (! find_chunk(pMemFile,mmioFOURCC('f', 'm', 't', ' '),&fmtInfo))
		return FALSE;

    // Expect 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>
    if (fmtInfo.dwSize < sizeof(PCMWAVEFORMAT))
		return FALSE;

    // Read the 'fmt ' chunk into <pcm>
	PCMWAVEFORMAT pcm;
	if (! MemFileRead(pMemFile,&pcm,sizeof(pcm)))
		return FALSE;

	// seek over any additional junk in the 'fmt ' chunk
	if (0xffffFFFF == MemFileSeek(pMemFile,fmtInfo.dwSize - sizeof(pcm),FILE_CURRENT))
		return FALSE;
	
    // copy the bytes from the pcm structure to the waveformatex structure
	pwfx->wFormatTag = pcm.wf.wFormatTag;
	pwfx->nChannels = pcm.wf.nChannels;
	pwfx->nSamplesPerSec = pcm.wf.nSamplesPerSec;
	pwfx->nAvgBytesPerSec = pcm.wf.nAvgBytesPerSec;
	pwfx->nBlockAlign = pcm.wf.nBlockAlign;
	pwfx->wBitsPerSample = pcm.wBitsPerSample;
	pwfx->cbSize = 0;

	// do we need to find the body info?
	if (! pWaveInfo) return TRUE;
	return find_chunk(pMemFile,mmioFOURCC('d', 'a', 't', 'a'),pWaveInfo);
}


//******************************************************************
//******************************************************************
BOOL wave_read_header(HSFILE hsFile,WAVEFORMATEX * pwfx) {
	TMemFile memFile;
	MemFileLoad(hsFile,&memFile,0);
	BOOL bResult = read_header(&memFile,pwfx,NULL);
	MemFileFree(&memFile);
	return bResult;
}


//******************************************************************
//******************************************************************
LPBYTE wave_load_file(HSFILE hsFile,WAVEFORMATEX * pwfx,CKINFO * pWaveInfo) {
	TMemFile memFile;
	MemFileLoad(hsFile,&memFile);
	if (! read_header(&memFile,pwfx,pWaveInfo)) {
		MemFileFree(&memFile);
		return NULL;
	}

	return memFile.lpBuf;
}


//******************************************************************
//******************************************************************
void wave_free_file(LPBYTE lpWave) {
	DiabloFreePtr(lpWave);
}
