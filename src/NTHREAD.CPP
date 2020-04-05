//******************************************************************
// nthread.cpp
// contains network code which runs in either main thread
// or auxiliary "progress" thread
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include <process.h>
#include "storm.h"
#include "msg.h"
#include "multi.h"
#include "gendung.h"
#include "sound.h"
#include "storm.h"
#include "items.h"
#include "player.h"


//******************************************************************
// debugging
//******************************************************************
#define PRINT_STATUS	1	// 0 in final
#define STATUS_LASTERR	0	// 0 in final
#define DUMP_STATUS		0	// 0 in final

#ifdef NDEBUG
#undef PRINT_STATUS
#undef STATUS_LASTERR
#undef DUMP_STATUS
#define PRINT_STATUS 0
#define STATUS_LASTERR 0
#define DUMP_STATUS 0
#endif


//******************************************************************
// public
//******************************************************************
BYTE gbGameLoopsPerPacket;
DWORD gdwTurnsInTransit;
DWORD gdwNormalMsgSize;		// MIN_MSG_SIZE..MAX_MSG_SIZE
DWORD gdwLargestMsgSize;	// MIN_MSG_SIZE..MAX_MSG_SIZE
DWORD gdwDeltaBytesSec;


// results from receiving the last synchronous turn
DWORD gdwMsgLenTbl[MAX_PLRS];
DWORD gdwMsgStatTbl[MAX_PLRS];
LPVOID glpMsgTbl[MAX_PLRS];

#ifndef NDEBUG
DWORD gdwAsyncRecvTbl[MAX_PLRS];
DWORD gdwAsyncSendTbl[MAX_PLRS];
#endif


//******************************************************************
// private -- packets
//******************************************************************
static DWORD sgdwRequestDeltaFlag;

static BYTE sgbRunThread;
static BYTE sgbThreadIsRunning;
static CCritSect sgCrit;
static HANDLE sghThread = INVALID_HANDLE_VALUE;
static unsigned sgThreadID;

// are we waiting for a packet?
static BYTE sgbGotPacketOK;

// countdown timer to the next time we need to send an
// async packet and value to initialize countdown timer
static BYTE sgbPacketCountdown;

// countdown timer for synchronous message
static BYTE sgbSyncCountdown;

// timer which marches forward to match the real time clock
// used to determine when to run game loops
static long sglGameClock = 0;

// if game clock and the real clock differ by more than CATCH_UP_DELTA
// then reset the game clock to be equal to the real clock.  This ensures
// that the game will attempt to maintain a normal frame rate, but if it
// falls too far outside normal bounds, it will not struggle indefinitely.
#define CATCH_UP_DELTA	500 // milliseconds


#if PRINT_STATUS
static BYTE sgbPrintStatus;
static DWORD sgdwPrintTime;
#if STATUS_LASTERR
static DWORD sgdwPrintTbl[MAX_PLRS];
#endif
#endif

#if DUMP_STATUS
static BYTE sgbDumpStatus;
static BYTE sgbDumpStatusOK;
static BYTE sbDumpInit = TRUE;
#endif

#if ALLOW_TRACE_FCN
static BYTE sgbTraceFcn;
static const char * sgpszLastTrace;
#endif


//******************************************************************
//******************************************************************
#if ALLOW_TRACE_FCN
void trace_fcn(const char * pszFcn) {
	// don't write stuff if we're not the active application
	if (! bActive) return;
	if (! sgbTraceFcn) return;
	if (! lpDDSPrimary) return;
	if (sgpszLastTrace == pszFcn) return;
	sgpszLastTrace = pszFcn;

	// ooh -- how cheesy...
	if (! pszFcn) pszFcn = "                                             ";

	HDC hDC;
	HRESULT ddr = lpDDSPrimary->GetDC(&hDC);
	if (ddr != DD_OK) return;
	TextOut(hDC,5,370,pszFcn,strlen(pszFcn));
	lpDDSPrimary->ReleaseDC(hDC);
}
#endif


//******************************************************************
//******************************************************************
void nthread_check_snet_error(const char * pszFcn) {
	app_assert(pszFcn);
	DWORD dwErr = GetLastError();
	// we don't keep track of players, so we may have sent a
	// message to somebody who isn't there anymore.
	if (dwErr == SNET_ERROR_INVALID_PLAYER)
		NULL;
	else if (dwErr == SNET_ERROR_GAME_TERMINATED)
		gbGameDestroyed = TRUE;
	else if (dwErr == SNET_ERROR_NOT_IN_GAME)
		gbGameDestroyed = TRUE;
	else
		app_fatal("%s:\n%s",pszFcn,strGetLastError());
}


//******************************************************************
//******************************************************************
#ifdef _DEBUG
BOOL is_pat_debug_cmd(WPARAM wKey) {
	// only a debug key if the control key is down
	if (! (GetKeyState(VK_CONTROL) & 0x8000))
		return FALSE;

	switch (wKey) {
	
		case 0x03:	// ctrl - break;
			// debugger
			if (!fullscreen) {
				void myDebugBreak();
				myDebugBreak();
				force_redraw = FULLDRAW;
			}
		break;

		case 0x13:	// ctrl - S
			// stall the network by sleeping
			Sleep(50);
		break;

		#if PRINT_STATUS && STATUS_LASTERR
		case 0x12:	// ctrl - R
			ZeroMemory(sgdwPrintTbl,sizeof(sgdwPrintTbl));
		break;
		#endif

		#if PRINT_STATUS
		case 0x10:	// ctrl - P
			// toggle print status
			sgbPrintStatus = !sgbPrintStatus;
			sgdwPrintTime = GetTickCount();
		break;
		#endif

		#if DUMP_STATUS
		case 0x4:	// ctrl - D
			sgbDumpStatus = !sgbDumpStatus;
			sgbDumpStatusOK = TRUE;
			sbDumpInit = TRUE;

			#if PRINT_STATUS
			sgbPrintStatus = sgbDumpStatus;
			#endif
		break;
		#endif

		#if ALLOW_TRACE_FCN
		case 0xa:	// ctrl - J
			sgbTraceFcn = !sgbTraceFcn;
			sgpszLastTrace = NULL;
		break;
		#endif

		default:
		return FALSE;
	}

	return TRUE;
}
#endif


//******************************************************************
//******************************************************************
DWORD nthread_fill_sync_queue(DWORD dwCounter,DWORD dwIncrement) {
	// MAKE SURE THERE ARE ALWAYS SEVERAL SYNC TURNS IN TRANSIT
	DWORD turns;

	TRACE_FCN("SNetGetTurnsInTransit");
	if (! SNetGetTurnsInTransit(&turns)) {
		TRACE_FCN(NULL);
		nthread_check_snet_error("SNetGetTurnsInTransit");
		return 0;
	}
	TRACE_FCN(NULL);

	app_assert(gdwTurnsInTransit);
	while (turns++ < gdwTurnsInTransit) {

		// counter = low 31 bits
		// hi bit = request delta flag or zero
		// reset flag after first use
		DWORD dwTemp = dwCounter & TURN_COUNTER_MASK;
		dwTemp |= sgdwRequestDeltaFlag;
		sgdwRequestDeltaFlag = 0;

		// send turn
		TRACE_FCN("SNetSendTurn");
		if (! SNetSendTurn(&dwTemp,sizeof dwTemp)) {
			TRACE_FCN(NULL);
			nthread_check_snet_error("SNetSendTurn");
			return 0;
		}
		TRACE_FCN(NULL);

		// increment counter and reset wraparound
		dwCounter += dwIncrement;
		if (dwCounter >= TURN_COUNTER_MASK)
			dwCounter &= TURN_COUNTER_RESET_MASK;
	}

	return dwCounter;
}


//******************************************************************
//******************************************************************
#if DUMP_STATUS
void __cdecl dump_string(const char * pszFmt,...) {
	app_assert(pszFmt);

	// open dumpfile
	FILE * f = fopen("c:\\netdump.txt",sbDumpInit ? "wb" : "ab");
	if (! f) return;
	sbDumpInit = FALSE;

	va_list args;
	va_start(args,pszFmt);
	vfprintf(f,pszFmt,args);
	va_end(args);

	fclose(f);
}
#endif


//******************************************************************
//******************************************************************
#if DUMP_STATUS
static void DumpStatus() {
	// don't write stuff if we're not the active application
	if (! bActive) return;

	for (int i = 0; i < MAX_PLRS; i++) {
		if (! (gdwMsgStatTbl[i] & SNET_PSF_RESPONDING))
			break;
	}

	if (i >= MAX_PLRS) {
		if (sgbDumpStatusOK) return;
		sgbDumpStatusOK = TRUE;
	}
	else {
		sgbDumpStatusOK = FALSE;
	}

	// create status string
	char szBuf[64];
	char * pszBuf = szBuf;
	for (i = 0; i < MAX_PLRS; i++) {
		if (i == myplr) *pszBuf++ = '>';
		*pszBuf++ = (char) (gdwMsgStatTbl[i] >> 16) + '0';
		if (plr[i].plractive) *pszBuf++ = '*';
		*pszBuf++ = ' ';
	}
	pszBuf--;
	*pszBuf++ = '\r';
	*pszBuf++ = '\n';
	*pszBuf = 0;
	dump_string(szBuf);
}
#endif


//******************************************************************
//******************************************************************
#if PRINT_STATUS
static void print_status() {

	// don't write stuff if we're not the active application
	if (! bActive) return;

	// create status string
	char szBuf[128];
	char * pszBuf = szBuf;
	for (int i = 0; i < MAX_PLRS; i++) {
		*pszBuf++ = (i == myplr) ? '<' : ' ';
		#if STATUS_LASTERR
		*pszBuf++ = (char) (sgdwPrintTbl[i] >> 16) + '0';
		#else
		*pszBuf++ = (char) (gdwMsgStatTbl[i] >> 16) + '0';
		#endif
		*pszBuf++ = plr[i].plractive ? 'A' : 'I';
		*pszBuf++ = (i == myplr) ? '>' : ' ';
	}
	*pszBuf++ = ' ';
	*pszBuf = 0;

	if (! lpDDSPrimary) return;
	HDC hDC;
	HRESULT ddr = lpDDSPrimary->GetDC(&hDC);
	if (ddr != DD_OK) return;
	COLORREF oldTextColor = SetTextColor(hDC,RGB(0xff,0xff,0));
	COLORREF oldBkColor = SetBkColor(hDC,RGB(0,0,0));
	int oldBkMode = SetBkMode(hDC,OPAQUE);
	TextOut(hDC,5,385,szBuf,strlen(szBuf));

	#ifndef NDEBUG
	sprintf(szBuf,"%02x %02x %02x %02x",
		gdwAsyncRecvTbl[0] & 0xff,
		gdwAsyncRecvTbl[1] & 0xff,
		gdwAsyncRecvTbl[2] & 0xff,
		gdwAsyncRecvTbl[3] & 0xff
	);
	TextOut(hDC,5,400,szBuf,strlen(szBuf));
	sprintf(szBuf,"%02x %02x %02x %02x",
		gdwAsyncSendTbl[0] & 0xff,
		gdwAsyncSendTbl[1] & 0xff,
		gdwAsyncSendTbl[2] & 0xff,
		gdwAsyncSendTbl[3] & 0xff
	);
	TextOut(hDC,5,415,szBuf,strlen(szBuf));
	#endif

	static DWORD foo = 0;
	sprintf(szBuf,"%d",foo++);
	TextOut(hDC,5,430,szBuf,strlen(szBuf));
/*
	DWORD dwCurrTime = GetTickCount();
	if (dwCurrTime - sgdwPrintTime >= 1000) {
		sgdwPrintTime = dwCurrTime;

		DWORD dwTurn,dwBytes;
		SNetGetPerformanceData(SNET_PERFID_TURN,&dwTurn,NULL,NULL,NULL,NULL);
		sprintf(szBuf,"turn: 0x%08x   ",dwTurn);
		TextOut(hDC,5,400,szBuf,strlen(szBuf));

		static DWORD sgdwLastBytes;
		SNetGetPerformanceData(SNET_PERFID_BYTESSENTONWIRE,&dwBytes,NULL,NULL,NULL,NULL);
		sprintf(szBuf,"bytes: %8d    ",dwBytes - sgdwLastBytes);
		sgdwLastBytes = dwBytes;
		TextOut(hDC,5,415,szBuf,strlen(szBuf));

		DWORD dwUser,dwTotal;
		static DWORD sdwLastUser;
		static DWORD sdwLastTotal;
		SNetGetPerformanceData(SNET_PERFID_USERBYTESSENT,&dwUser,NULL,NULL,NULL,NULL);
		SNetGetPerformanceData(SNET_PERFID_TOTALBYTESSENT,&dwTotal,NULL,NULL,NULL,NULL);
		dwBytes = dwTotal - sdwLastTotal;
		if (! dwBytes) dwBytes = 1;
		dwBytes = ((dwUser - sdwLastUser) * 100) / dwBytes;
		sdwLastUser = dwUser;
		sdwLastTotal = dwTotal;
		sprintf(szBuf,"user: %3d%%    ",dwBytes);
		TextOut(hDC,5,430,szBuf,strlen(szBuf));
	}
*/

	SetTextColor(hDC,oldTextColor);
	SetBkColor(hDC,oldBkColor);
	SetBkMode(hDC,oldBkMode);
	lpDDSPrimary->ReleaseDC(hDC);
}
#endif


//******************************************************************
//******************************************************************
BOOL nthread_msg_check(BOOL * pfSendAsync) {
	app_assert(pfSendAsync);
	*pfSendAsync = FALSE;

	#if PRINT_STATUS
	if (sgbPrintStatus) print_status();
	#endif

	// count down until time to send async packet
	app_assert(sgbPacketCountdown);
	if (--sgbPacketCountdown) {
		// advance the game clock to the next
		// time we will need to run a game loop
		sglGameClock += 1000 / GAME_FRAMES_PER_SECOND;
		return TRUE;
	}

	// reset async countdown timer
	sgbPacketCountdown = gbGameLoopsPerPacket;

	// is it time to receive sync message?
	app_assert(sgbSyncCountdown);
	if (! --sgbSyncCountdown) {
		TRACE_FCN("SNetReceiveTurns");
		if (! SNetReceiveTurns(0,MAX_PLRS,glpMsgTbl,gdwMsgLenTbl,gdwMsgStatTbl)) {
			TRACE_FCN(NULL);
			DWORD dwErr = GetLastError();
			if (dwErr != SNET_ERROR_NO_MESSAGES_WAITING)
				nthread_check_snet_error("SNetReceiveTurns");

			// we didn't successfully receive a turn, so go run some
			// user stuff, and come back here to try again next time
			sgbPacketCountdown = sgbSyncCountdown = 1;

			#if PRINT_STATUS && STATUS_LASTERR
			// copy results of last bad status info to private table
			CopyMemory(sgdwPrintTbl,gdwMsgStatTbl,sizeof(sgdwPrintTbl));
			#endif

			#if DUMP_STATUS
			DumpStatus();
			#endif

			sgbGotPacketOK = 0;
			return FALSE;
		}
		else if (! sgbGotPacketOK) {
			TRACE_FCN(NULL);
			// we were waiting for the packet.  Since
			// there should be a bunch of messages in the
			// queue, we should never have to wait.
			sgbGotPacketOK = 1;
			sglGameClock = GetTickCount();

			#if DUMP_STATUS
			DumpStatus();
			#endif
		}
		else {
			TRACE_FCN(NULL);
		}

		// reset sync countdown timer
		sgbSyncCountdown = ASYNC_CYCLES_PER_SYNC;

		// process synchronous turn
		process_turn();
	}

	// it's time to send an async message
	*pfSendAsync = TRUE;

	// advance the game clock to the next
	// time we will need to run a game loop
	sglGameClock += 1000 / GAME_FRAMES_PER_SECOND;
	return TRUE;
}


//******************************************************************
//******************************************************************
static unsigned __stdcall net_thread_proc(void *) {
	long lDelta;

	while (sgbRunThread) {
		sgCrit.Enter();

		// if nthread_free() was called, it may have released the
		// critical section so that this thread could run.  However,
		// we're just supposed to get out of here...
		if (! sgbRunThread) {
			sgCrit.Leave();
			return 0;
		}

		// fill queue with minimum number of synchronous
		// messages before we attempt to read any
		BOOL bSendAsync;
		nthread_fill_sync_queue(0,0);
		if (nthread_msg_check(&bSendAsync)) {
			// sleep until it is approximately time
			// to send the next synchronous turn
			lDelta = sglGameClock - (long) GetTickCount();
		}
		else {
			// we are waiting for a message from one of the other players
			// since it is not here yet, sleep for a longish while so 
			// that we don't do any busy waiting
			lDelta = 1000 / GAME_FRAMES_PER_SECOND;
		}
		sgCrit.Leave();
		if (lDelta > 0) Sleep(lDelta);

		// pjw.patch2.start
		#if CHEATS
		static DWORD sdwSkips = 0;
		sdwSkips++;
		if (lpDDSPrimary)
		{
			HDC hDC;
			HRESULT ddr = lpDDSPrimary->GetDC(&hDC);
			if (ddr == DD_OK) {
				char szBuf[16];
				wsprintf(szBuf,"n:%u",sdwSkips);
				TextOut(hDC,5,440,szBuf,strlen(szBuf));
				lpDDSPrimary->ReleaseDC(hDC);
			}
		}
		#endif
		// pjw.patch2.end
	}

	return 0;
}


//******************************************************************
//******************************************************************
void nthread_set_delta_request() {
	sgdwRequestDeltaFlag = TURN_REQUEST_DELTA_FLAG;
}


//******************************************************************
//******************************************************************
void nthread_init(BOOL bRequestDelta) {
	// start game clock and countdown timers
	sglGameClock = GetTickCount();
	sgbPacketCountdown = 1;
	sgbSyncCountdown = 1;
	sgbGotPacketOK = 1;
	if (bRequestDelta) nthread_set_delta_request();
	else sgdwRequestDeltaFlag = 0;

	// calculate the number of packets we should send
	// per game loop based on the provider latency
	SNETCAPS caps;
	caps.size = sizeof caps;
	if (! SNetGetProviderCaps(&caps))
		app_fatal("SNetGetProviderCaps:\n%s",strGetLastError());

	// find out how many messages we should put into the queue
	if (caps.defaultturnsintransit)
		gdwTurnsInTransit = caps.defaultturnsintransit;
	else
		gdwTurnsInTransit = 1;

	// calculate the number of game loops to run per packet
	// based on the speed of the network connection
	if (caps.defaultturnssec > GAME_FRAMES_PER_SECOND || ! caps.defaultturnssec)
		gbGameLoopsPerPacket = 1;
	else 
		gbGameLoopsPerPacket = (BYTE) (GAME_FRAMES_PER_SECOND / caps.defaultturnssec);

	// size of largest packet which can be sent over network
	gdwLargestMsgSize = min(caps.maxmessagesize,MAX_MSG_SIZE);
	app_assert(gdwLargestMsgSize >= MIN_MSG_SIZE);

	// calculate how big our messages can be based on the available
	// bandwidth and the number of messages we'll be sending per turn
	// (bytes/sec) * (frames/pkt) / (frames/sec) = (bytes/pkt)
	gdwNormalMsgSize = caps.bytessec * gbGameLoopsPerPacket / GAME_FRAMES_PER_SECOND;

	// use 1/4 of the channel for sending delta info to other players
	gdwDeltaBytesSec = caps.bytessec * 1 / 4;

	// use 3/4 of channel for player messages
	gdwNormalMsgSize *= 3;
	gdwNormalMsgSize /= 4;

	// Divide by the number of players since we have to send a msg to each
	// NOTE: we could subtract out the local player, since we send no
	// info over the wire to local system, but leave this entry in
	// to allow for "overhead"
	app_assert(caps.maxplayers);
	if (caps.maxplayers > MAX_PLRS) caps.maxplayers = MAX_PLRS;
	gdwNormalMsgSize /= caps.maxplayers;

	// if our packet is too small, send fewer messages per turn
	while (gdwNormalMsgSize < MIN_MSG_SIZE) {
		gdwNormalMsgSize *= 2;
		gbGameLoopsPerPacket *= 2;
	}

	// bounds check maximum value
	if (gdwNormalMsgSize > gdwLargestMsgSize)
		gdwNormalMsgSize = gdwLargestMsgSize;

	// create synchronization object for thread
	if (gbMaxPlayers > 1) {
		sgbThreadIsRunning = FALSE;
		sgCrit.Enter();

		// create loader thread
		sgbRunThread = TRUE;
		app_assert(sghThread == INVALID_HANDLE_VALUE);
		sghThread = (HANDLE) _beginthreadex(
			NULL,				// no security info
			0,					// stack size
			net_thread_proc,	// start address
			NULL,				// argument list
			0,					// initial state
			&sgThreadID			// sgThreadID
		);
		if (sghThread == INVALID_HANDLE_VALUE)
			app_fatal(TEXT("nthread2:\n%s"),strGetLastError());

		// make sure this thread runs at higher priority than
		// SFile, and at the same priority as SNet
		SetThreadPriority(sghThread,THREAD_PRIORITY_HIGHEST);
	}
}


//******************************************************************
//******************************************************************
void nthread_free() {
	// kill off the thread
	sgbRunThread = FALSE;

	// set parameters to invalid values
	gdwTurnsInTransit = 0;
	gdwNormalMsgSize = 0;
	gdwLargestMsgSize = 0;

	// wait until it finishes
	if (sghThread != INVALID_HANDLE_VALUE) {
		if (sgThreadID != GetCurrentThreadId()) {
			if (! sgbThreadIsRunning) sgCrit.Leave();
			if (WAIT_FAILED == WaitForSingleObject(sghThread,INFINITE))
				app_fatal(TEXT("nthread3:\n(%s)"),strGetLastError());
			CloseHandle(sghThread);
			sghThread = INVALID_HANDLE_VALUE;
		}
	}
}


//******************************************************************
//******************************************************************
void nthread_perform_keepalive(BOOL bStart) {
	if (sghThread == INVALID_HANDLE_VALUE)
		return;
	app_assert(sgbThreadIsRunning != bStart);

	if (bStart) {
		// allow thread to run
		sgCrit.Leave();
	}
	else {
		// prevent thread from running
		sgCrit.Enter();
	}

	sgbThreadIsRunning = bStart;
}


//******************************************************************
//******************************************************************
BOOL nthread_run_gameloop(BOOL bReloop) {
	long lRealClock = (long) GetTickCount();
	long lDelta = lRealClock - sglGameClock;

	// if it has been too long since we last ran a game loop,
	// then pretend we have caught up and are running normally
	if (gbMaxPlayers == 1 && lDelta > CATCH_UP_DELTA) {
		sglGameClock = lRealClock;
		lDelta = 0;
	}

	return (lDelta >= 0);
}
