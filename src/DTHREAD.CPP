//******************************************************************
// dthread.cpp
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include <process.h>
#include "msg.h"
#include "multi.h"
#include "gendung.h"
#include "sound.h"
#include "storm.h"
#include "items.h"
#include "player.h"
#include "engine.h"


//******************************************************************
// extern
//******************************************************************
extern DWORD gdwDeltaBytesSec;
void SendPlayerInfoChunk(int pnum,BYTE bCmd,const BYTE * pbSrc,DWORD dwLen);


//******************************************************************
// private
//******************************************************************
typedef struct TInfo {
	struct TInfo * pNext;
	int pnum;
	BYTE bCmd;
	DWORD dwLen;
	BYTE bData[1];
} TInfo;

// linked list and critical section to protect it
static TInfo * sgpInfoHead;
static CCritSect sgInfoCrit;

// thread and synchronization objects
static BYTE sgbRunThread = FALSE;
static HANDLE sghThread = INVALID_HANDLE_VALUE;
static HANDLE sghWorkToDoEvent = NULL;
static unsigned sgThreadID;


//******************************************************************
//******************************************************************
static unsigned __stdcall dthread_proc(void *) {
	TInfo * pInfo;

	while (sgbRunThread) {
		// sleep until there is work to do
		if (! sgpInfoHead && WAIT_FAILED == WaitForSingleObject(sghWorkToDoEvent,INFINITE))
			app_fatal(TEXT("dthread4:\n%s"),strGetLastError());

		// pull one item off the list
		sgInfoCrit.Enter();
		pInfo = sgpInfoHead;
		if (sgpInfoHead) sgpInfoHead = sgpInfoHead->pNext;
		else ResetEvent(sghWorkToDoEvent);
		sgInfoCrit.Leave();
		if (! pInfo) continue;

		// send the item if it still has a valid destination
		if (pInfo->pnum != MAX_PLRS) {
			SendPlayerInfoChunk(
				pInfo->pnum,
				pInfo->bCmd,
				&pInfo->bData[0],
				pInfo->dwLen
			);
		}

		// (bytes * 1000 ms/sec) / (bytes/sec) = ms sleep time
		app_assert(gdwDeltaBytesSec);
		DWORD dwSleepTime = pInfo->dwLen * 1000 / gdwDeltaBytesSec;
		dwSleepTime = min(dwSleepTime,1);

		// free item
		DiabloFreePtr(pInfo);

		// wait for output queue to empty before sending again
		if (dwSleepTime) Sleep(dwSleepTime);

		// pjw.patch2.start
		#if CHEATS
		static DWORD sdwSkips = 0;
		sdwSkips++;
		HDC hDC;
		HRESULT ddr = lpDDSPrimary->GetDC(&hDC);
		if (ddr == DD_OK) {
			char szBuf[16];
			wsprintf(szBuf,"d:%u",sdwSkips);
			TextOut(hDC,5,460,szBuf,strlen(szBuf));
			lpDDSPrimary->ReleaseDC(hDC);
		}
		#endif
		// pjw.patch2.end

	}

	return 0;
}


//******************************************************************
//******************************************************************
void dthread_remove_player(int pnum) {
	sgInfoCrit.Enter();
	for (TInfo * pInfo = sgpInfoHead; pInfo; pInfo = pInfo->pNext)
		if (pInfo->pnum == pnum) pInfo->pnum = MAX_PLRS;
	sgInfoCrit.Leave();
}


//******************************************************************
//******************************************************************
void dthread_SendPlayerInfoChunk(int pnum,BYTE bCmd,const BYTE * pbSrc,DWORD dwLen) {
// pjw.patch2.start
//	app_assert((DWORD) pnum < MAX_PLRS);
	if (gbMaxPlayers == 1) return;
// pjw.patch2.end
	app_assert(pnum != myplr);
	app_assert(pbSrc);
	app_assert(dwLen);

	// create a block of memory for the player info
	TInfo * pInfo = (TInfo *) DiabloAllocPtrSig(sizeof(TInfo) + dwLen,'DLTA');
	pInfo->pNext = NULL;
	pInfo->pnum = pnum;
	pInfo->bCmd = bCmd;
	pInfo->dwLen = dwLen;
	CopyMemory(pInfo->bData,pbSrc,dwLen);

	// link to tail of list
	sgInfoCrit.Enter();
	TInfo ** ppInfo = &sgpInfoHead;
	while (*ppInfo) ppInfo = &(*ppInfo)->pNext;
	*ppInfo = pInfo;

	// wake up thread if necessary -- inside crit section
	SetEvent(sghWorkToDoEvent);

	sgInfoCrit.Leave();
}


//******************************************************************
//******************************************************************
void dthread_init() {
	// we don't have to send delta info in single
	// player mode, so don't bother initializing thread
	app_assert(sghThread == INVALID_HANDLE_VALUE);
	if (gbMaxPlayers == 1) return;

	// make sure linked list is properly initialized
	app_assert(! sgpInfoHead);

	// create synchronization object for thread
	app_assert(! sghWorkToDoEvent);
	if (NULL == (sghWorkToDoEvent = CreateEvent(
		NULL,	// security info
		TRUE,	// manual reset
		FALSE,	// initial state
		NULL	// name
	))) app_fatal("dthread:1\n%s",strGetLastError());

	// create worker thread
	sgbRunThread = TRUE;
	app_assert(sghThread == INVALID_HANDLE_VALUE);
	if (INVALID_HANDLE_VALUE == (sghThread = (HANDLE) _beginthreadex(
		NULL,				// no security info
		0,					// stack size
		dthread_proc,		// start address
		NULL,				// argument list
		0,					// initial state
		&sgThreadID			// sgThreadID
	)))	app_fatal(TEXT("dthread2:\n%s"),strGetLastError());
}


//******************************************************************
//******************************************************************
void dthread_free() {
	// if the event was never initialized, the thread cannot be running
	if (! sghWorkToDoEvent) return;

	// kill off the loader thread
	sgbRunThread = FALSE;
	SetEvent(sghWorkToDoEvent);

	// wait til the thread is done
	if (sghThread != INVALID_HANDLE_VALUE) {
		if (sgThreadID != GetCurrentThreadId()) {
			if (WAIT_FAILED == WaitForSingleObject(sghThread,INFINITE))
				app_fatal("dthread3:\n(%s)",strGetLastError());
			CloseHandle(sghThread);
			sghThread = INVALID_HANDLE_VALUE;
		}
	}

	// clean up event
	CloseHandle(sghWorkToDoEvent);
	sghWorkToDoEvent = NULL;

	// clean up linked list
	while (sgpInfoHead) {
		TInfo * pNext = sgpInfoHead->pNext;
		DiabloFreePtr(sgpInfoHead);
		sgpInfoHead = pNext;
	}
}
