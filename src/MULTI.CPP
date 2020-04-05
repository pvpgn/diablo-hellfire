/*-----------------------------------------------------------------------**
** 								Diablo
**
**							  Multi file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MULTI.CPP 9     4/01/97 5:09p Pwyatt $
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"storm.h"
#include	"msg.h"
#include	"multi.h"
#include	"gendung.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"spells.h"
#include	"packplr.h"
#include	"engine.h"
#include	"diabloui.h"
#include	"portal.h"


//******************************************************************
// extern
//******************************************************************
extern char gszHero[];
extern BOOL gbRunGame;
extern char gszVersionNumber[];
extern SNETVERSIONDATA gVersion;
extern int gnDifficulty;


void tmsg_init();
void tmsg_free();
void dthread_init();
void dthread_free();
void delta_init();
void dthread_remove_player(int pnum);
void SetupLocalPlayer();
void FixPlrWalkTags(int pnum);
void StartStand(int,int);
BOOL wait_delta_info();
void plrmsg_init();
void __cdecl sysmsg_add(const char * pszFmt,...);
void sysmsg_add_string(const char * pszMsg);
void game_2_ui_player(const PlayerStruct * p,TPUIHEROINFO heroinfo,BOOL bHasSaveFile);
void nthread_check_snet_error(const char * pszFcn);
DWORD tmsg_get(BYTE * pbMsg,DWORD dwMaxLen);
void tmsg_add(const BYTE * pbMsg,BYTE bLen);
void delta_close_portal(int pnum);
void NewPlrAnim(int pnum, BYTE *pAnim, int numFrames, int Delay, long width);


//******************************************************************
// public
//******************************************************************
BYTE gbMaxPlayers;
BYTE gbActivePlayers;
BYTE gbGameDestroyed;
BYTE gbSelectProvider;
BYTE gbDeltaSender;
BYTE gbSomebodyWonGameKludge;

char gszGameName[128];
char gszGamePass[128];


//******************************************************************
// private -- packets
//******************************************************************
// network initialized
static BOOL sgbNetInited = FALSE;

// did we send an async message in this messaging cycle?
static BOOL sgbSentThisCycle;

// number of sync messages/game loops -- will be synchronized
// across all systems as new players asychronously join the game
static DWORD sgdwSyncMsgs;
static DWORD sgdwGameLoops;

// buffers for player information which
// is too big to fit into one packet
// PATCH3.JMM
WORD sgwPackPlrOffsetTbl[MAX_PLRS];
// ENDPATCH3.JMM
static PkPlayerStruct sgPackPlr[MAX_PLRS];

static BYTE sgbPlayerLeftGameTbl[MAX_PLRS];
static DWORD sgdwPlayerLeftReasonTbl[MAX_PLRS];
static BYTE sgbSendDeltaTbl[MAX_PLRS];
static TGAMEDATA sgGameInitInfo;

// pjw.patch2.start
static BYTE sgbGameJoiner[MAX_PLRS];
// pjw.patch2.end

static BYTE sgbTimeout;
static long sglTimeoutStart;

// time to hang out in timeout state before dropping nonresponsive players
#define DROP_NOTRESPONDING_DELAY	(10*1000) // milliseconds

// time to hang out in timeout state if we're not the master
#define LEAVE_GAME_DELAY			(20*1000) // milliseconds


//******************************************************************
// private -- internal buffering
//******************************************************************
#define MAX_BUF_DATA	4096
typedef struct TBuffer {
	DWORD dwNextWriteOffset;
	BYTE bData[MAX_BUF_DATA];
} TBuffer;
static TBuffer sgLoPriBuf;
static TBuffer sgHiPriBuf;


//******************************************************************
//******************************************************************
#if TRACEOUT
static DWORD sgdwTraceStartTime;
void __cdecl TraceOut(const char * pszFmt, ...) {
	static FILE * f = NULL;
	if (! f) f = fopen("c:\\dumphist.txt","wb");
	if (! f) return;

	va_list args;
	va_start(args,pszFmt);
	DWORD dwCurrTime = GetTickCount() - sgdwTraceStartTime;
	fprintf(f,"%4u.%02u  ",dwCurrTime / 1000, (dwCurrTime % 1000) / 10);
	vfprintf(f,pszFmt,args);
	fprintf(
		f,"\r\n          %x%c %x%c %x%c %x%c\r\n",
		gdwMsgStatTbl[0] >> 16,plr[0].plractive ? 'A' : ' ',
		gdwMsgStatTbl[1] >> 16,plr[1].plractive ? 'A' : ' ',
		gdwMsgStatTbl[2] >> 16,plr[2].plractive ? 'A' : ' ',
		gdwMsgStatTbl[3] >> 16,plr[3].plractive ? 'A' : ' '
	);
	va_end(args);
	fflush(f);
}
#endif


//******************************************************************
//******************************************************************
static void buffer_init(TBuffer * pBuf) {
	pBuf->dwNextWriteOffset = 0;
	pBuf->bData[0] = 0;
}


//******************************************************************
//******************************************************************
static BOOL buffer_empty(TBuffer * pBuf) {
	return pBuf->dwNextWriteOffset == 0;
}


//******************************************************************
//******************************************************************
static void buffer_add(TBuffer * pBuf,const BYTE * pbMsg,BYTE bLen) {
	// is there enough space in the buffer for length byte + data
	// plus one more byte for terminating NULL?
	if (pBuf->dwNextWriteOffset + bLen + 2 > MAX_BUF_DATA) {
		#ifndef NDEBUG
		app_fatal("msg buffer failure");
		#endif
		return;
	}

	// get ptr to curr location, and setup next location
	BYTE * pbData = &pBuf->bData[pBuf->dwNextWriteOffset];
	pBuf->dwNextWriteOffset += bLen + 1;

	// write length, data, terminating NULL
	*pbData++ = bLen;
	CopyMemory(pbData,pbMsg,bLen);
	pbData += bLen;
	*pbData = 0;
}


//******************************************************************
//******************************************************************
static BYTE * buffer_get(TBuffer * pBuf,BYTE * pbMsg,DWORD * pdwMaxLen) {

	// is there any data in the buffer?
	if (! pBuf->dwNextWriteOffset)
		return pbMsg;
	
	BYTE * pbData = pBuf->bData;
	while (1) {
		// is there enough space to copy the data from the buffer
		BYTE bLen = *pbData;
		if (! bLen) break;
		if (bLen > *pdwMaxLen) break;

		// skip over the length byte, and copy the data
		CopyMemory(pbMsg,++pbData,bLen);
		pbData += bLen;
		pbMsg += bLen;
		*pdwMaxLen -= bLen;
	}

	// fixup buffer
	MoveMemory(
		pBuf->bData,	// start of buffer
		pbData,			// curr position in buffer
		pBuf->dwNextWriteOffset - (pbData - pBuf->bData) + 1
	);

	// fixup buffer write offset
	pBuf->dwNextWriteOffset -= (pbData - pBuf->bData);

	return pbMsg;
}


//******************************************************************
//******************************************************************
static void build_pkt_hdr(TPkt * pkt) {
	app_assert(pkt);
	pkt->hdr.wCheck = 0x6970;
	pkt->hdr.px = plr[myplr]._px;
	pkt->hdr.py = plr[myplr]._py;
	pkt->hdr.targx = plr[myplr]._ptargx;
	pkt->hdr.targy = plr[myplr]._ptargy;
	pkt->hdr.php = plr[myplr]._pHitPoints;
	pkt->hdr.pmhp = plr[myplr]._pMaxHP;
	pkt->hdr.bstr = plr[myplr]._pBaseStr;
	pkt->hdr.bmag = plr[myplr]._pBaseMag;
	pkt->hdr.bdex = plr[myplr]._pBaseDex;
}


//******************************************************************
//******************************************************************
void NetSendMyselfPri(const BYTE * pbMsg,BYTE bLen) {
	app_assert(sgbNetInited);
	if (! pbMsg) return;
	if (! bLen) return;
	app_assert(bLen <= gdwNormalMsgSize - sizeof(TPktHdr));
	tmsg_add(pbMsg,bLen);
}


//******************************************************************
//******************************************************************
static void NetSendLocal(const BYTE * pbMsg,BYTE bLen) {
	app_assert(sgbNetInited);
	app_assert(pbMsg);
	app_assert(bLen);
	app_assert(bLen <= gdwNormalMsgSize - sizeof(TPktHdr));

	// build message
	TPkt pkt;
	build_pkt_hdr(&pkt);
	pkt.hdr.wLen = sizeof(pkt.hdr) + bLen;
	CopyMemory(pkt.body,pbMsg,bLen);

	// send it
	TRACE_FCN("SNetSendMessage0");
	if (! SNetSendMessage(myplr,&pkt,pkt.hdr.wLen))
		nthread_check_snet_error("SNetSendMessage0");
	TRACE_FCN(NULL);

	#ifndef NDEBUG
	extern DWORD gdwAsyncSendTbl[MAX_PLRS];
	gdwAsyncSendTbl[myplr]++;
	#endif
}


//******************************************************************
//******************************************************************
void NetSendLoPri(const BYTE * pbMsg,BYTE bLen) {
	app_assert(sgbNetInited);
	if (! pbMsg) return;
	if (! bLen) return;
	app_assert(bLen <= gdwNormalMsgSize - sizeof(TPktHdr));

	// message not sent out of deference to higher priority
	// commands which might come later in this message cycle
	buffer_add(&sgLoPriBuf,pbMsg,bLen);
	NetSendLocal(pbMsg,bLen);
}


//******************************************************************
//******************************************************************
void NetSendHiPri(const BYTE * pbMsg,BYTE bLen) {
	app_assert(sgbNetInited);
	if (pbMsg && bLen) {
		app_assert(bLen <= gdwNormalMsgSize - sizeof(TPktHdr));
		buffer_add(&sgHiPriBuf,pbMsg,bLen);
		NetSendLocal(pbMsg,bLen);
	}

	// if we have already sent a message this cycle, then
	// we cannot send another until the next cycle.
	if (sgbSentThisCycle)
		return;
	sgbSentThisCycle = TRUE;

	app_assert((DWORD) myplr < MAX_PLRS);

	// setup packet header
	TPkt pkt;
	build_pkt_hdr(&pkt);

	// fill in packet body
	BYTE * pbBody = pkt.body;
	DWORD dwBytesLeft = gdwNormalMsgSize - sizeof(pkt.hdr);
	pbBody = buffer_get(&sgHiPriBuf,pbBody,&dwBytesLeft);
	pbBody = buffer_get(&sgLoPriBuf,pbBody,&dwBytesLeft);
	dwBytesLeft = sync_get(pbBody,dwBytesLeft);
	DWORD dwSendBytes = gdwNormalMsgSize - dwBytesLeft;
	pkt.hdr.wLen = (WORD) dwSendBytes;

	// send it
	TRACE_FCN("SNetSendMessage");
	if (! SNetSendMessage(SNET_BROADCASTNONLOCALPLAYERID,&pkt,dwSendBytes))
		nthread_check_snet_error("SNetSendMessage");
	TRACE_FCN(NULL);

	#ifndef NDEBUG
	extern DWORD gdwAsyncSendTbl[MAX_PLRS];
	if (myplr != 0) gdwAsyncSendTbl[0]++;
	if (myplr != 1) gdwAsyncSendTbl[1]++;
	if (myplr != 2) gdwAsyncSendTbl[2]++;
	if (myplr != 3) gdwAsyncSendTbl[3]++;
	#endif
}


//******************************************************************
//******************************************************************
void NetSendMask(DWORD dwSendMask,const BYTE * pbMsg,BYTE bLen) {
	app_assert(sgbNetInited);
	app_assert(pbMsg);
	app_assert(bLen);

	// setup packet header
	TPkt pkt;
	build_pkt_hdr(&pkt);

	// fill in packet body
	DWORD dwSendBytes = sizeof(pkt.hdr) + bLen;
	app_assert(dwSendBytes < gdwNormalMsgSize);
	pkt.hdr.wLen = (WORD) dwSendBytes;
	CopyMemory(pkt.body,pbMsg,bLen);

	// send to each player listed in send mask
	DWORD dwMaskFlag = 0x1;
	for (DWORD dwID = 0; dwID < MAX_PLRS; dwID++,dwMaskFlag <<= 1) {
		if (! (dwMaskFlag & dwSendMask)) continue;

		TRACE_FCN("SNetSendMessage");
		BOOL bSent = SNetSendMessage(dwID,&pkt,dwSendBytes);
		TRACE_FCN(NULL);
		if (bSent) {
			#ifndef NDEBUG
			extern DWORD gdwAsyncSendTbl[MAX_PLRS];
			gdwAsyncSendTbl[dwID]++;
			#endif
			continue;
		}

		// we don't keep track of players, so we may have sent a
		// message to somebody who isn't there anymore.
		if (GetLastError() == SNET_ERROR_INVALID_PLAYER) continue;

		nthread_check_snet_error("SNetSendMessage");
		break;
	}
}


//******************************************************************
//******************************************************************
static void NetResync() {
	// resync all monster seeds based on current value of game loops
	sgdwGameLoops++;

	// reinitialize monster AI seed so that
	// all systems can try to resynchronize
	DWORD dwSeed = _rotr(sgdwGameLoops,8);
	for (int i = 0; i < MAXMONSTERS; i++)
		monster[i]._mAISeed = i + dwSeed;


#if 0
	if (! lpDDSPrimary) return;
	if (sgdwGameLoops % 10) return;

	#define ACCUM_ENTRIES 4
	static DWORD sgdwTime[ACCUM_ENTRIES];
	static DWORD sgdwCount[ACCUM_ENTRIES];

	MoveMemory(&sgdwTime[0],&sgdwTime[1],sizeof(DWORD) * (ACCUM_ENTRIES-1));
	MoveMemory(&sgdwCount[0],&sgdwCount[1],sizeof(DWORD) * (ACCUM_ENTRIES-1));
	sgdwTime[ACCUM_ENTRIES - 1] = GetTickCount();
	sgdwCount[ACCUM_ENTRIES - 1] = sgdwGameLoops;

	DWORD dwRate = sgdwTime[ACCUM_ENTRIES - 1] - sgdwTime[0];
	if (! dwRate) return;
	dwRate = (sgdwCount[ACCUM_ENTRIES - 1] - sgdwCount[0]) * 1000 / dwRate;

	HDC hDC;
	char szRate[16];
	sprintf(szRate,"%d",dwRate);
	HRESULT ddr = lpDDSPrimary->GetDC(&hDC);
	if (ddr != DD_OK) return;
	SetTextColor(hDC,0x00ff0000);
	TextOut(hDC,0,420,szRate,strlen(szRate));
	lpDDSPrimary->ReleaseDC(hDC);
#endif
}


//******************************************************************
//******************************************************************
static void check_DeltaSendAllLevels(int nReceiver) {
	// the lowest numbered active player is
	// responsible for sending the level delta
	// information to the new player
	for (int nSender = 0; nSender < MAX_PLRS; nSender++) {
		// inactive non-player cannot send info
		// pjw.patch2.start
		if (! (gdwMsgStatTbl[nSender] & SNET_PSF_ACTIVE)) continue;
		// pjw.patch2.end

		// player requesting info cannot send info
		if (nSender == nReceiver) continue;

		// we found a player who is responsible
		break;
	}

	if (myplr == nSender)
		sgbSendDeltaTbl[nReceiver] = TRUE;
	else if (myplr == nReceiver)
		gbDeltaSender = (BYTE) nSender;
}


//******************************************************************
//******************************************************************
static void check_sync(int pnum,DWORD dwSyncMsgs) {
	if (dwSyncMsgs & TURN_REQUEST_DELTA_FLAG) {
		// player has just joined and needs delta information
		check_DeltaSendAllLevels(pnum);
	}

	// remove flag bits
	dwSyncMsgs &= TURN_COUNTER_MASK;

	// if the other player has sent more sync msgs than we have,
	// then recompute the number of game loops which have occurred
	// using his msg count to keep both systems synchronized
	if (sgdwSyncMsgs < dwSyncMsgs + gdwTurnsInTransit) {
		if (dwSyncMsgs >= TURN_COUNTER_MASK)
			dwSyncMsgs &= TURN_COUNTER_RESET_MASK;
		sgdwSyncMsgs = dwSyncMsgs + gdwTurnsInTransit;
		dwSyncMsgs *= ASYNC_CYCLES_PER_SYNC;
		dwSyncMsgs *= gbGameLoopsPerPacket;
		sgdwGameLoops = dwSyncMsgs;
	}
}


//******************************************************************
//******************************************************************
// pjw.patch2.start
void process_turn() {
	for (int i = 0; i < MAX_PLRS; i++) {
		if (gdwMsgStatTbl[i] & SNET_PSF_TURNAVAILABLE) {
			// make sure we got a valid message
			app_assert(glpMsgTbl[i]);

			// odds are we will eventually get a bad packet
			if (gdwMsgLenTbl[i] != sizeof(DWORD)) continue;

			// process turn
			check_sync(i,*(DWORD *)glpMsgTbl[i]);
		}
	}
}
// pjw.patch2.end


//******************************************************************
//******************************************************************
static void remove_active_player(int i,BOOL bMsg) {
	if (! plr[i].plractive) return;

	#if TRACEOUT
	TraceOut("(%d) player %d --> inactive",myplr,i);
	#endif

	// remove player from map
	void RemovePlrFromMap(int pnum);
	RemovePlrFromMap(i);
	RemovePortalMissile(i);
	DeactivatePortal(i);
	delta_close_portal(i);
	void RemovePlrMissiles(int pnum);
	RemovePlrMissiles(i);

	if (bMsg) {
		const char * pszMsg_s = "Player '%s' just left the game";
		switch (sgdwPlayerLeftReasonTbl[i]) {
			case SNET_EXIT_NOTRESPONDING:
				pszMsg_s = "Player '%s' dropped due to timeout";
			break;
			case SNET_EXIT_PLAYERWON:
				pszMsg_s = "Player '%s' killed Diablo and left the game!";
				gbSomebodyWonGameKludge = TRUE;
			break;
		}
		sysmsg_add(pszMsg_s,plr[i]._pName);
	}

	plr[i].plractive = 0;
	plr[i]._pName[0] = 0;
	gbActivePlayers--;
}


//******************************************************************
//******************************************************************
static void update_players() {
	for (int i = 0; i < MAX_PLRS; i++) {
		// did the player leave the game
		if (! sgbPlayerLeftGameTbl[i]) continue;

		if (gbBufferMsgs == BUFFER_ON) {
			#if TRACEOUT
			TraceOut("(%d) buffering -- player %d left game due to 0x%x",myplr,i,sgdwPlayerLeftReasonTbl[i]);
			#endif
			void buffer_drop_player(int pnum,DWORD dwReason);
			buffer_drop_player(i,sgdwPlayerLeftReasonTbl[i]);
		}
		else {
			#if TRACEOUT
			TraceOut("(%d) player %d left game due to 0x%x",myplr,i,sgdwPlayerLeftReasonTbl[i]);
			#endif
			remove_active_player(i,TRUE);
		}

		sgbPlayerLeftGameTbl[i] = FALSE;
		sgdwPlayerLeftReasonTbl[i] = 0;
	}
}


//******************************************************************
//******************************************************************
void unbuffer_remove_player(int pnum,DWORD dwReason) {
	sgbPlayerLeftGameTbl[pnum] = TRUE;
	sgdwPlayerLeftReasonTbl[pnum] = dwReason;
	update_players();
}


//******************************************************************
//******************************************************************
void NetStartTimeout() {
	sgbTimeout = TRUE;
	sglTimeoutStart = (long) GetTickCount();
}


//******************************************************************
//******************************************************************
// pjw.patch2.start
static void drop_players() {
	// kill other players
	for (int i = 0; i < MAX_PLRS; i++) {
		if (gdwMsgStatTbl[i] & SNET_PSF_RESPONDING) continue;
		if (! (gdwMsgStatTbl[i] & SNET_PSF_ACTIVE)) continue;

		#if TRACEOUT
		TraceOut("(%d) dropping player %d state 0x%x",myplr,i,gdwMsgStatTbl[i]);
		#endif

		// snet drop player will perform a callback to
		// our event function to indicate that the player "left"
		TRACE_FCN("SNetDropPlayer");
		SNetDropPlayer(i,SNET_EXIT_NOTRESPONDING);
		TRACE_FCN(NULL);
	}
}
// pjw.patch2.end


//******************************************************************
//******************************************************************
// pjw.patch2.start
static void drop_inactive_players() {
	// if we're not in a timeout state, don't drop players
	if (! sgbTimeout) return;

	// debug mode
	#if CHEATS
	extern BOOL gbNoDropInactive;
	if (gbNoDropInactive) return;
	#endif

	// check delay since timeout started
	long lCurrTime = (long) GetTickCount();
	lCurrTime -= sglTimeoutStart;

	// if we've been hanging out forever, just exit game
	if (lCurrTime > LEAVE_GAME_DELAY) {
		gbRunGame = FALSE;
		return;
	}

	// time to kill other players?
	if (lCurrTime < DROP_NOTRESPONDING_DELAY) return;

// if there are only two players in the game, then the high
// player bails out, and the low player keeps playing.  If there
// are three or more players in the game, then the group that has
// the most players keeps playing and the other player(s) bail.
	int nLowestActive = -1;
	int nLowestPlayer = -1;
	BYTE bGroupPlayers = 0;
	BYTE bNonGroupPlayers = 0;
	for (int i = 0; i < MAX_PLRS; i++) {
		if (! (gdwMsgStatTbl[i] & SNET_PSF_ACTIVE)) continue;

		if (nLowestPlayer == -1) nLowestPlayer = i;

		// get player state
		if (gdwMsgStatTbl[i] & SNET_PSF_RESPONDING) {
			// one more player in our group
			bGroupPlayers++;
			if (nLowestActive == -1) nLowestActive = i;
		}
		else {
			// one more player not in our group
			bNonGroupPlayers++;
		}
	}
	app_assert(bGroupPlayers);
	app_assert(nLowestActive != -1);
	app_assert(nLowestPlayer != -1);


	#if 0
	TraceOut(
		"(%d) grp:%d ngrp:%d lowp:%d lowa:%d",
		myplr,
		bGroupPlayers,
		bNonGroupPlayers,
		nLowestPlayer,
		nLowestActive
	);
	#endif

	if (bGroupPlayers < bNonGroupPlayers) {
		// we're not part of the big group, give up
		gbGameDestroyed = TRUE;
	}
	else if (bGroupPlayers == bNonGroupPlayers) {
		if (nLowestPlayer != nLowestActive)
			gbGameDestroyed = TRUE;
		else if (nLowestActive == myplr)
			drop_players();
	}
	else if (nLowestActive == myplr) {
		drop_players();
	}
}
// pjw.patch2.end


//******************************************************************
//******************************************************************
BOOL NetEndSendCycle() {
	app_assert(sgbNetInited);

	if (gbGameDestroyed) {
		gbRunGame = FALSE;
		return FALSE;
	}

	// send delta information to other players
	for (int i = 0; i < MAX_PLRS; i++) {
		// are we supposed to send somebody delta information?
		if (! sgbSendDeltaTbl[i]) continue;
		sgbSendDeltaTbl[i] = FALSE;
		DeltaSendAllLevels(i);
	}

	// fill up the outgoing message queue
	// send the number of sync messages we think have been
	// sent to other players (and increment by 1 each msg)
	BOOL bSendAsync;
	sgdwSyncMsgs = nthread_fill_sync_queue(sgdwSyncMsgs,1);
	if (! nthread_msg_check(&bSendAsync)) {
		drop_inactive_players();
		return FALSE;
	}

	// we got some data -- reset timeout mode
	sgbTimeout = FALSE;

	if (! bSendAsync) {
		// it's not time to send an async message yet
	}
	else if (! sgbSentThisCycle) {
		// we haven't sent any async messages this cycle so
		// send a blank message and reset for next cycle
		NetSendHiPri(NULL,0);
		sgbSentThisCycle = FALSE;
	}
	else {
		// we already sent an async message this cycle
		// if there is any high priority stuff
		// still in the queue, send the next cycle's
		// message immediately
		sgbSentThisCycle = FALSE;
		if (! buffer_empty(&sgHiPriBuf))
			NetSendHiPri(NULL,0);
	}

	// update the monster random number seeds
	NetResync();
	return TRUE;
}


//******************************************************************
//******************************************************************
static void process_msg_body(DWORD dwID,const BYTE * pbCmd,DWORD dwBytes) {
	while (dwBytes) {
		DWORD dwCmdLen = ParseCmd(dwID,(const TCmd *) pbCmd);
		// patch3.jmm
		// zero is an error condition:
		// skip the rest of the packet
		if( dwCmdLen == 0 ) {
			return;
		}
		// endpatch3.jmm

		pbCmd += dwCmdLen;
		dwBytes -= dwCmdLen;
	}
}


//******************************************************************
//******************************************************************
static void handle_local_msgs() {
	DWORD dwLen;
	BYTE msg[MAX_MSG_SIZE];
	while (0 != (dwLen = tmsg_get(msg,sizeof(msg))))
		process_msg_body(myplr,msg,dwLen);
}


//******************************************************************
//******************************************************************
// patch1.jmm
extern DWORD dwRecCount;
// endpatch1.jmm

void NetReceivePackets() {
	DWORD dwID;
	LPVOID lpMsg;
	DWORD dwMsgSize;

	// make sure all players are dropped ASAP
	update_players();
	handle_local_msgs();

	app_assert(sgbNetInited);
	TRACE_FCN("SNetReceiveMessage");
	while (SNetReceiveMessage(&dwID,&lpMsg,&dwMsgSize)) {
		TRACE_FCN(NULL);

		// patch1.jmm
		dwRecCount++;
		// endpatch1.jmm

		// do not dispatch any messages until
		// we've dropped players who left the game
		update_players();

		// odds are we will eventually get a bad packet so ignore bad ones
		const TPkt * pkt = (const TPkt *) lpMsg;
		if (dwMsgSize < sizeof(TPktHdr)) continue;
		if (dwID >= MAX_PLRS) continue;
		if (pkt->hdr.wCheck != 0x6970) continue;
		if (pkt->hdr.wLen != dwMsgSize) continue;

		plr[dwID]._pownerx = pkt->hdr.px;
		plr[dwID]._pownery = pkt->hdr.py;

		#ifndef NDEBUG
		extern DWORD gdwAsyncRecvTbl[MAX_PLRS];
		gdwAsyncRecvTbl[dwID]++;
		#endif

		// the header contains the last known location of the
		// player at the time the message was sent.  Try to get
		// the player to walk to his correct location.  Any other
		// commands in the message will be executed later and so
		// can overwrite the results of this MakePlrPath.
		if (dwID != (DWORD) myplr) {
			app_assert(gbBufferMsgs != BUFFER_PROCESS);
			plr[dwID]._pHitPoints = pkt->hdr.php;
			plr[dwID]._pMaxHP = pkt->hdr.pmhp;
			plr[dwID]._pBaseStr = pkt->hdr.bstr;
			plr[dwID]._pBaseMag = pkt->hdr.bmag;
			plr[dwID]._pBaseDex = pkt->hdr.bdex;
			if (gbBufferMsgs == BUFFER_ON) {
				// do nothing
			}
			else if (! plr[dwID].plractive) {
				// do nothing
			}
			else if (plr[dwID]._pHitPoints == 0) {
				// do nothing
			}
			else if ((currlevel == plr[dwID].plrlevel) && (!plr[dwID]._pLvlChanging)) {
				int dx = abs(plr[dwID]._px - pkt->hdr.px);
				int dy = abs(plr[dwID]._py - pkt->hdr.py);
				// Is player in "ok delta" dist of where he should be?
				if (((dx > 3) || (dy > 3)) && (!dPlayer[pkt->hdr.px][pkt->hdr.py])) {
					// Warp player to location
					FixPlrWalkTags(dwID);
					plr[dwID]._poldx = plr[dwID]._px;
					plr[dwID]._poldy = plr[dwID]._py;
					FixPlrWalkTags(dwID);
					plr[dwID]._px = pkt->hdr.px;
					plr[dwID]._py = pkt->hdr.py;
					plr[dwID]._pfutx = pkt->hdr.px;
					plr[dwID]._pfuty = pkt->hdr.py;
					dPlayer[plr[dwID]._px][plr[dwID]._py] = 1 + (char)dwID;
				}
				dx = abs(plr[dwID]._pfutx - plr[dwID]._px);
				dy = abs(plr[dwID]._pfuty - plr[dwID]._py);
				if ((dx > 1) || (dy > 1)) {
					plr[dwID]._pfutx = plr[dwID]._px;
					plr[dwID]._pfuty = plr[dwID]._py;
				}
				MakePlrPath(dwID, pkt->hdr.targx, pkt->hdr.targy, TRUE);
				// do not override destAction
			}
			else {
				plr[dwID]._px = pkt->hdr.px;
				plr[dwID]._py = pkt->hdr.py;
				plr[dwID]._pfutx = pkt->hdr.px;
				plr[dwID]._pfuty = pkt->hdr.py;
				plr[dwID]._ptargx = pkt->hdr.targx;
				plr[dwID]._ptargy = pkt->hdr.targy;
				// do not override destAction
			}
		}
		process_msg_body(dwID,pkt->body,dwMsgSize - sizeof(TPktHdr));
	}
	TRACE_FCN(NULL);

	if (GetLastError() != SNET_ERROR_NO_MESSAGES_WAITING)
		nthread_check_snet_error("SNetReceiveMsg");
}


//******************************************************************
// NOTE: this routine is called from multiple threads
// don't do anything that would be thread unsafe
//******************************************************************
void SendPlayerInfoChunk(int pnum,BYTE bCmd,const BYTE * pbSrc,DWORD dwLen) {
	app_assert(pnum != myplr);

	// write info piece by piece
	app_assert(pbSrc);
	app_assert(dwLen <= 0x0ffff);
	DWORD dwOffset = 0;
	while (dwLen) {
		// setup packet header
		TPkt pkt;
		pkt.hdr.wCheck = 0x6970;
		pkt.hdr.px = 0;
		pkt.hdr.py = 0;
		pkt.hdr.targx = 0;
		pkt.hdr.targy = 0;
		pkt.hdr.php = 0;
		pkt.hdr.pmhp = 0;
		pkt.hdr.bstr = 0;
		pkt.hdr.bmag = 0;
		pkt.hdr.bdex = 0;

		// setup cmd header
		TCmdPlrInfoHdr * p = (TCmdPlrInfoHdr *) &pkt.body[0];
		p->bCmd = bCmd;
		p->wOffset = (WORD) dwOffset;

		// calculate how much data we can send
		DWORD dwBody = gdwLargestMsgSize - sizeof(pkt.hdr) - sizeof(TCmdPlrInfoHdr);
		dwBody = min(dwLen,dwBody);
		app_assert(dwBody <= 0x0ffff);
		p->wBytes = (WORD) dwBody;

		// copy cmd data
		CopyMemory(&pkt.body[sizeof TCmdPlrInfoHdr],pbSrc,p->wBytes);

		// send message to target
		DWORD dwSendBytes = sizeof(pkt.hdr);
		dwSendBytes += sizeof(TCmdPlrInfoHdr);
		dwSendBytes += p->wBytes;
		pkt.hdr.wLen = (WORD) dwSendBytes;

		TRACE_FCN("SnetSendMessage");
		if (! SNetSendMessage(pnum,&pkt,dwSendBytes)) {
			TRACE_FCN(NULL);
			nthread_check_snet_error("SNetSendMessage2");
			break;
		}
		TRACE_FCN(NULL);

		#ifndef NDEBUG
		extern DWORD gdwAsyncSendTbl[MAX_PLRS];
		if ((DWORD) pnum < MAX_PLRS) {
			gdwAsyncSendTbl[pnum]++;
		}
		else {
			if (myplr != 0) gdwAsyncSendTbl[0]++;
			if (myplr != 1) gdwAsyncSendTbl[1]++;
			if (myplr != 2) gdwAsyncSendTbl[2]++;
			if (myplr != 3) gdwAsyncSendTbl[3]++;
		}
		#endif


		// next data section
		pbSrc += p->wBytes;
		dwLen -= p->wBytes;
		dwOffset += p->wBytes;
	}
}


//******************************************************************
//******************************************************************
// pjw.patch2.start -- for patch, changed SendPlayerInfoChunk to 
// dthread_SendPlayerInfoChunk to more closely attempt to match the
// available bandwidth.  A player with a slow modem joining
// a game with 3 players would have to broadcast ~3k of playerdata,
// clogging the modem and preventing turns from getting out.
void dthread_SendPlayerInfoChunk(int pnum,BYTE bCmd,const BYTE * pbSrc,DWORD dwLen);
static void SendLocalPlayerInfo(int pnum,BYTE bCmd) {
	// get local player information and send it to the specified net addr
	PkPlayerStruct pack;
	PackPlayer(&pack,myplr);
	dthread_SendPlayerInfoChunk(pnum,bCmd,(const BYTE *) &pack,sizeof(pack));
}
// pjw.patch2.end


//******************************************************************
//******************************************************************
static int InitLevelType(int l) {
	if (l == 0) return(0);
	if ((l >= 1) && (l <= 4)) return(1);
	if ((l >= 5) && (l <= 8)) return(2);
	if ((l >= 9) && (l <= 12)) return(3);
	//return(4);	JKE 7/29
	if ((l >=13) && (l <= 16)) return(4);	// Original level types JKE
	if ((l >= CRYPTSTART) && (l <= CRYPTEND)) return(1);	// change this to crypt type later JKE 7/29
	if ((l >= HIVESTART) && (l <= HIVEEND)) return(3);		// use this for hives JKE
	return(1);
}


//******************************************************************
//******************************************************************
static void SetupLocalCoords() {
	// set the current level to start at the town
	if (! leveldebug || gbMaxPlayers > 1) {
		currlevel = 0;
		leveltype = 0;
		setlevel = FALSE;
	}

	// put character near house on town level
	int x = STARTX;
	int y = STARTY;

	#if CHEATS
	if (cheatflag || davedebug) {
		//x = 25;		// Near church
		//y = 31;
		x = 49;			// Near maus
		y = 23;
	}
	#endif

	// adjust character position for character number
	// so everyone doesn't start on the same location
	extern int plrxoff[9];
	extern int plryoff[9];
	x += plrxoff[myplr];
	y += plryoff[myplr];

	plr[myplr]._px = x;
	plr[myplr]._py = y;
	plr[myplr]._pfutx = x;
	plr[myplr]._pfuty = y;
	plr[myplr]._ptargx = x;
	plr[myplr]._ptargy = y;
	plr[myplr].plrlevel = currlevel;
	plr[myplr]._pLvlChanging = TRUE;
	plr[myplr].pLvlLoad = 0;
	plr[myplr]._pmode = PM_NEWLVL;
	plr[myplr].destAction = PCMD_NOTHING;
}


//******************************************************************
//******************************************************************
static BOOL handle_upgrade(BOOL * pfExitProgram) {
	DWORD dwStatus;
	SNetPerformUpgrade(&dwStatus);
	switch (dwStatus) {
		case SNET_UPGRADE_FAILED:
			app_warning("Network upgrade failed");
		break;
		
		case SNET_UPGRADE_NOT_NEEDED:
			// continue game
		return TRUE;

		case SNET_UPGRADE_SUCCEEDED:
			// continue game
		return TRUE;

		case SNET_UPGRADING_TERMINATE:
			*pfExitProgram = TRUE;
		break;
	}

	return FALSE;
}


//******************************************************************
//******************************************************************
static void CALLBACK net_callback(SNETEVENTPTR pEvt) {
	DWORD dwReason;
	TGAMEDATA * pGameData;

// NOTE: this function must be thread-safe since
// it can be called from either the main thread, or during
// the progress screen, called from nthread_*()
	switch (pEvt->eventid) {
		case SNET_EVENT_INITDATA:
			app_assert(pEvt->data);
			app_assert(pEvt->databytes >= sizeof(DWORD));
			pGameData = (TGAMEDATA *) pEvt->data;
			sgGameInitInfo.dwSeed = pGameData->dwSeed;
			sgGameInitInfo.bDiff = pGameData->bDiff;

			// if we get a callback containing init data
			// then we cannot be the one who started the game
			// pjw.patch2.start
			sgbGameJoiner[pEvt->playerid] = TRUE;
			//pjw.patch2.end
		break;

		case SNET_EVENT_PLAYERLEAVE:
			app_assert(pEvt->playerid >= 0 && pEvt->playerid < MAX_PLRS);
			sgbPlayerLeftGameTbl[pEvt->playerid] = TRUE;
			sgbGameJoiner[pEvt->playerid] = FALSE;
			dwReason = 0;
			if (pEvt->data && pEvt->databytes >= sizeof(DWORD))
				dwReason = * (DWORD *) pEvt->data;
			sgdwPlayerLeftReasonTbl[pEvt->playerid] = dwReason;

			if (dwReason == SNET_EXIT_PLAYERWON)
				gbSomebodyWonGameKludge = TRUE;

			// protected by critical section -- OK
			sgbSendDeltaTbl[pEvt->playerid] = 0;
			dthread_remove_player(pEvt->playerid);

			// was this the guy who was supposed to send us delta info?
			if (gbDeltaSender == pEvt->playerid)
				gbDeltaSender = MAX_PLRS;

			#if TRACEOUT
			TraceOut("(%d) callback: player %d left game due to 0x%x",myplr,pEvt->playerid,dwReason);
			#endif
		break;

		case SNET_EVENT_SERVERMESSAGE:
			app_assert(pEvt->data);
			sysmsg_add_string((const char *) pEvt->data);
		break;
	}
}


//******************************************************************
//******************************************************************
static void RegisterEventHandler(BOOL bRegister) {
	static const DWORD sdwEventTbl[] = {
		SNET_EVENT_PLAYERLEAVE,
		SNET_EVENT_INITDATA,
		SNET_EVENT_SERVERMESSAGE,
	};

	BOOL (__stdcall * fnReg)(DWORD,SNETEVENTPROC) = 
		bRegister ? SNetRegisterEventHandler : SNetUnregisterEventHandler;

	for (int i = 0; i < sizeof(sdwEventTbl) / sizeof(sdwEventTbl[0]); i++) {
		if (! fnReg(sdwEventTbl[i],net_callback) && bRegister)
			app_fatal("SNetRegisterEventHandler:\n%s",strGetLastError());
	}
}


//******************************************************************
//******************************************************************
void NetClose() {
	if (sgbNetInited) {
		sgbNetInited = FALSE;
		nthread_free();
		dthread_free();
		tmsg_free();
		RegisterEventHandler(FALSE);
		TRACE_FCN("SNetLeaveGame");
		SNetLeaveGame(SNET_EXIT_AUTO_SHUTDOWN);
		TRACE_FCN(NULL);

		#if TRACEOUT
		TraceOut("(%d) NetClose",myplr);
		#endif

		// give storm enough time to send leave pkt
		if (gbMaxPlayers > 1) Sleep(2000);
	}
}


//******************************************************************
//******************************************************************
// pjw.patch2.start
BOOL NetInit(BOOL bSinglePlayer,BOOL * pfExitProgram) {
top_of_routine:

	app_assert(pfExitProgram);
	*pfExitProgram = FALSE;

	// get player description
	char szPlayerDescript[UI_DESC_MAXLENGTH];
	ZeroMemory(szPlayerDescript,sizeof szPlayerDescript);
	if (! bSinglePlayer) {
		TUIHEROINFO heroinfo;
		myplr = 0;
		SetupLocalPlayer();
		game_2_ui_player(&plr[0],&heroinfo,gbValidSaveFile);
		if (! UiCreatePlayerDescription(&heroinfo,PROGRAMID,szPlayerDescript))
			return FALSE;
	}
	
	// initialize random seed for game
	SetRndSeed(0);
	sgGameInitInfo.dwSeed = (DWORD) time(NULL);
	sgGameInitInfo.bDiff = (BYTE) gnDifficulty;

	// BUILD A PROGRAM DATA RECORD
	SNETPROGRAMDATA progdata;
	ZeroMemory(&progdata,sizeof(progdata));
	progdata.size				= sizeof(progdata);
	#if IS_VERSION(RETAIL)
	//progdata.programname		= TEXT("Diablo Retail");
	progdata.programname		= TEXT("Hellfire Retail");
	#elif IS_VERSION(BETA)
	progdata.programname		= TEXT("Diablo");
	#elif IS_VERSION(SHAREWARE)
	progdata.programname		= TEXT("Diablo Shareware");
	#else
	#error VERSION NOT DEFINED
	#endif
	progdata.programdescription	= gszVersionNumber;
	progdata.programid			= PROGRAMID;
	progdata.versionid			= VERSIONID;
	progdata.maxplayers			= MAX_PLRS;
	progdata.initdata			= &sgGameInitInfo.dwSeed;
	progdata.initdatabytes		= sizeof(sgGameInitInfo);
	progdata.optcategorybits	= 0x0f;

	// BUILD A PLAYER DATA RECORD
	SNETPLAYERDATA plrdata;
	ZeroMemory(&plrdata,sizeof(plrdata));
	plrdata.size       = sizeof(plrdata);
	plrdata.playername = gszHero;
	plrdata.playerdescription = szPlayerDescript;

	// BUILD AN INTERFACE DATA RECORD
	SNETUIDATA uidata;
	ZeroMemory(&uidata,sizeof(SNETUIDATA));
	uidata.size           = sizeof(SNETUIDATA);
	uidata.parentwindow   = SDrawGetFrameWindow();
	uidata.artcallback    = UiArtCallback;
	uidata.createcallback = UiCreateGameCallback;
	uidata.drawdesccallback = UiDrawDescCallback;
	uidata.messageboxcallback = UiMessageBoxCallback;
	uidata.soundcallback  = UiSoundCallback;
	uidata.authcallback = UiAuthCallback;
	uidata.getdatacallback = UiGetDataCallback;
	uidata.categorycallback = UiCategoryCallback;

	// pjw.patch2.start
	ZeroMemory(sgbGameJoiner,sizeof(sgbGameJoiner));
	// pjw.patch2.end
	gbGameDestroyed = FALSE;
	ZeroMemory(sgbPlayerLeftGameTbl,sizeof(sgbPlayerLeftGameTbl));
	ZeroMemory(sgdwPlayerLeftReasonTbl,sizeof(sgdwPlayerLeftReasonTbl));
	ZeroMemory(sgbSendDeltaTbl,sizeof(sgbSendDeltaTbl));
	// pjw.patch1.start
	ZeroMemory(plr,sizeof(PlayerStruct) * MAX_PLRS);
	// pjw.patch1.end
	ZeroMemory(sgwPackPlrOffsetTbl,sizeof(sgwPackPlrOffsetTbl));
	SNetSetBasePlayer(0);

	if (bSinglePlayer) {
		if (! SNetInitializeProvider(NULL,&progdata,&plrdata,&uidata,&gVersion))
			app_fatal("SNetInitializeProvider:\n%s",strGetLastError());

		DWORD dwID = 0;
		if (! SNetCreateGame (
				"local",				// gamename
				"local",				// password
				"local",				// gamedescription
				0,						// game category bits
				&sgGameInitInfo,		// initdata
				sizeof(sgGameInitInfo),	// initdatabytes
				1,						// maxplayers
				"local",				// playername
				"local",				// playerdescription
				&dwID					// playerid
			)) app_fatal("SNetCreateGame1:\n%s",strGetLastError());
		app_assert(dwID == 0);
		myplr = 0;
		gbMaxPlayers = 1;
	}
	else {

		if (gbSelectProvider) {
			BOOL bTryAgain = TRUE;
			while (1) {
				DWORD dwProviderID;
				if (UiSelectProvider(NULL,&progdata,&plrdata,&uidata,&gVersion,&dwProviderID))
					break;
				if (! bTryAgain)
					return FALSE;
				if (GetLastError() != SNET_ERROR_REQUIRES_UPGRADE)
					return FALSE;
				if (! handle_upgrade(pfExitProgram))
					return FALSE;
				bTryAgain = FALSE;
			}
		}

		RegisterEventHandler(TRUE);
	
		DWORD dwID;
		if (! UiSelectGame(SNET_SF_ALLOWCREATE,&progdata,&plrdata,&uidata,&gVersion,&dwID)) {
			app_assert(! *pfExitProgram);
			return FALSE;
		}
		// this happened once, so handle it...
		if (dwID >= MAX_PLRS) return FALSE;

		myplr = dwID;
		gbMaxPlayers = MAX_PLRS;
	}

	#if TRACEOUT
	sgdwTraceStartTime = GetTickCount();
	TraceOut("(%d) new game started",myplr);
	#endif

	// do various network initializations
	sgbNetInited = TRUE;
	sgbTimeout = FALSE;
	delta_init();
	plrmsg_init();
	buffer_init(&sgHiPriBuf);
	buffer_init(&sgLoPriBuf);
	sgbSentThisCycle = FALSE;
	sync_init();
	// pjw.patch2.start
	nthread_init(sgbGameJoiner[myplr]);
	// pjw.patch2.end
	dthread_init();
	tmsg_init();
	sgdwGameLoops = 0;
	sgdwSyncMsgs = 0;
	gbDeltaSender = (BYTE) myplr;
	gbSomebodyWonGameKludge = FALSE;

	// request everyone's player info
	SetupLocalPlayer();

// pjw.patch2.start -- added this code so that the first thing
// out of the modem is the turn, not tons of playerdata.  If we send
// only playerdata, the modem queue gets filled up, and we don't respond
// fast enough to other players - sometimes causing a timeout
	nthread_fill_sync_queue(0,0);
// pjw.patch2.end

	// broadcast our player info to everyone
	SetupLocalCoords();
	SendLocalPlayerInfo(SNET_BROADCASTNONLOCALPLAYERID,CMD_SEND_PLRINFO);

	// i'm always active on my system
	plr[myplr].plractive = 1;
	gbActivePlayers = 1;

	// if we started the game, then we can proceed immediately.
	// if we joined the game, we must wait until we receive
	// all the level delta information before proceeding
	// pjw.patch2.start
	if (sgbGameJoiner[myplr] && !wait_delta_info()) {
		NetClose();
		gbSelectProvider = FALSE;
		goto top_of_routine;	// sorry :(
	}
	// pjw.patch2.end

	// initialize map random number seeds
	gnDifficulty = sgGameInitInfo.bDiff;
	SetRndSeed(sgGameInitInfo.dwSeed);
	for (int i = 0; i < NUMLEVELS; i++) {
		glSeedTbl[i] = GetRndSeed();
		gnLevelTypeTbl[i] = InitLevelType(i);
	}

	DWORD dwTemp;
	if (! SNetGetGameInfo(SNET_INFO_GAMENAME,gszGameName,SNET_MAXNAMELENGTH,&dwTemp))
		nthread_check_snet_error("SNetGetGameInfo1");
	if (! SNetGetGameInfo(SNET_INFO_GAMEPASSWORD,gszGamePass,SNET_MAXNAMELENGTH,&dwTemp))
		nthread_check_snet_error("SNetGetGameInfo2");

	return TRUE;
}
// pjw.patch2.end


//******************************************************************
//******************************************************************
void recv_plrinfo(int pnum,const TCmdPlrInfoHdr * p,BOOL bAck) {
	// local player is always properly set up
	// and doesn't need to be unpacked
	if (myplr == pnum)
		return;

	app_assert((DWORD)pnum < MAX_PLRS);

	// check for out of order packet
	if (sgwPackPlrOffsetTbl[pnum] != p->wOffset) {
		sgwPackPlrOffsetTbl[pnum] = 0;
		if (p->wOffset != 0) return;
	}

	// if this packet came to us unrequested
	// and we haven't responded before,
	// respond by sending back our plrinfo
	if (! bAck && ! sgwPackPlrOffsetTbl[pnum])
		SendLocalPlayerInfo(pnum,CMD_ACK_PLRINFO);

	// copy the message information into a global structure
	// until we have received the entire packed player
	CopyMemory(
		((BYTE *)&sgPackPlr[pnum]) + p->wOffset,
		((BYTE *)p) + sizeof(TCmdPlrInfoHdr),
		p->wBytes
	);
	sgwPackPlrOffsetTbl[pnum] += p->wBytes;

	// did we get the entire pack structure yet?
	if (sgwPackPlrOffsetTbl[pnum] != sizeof(PkPlayerStruct))
		return;
	sgwPackPlrOffsetTbl[pnum] = 0;

	remove_active_player(pnum,FALSE);
	plr[pnum]._pGFXLoad = 0;
	UnPackPlayer(&sgPackPlr[pnum],pnum, TRUE);

	// if this packet was an acknowledgement to our request
	// for character information, then this character must
	// already be in the game, so activate him
	// Otherwise, this packet was being broadcast by a newbie
	// joining the game.  Wait until he sends a joinlevel command
	// before activating his character
	if (bAck) {
		plr[pnum].plractive = 1;
		gbActivePlayers++;

		// pjw.patch2.start
		sysmsg_add(
			sgbGameJoiner[pnum] ?
 			 "Player '%s' (level %d) just joined the game" :
			 "Player '%s' (level %d) is already in the game",
			plr[pnum]._pName,plr[pnum]._pLevel
		);
		// pjw.patch2.end

		LoadPlrGFX(pnum,PGL_STAND);
		SyncInitPlr(pnum);
		if (plr[pnum].plrlevel == currlevel) {
			if ((plr[pnum]._pHitPoints >> HP_SHIFT) > 0) {
				StartStand(pnum,0);
			} else {
				plr[pnum]._pgfxnum = PGFX_NGUY;
				LoadPlrGFX(pnum, PGL_DEAD);
				plr[pnum]._pmode = PM_DEATH;
				NewPlrAnim(pnum, plr[pnum]._pDAnim[DIR_D], plr[pnum]._pDFrames, 1, plr[pnum]._pDWidth);
				plr[pnum]._pAnimFrame = plr[pnum]._pAnimLen - 1;
				plr[pnum]._pVar8 = plr[pnum]._pAnimLen << 1;
				dFlags[plr[pnum]._px][plr[pnum]._py] |= BFLAG_DEADPLR;
			}
		}

		#if TRACEOUT
		TraceOut("(%d) making %d active -- recv_plrinfo",myplr,pnum);
		#endif
	}
	else {
		#if TRACEOUT
		TraceOut("(%d) received all %d plrinfo",myplr,pnum);
		#endif
	}
}
