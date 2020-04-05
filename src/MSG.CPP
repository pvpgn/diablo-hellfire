//******************************************************************
// msg.cpp
//******************************************************************

/* dig.patch1.start.1/30/97
	Replaced all occurrences of "DObject" with "DObjectStr"
	Replaced all occurrences of "DMonster" with "DMonsterStr"
	This was done because of a debugger bug.  The debugger was
	not able to distinguish between the global variable dObject
	and the structure name DObject.
	The change should have no effect on the executable.
   dig.patch1.end.1/30/97
*/

/* PATCH1.JMM
    Added extensive logging to the msg handlers.  All droplogs do not 
	compile in non-_DEBUG compiles.
*/

#include        "diablo.h"
#pragma hdrstop
#include        "sound.h"
#include        "msg.h"
#include        "multi.h"
#include        "engine.h"
#include        "gendung.h"
#include        "items.h"
#include        "player.h"
#include        "monster.h"
#include        "monstdat.h"
#include        "inv.h"
#include        "lighting.h"
#include        "objects.h"
#include        "objdat.h"
#include        "spells.h"
#include		"spelldat.h"
#include        "control.h"
#include        "dead.h"
#include        "storm.h"
#include        "diabloui.h"
#include        "cursor.h"
#include        "automap.h"
#include        "portal.h"
#include        "quests.h"
#include        "missiles.h"
#include		"drlg_l1.h"
#include		"trigs.h"
#include		"effects.h"

/*-----------------------------------------------------------------------**
** extern
**-----------------------------------------------------------------------*/
DWORD Compress(LPBYTE data, DWORD bytes);
void Expand(LPBYTE data, DWORD bytes, DWORD dwMaxBytes);
void M_ClearSquares(int nMonster);
void Obj_Trap(int i);
void StartStand(int pnum, int dir);
void SyncPlrAnim(int p);
void SyncInitPlr(int pnum);
void SendLocalPlayerInfo(int pnum,BYTE bCmd);
void recv_plrinfo(int pnum,const TCmdPlrInfoHdr * p,BOOL bAck);
void dthread_SendPlayerInfoChunk(int pnum,BYTE bCmd,const BYTE * pbSrc,DWORD dwLen);
void plrmsg_add(int pnum,const char * pszStr);
void M_UpdateLeader(int i);
void gamemenu_off();
void DeleteItem(int ii, int i);
void SyncPortal(int i, BOOL open, int x, int y, int level, int ltype);
void __cdecl sysmsg_add(const char * pszFmt,...);
void NextPlrLevel(int pnum);
void nthread_set_delta_request();
void NewPlrAnim(int pnum, BYTE *pAnim, int numFrames, int Delay, long width);
void AddWarpMissile(int i, int x, int y);

// JMM.PATCH1.2.22.97
void sysmsg_add_string(const char * pszMsg);
// END.JMM.PATCH1.2.22.97

void OpenNaKrul();
void OpenNest();
void OpenCrypt();

/*-----------------------------------------------------------------------**
** public
**-----------------------------------------------------------------------*/
BOOL deltaload = FALSE;
BYTE gbBufferMsgs = BUFFER_OFF;


/*-----------------------------------------------------------------------**
** private
**-----------------------------------------------------------------------*/
#pragma pack(push,1)

typedef struct TCmdLoc {
	BYTE bCmd;
	BYTE x;
	BYTE y;
} TCmdLoc;

typedef struct TCmdLocParam1 {
	BYTE bCmd;
	BYTE x;
	BYTE y;
	WORD wParam1;
} TCmdLocParam1;

typedef struct TCmdLocParam2 {
	BYTE bCmd;
	BYTE x;
	BYTE y;
	WORD wParam1;
	WORD wParam2;
} TCmdLocParam2;

typedef struct TCmdLocParam3 {
	BYTE bCmd;
	BYTE x;
	BYTE y;
	WORD wParam1;
	WORD wParam2;
	WORD wParam3;
} TCmdLocParam3;

typedef struct TCmdParam1 {
	BYTE bCmd;
	WORD wParam1;
} TCmdParam1;

typedef struct TCmdParam2 {
	BYTE bCmd;
	WORD wParam1;
	WORD wParam2;
} TCmdParam2;

typedef struct TCmdParam3 {
	BYTE bCmd;
	WORD wParam1;
	WORD wParam2;
	WORD wParam3;
} TCmdParam3;

typedef struct TCmdString {
	BYTE bCmd;
	char str[MAX_SEND_STR_LEN];
} TCmdString;

typedef struct TCmdGolem {
	BYTE bCmd;
	BYTE _mx;
	BYTE _my;
	BYTE _mdir;
	BYTE _menemy;
	long _mhitpoints;
	BYTE _currlevel;
} TCmdGolem;

typedef struct TCmdQuest {
	BYTE bCmd;
	BYTE q;
	BYTE qstate;
	BYTE qlog;
	BYTE qvar1;
} TCmdQuest;

typedef struct TCmdGItem {
	BYTE bCmd;
	BYTE bMaster;
	BYTE bPnum;
	BYTE bCursitem;
	// drb.patch1.start.2/05/97
	BYTE bLevel;
	// drb.patch1.end.2/05/97
	BYTE x;
	BYTE y;
	WORD wIndx;
	WORD wCI;
	DWORD dwSeed;
	BYTE bId;
	BYTE bDur;
	BYTE bMDur;
	BYTE bCh;
	BYTE bMCh;
	WORD wValue;
	DWORD dwBuff;
	DWORD dwTime;
	WORD wPLToHit;
	WORD wMaxDam;
	BYTE bMinStr;
	BYTE bMinMag;
	BYTE bMinDex;
	BYTE bAC;
} TCmdGItem;

typedef struct TCmdPItem {
	BYTE bCmd;
	BYTE x;
	BYTE y;
	WORD wIndx;
	WORD wCI;
	DWORD dwSeed;
	BYTE bId;
	BYTE bDur;
	BYTE bMDur;
	BYTE bCh;
	BYTE bMCh;
	WORD wValue;
	DWORD dwBuff;
	WORD wPLToHit;
	WORD wMaxDam;
	BYTE bMinStr;
	BYTE bMinMag;
	BYTE bMinDex;
	BYTE bAC;
} TCmdPItem;
typedef struct TCmdChItem {
	BYTE bCmd;
	BYTE bLoc;
	WORD wIndx;
	WORD wCI;
	DWORD dwSeed;
	// drb.patch1.start.02/10/97
	BYTE bId;
	// drb.patch1.end.02/10/97
} TCmdChItem;

typedef struct TCmdDelItem {
	BYTE bCmd;
	BYTE bLoc;
} TCmdDelItem;


typedef struct TCmdDamage{
	BYTE bCmd;
	BYTE bPlr;
	DWORD dwDam;
} TCmdDamage;

typedef struct TCmdMonstDamage{
	BYTE bCmd;
	WORD wMonst;
	DWORD dwDam;
} TCmdMonstDamage;

typedef struct TFakeCmdPlr {
	BYTE bCmd;
	BYTE bPlr;
} TFakeCmdPlr;

typedef struct TFakeDropPlr {
	BYTE bCmd;
	BYTE bPlr;
	DWORD dwReason;
} TFakeDropPlr;
// stuff that is on every level

typedef struct DMonsterStr {
	BYTE _mx;
	BYTE _my;
	BYTE _mdir;
	BYTE _menemy;
	BYTE _msquelch;
	long _mhitpoints;
} DMonsterStr;

typedef struct DObjectStr {
	BYTE bCmd;
} DObjectStr;

typedef struct DLevel {
	TCmdPItem item[MAXITEMS];
	DObjectStr object[MAXOBJECTS];
	DMonsterStr monster[MAXMONSTERS];
} DLevel;

typedef struct LocalLevel {
	BYTE automapsv[AUTOMAPX][AUTOMAPY];
} LocalLevel;

// stuff which is dungeon global
typedef struct DPortal {
	BYTE x;
	BYTE y;
	BYTE level;
	BYTE ltype;
	BYTE setlvl;
} DPortal;
typedef struct MultiQuests {
	BYTE qstate;
	BYTE qlog;
	BYTE qvar1;
} MultiQuests;
typedef struct DJunk {
	DPortal portal[MAXPORTAL];
	MultiQuests quests[MAXMULTIQUESTS + 1];
} DJunk;

#pragma pack(pop)

#define INIT_VAL 0xff
static DJunk sgJunk;
static DLevel sgLevels[NUMLEVELS];              // sent to other players
static LocalLevel sgLocals[NUMLEVELS];  // not sent

// number of chunks of info we must receive to get all delta info
// 16 levels + 1 town level = NUMLEVELS
// + 1 global "junk" chunk
// + 1 terminator
// + 1 startup
// + 1 "99%" complete
#define DELTA_CHUNKS (NUMLEVELS+1+1+1+1)

// allocate extra space to account for flag byte
#define SEND_LEVEL_SIZE (max(sizeof(DLevel),sizeof(DJunk)) + 1)

// buffer to receive level information
static BYTE sgbRecvLevel[SEND_LEVEL_SIZE];
static BYTE sgbRecvCmd;
static DWORD sgdwRecvOffset;
static BYTE sgbDeltaChunks;
static BYTE sgbDeltaChanged;
static DWORD sgdwOwnerWait;


// item flags which go into sgLevel[].item[].bCmd
	// item generated on floor of dungeon -- presently on floor
	#define ITEM_GEN_FLOOR 0

	// item generated on floor of dungeon -- presently not on floor
	#define ITEM_GEN_TAKEN 1

	// item was dropped -- presently on floor
	#define ITEM_DROP_FLOOR 2

	// we don't have to keep track of items which were not originally
	// part of the dungeon and were dropped and then picked up

	// item slot not in use
	#define ITEM_FREE INIT_VAL


//******************************************************************
//******************************************************************
// time to keep resending messages if they don't make sense yet
#define RESEND_TIME             5000    // milliseconds


//******************************************************************
// delta buffering
//******************************************************************
#pragma pack(push,1)

// this structure contains a whole bunch of link packets
typedef struct TMegaPkt {
	struct TMegaPkt * pNext;
	DWORD dwSpaceLeft;
	BYTE data[32000];
} TMegaPkt;

#pragma pack(pop)

static TMegaPkt * sgpMegaPkt;
static TMegaPkt * sgpCurrPkt;
static int sgnCurrMegaPlayer;

// PATCH1.JMM
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

DWORD dwRecCount = 0;

#if _DEBUG
void __cdecl DROPLOG(const char * pszFmt,...) {
	extern BOOL gbDumpDropLog;
	if (! gbDumpDropLog) return;
	static FILE * f = NULL;
	if (! f) f = fopen("c:\\droplog.txt","wb");
	if (! f) return;

	va_list args;
	va_start(args,pszFmt);
	fprintf(f,"(%8.8x,%2.2d):",dwRecCount,numitems);
	vfprintf(f,pszFmt,args);
	fflush(f);
	va_end(args);
}
#else
#define DROPLOG //
#endif
// ENDPATCH1.JMM



//******************************************************************
//******************************************************************
static void mega_add_pkt() {
	sgpCurrPkt = (TMegaPkt *) DiabloAllocPtrSig(sizeof(TMegaPkt),'MEGA');
	sgpCurrPkt->pNext = NULL;
	sgpCurrPkt->dwSpaceLeft = sizeof(sgpCurrPkt->data);

	TMegaPkt ** ppCurr = &sgpMegaPkt;
	while (*ppCurr) ppCurr = &(*ppCurr)->pNext;
	*ppCurr = sgpCurrPkt;
}


//******************************************************************
//******************************************************************
static void mega_free_pkts() {
	while (sgpMegaPkt) {
		sgpCurrPkt = sgpMegaPkt->pNext;
		DiabloFreePtr(sgpMegaPkt);
		sgpMegaPkt = sgpCurrPkt;
	}

	// NOTE: both sgpMegaPkt & sgpCurrPkt are NULL
}


//******************************************************************
//******************************************************************
static void mega_run_pkts() {
	int nPlayer = -1;
	for (TMegaPkt * pMega = sgpMegaPkt; pMega; pMega = pMega->pNext) {
		const BYTE * pbData = pMega->data;
		DWORD dwBytes = sizeof(pMega->data);
		while (dwBytes != pMega->dwSpaceLeft) {
			if (*pbData == FAKE_CMD_SETID) {
				const TFakeCmdPlr * p = (const TFakeCmdPlr *) pbData;
				pbData += sizeof(TFakeCmdPlr);
				dwBytes -= sizeof(TFakeCmdPlr);
				nPlayer = p->bPlr;
			}
			else if (*pbData == FAKE_CMD_DROPID) {
				const TFakeDropPlr * p = (const TFakeDropPlr *) pbData;
				pbData += sizeof(TFakeDropPlr);
				dwBytes -= sizeof(TFakeDropPlr);
				void unbuffer_remove_player(int pnum,DWORD dwReason);
				unbuffer_remove_player(p->bPlr,p->dwReason);
			}
			else {
				app_assert((DWORD) nPlayer < MAX_PLRS);
				const TCmd * p = (const TCmd *) pbData;
				DWORD dwTemp = ParseCmd(nPlayer,p);
				pbData += dwTemp;
				dwBytes -= dwTemp;
			}
		}
	}
}


//******************************************************************
//******************************************************************
static void mega_add_data(int pnum,const void * pMsg,DWORD dwLen) {
	app_assert((DWORD) pnum < MAX_PLRS);
	app_assert(pMsg);
	app_assert(dwLen <= gdwLargestMsgSize);

	if (pnum != sgnCurrMegaPlayer) {
		// add a command to set the current player
		TFakeCmdPlr cmd;
		sgnCurrMegaPlayer = pnum;
		cmd.bCmd = FAKE_CMD_SETID;
		cmd.bPlr = (BYTE) pnum;
		mega_add_data(pnum,&cmd,sizeof(cmd));
	}

	// if there isn't enough space in the current megapkt, add another
	app_assert(sgpCurrPkt);
	if (sgpCurrPkt->dwSpaceLeft < dwLen)
		mega_add_pkt();
	app_assert(sgpCurrPkt->dwSpaceLeft >= dwLen);

	// add data to megapacket
	CopyMemory(
		sgpCurrPkt->data + sizeof(sgpCurrPkt->data) - sgpCurrPkt->dwSpaceLeft,
		pMsg,
		dwLen
	);
	sgpCurrPkt->dwSpaceLeft -= dwLen;
}


//******************************************************************
//******************************************************************
void buffer_drop_player(int pnum,DWORD dwReason) {
	TFakeDropPlr cmd;
	cmd.bCmd = FAKE_CMD_DROPID;
	cmd.bPlr = (BYTE) pnum;
	cmd.dwReason = dwReason;
	mega_add_data(pnum,&cmd,sizeof(cmd));
}


//******************************************************************
//******************************************************************
// pjw.patch2.start
static int CALLBACK delta_progress() {
	if (sgbDeltaChunks == 0) {
		// wait until we receive master player's future
		// turn before we process our first turn.  This is
		// designed so that we don't start several turns ahead
		// of the master player and stay there forever.  We are
		// waiting for turn X from all players.  We wait for 
		// turn (X+gdwTurnsInTransit) from the master player, which
		// means that the master player processed turn X and has
		// sent turns X+1 and X+2.  When we process turn X, we will be
		// synced with the master player less the one-way packet 
		// latency from the master player.
		DWORD dwTurns;
		nthread_fill_sync_queue(0,0);
		if (! SNetGetOwnerTurnsWaiting(&dwTurns)) {
		    if (GetLastError() == SNET_ERROR_NOT_IN_GAME)
				return 100;
		}

		// wait a maximum of 2 seconds before we give
		// up syncing and just try for whatever we can get
		// if a turn got dropped by the owner, it is possible
		// we could wait forever for the owner, while he waits
		// forever for us to resend the dropped turn.
		if (GetTickCount() - sgdwOwnerWait <= 2000) {
			if (dwTurns < gdwTurnsInTransit)
				return 0;
		}

		sgbDeltaChunks++;
	}

	// handle asynchronous messages
	NetReceivePackets();

	// handle synchronous messages
	BOOL bSendAsync;
	nthread_fill_sync_queue(0,0);
	// pjw.patch2.start -- if we call nthread_msg_check() without
	// protection against "overcalling" then the gameclock will get
	// continually reset and other systems will drop into timeout mode
	// nthread_msg_check(&bSendAsync);
	if (nthread_run_gameloop(FALSE)) nthread_msg_check(&bSendAsync);
	// pjw.patch2.end

	// if the game was destroyed, return "100% complete" to exit dialog
	if (gbGameDestroyed) return 100;

	// if the person sending us delta info dropped out,
	// then we need to re-request the delta information
	if (gbDeltaSender >= MAX_PLRS) {
		sgbRecvCmd = CMD_DLEVEL_END;
		sgbDeltaChunks = 0;
		gbDeltaSender = (BYTE) myplr;
		nthread_set_delta_request();
	}

	// make bar "full"
	if (sgbDeltaChunks == DELTA_CHUNKS - 1) {
		sgbDeltaChunks++;
		return 99;
	}

	// return completion status
	return (sgbDeltaChunks * 100) / DELTA_CHUNKS;
}
// pjw.patch2.end


//******************************************************************
//******************************************************************
BOOL wait_delta_info() {
	app_assert(ghMainWnd);
	app_assert(! sgpMegaPkt);
	app_assert(! sgpCurrPkt);
	mega_add_pkt();

	sgbRecvCmd = CMD_DLEVEL_END;
	sgbDeltaChunks = 0;
	gbBufferMsgs = BUFFER_ON;
	sgnCurrMegaPlayer = -1;
	sgdwOwnerWait = GetTickCount();

	BOOL bResult = UiProgressDialog(
		ghMainWnd,
		"Waiting for game data...",
		TRUE,                                   // allow abort
		delta_progress,
		GAME_FRAMES_PER_SECOND  // calls per second
	);

	// done buffering messages, but don't go into BUFFER_PROCESS
	// mode until we've successfully loaded the level
	gbBufferMsgs = BUFFER_OFF;

	if (! bResult) {
		// no message -- user canceled
		mega_free_pkts();
		return FALSE;
	}
	else if (gbGameDestroyed) {
		app_warning("The game ended");
		mega_free_pkts();
		return FALSE;
	}
	else if (sgbDeltaChunks != DELTA_CHUNKS) {
		app_warning("Unable to get level data");
		mega_free_pkts();
		return FALSE;
	}

	// leave the delta information around
	// until the game level has been loaded
	// and then run all the accumulated packets
	return TRUE;
}


//******************************************************************
//******************************************************************
void run_delta_info() {
	if (gbMaxPlayers == 1) return;

	app_assert(currlevel == 0);
	app_assert(plr[myplr].plrlevel == 0);

	gbBufferMsgs = BUFFER_PROCESS;
	mega_run_pkts();
	gbBufferMsgs = BUFFER_OFF;

	mega_free_pkts();
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BYTE * dbuild_items(BYTE * pbData,const TCmdPItem * pItem) {
	for (int i = 0; i < MAXITEMS; i++,pItem++) {
		if (pItem->bCmd == INIT_VAL) {
			*pbData++ = INIT_VAL;
		}
		else {
			//CopyMemory(pbData,pItem,sizeof(TCmdPItem));
			*(reinterpret_cast<TCmdPItem *>(pbData)) = *pItem;
			pbData += sizeof(TCmdPItem);
		}
	}

	return pbData;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static const BYTE * dparse_items(const BYTE * pbData,TCmdPItem * pItem) {
	for (int i = 0; i < MAXITEMS; i++,pItem++) {
		if (*pbData == INIT_VAL) {
			FillMemory(pItem,sizeof(TCmdPItem),INIT_VAL);
			pbData++;
		}
		else {
			//CopyMemory(pItem,pbData,sizeof(TCmdPItem));
			*pItem = *(reinterpret_cast<const TCmdPItem *>(pbData));
			pbData += sizeof(TCmdPItem);
		}
	}

	return pbData;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BYTE * dbuild_objects(BYTE * pbData,const DObjectStr * pObj) {
	// the object structure is only one byte, so just
	// rely upon data compression to make it smaller
	CopyMemory(pbData,pObj,MAXOBJECTS * sizeof(DObjectStr));
	return pbData + MAXOBJECTS * sizeof(DObjectStr);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static const BYTE * dparse_objects(const BYTE * pbData,DObjectStr * pObj) {
	// the object structure is only one byte, so just
	// rely upon data compression to make it smaller
	CopyMemory(pObj,pbData,MAXOBJECTS * sizeof(DObjectStr));
	return pbData + MAXOBJECTS * sizeof(DObjectStr);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BYTE * dbuild_monsters(BYTE * pbData,const DMonsterStr * pM) {
	for (int m = 0; m < MAXMONSTERS; m++,pM++) {
		if (pM->_mx == INIT_VAL) {
			*pbData++ = INIT_VAL;
		}
		else {
			//CopyMemory(pbData,pM,sizeof(DMonsterStr));
			*(reinterpret_cast<DMonsterStr *>(pbData)) = *pM;
			pbData += sizeof(DMonsterStr);
		}
	}

	return pbData;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static const BYTE * dparse_monsters(const BYTE * pbData,DMonsterStr * pM) {
	for (int m = 0; m < MAXMONSTERS; m++,pM++) {
		if (*pbData == INIT_VAL) {
			FillMemory(pM,sizeof(DMonsterStr),INIT_VAL);
			pbData++;
		}
		else {
			//CopyMemory(pM,pbData,sizeof(DMonsterStr));
			*pM = *(reinterpret_cast<const DMonsterStr *> (pbData));
			pbData += sizeof(DMonsterStr);
		}
	}

	return pbData;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
extern QuestData questlist[];

static BYTE * dbuild_junk(BYTE * pbData) {
	for (int i = 0; i < MAXPORTAL; i++) {
		if (sgJunk.portal[i].x == INIT_VAL) {
			*pbData++ = INIT_VAL;
		}
		else {
			//CopyMemory(pbData,&sgJunk.portal[i],sizeof(DPortal));
			*(reinterpret_cast<DPortal *>(pbData)) = sgJunk.portal[i];
			pbData += sizeof(DPortal);
		}
	}
	i = 0;
	for (int q = 0; q < MAXQUESTS; q++) {
		if (questlist[q]._qflags & QFLAG_MULTI) {
			sgJunk.quests[i].qlog = quests[q]._qlog;
			sgJunk.quests[i].qstate = quests[q]._qactive;
			sgJunk.quests[i].qvar1 = quests[q]._qvar1;
			//CopyMemory(pbData,&sgJunk.quests[i],sizeof(MultiQuests));
			*(reinterpret_cast<MultiQuests *>(pbData)) = sgJunk.quests[i];
			pbData += sizeof(MultiQuests);
			i++;
		}
	}

	return pbData;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void dparse_junk(const BYTE * pbData) {
	int i;
	for (i = 0; i < MAXPORTAL; i++) {
		if (*pbData == INIT_VAL) {
			FillMemory(&sgJunk.portal[i],sizeof(DPortal),INIT_VAL);
			pbData++;
			SyncPortal(i, FALSE, 0, 0, 0, 0);
		}
		else {
			//CopyMemory(&sgJunk.portal[i],pbData,sizeof(DPortal));
			sgJunk.portal[i] = *(reinterpret_cast<const DPortal *>(pbData));
			pbData += sizeof(DPortal);
			SyncPortal(i, TRUE, sgJunk.portal[i].x, sgJunk.portal[i].y, sgJunk.portal[i].level, sgJunk.portal[i].ltype);
		}
	}
	i = 0;
	for (int q = 0; q < MAXQUESTS; q++) {
		if (questlist[q]._qflags & QFLAG_MULTI) {
			//CopyMemory(&sgJunk.quests[i],pbData,sizeof(MultiQuests));
			sgJunk.quests[i] = *(reinterpret_cast<const MultiQuests *>(pbData));
			pbData += sizeof(MultiQuests);
			quests[q]._qlog = sgJunk.quests[i].qlog;
			quests[q]._qactive = sgJunk.quests[i].qstate;
			quests[q]._qvar1 = sgJunk.quests[i].qvar1;
			i++;
		}
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD compress_chunk(BYTE * pbStart,BYTE * pbEnd) {
	// calculate length excluding leading byte
	DWORD dwLen = pbEnd - pbStart - 1;

	// compress data excluding leading byte
	DWORD dwBytes = Compress(pbStart + 1,dwLen);

	// set compress flag ==> TRUE = compressed
	*pbStart = (dwLen != dwBytes);

	// add one extra byte for compress flag
	return dwBytes + 1;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DeltaSendAllLevels(int pnum) {
	// send all delta information to player pnum
	if (sgbDeltaChanged) {
		BYTE * pbData;
		DWORD dwBytes;
		BYTE * pbBase = (BYTE *) DiabloAllocPtrSig(SEND_LEVEL_SIZE,'DLTt');
		for (int bLevel = 0; bLevel < NUMLEVELS; bLevel++) {
			// save 1 byte for compress/uncompress flag
			pbData = pbBase + 1;

			// build level data
			pbData = dbuild_items(pbData,&sgLevels[bLevel].item[0]);
			pbData = dbuild_objects(pbData,&sgLevels[bLevel].object[0]);
			pbData = dbuild_monsters(pbData,&sgLevels[bLevel].monster[0]);
			dwBytes = compress_chunk(pbBase,pbData);
			dthread_SendPlayerInfoChunk(pnum,CMD_DLEVEL_0 + bLevel,pbBase,dwBytes);
		}

		// send junk chunk -- save one byte for compress/uncompress flag
		pbData = dbuild_junk(pbBase + 1);
		dwBytes = compress_chunk(pbBase,pbData);
		dthread_SendPlayerInfoChunk(pnum,CMD_DLEVEL_JUNK,pbBase,dwBytes);
		DiabloFreePtr(pbBase);
	}

	// send 1 byte terminator chunk
	BYTE c = 0;     // mark as "uncompressed"
	dthread_SendPlayerInfoChunk(pnum,CMD_DLEVEL_END,&c,1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void dparse_chunk(BYTE bMsg,DWORD dwLen) {
	// first byte is a flag which indicates whether the data is compressed
	if (sgbRecvLevel[0]) Expand(&sgbRecvLevel[1],dwLen,sizeof(sgbRecvLevel) - 1);
	const BYTE * pbData = &sgbRecvLevel[1];

	if (bMsg == CMD_DLEVEL_JUNK) {
		dparse_junk(pbData);
	}
	else if (bMsg >= CMD_DLEVEL_0 && bMsg <= CMD_DLEVEL_24) {
		BYTE bLevel = bMsg - CMD_DLEVEL_0;
		pbData = dparse_items(pbData,&sgLevels[bLevel].item[0]);
		pbData = dparse_objects(pbData,&sgLevels[bLevel].object[0]);
		pbData = dparse_monsters(pbData,&sgLevels[bLevel].monster[0]);
	}
	else {
		app_fatal("msg:1");
	}

	sgbDeltaChanged = TRUE;
	sgbDeltaChunks++;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD dreceive_chunk(int pnum,const TCmdPlrInfoHdr * p) {

	if (gbDeltaSender != pnum) {
		// we are unexpectedly receiving delta info from somebody
		// if they aren't sending us the starting information, ignore it
		// otherwise, make them the new delta source
		if (p->bCmd == CMD_DLEVEL_END) {
			// we're getting an end chunk without any intervening delta
			// which means there is no delta information
		}
		else if (p->bCmd != CMD_DLEVEL_0 || p->wOffset != 0) {
			return p->wBytes + sizeof(TCmdPlrInfoHdr);
		}
		gbDeltaSender = (BYTE) pnum;
		sgbRecvCmd = CMD_DLEVEL_END;
	}

	if (sgbRecvCmd == CMD_DLEVEL_END) {
		if (p->bCmd == CMD_DLEVEL_END) {
			// we received the end command immediately, which 
			// means there was no level delta info
			sgbDeltaChunks = DELTA_CHUNKS - 1;      // set 99% done
			return p->wBytes + sizeof(TCmdPlrInfoHdr);
		}

		if (p->bCmd != CMD_DLEVEL_0 || p->wOffset != 0) {
			// someone is in the middle of sending us info
			// but we don't have the beginning
			// just ignore the message
			return p->wBytes + sizeof(TCmdPlrInfoHdr);
		}

		// we just started receiving new data from the player
		sgdwRecvOffset = 0;
		sgbRecvCmd = p->bCmd;
	}
	else if (sgbRecvCmd != p->bCmd) {
		// since we are receiving a new delta chunk, 
		// we must have finished receiving the last delta chunk.
		// process last chunk before handling next chunk.
		// ???dparse_chunk(sgbRecvCmd,sgdwRecvOffset + p->wBytes);
		dparse_chunk(sgbRecvCmd,sgdwRecvOffset);

		// what's the new command?
		if (p->bCmd == CMD_DLEVEL_END) {
			// set 99% done flag
			sgbDeltaChunks = DELTA_CHUNKS - 1;
			sgbRecvCmd = CMD_DLEVEL_END;
			return p->wBytes + sizeof(TCmdPlrInfoHdr);
		}
			
		sgdwRecvOffset = 0;
		sgbRecvCmd = p->bCmd;
	}

	app_assert(p->wOffset == sgdwRecvOffset);
	CopyMemory( &sgbRecvLevel[p->wOffset],
				((BYTE *)p) + sizeof(TCmdPlrInfoHdr),
				p->wBytes
				);

	sgdwRecvOffset += p->wBytes;
	return p->wBytes + sizeof(TCmdPlrInfoHdr);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void delta_init() {
	sgbDeltaChanged = FALSE;
	FillMemory(&sgJunk,sizeof sgJunk,INIT_VAL);
	FillMemory(&sgLevels[0],sizeof sgLevels,INIT_VAL);
	ZeroMemory(sgLocals,sizeof sgLocals);
	deltaload = FALSE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void delta_kill_monster(int mi, BYTE x, BYTE y, BYTE bLevel) {
	if (gbMaxPlayers == 1) return;
	app_assert((DWORD)mi < MAXMONSTERS);
	app_assert(x < DMAXX);
	app_assert(y < DMAXY);
	app_assert(bLevel < NUMLEVELS);

	sgbDeltaChanged = TRUE;
	DMonsterStr * p = &sgLevels[bLevel].monster[mi];
	p->_mx = x;
	p->_my = y;
	p->_mdir = monster[mi]._mdir;
	p->_mhitpoints = 0;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void delta_monster_hp(int mi,long hp,BYTE bLevel) {
	if (gbMaxPlayers == 1) return;
	app_assert((DWORD)mi < MAXMONSTERS);
	app_assert(bLevel < NUMLEVELS);

	sgbDeltaChanged = TRUE;
	DMonsterStr * p = &sgLevels[bLevel].monster[mi];
	if (p->_mhitpoints > hp) p->_mhitpoints = hp;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void delta_sync_monster(const TSyncMonster * pSync,BYTE bLevel) {
	if (gbMaxPlayers == 1) return;
	app_assert(pSync != NULL);
	app_assert(bLevel < NUMLEVELS);

	sgbDeltaChanged = TRUE;
	DMonsterStr * pD = &sgLevels[bLevel].monster[pSync->_mndx];

	// is this monster already dead?
	if (! pD->_mhitpoints) return;

	pD->_mx = pSync->_mx;
	pD->_my = pSync->_my;
	pD->_msquelch = 255;

	//app_assert((DWORD) pSync->_menemy < MAX_PLRS);   @@@ Taken out becasue monster enemy could be a monster
	pD->_menemy = pSync->_menemy;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void delta_sync_golem(const TCmdGolem * pG, int pnum, BYTE bLevel) {
	if (gbMaxPlayers == 1) return;

	app_assert(bLevel < NUMLEVELS);
	app_assert(bLevel != 0);

	sgbDeltaChanged = TRUE;
	DMonsterStr * pD = &sgLevels[bLevel].monster[pnum];

	pD->_mx = pG->_mx;
	pD->_my = pG->_my;
	pD->_msquelch = 255;
	pD->_menemy = pG->_menemy;
	pD->_mdir = pG->_mdir;
	pD->_mhitpoints = pG->_mhitpoints;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void delta_leave_sync(BYTE bLevel) {
	if (gbMaxPlayers == 1) return;

	// recycle town store seeds
	if (currlevel == 0) glSeedTbl[0] = GetRndSeed();
	if (currlevel <= 0) return;
	app_assert(bLevel < NUMLEVELS);

	for (int i = 0; i < nummonsters; i++) {
		int ii = monstactive[i];
		if (monster[ii]._mhitpoints == 0) continue;

		sgbDeltaChanged = TRUE;
		DMonsterStr * pD = &sgLevels[bLevel].monster[ii];
		pD->_mx = monster[ii]._mx;
		pD->_my = monster[ii]._my;
		pD->_mdir = monster[ii]._mdir;
		pD->_menemy = encode_enemy(ii);
		//app_assert((DWORD) pD->_menemy < MAX_PLRS);
		pD->_mhitpoints = monster[ii]._mhitpoints;
		pD->_msquelch = monster[ii]._msquelch;
	}

	// copy automap
	app_assert(sizeof(sgLocals[bLevel].automapsv) == sizeof(automapview));
	CopyMemory(
		&sgLocals[bLevel].automapsv[0][0],
		&automapview[0][0],
		sizeof(sgLocals[bLevel].automapsv)
	);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void delta_sync_object(int oi,BYTE bCmd,BYTE bLevel) {
	if (gbMaxPlayers == 1) return;
	app_assert((DWORD)oi < MAXOBJECTS);
	app_assert(bLevel < NUMLEVELS);

	sgbDeltaChanged = TRUE;
	DObjectStr * p = &sgLevels[bLevel].object[oi];
	p->bCmd = bCmd;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL delta_get_item(const TCmdGItem * pI, BYTE bLevel) {
	if (gbMaxPlayers == 1) return(TRUE);
	app_assert(pI != NULL);
	app_assert(bLevel < NUMLEVELS);

	TCmdPItem * pD = &sgLevels[bLevel].item[0];
	for (int i = 0; i < MAXITEMS; i++,pD++) {

		// find item
		if (pD->bCmd == ITEM_FREE) continue;
		if (pD->wIndx != pI->wIndx) continue;
		if (pD->wCI != pI->wCI) continue;
		if (pD->dwSeed != pI->dwSeed) continue;

		if (pD->bCmd == ITEM_GEN_TAKEN) {
			// PATCH1.JMM
			DROPLOG("  delta_get_item:  item already picked up?!\n");
			// ENDPATCH1.JMM
			// someone else already picked it up?!
			return(TRUE);
		}
		else if (pD->bCmd == ITEM_GEN_FLOOR) {
			// pick item up from floor, but keep track
			// of the fact that it is a generated item
			// PATCH1.JMM
			DROPLOG("  delta_get_item:  native item picked up from floor\n");
			// ENDPATCH1.JMM

			sgbDeltaChanged = TRUE;
			pD->bCmd = ITEM_GEN_TAKEN;
			return(TRUE);
		}
		else if (pD->bCmd == ITEM_DROP_FLOOR) {
			// since the item was dropped on the floor but
			// not part of the generated dungeon, as soon
			// as it is picked up we can forget about it
			// PATCH1.JMM
			DROPLOG("  delta_get_item:  foreign item picked up from floor\n");
			// ENDPATCH1.JMM

			sgbDeltaChanged = TRUE;
			pD->bCmd = ITEM_FREE;
			return(TRUE);
		}
		else {
			app_fatal("delta:1");
		}
		break;
	}


	// PATCH1.JMM
	DROPLOG("  delta_get_item:  item not found\n");
	// ENDPATCH1.JMM
	// If we are trying to remove an item from the delta list and it is not a dungeon
	// pregenerated item, then forget it. Chances are that it was a message sent and
	// buffered before the level delta was passed to us. So the level delta has the
	// change in it already and we are processing the buffered message which is just
	// duplicating what is already there.
	// The above is true, or it could be we are processing the pickup before the drop
	// so, we will be resending this message to ourselves
	if (!(pI->wCI & ICI_PREGEN)) 
		return(FALSE);

	//app_assert(bLevel > 0);
	// If not found, then an item generated by a level that I have not been to yet
	pD = &sgLevels[bLevel].item[0];
	for (i = 0; i < MAXITEMS; i++,pD++) {

		// Free slot?
		if (pD->bCmd != ITEM_FREE) continue;

		// put the item into the delta table
		// PATCH1.JMM
		DROPLOG("  delta_get_item:  added item to delta tbl");
		// ENDPATCH1.JMM

		sgbDeltaChanged = TRUE;
		pD->bCmd = ITEM_GEN_TAKEN;
		pD->x = pI->x;
		pD->y = pI->y;
		pD->wIndx = pI->wIndx;
		pD->wCI = pI->wCI;
		pD->dwSeed = pI->dwSeed;
		pD->bId = pI->bId;
		pD->bDur = pI->bDur;
		pD->bMDur = pI->bMDur;
		pD->bCh = pI->bCh;
		pD->bMCh = pI->bMCh;
		pD->wValue = pI->wValue;
		pD->dwBuff = pI->dwBuff;
		pD->wPLToHit = pI->wPLToHit;
		pD->wMaxDam = pI->wMaxDam;
		pD->bMinStr = pI->bMinStr;
		pD->bMinMag = pI->bMinMag;
		pD->bMinDex = pI->bMinDex;
		pD->bAC = pI->bAC;
		return(TRUE);
	}
	// PATCH1.JMM
	DROPLOG("  delta_get_item:  could not add item to delta tbl");
	// ENDPATCH1.JMM
	return(TRUE);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void delta_put_item(const TCmdPItem * pI, int x, int y, BYTE bLevel) {
	if (gbMaxPlayers == 1) return;
	app_assert(pI != NULL);
	app_assert(x < DMAXX);
	app_assert(y < DMAXY);
	app_assert(bLevel < NUMLEVELS);

	// see if the item was already part of the generated dungeon
	TCmdPItem * pD = &sgLevels[bLevel].item[0];
	for (int i = 0; i < MAXITEMS; i++,pD++) {
		if (pD->bCmd == ITEM_GEN_TAKEN) continue;
		if (pD->bCmd == ITEM_FREE) continue;
		if (pD->wIndx != pI->wIndx) continue;
		if (pD->wCI != pI->wCI) continue;
		if (pD->dwSeed != pI->dwSeed) continue;

		// Already placed?
		if (pD->bCmd == ITEM_DROP_FLOOR) {
			return;
		}

		app_fatal("Trying to drop a floor item?");
	}

	// find a location to drop this item
	pD = &sgLevels[bLevel].item[0];
	for (i = 0; i < MAXITEMS; i++,pD++) {
		if (pD->bCmd != ITEM_FREE) continue;

		// put the item back onto the floor at its 
		// new location and with its new state

		sgbDeltaChanged = TRUE;
		//CopyMemory(pD,pI,sizeof(TCmdPItem));
		*pD = *pI;	// struct copy.
		pD->bCmd = ITEM_DROP_FLOOR;
		pD->x = x;
		pD->y = y;
		return;
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BOOL delta_portal_inited(int i)
{
	app_assert((DWORD)i < MAXPORTAL);
	if (sgJunk.portal[i].x == INIT_VAL) return(TRUE);
	else return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BOOL delta_quest_inited(int i)
{
	app_assert((DWORD)i <= MAXMULTIQUESTS);
	if (sgJunk.quests[i].qstate == INIT_VAL) return(FALSE);
	else return(TRUE);
}

/*-----------------------------------------------------------------------**
** Called when level creating
**-----------------------------------------------------------------------*/
void DeltaAddItem(int ii) {
	if (gbMaxPlayers == 1) return;
	app_assert((DWORD)ii < MAXITEMS);

	// Already in delta?
	TCmdPItem * pD = &sgLevels[currlevel].item[0];
	for (int i = 0; i < MAXITEMS; i++,pD++) {
		if (pD->bCmd == ITEM_FREE) continue;
		if (pD->wIndx != item[ii].IDidx) continue;
		if (pD->wCI != item[ii]._iCreateInfo) continue;
		if (pD->dwSeed != (DWORD)item[ii]._iSeed) continue;

		// taken?
		if (pD->bCmd == ITEM_GEN_TAKEN) return;
		// 2nd time down?
		if (pD->bCmd == ITEM_GEN_FLOOR) return;
	}


	// find a location to drop this item
	pD = &sgLevels[currlevel].item[0];
	for (i = 0; i < MAXITEMS; i++,pD++) {
		if (pD->bCmd != ITEM_FREE) continue;

		// put the item back onto the floor at its 
		// new location and with its new state

		sgbDeltaChanged = TRUE;
		pD->bCmd = ITEM_GEN_FLOOR;
		pD->x = item[ii]._ix;
		pD->y = item[ii]._iy;
		pD->wIndx = item[ii].IDidx;
		pD->wCI = item[ii]._iCreateInfo;
		pD->dwSeed = item[ii]._iSeed;
		pD->bId = item[ii]._iIdentified;
		pD->bDur = item[ii]._iDurability;
		pD->bMDur = item[ii]._iMaxDur;
		pD->bCh = item[ii]._iCharges;
		pD->bMCh = item[ii]._iMaxCharges;
		pD->wValue = item[ii]._ivalue;
		pD->wPLToHit = item[ii]._iPLToHit;
		pD->wMaxDam = item[ii]._iMaxDam;
		pD->bMinStr = item[ii]._iMinStr;
		pD->bMinMag = item[ii]._iMinMag;
		pD->bMinDex = item[ii]._iMinDex;
		pD->bAC = item[ii]._iAC;
		return;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DeltaSaveLevel() {
	if (gbMaxPlayers == 1) return;

	// throw away everyone else's graphics, they 
	// are probably out of date anyway
	for (int i = 0; i < MAX_PLRS; i++)
		if (i != myplr) plr[i]._pGFXLoad = 0;

	app_assert((DWORD) currlevel < NUMLEVELS);
	plr[myplr]._pLvlVisited[currlevel] = TRUE;
	delta_leave_sync(currlevel);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
// PATCH1.JMM
BOOL CanPut(int i, int j);
// ENDPATCH1.JMM


void DeltaLoadLevel() {
	if (gbMaxPlayers == 1) return;

	int i, ii;

	deltaload = TRUE;

	// don't do the monster stuff for level 0, that is the town!
	if (currlevel != 0) {
		for (i = 0; i < nummonsters; i++) {
			if (sgLevels[currlevel].monster[i]._mx == INIT_VAL) continue;
			M_ClearSquares(i);
			monster[i]._mx = sgLevels[currlevel].monster[i]._mx;
			monster[i]._my = sgLevels[currlevel].monster[i]._my;
			monster[i]._moldx = monster[i]._mx;
			monster[i]._moldy = monster[i]._my;
			monster[i]._mfutx = monster[i]._mx;
			monster[i]._mfuty = monster[i]._my;
			if (sgLevels[currlevel].monster[i]._mhitpoints != -1)
				monster[i]._mhitpoints = sgLevels[currlevel].monster[i]._mhitpoints;
			// Is monster dead?
			if (sgLevels[currlevel].monster[i]._mhitpoints == 0) {
				monster[i]._moldx = sgLevels[currlevel].monster[i]._mx;
				monster[i]._moldy = sgLevels[currlevel].monster[i]._my;
				M_ClearSquares(i);
				if (monster[i]._mAi != AI_DIABLO) {     // Diablo has no death frame
					if (monster[i]._uniqtype == 0) {
						app_assert(monster[i].MType != NULL);
						AddDead(monster[i]._mx, monster[i]._my, monster[i].MType->mdeadval, monster[i]._mdir);
					} else {
						AddDead(monster[i]._mx, monster[i]._my, monster[i]._udeadval, monster[i]._mdir);
					}
				}
				monster[i]._mDelFlag = TRUE;
				M_UpdateLeader(i);
			} else {
				int enemy = sgLevels[currlevel].monster[i]._menemy;
				//app_assert((DWORD) enemy < MAX_PLRS);
				decode_enemy(i, enemy);
				if (((monster[i]._mx != 0) && (monster[i]._mx != 1)) || (monster[i]._my != 0))  dMonster[monster[i]._mx][monster[i]._my] = i + 1;
				if (i < 4) {
					MAI_Golum(i);
					monster[i]._mFlags |= MFLAG_MID;
					monster[i]._mFlags |= MFLAG_MKILLER;
				} else M_StartStand(i, monster[i]._mdir);
				monster[i]._msquelch = sgLevels[currlevel].monster[i]._msquelch;
			}
		}

		// copy automap
		app_assert(sizeof(sgLocals[currlevel].automapsv) == sizeof(automapview));
		CopyMemory(
			&automapview[0][0],
			&sgLocals[currlevel].automapsv[0][0],
			sizeof(automapview)
		);
	}

	for (i = 0; i < MAXITEMS; i++) {
		if (sgLevels[currlevel].item[i].bCmd == ITEM_FREE) continue;
		if (sgLevels[currlevel].item[i].bCmd == ITEM_GEN_TAKEN) {
			ii = FindGetItem(sgLevels[currlevel].item[i].wIndx,
				sgLevels[currlevel].item[i].wCI,
				sgLevels[currlevel].item[i].dwSeed);
			if (ii != -1) {
				if (dItem[item[ii]._ix][item[ii]._iy] == (ii+1)) dItem[item[ii]._ix][item[ii]._iy] = 0;
				DeleteItem(ii, i);
			}
		}
		if (sgLevels[currlevel].item[i].bCmd == ITEM_DROP_FLOOR) {
			ii = itemavail[0];
			itemavail[0] = itemavail[MAXITEMS - numitems - 1];
			itemactive[numitems] = ii;
			if (sgLevels[currlevel].item[i].wIndx == IDI_EAR) {
				RecreateEar(ii,
					sgLevels[currlevel].item[i].wCI,
					sgLevels[currlevel].item[i].dwSeed,
					sgLevels[currlevel].item[i].bId,
					sgLevels[currlevel].item[i].bDur,
					sgLevels[currlevel].item[i].bMDur,
					sgLevels[currlevel].item[i].bCh,
					sgLevels[currlevel].item[i].bMCh,
					sgLevels[currlevel].item[i].wValue,
					sgLevels[currlevel].item[i].dwBuff);
			} else {
				RecreateItem(ii,
					sgLevels[currlevel].item[i].wIndx,
					sgLevels[currlevel].item[i].wCI,
					sgLevels[currlevel].item[i].dwSeed,
					sgLevels[currlevel].item[i].wValue);
				if (sgLevels[currlevel].item[i].bId) item[ii]._iIdentified = TRUE;
				item[ii]._iDurability = sgLevels[currlevel].item[i].bDur;
				item[ii]._iMaxDur = sgLevels[currlevel].item[i].bMDur;
				item[ii]._iCharges = sgLevels[currlevel].item[i].bCh;
				item[ii]._iMaxCharges = sgLevels[currlevel].item[i].bMCh;
				item[ii]._iPLToHit	 = sgLevels[currlevel].item[i].wPLToHit;
				item[ii]._iMaxDam = sgLevels[currlevel].item[i].wMaxDam;
				item[ii]._iMinStr = sgLevels[currlevel].item[i].bMinStr;
				item[ii]._iMinMag = sgLevels[currlevel].item[i].bMinMag;
				item[ii]._iMinDex = sgLevels[currlevel].item[i].bMinDex;
				item[ii]._iAC = sgLevels[currlevel].item[i].bAC;
			}

			int ox = sgLevels[currlevel].item[i].x;
			int oy = sgLevels[currlevel].item[i].y;

			// PATCH1.JMM
			if (!CanPut(ox,oy)) {
				BOOL done = FALSE;
				// radial search outward until a space is found
				for (int l = 1; (l < 50) && !done; l++) {
					for (int j = -l; (j <= l) && !done; j++) {
						int yy = oy + j;
						for (int i = -l; (i <= l) && !done; i++) {
							int xx = ox + i;
							if (!CanPut(xx,yy)) continue;
							done = TRUE;
							DROPLOG("  DeltaLoadLevel:  moving object from (x,y)->(%2.2x,%2.2x) to (x,y)->(%2.2x,%2.2x)!\n",ox,oy,xx,yy);
							ox = xx;
							oy = yy;
						}
					}
				}

				app_assert(done);
			}
			// ENDPATCH1.JMM


			item[ii]._ix = ox;
			item[ii]._iy = oy;
			dItem[item[ii]._ix][item[ii]._iy] = ii+1;
			// PATCH1.JMM
			DROPLOG("  DeltaLoadLevel:  respawning %d (%4.4x,%4.4x,%8.8x) @ (x,y)->(%2.2x,%2.2x)\n",ii,item[ii]._itype,item[ii]._iCreateInfo,item[ii]._iSeed,item[ii]._ix,item[ii]._iy);
			// ENDPATCH1.JMM
			RespawnItem(ii, FALSE);
			numitems++;
		}
	}

	if (currlevel != 0) {
		for (i = 0; i < MAXOBJECTS; i++) {
			switch (sgLevels[currlevel].object[i].bCmd) {
				case INIT_VAL:
					// no object
				break;

				case CMD_OPENDOOR:
				case CMD_CLOSEDOOR:
				case CMD_OPERATEOBJ:
				case CMD_PLROPOBJ:
					SyncOpObject(-1, sgLevels[currlevel].object[i].bCmd, i);
				break;

				case CMD_BREAKOBJ:
					SyncBreakObj(-1, i);
				break;
			}
		}
		for (i = 0; i < numobjects; i++) {
			ii = objectactive[i];
			if ((object[ii]._otype == OBJ_TRAPL) || (object[ii]._otype == OBJ_TRAPR)) Obj_Trap(ii);
		}
	}

	deltaload = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmd(BOOL bHiPri,BYTE bCmd) {
	TCmd cmd;
	cmd.bCmd = bCmd;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdGolem(BYTE mx, BYTE my, BYTE dir, BYTE menemy, long hp, BYTE cl) {
	app_assert(mx < DMAXX);
	app_assert(my < DMAXY);

	TCmdGolem cmd;
	
	cmd.bCmd = CMD_AWAKEGOLEM;
	cmd._mx = mx;
	cmd._my = my;
	cmd._mdir = dir;
	cmd._menemy = menemy;
	cmd._mhitpoints = hp;
	cmd._currlevel = cl;

	NetSendLoPri((BYTE *) &cmd, sizeof(cmd));
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdLoc(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y) {
	app_assert(x < DMAXX);
	app_assert(y < DMAXY);
	TCmdLoc cmd;
	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	
	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdLocParam1(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y,WORD wParam1) {
	app_assert(x < DMAXX);
	app_assert(y < DMAXY);
	TCmdLocParam1 cmd;
	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	cmd.wParam1 = wParam1;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdLocParam2(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y,WORD wParam1,WORD wParam2) {
	app_assert(x < DMAXX);
	app_assert(y < DMAXY);
	TCmdLocParam2 cmd;
	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdLocParam3(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y,WORD wParam1,WORD wParam2,WORD wParam3) {
	app_assert(x < DMAXX);
	app_assert(y < DMAXY);
	TCmdLocParam3 cmd;
	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdParam1(BOOL bHiPri,BYTE bCmd,WORD wParam1) {
	TCmdParam1 cmd;
	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdParam2(BOOL bHiPri,BYTE bCmd,WORD wParam1,WORD wParam2) {
	TCmdParam2 cmd;
	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdParam3(BOOL bHiPri,BYTE bCmd,WORD wParam1,WORD wParam2, WORD wParam3) {
	TCmdParam3 cmd;
	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdQuest(BOOL bHiPri,BYTE q) {
	TCmdQuest cmd;
	cmd.bCmd = CMD_SYNCQUEST;
	cmd.q = q;
	cmd.qstate = quests[q]._qactive;
	cmd.qlog = quests[q]._qlog;
	cmd.qvar1 = quests[q]._qvar1;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdGItem(BOOL bHiPri,BYTE bCmd,BYTE mast,BYTE pnum,BYTE ii) {
	app_assert(ii < MAXITEMS);
	app_assert(pnum < MAX_PLRS);

	TCmdGItem cmd;
	cmd.bCmd = bCmd;
	cmd.bPnum = pnum;
	cmd.bMaster = mast;
	cmd.dwTime = 0;
	cmd.bLevel = currlevel;
	cmd.bCursitem = ii;
	cmd.x = item[ii]._ix;
	cmd.y = item[ii]._iy;
	cmd.wIndx = item[ii].IDidx;
	if (item[ii].IDidx == IDI_EAR) {
		cmd.wCI = (item[ii]._iName[7] << 8) | item[ii]._iName[8];
		cmd.dwSeed = (item[ii]._iName[9] << 24) |
					(item[ii]._iName[10] << 16) |
					(item[ii]._iName[11] << 8) |
					item[ii]._iName[12];
		cmd.bId = item[ii]._iName[13];
		cmd.bDur = item[ii]._iName[14];
		cmd.bMDur = item[ii]._iName[15];
		cmd.bCh = item[ii]._iName[16];
		cmd.bMCh = item[ii]._iName[17];
		cmd.wValue = (item[ii]._iName[18] << 8) | ((item[ii]._iCurs - ITEM_EAR1) << 6) | item[ii]._ivalue;
		cmd.dwBuff = (item[ii]._iName[19] << 24) |
					(item[ii]._iName[20] << 16) |
					(item[ii]._iName[21] << 8) |
					item[ii]._iName[22];
	} else {
		cmd.wCI = item[ii]._iCreateInfo;
		cmd.dwSeed = item[ii]._iSeed;
		cmd.bId = item[ii]._iIdentified;
		cmd.bDur = item[ii]._iDurability;
		cmd.bMDur = item[ii]._iMaxDur;
		cmd.bCh = item[ii]._iCharges;
		cmd.bMCh = item[ii]._iMaxCharges;
		cmd.wValue = item[ii]._ivalue;
		cmd.wPLToHit = item[ii]._iPLToHit;
		cmd.wMaxDam = item[ii]._iMaxDam;
		cmd.bMinStr = item[ii]._iMinStr;
		cmd.bMinMag = item[ii]._iMinMag;
		cmd.bMinDex = item[ii]._iMinDex;
		cmd.bAC = item[ii]._iAC;
	}

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void NetSendCmdGItem2(BOOL usonly,BYTE bCmd,BYTE mast,BYTE pnum,const TCmdGItem * p) {
	app_assert(pnum < MAX_PLRS);
	app_assert(p != NULL);

	TCmdGItem cmd;
	//CopyMemory(&cmd,p,sizeof(TCmdGItem));
	cmd = *p;	// struct copy.
	cmd.bCmd = bCmd;
	cmd.bPnum = pnum;
	cmd.bMaster = mast;

	// Should I init the timer?
	if (!usonly) {
		cmd.dwTime = 0;
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	}
	else {
		// Have I been sending this back to myself for more than 5 seconds?
		DWORD dwCurr = GetTickCount();
		if (cmd.dwTime == 0)
			cmd.dwTime = dwCurr;
		else if ((long) (dwCurr - cmd.dwTime) > RESEND_TIME)
			return;
		NetSendMyselfPri((BYTE *) &cmd,sizeof(cmd));
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BOOL NetSendCmdReq2(BYTE bCmd,BYTE mast,BYTE pnum,const TCmdGItem * p) {
	app_assert(pnum < MAX_PLRS);
	app_assert(p != NULL);

	TCmdGItem cmd;
	//CopyMemory(&cmd,p,sizeof(TCmdGItem));
	cmd = *p;	// struct copy.
	cmd.bCmd = bCmd;
	cmd.bPnum = pnum;
	cmd.bMaster = mast;

	// Have I been sending this back to myself for more than 5 seconds?
	DWORD dwCurr = GetTickCount();
	if (cmd.dwTime == 0)            // Timer inited in original send
		cmd.dwTime = dwCurr;
	else if ((long) (dwCurr - cmd.dwTime) > RESEND_TIME)
		return(FALSE);
	NetSendMyselfPri((BYTE *) &cmd,sizeof(cmd));
	return(TRUE);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdExtra(const TCmdGItem * p)
{
	app_assert(p != NULL);

	TCmdGItem cmd;
	//CopyMemory(&cmd,p,sizeof(TCmdGItem));
	cmd = *p;	// struct copy
	cmd.bCmd = CMD_ITEMEXTRA;
	cmd.dwTime = 0;
	NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdPItem(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y) {
	app_assert(x < DMAXX);
	app_assert(y < DMAXY);

	TCmdPItem cmd;
	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	cmd.wIndx = plr[myplr].HoldItem.IDidx;
	if (plr[myplr].HoldItem.IDidx == IDI_EAR) {
		cmd.wCI = (plr[myplr].HoldItem._iName[7] << 8) | plr[myplr].HoldItem._iName[8];
		cmd.dwSeed = (plr[myplr].HoldItem._iName[9] << 24) |
					(plr[myplr].HoldItem._iName[10] << 16) |
					(plr[myplr].HoldItem._iName[11] << 8) |
					plr[myplr].HoldItem._iName[12];
		cmd.bId = plr[myplr].HoldItem._iName[13];
		cmd.bDur = plr[myplr].HoldItem._iName[14];
		cmd.bMDur = plr[myplr].HoldItem._iName[15];
		cmd.bCh = plr[myplr].HoldItem._iName[16];
		cmd.bMCh = plr[myplr].HoldItem._iName[17];
		cmd.wValue = (plr[myplr].HoldItem._iName[18] << 8) |
					((plr[myplr].HoldItem._iCurs - ITEM_EAR1) << 6) |
					plr[myplr].HoldItem._ivalue;
		cmd.dwBuff = (plr[myplr].HoldItem._iName[19] << 24) |
					(plr[myplr].HoldItem._iName[20] << 16) |
					(plr[myplr].HoldItem._iName[21] << 8) |
					plr[myplr].HoldItem._iName[22];
	} else {
		cmd.wCI = plr[myplr].HoldItem._iCreateInfo;
		cmd.dwSeed = plr[myplr].HoldItem._iSeed;
		cmd.bId = plr[myplr].HoldItem._iIdentified;
		cmd.bDur = plr[myplr].HoldItem._iDurability;
		cmd.bMDur = plr[myplr].HoldItem._iMaxDur;
		cmd.bCh = plr[myplr].HoldItem._iCharges;
		cmd.bMCh = plr[myplr].HoldItem._iMaxCharges;
		cmd.wValue = plr[myplr].HoldItem._ivalue;
		cmd.wPLToHit = plr[myplr].HoldItem._iPLToHit;
		cmd.wMaxDam = plr[myplr].HoldItem._iMaxDam;
		cmd.bMinStr = plr[myplr].HoldItem._iMinStr;
		cmd.bMinMag = plr[myplr].HoldItem._iMinMag;
		cmd.bMinDex = plr[myplr].HoldItem._iMinDex;
		cmd.bAC = plr[myplr].HoldItem._iAC;
	}

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdChItem(BOOL bHiPri,BYTE bLoc) {
	TCmdChItem cmd;
	cmd.bCmd = CMD_CHANGEPLRITEMS;
	cmd.bLoc = bLoc;
	cmd.wIndx = plr[myplr].HoldItem.IDidx;
	cmd.wCI = plr[myplr].HoldItem._iCreateInfo;
	cmd.dwSeed = plr[myplr].HoldItem._iSeed;
	// drb.patch1.start.02/10/97
	cmd.bId = plr[myplr].HoldItem._iIdentified;
	// drb.patch1.end.02/10/97

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdDelItem(BOOL bHiPri,BYTE bLoc) {
	TCmdDelItem cmd;
	cmd.bCmd = CMD_DELPLRITEMS;
	cmd.bLoc = bLoc;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdDItem(BOOL bHiPri,int ii) {
	app_assert((DWORD)ii < MAXITEMS);

	TCmdPItem cmd;
	cmd.bCmd = CMD_DROPITEM;
	cmd.x = item[ii]._ix;
	cmd.y = item[ii]._iy;
	cmd.wIndx = item[ii].IDidx;
	if (item[ii].IDidx == IDI_EAR) {
		cmd.wCI = (item[ii]._iName[7] << 8) | item[ii]._iName[8];
		cmd.dwSeed = (item[ii]._iName[9] << 24) |
					(item[ii]._iName[10] << 16) |
					(item[ii]._iName[11] << 8) |
					item[ii]._iName[12];
		cmd.bId = item[ii]._iName[13];
		cmd.bDur = item[ii]._iName[14];
		cmd.bMDur = item[ii]._iName[15];
		cmd.bCh = item[ii]._iName[16];
		cmd.bMCh = item[ii]._iName[17];
		cmd.wValue = (item[ii]._iName[18] << 8) |
						((item[ii]._iCurs - ITEM_EAR1) << 6) |
						item[ii]._ivalue;
		cmd.dwBuff = (item[ii]._iName[19] << 24) |
					(item[ii]._iName[20] << 16) |
					(item[ii]._iName[21] << 8) |
					item[ii]._iName[22];
	} else {
		cmd.wCI = item[ii]._iCreateInfo;
		cmd.dwSeed = item[ii]._iSeed;
		cmd.bId = item[ii]._iIdentified;
		cmd.bDur = item[ii]._iDurability;
		cmd.bMDur = item[ii]._iMaxDur;
		cmd.bCh = item[ii]._iCharges;
		cmd.bMCh = item[ii]._iMaxCharges;
		cmd.wValue = item[ii]._ivalue;
		cmd.wPLToHit = item[ii]._iPLToHit;
		cmd.wMaxDam = item[ii]._iMaxDam;
		cmd.bMinStr = item[ii]._iMinStr;
		cmd.bMinMag = item[ii]._iMinMag;
		cmd.bMinDex = item[ii]._iMinDex;
		cmd.bAC = item[ii]._iAC;
	}

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL i_own_level(int nReqLevel) {
	// the lowest numbered player on a level is
	// responsible for arbitrating disputes.
	for (int i = 0; i < MAX_PLRS; i++) {
		if (! plr[i].plractive) continue;
		if (plr[i]._pLvlChanging) continue;
		if (plr[i].plrlevel != nReqLevel) continue;
		if (i == myplr && gbBufferMsgs != BUFFER_OFF) continue;
		break;
	}

	return i == myplr;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdDamage(BOOL bHiPri,BYTE bPlr,DWORD dwDam) {
	app_assert(bPlr < MAX_PLRS);

	TCmdDamage cmd;
	cmd.bCmd = CMD_PLRDAMAGE;
	cmd.bPlr = bPlr;
	cmd.dwDam = dwDam;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendCmdMonstDamage(BOOL bHiPri,WORD wMonst,DWORD dwDam) {
	app_assert(wMonst < MAXMONSTERS);

	TCmdMonstDamage cmd;
	cmd.bCmd = CMD_MONSTDAMAGE;
	cmd.wMonst = wMonst;
	cmd.dwDam = dwDam;

	if (bHiPri)
		NetSendHiPri((BYTE *) &cmd,sizeof(cmd));
	else
		NetSendLoPri((BYTE *) &cmd,sizeof(cmd));
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void NetSendString(DWORD dwSendMask,const char * pszStr) {
	app_assert(pszStr);

	TCmdString cmd;
	DWORD dwStrLen = strlen(pszStr);
	app_assert(dwStrLen < sizeof(cmd.str));

	cmd.bCmd = CMD_STRING;
	strcpy(cmd.str,pszStr);
	NetSendMask(
		dwSendMask,
		(BYTE *) &cmd,
		(BYTE) (sizeof(cmd) - sizeof(cmd.str) + dwStrLen + 1)
	);
}       


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD cmd_string(int pnum,const TCmdString * p) {
	app_assert((DWORD)pnum < MAX_PLRS);
	app_assert(p != NULL);

	DWORD dwStrLen = strlen(p->str);

	// don't display messages while buffering or processing
	if (gbBufferMsgs == BUFFER_OFF)
		plrmsg_add(pnum,p->str);

	return sizeof(*p) - sizeof(p->str) + dwStrLen + 1;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void delta_open_portal(
	int pnum,
	BYTE x,
	BYTE y,
	BYTE bLevel,
	BYTE bLType,
	BYTE bSetLvl
) {
	app_assert((DWORD)pnum < MAX_PLRS);
	app_assert(x < DMAXX);
	app_assert(y < DMAXY);
	app_assert(bLevel < NUMLEVELS);

	sgJunk.portal[pnum].x = x;
	sgJunk.portal[pnum].y = y;
	sgJunk.portal[pnum].level = bLevel;
	sgJunk.portal[pnum].ltype = bLType;
	sgJunk.portal[pnum].setlvl = bSetLvl;
	sgbDeltaChanged = TRUE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void delta_close_portal(int pnum) {
	app_assert((DWORD)pnum < MAX_PLRS);
	FillMemory(&sgJunk.portal[pnum],sizeof(sgJunk.portal[pnum]),INIT_VAL);
	sgbDeltaChanged = TRUE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void check_update_plr(int pnum) {
	if (gbMaxPlayers == 1) return;

	app_assert((DWORD)pnum < MAX_PLRS);
	if (pnum != myplr) return;

	void TimedUpdatePlayerFile(BOOL bForce);
	TimedUpdatePlayerFile(TRUE);
}

// PATCH2.JMM.3/5/97
#define MAX_ADDSTAT	256
#define MAX_LEVEL 51
#define MAX_STAT 750

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define CHEATER_DISP_INTERVAL 5
static DWORD sgdwLastCheaterTime = 0;

static void __cdecl IdentifyCheater( LPCSTR szFormat, ... ) {
	DWORD dwCurrTime;
	char szOut[256];

	dwCurrTime = GetTickCount( );

	if((dwCurrTime - sgdwLastCheaterTime) < (CHEATER_DISP_INTERVAL*1000))
		return;

	sgdwLastCheaterTime = dwCurrTime;

	va_list args;
	va_start(args,szFormat);
	vsprintf(szOut,szFormat,args);
	va_end(args);

	sysmsg_add_string( szOut );

}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SYNCDATA( const TCmd* pCmd, int pnum ) {
	// sync_update handles gbBufferMsgs

	DROPLOG( "CMD_SYNCDATA(%d)\n", pnum );
	return sync_update( pnum, (CONST BYTE *) pCmd );
}



/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_WALKXY( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLoc * p = (const TCmdLoc *) pCmd;

		DROPLOG("CMD_WALKXY(%d):(%d,%d)\n",pnum,p->x,p->y);
		
		ClrPlrPath(pnum);
		MakePlrPath(pnum, p->x, p->y, TRUE);
		plr[pnum].destAction = PCMD_NOTHING;
	}
	return sizeof(TCmdLoc);
}



/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ADDSTR( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_ADDSTR(%d)\n",pnum);

		// PATCH2.JMM.3/5/97
		if(p->wParam1 < 0 || p->wParam1 > MAX_ADDSTAT) {
			return sizeof(TCmdParam1);
		}
		// ENDPATCH2.JMM.3/5/97
			
		ModifyPlrStr(pnum, p->wParam1);
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ADDMAG( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_ADDMAG(%d)\n",pnum);
		
		// PATCH2.JMM.3/5/97
		if(p->wParam1 < 0 || p->wParam1 > MAX_ADDSTAT) {
			return sizeof(TCmdParam1);
		}
		// ENDPATCH2.JMM.3/5/97

		ModifyPlrMag(pnum, p->wParam1);
	}
	
	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ADDDEX( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_ADDDEX(%d)\n",pnum);
		
		// PATCH2.JMM.3/5/97
		if(p->wParam1 < 0 || p->wParam1 > MAX_ADDSTAT) {
			return sizeof(TCmdParam1);
		}
		// ENDPATCH2.JMM.3/5/97

		ModifyPlrDex(pnum, p->wParam1);
	}

	return sizeof(TCmdParam1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ADDVIT( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_ADDVIT(%d)\n",pnum);

		// PATCH2.JMM.3/5/97
		if(p->wParam1 < 0 || p->wParam1 > MAX_ADDSTAT) {
			return sizeof(TCmdParam1);
		}
		// ENDPATCH2.JMM.3/5/97

		ModifyPlrVit(pnum, p->wParam1);
	}

	return sizeof(TCmdParam1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SBSPELL( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_SBSPELL(%d)\n",pnum);

		// PATCH2.JMM.3/5/97
		UINT nSplType = p->wParam1;

		if(currlevel == 0 && !spelldata[nSplType].sTownSpell) {
			IdentifyCheater("%s has cast an illegal spell.",plr[pnum]._pName);
			return sizeof(TCmdParam1);
		}
		// ENDPATCH2.JMM.3/5/97

		plr[pnum]._pSpell = p->wParam1;
		plr[pnum]._pSplType = plr[pnum]._pSBkSplType;
		plr[pnum]._pSplFrom = SPL_FROMBK;
		plr[pnum].destAction = PCMD_SPELL;
	}
	return sizeof(TCmdParam1);
}



/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_GOTOGETITEM( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLocParam1 * p = (const TCmdLocParam1 *) pCmd;
	
		DROPLOG("CMD_GOTOGETITEM(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		MakePlrPath(pnum, p->x, p->y, FALSE);
		plr[pnum].destAction = PCMD_REQGETITEM;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(TCmdLocParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_REQUESTGITEM( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON) {
	// doesn't need to be buffered
	}
	else {
		const TCmdGItem * p = (const TCmdGItem *) pCmd;

		DROPLOG("CMD_REQUESTGETITEM(%d,%8.8x):  index->0x%8.8x ci->0x%8.8x seed->0x%8.8x xy->(%d,%d)\n",pnum,p->dwTime,p->wIndx,p->wCI,p->dwSeed,p->x,p->y);

		if (i_own_level(plr[pnum].plrlevel)) {
			app_assert(currlevel == plr[myplr].plrlevel);

			// make sure we don't start resending an item we just picked up.
			if( !CheckGetRecord( p->dwSeed, p->wCI, p->wIndx ) )
				return sizeof(TCmdGItem);
			
			int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
			if (ii != -1) {
				DROPLOG("  Item found.  FindGetItem returned %d and iDelFlag is FALSE.\n",ii);
				NetSendCmdGItem2(FALSE, CMD_GETITEM, myplr, p->bPnum, p);

				if (p->bPnum != myplr) {
					SyncGetItem(p->x, p->y, p->wIndx, p->wCI, p->dwSeed);
				}
				else {
					InvGetItem(myplr, ii);
				}
  
				AddGetRecord( p->dwSeed, p->wCI, p->wIndx );
			} 
			else {
				if (ii == -1) {
					DROPLOG("  Item not found.  Resending.\n");
					if (! NetSendCmdReq2(CMD_REQUESTGITEM, myplr, p->bPnum, p)) {
						DROPLOG("  Resend expired.  Extra Item.\n");
						NetSendCmdExtra(p);
					}
				}
			}
		}
	}

	return sizeof(TCmdGItem);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_GETITEM( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdGItem));
	}
	else {
		const TCmdGItem * p = (const TCmdGItem *) pCmd;

		DROPLOG("CMD_GETITEM(%d,%8.8x,%d):  index->0x%8.8x ci->0x%8.8x seed->0x%8.8x xy->(%d,%d)\n",pnum,p->dwTime,p->bPnum,p->wIndx,p->wCI,p->dwSeed,p->x,p->y);
		DROPLOG("  Data:  bCmd = %2.2x\n", p->bCmd);
		DROPLOG("  Data:  bMaster = %2.2x\n",p->bMaster);
		DROPLOG("  Data:  bPnum = %2.2x\n",p->bPnum);
		DROPLOG("  Data:  bCursitem = %2.2x\n",p->bCursitem);
		DROPLOG("  Data:  bLevel = %2.2x\n",p->bLevel);
		DROPLOG("  Data:  x = %2.2x\n",p->x);
		DROPLOG("  Data:  y = %2.2x\n",p->y);
		DROPLOG("  Data:  wIndx = %4.4x\n",p->wIndx);
		DROPLOG("  Data:  wCI = %4.4x\n",p->wCI);
		DROPLOG("  Data:  dwSeed = %8.8x\n",p->dwSeed);
		DROPLOG("  Data:  bID = %2.2x\n",p->bId);
		DROPLOG("  Data:  bDur = %2.2x\n",p->bDur);
		DROPLOG("  Data:  bMDur = %2.2x\n",p->bMDur);
		DROPLOG("  Data:  bCh = %2.2x\n",p->bCh);
		DROPLOG("  Data:  bMCh = %2.2x\n",p->bMCh);
		DROPLOG("  Data:  wValue = %4.4x\n",p->wValue);
		DROPLOG("  Data:  dwBuff = %8.8x\n",p->dwBuff);
		DROPLOG("  Data:  dwTime = %8.8x\n",p->dwTime);

		int nIndex = FindGetItem( p->wIndx, p->wCI, p->dwSeed );

		// Check if in the delta table
		DROPLOG("  Looking in delta tbl[%d]\n",p->bLevel);

		//if (delta_get_item(p, plr[pnum].plrlevel) && (nIndex != -1)) {
		if (delta_get_item(p, p->bLevel)) {
			// Found in delta
			DROPLOG("  Found in delta tbl.\n");

			//if (currlevel == plr[pnum].plrlevel) {
			if ((currlevel == p->bLevel) || (p->bPnum == myplr)) {
				if (p->bMaster == myplr) {
					// item has already been picked up
					DROPLOG("  Item already picked up.\n");
				}
				else if (p->bPnum == myplr) {
					if (currlevel != p->bLevel) {
						int hitem = SyncPutItem(myplr, 
												plr[myplr]._px, 
												plr[myplr]._py,
												p->wIndx, 
												p->wCI, 
												p->dwSeed,
												p->bId, 
												p->bDur, 
												p->bMDur, 
												p->bCh, 
												p->bMCh,
												p->wValue, 
												p->dwBuff,
												p->wPLToHit,
												p->wMaxDam,
												p->bMinStr,
												p->bMinMag,
												p->bMinDex,
												p->bAC );
						if (hitem != -1) InvGetItem(myplr, hitem);
					} else
						InvGetItem(myplr, nIndex);
					DROPLOG("  Local player got item.\n");
				}
				else {
					SyncGetItem(p->x, p->y, p->wIndx, p->wCI, p->dwSeed);
					DROPLOG("  Remote player (%d) got item.\n",p->bPnum);
				}
			}
		}
		else {
			// Not found, so let's send it to ourselves again
			DROPLOG("  Not found in delta tbl.  Resending...\n");
			NetSendCmdGItem2(TRUE, CMD_GETITEM, p->bMaster, p->bPnum, p);
		}
	}

	return sizeof(TCmdGItem);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_GOTOAGETITEM( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLocParam1 * p = (const TCmdLocParam1 *) pCmd;
		DROPLOG("CMD_GOTOAUTOGETITEM(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);
		MakePlrPath(pnum, p->x, p->y, FALSE);
		plr[pnum].destAction = PCMD_REQAGETITEM;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(TCmdLocParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_REQUESTAGITEM( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else {
		const TCmdGItem * p = (const TCmdGItem *) pCmd;
		DROPLOG("CMD_REQUESTAUTOGETITEM(%d):  index->0x%8.8x ci->0x%8.8x seed->0x%8.8x xy->(%d,%d)\n",pnum,p->wIndx,p->wCI,p->dwSeed,p->x,p->y);
		if (i_own_level(plr[pnum].plrlevel)) {
			app_assert(currlevel == plr[myplr].plrlevel);

			// make sure we don't start resending an item we just picked up.
			if( !CheckGetRecord( p->dwSeed, p->wCI, p->wIndx ) )
				return sizeof(TCmdGItem);
					
			int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
			if (ii != -1) {
				NetSendCmdGItem2(FALSE, CMD_AGETITEM, myplr, p->bPnum, p);
				DROPLOG("  Item found.\n");
				if (p->bPnum != myplr) SyncGetItem(p->x, p->y, p->wIndx, p->wCI, p->dwSeed);
				else AutoGetItem(myplr, p->bCursitem);
				// PATCH1.JMM
				AddGetRecord( p->dwSeed, p->wCI, p->wIndx );
				// ENDPATCH1.JMM
			} else {
				if (ii == -1) {
					DROPLOG("  Item not found.  Resending.\n");
					if (! NetSendCmdReq2(CMD_REQUESTAGITEM, myplr, p->bPnum, p)) {
						DROPLOG("  Resend expired.  Extra Item.\n");
						NetSendCmdExtra(p);
					}
				}
				}
		}
	}
	return sizeof(TCmdGItem);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_AGETITEM( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdGItem));
	}
	else {
		const TCmdGItem * p = (const TCmdGItem *) pCmd;

		// PATCH1.JMM
		DROPLOG("CMD_AUTOGETITEM(%d):  index->0x%8.8x ci->0x%8.8x seed->0x%8.8x xy->(%d,%d)\n",pnum,p->wIndx,p->wCI,p->dwSeed,p->x,p->y);
		DROPLOG("  Data:  bCmd = %2.2x\n", p->bCmd);
		DROPLOG("  Data:  bMaster = %2.2x\n",p->bMaster);
		DROPLOG("  Data:  bPnum = %2.2x\n",p->bPnum);
		DROPLOG("  Data:  bCursitem = %2.2x\n",p->bCursitem);
		DROPLOG("  Data:  bLevel = %2.2x\n",p->bLevel);
		DROPLOG("  Data:  x = %2.2x\n",p->x);
		DROPLOG("  Data:  y = %2.2x\n",p->y);
		DROPLOG("  Data:  wIndx = %4.4x\n",p->wIndx);
		DROPLOG("  Data:  wCI = %4.4x\n",p->wCI);
		DROPLOG("  Data:  dwSeed = %8.8x\n",p->dwSeed);
		DROPLOG("  Data:  bID = %2.2x\n",p->bId);
		DROPLOG("  Data:  bDur = %2.2x\n",p->bDur);
		DROPLOG("  Data:  bMDur = %2.2x\n",p->bMDur);
		DROPLOG("  Data:  bCh = %2.2x\n",p->bCh);
		DROPLOG("  Data:  bMCh = %2.2x\n",p->bMCh);
		DROPLOG("  Data:  wValue = %4.4x\n",p->wValue);
		DROPLOG("  Data:  dwBuff = %8.8x\n",p->dwBuff);
		DROPLOG("  Data:  dwTime = %8.8x\n",p->dwTime);

		int nIndex = FindGetItem( p->wIndx, p->wCI, p->dwSeed );
		// ENDPATCH1.JMM

		// Check if in the delta table
		// drb.patch1.start.2/05/97
		//if (delta_get_item(p, plr[pnum].plrlevel)) {
		DROPLOG("  Looking in delta tbl[%d]\n",p->bLevel);
		if (delta_get_item(p, p->bLevel)) {
		// drb.patch1.end.2/05/97
			// Found in delta
			DROPLOG("  Found in delta tbl.\n");
			// PATCH1.JMM
			//if (currlevel == plr[pnum].plrlevel) {
			if ((currlevel == p->bLevel) || (p->bPnum == myplr)) {
			// ENDPATCH1.JMM
				if (p->bMaster == myplr) {
					// item already picked up
					DROPLOG("  Item already picked up.\n");
				}
				else if (p->bPnum == myplr) {
					// drb.patch1.start.02/14/97
					// This is the biggest kludge I have ever written.
					// I am so ashamed, but it will work
					if (currlevel != p->bLevel) {
						int hitem = SyncPutItem(myplr, 
												plr[myplr]._px, 
												plr[myplr]._py,
												p->wIndx, 
												p->wCI, 
												p->dwSeed,
												p->bId, 
												p->bDur, 
												p->bMDur, 
												p->bCh, 
												p->bMCh,
												p->wValue, 
												p->dwBuff,
												p->wPLToHit,
												p->wMaxDam,
												p->bMinStr,
												p->bMinMag,
												p->bMinDex,
												p->bAC );
						if (hitem != -1) AutoGetItem(myplr, hitem);
					} else
						AutoGetItem(myplr, p->bCursitem);
					// drb.patch1.end.02.14/97
					DROPLOG("  Local player got item.\n");
				}
				else {
					SyncGetItem(p->x, p->y, p->wIndx, p->wCI, p->dwSeed);
					DROPLOG("  Remote player (%d) got item.\n",p->bPnum);
				}
			}
		}
		else {
			// Not found, so let's send it to ourselves again
			NetSendCmdGItem2(TRUE, CMD_AGETITEM, p->bMaster, p->bPnum, p);
			DROPLOG("  Not found in delta tbl.  Resending...\n");
		}
	}
	
	return sizeof(TCmdGItem);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ITEMEXTRA( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdGItem));
	}
	else {
		const TCmdGItem * p = (const TCmdGItem *) pCmd;
	
		DROPLOG("CMD_ITEMEXTRA(%d):  index->0x%8.8x ci->0x%8.8x seed->0x%8.8x xy->(%d,%d)\n",pnum,p->wIndx,p->wCI,p->dwSeed,p->x,p->y);
		// Check if in the delta table
		// drb.patch1.start.2/05/97
		//delta_get_item(p, plr[p->bPnum].plrlevel);
		DROPLOG("  Looking in delta tbl[%d]\n",p->bLevel);
		delta_get_item(p, p->bLevel);
		if (currlevel == plr[pnum].plrlevel) {
		//if (currlevel == p->bLevel) {
		// drb.patch1.end.2/05/97
			SyncGetItem(p->x, p->y, p->wIndx, p->wCI, p->dwSeed);
		}
	}

	return sizeof(TCmdGItem);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_PUTITEM( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdPItem));
	}
	else {
		const TCmdPItem * p = (const TCmdPItem *) pCmd;

		DROPLOG("CMD_PUTITEM(%d):  index->0x%8.8x ci->0x%8.8x seed->0x%8.8x xy->(%d,%d)\n",pnum,p->wIndx,p->wCI,p->dwSeed,p->x,p->y);
				
		if (currlevel == plr[pnum].plrlevel) {
			DROPLOG("  Item on my level.\n");
				int ii;
			if (pnum == myplr) {
				ii = InvPutItem(pnum, p->x, p->y);
			}
			else {
				ii = SyncPutItem(pnum, 
								 p->x, 
								 p->y, 
								 p->wIndx, 
								 p->wCI, 
								 p->dwSeed, 
								 p->bId, 
								 p->bDur, 
								 p->bMDur, 
								 p->bCh, 
								 p->bMCh, 
								 p->wValue, 
								 p->dwBuff,
								 p->wPLToHit,
								 p->wMaxDam,
								 p->bMinStr,
								 p->bMinMag,
								 p->bMinDex,
								 p->bAC );
			}

			if (ii != -1) {
				DROPLOG("  Put successful.  Putting item into delta...\n");

				RemoveGetRecord( p->dwSeed, p->wCI, p->wIndx );

				DROPLOG("  Dropping into delta tbl[%d]\n",plr[pnum].plrlevel);
				delta_put_item(p,item[ii]._ix,item[ii]._iy,plr[pnum].plrlevel);
				check_update_plr(pnum);
			}
			else {
				DROPLOG("  Put unsuccessful.\n");
			}
					
			return sizeof(TCmdPItem);
		}

		RemoveGetRecord( p->dwSeed, p->wCI, p->wIndx );

		DROPLOG("  Item not on my level.  Adding to delta...\n");
		DROPLOG("  Dropping into delta tbl[%d]\n",plr[pnum].plrlevel);
		delta_put_item(p, p->x, p->y, plr[pnum].plrlevel);
		check_update_plr(pnum);
	}
	return sizeof(TCmdPItem);
}




/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SYNCPUTITEM( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdPItem));
	}
	else {
		const TCmdPItem * p = (const TCmdPItem *) pCmd;

		DROPLOG("CMD_SYNCPUTITEM(%d):  index->0x%8.8x ci->0x%8.8x seed->0x%8.8x xy->(%d,%d)\n",pnum,p->wIndx,p->wCI,p->dwSeed,p->x,p->y);

		if (currlevel == plr[pnum].plrlevel) {
			DROPLOG("  Item on my level.\n");
			int ii = SyncPutItem(pnum, 
								 p->x, 
								 p->y, 
								 p->wIndx, 
								 p->wCI, 
								 p->dwSeed, 
								 p->bId, 
								 p->bDur, 
								 p->bMDur, 
								 p->bCh, 
								 p->bMCh, 
								 p->wValue, 
								 p->dwBuff,
								 p->wPLToHit,
								 p->wMaxDam,
								 p->bMinStr,
								 p->bMinMag,
								 p->bMinDex,
								 p->bAC );
			if (ii != -1) {
				RemoveGetRecord( p->dwSeed, p->wCI, p->wIndx );

				DROPLOG("  Put successful.  Putting item into delta...\n");
				delta_put_item(p,item[ii]._ix,item[ii]._iy,plr[pnum].plrlevel);
				DROPLOG("  Dropping into delta tbl[%d]\n",plr[pnum].plrlevel);
				check_update_plr(pnum);
			}
			else {
				DROPLOG("  Put unsuccessful.\n");
			}

			return sizeof(TCmdPItem);
		}

		RemoveGetRecord( p->dwSeed, p->wCI, p->wIndx );

		DROPLOG("  Item not on my level.  Adding to delta...\n");
		delta_put_item(p, p->x, p->y, plr[pnum].plrlevel);
		DROPLOG("  Dropping into delta tbl[%d]\n",plr[pnum].plrlevel);
		check_update_plr(pnum);
	}

	return sizeof(TCmdPItem);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_RESPAWNITEM( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdPItem));
	}
	else {
		const TCmdPItem * p = (const TCmdPItem *) pCmd;

		DROPLOG("CMD_RESPAWNITEM(%d):  index->0x%8.8x ci->0x%8.8x seed->0x%8.8x xy->(%d,%d)\n",pnum,p->wIndx,p->wCI,p->dwSeed,p->x,p->y);

		if ((currlevel == plr[pnum].plrlevel) && (pnum != myplr)) {
			DROPLOG("  SyncPutting into level.\n");
			int ii = SyncPutItem(pnum, 
								 p->x, 
								 p->y, 
								 p->wIndx, 
								 p->wCI, 
								 p->dwSeed, 
								 p->bId, 
								 p->bDur, 
								 p->bMDur, 
								 p->bCh, 
								 p->bMCh, 
								 p->wValue, 
								 p->dwBuff,
								 p->wPLToHit,
								 p->wMaxDam,
								 p->bMinStr,
								 p->bMinMag,
								 p->bMinDex,
								 p->bAC );
		}

		RemoveGetRecord( p->dwSeed, p->wCI, p->wIndx );

		DROPLOG("  Adding to delta...\n");
		delta_put_item(p, p->x, p->y, plr[pnum].plrlevel);
		DROPLOG("  Dropping into delta tbl[%d]\n",plr[pnum].plrlevel);
	}

	return sizeof(TCmdPItem);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ATTACKXY( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLoc * p = (const TCmdLoc *) pCmd;

		DROPLOG("CMD_ATTACKXY(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		MakePlrPath(pnum, p->x, p->y, FALSE);
		plr[pnum].destAction = PCMD_ATTACK;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
	}

	return sizeof(TCmdLoc);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SATTACKXY( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLoc * p = (const TCmdLoc *) pCmd;
		
		DROPLOG("CMD_SATTACKXY(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_ATTACK;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
	}

	return sizeof(TCmdLoc);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_RATTACKXY( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLoc * p = (const TCmdLoc *) pCmd;

		DROPLOG("CMD_RATTACKXY(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_RATTACK;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
	}

	return sizeof(TCmdLoc);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SPELLXYD( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLocParam3 * p = (const TCmdLocParam3 *) pCmd;
		
		DROPLOG("CMD_SPELLXYD(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		// PATCH2.JMM.3/5/97
		UINT nSplType = p->wParam1;

		if(currlevel == 0 && !spelldata[nSplType].sTownSpell) {
			IdentifyCheater("%s has cast an illegal spell.",plr[pnum]._pName);
			return sizeof(TCmdLocParam3);
		}
		// ENDPATCH2.JMM.3/5/97

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_SPELLXYD;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
		plr[pnum].destParam3 = p->wParam2;
		plr[pnum].destParam4 = p->wParam3;
		plr[pnum]._pSpell = p->wParam1;
		plr[pnum]._pSplType = plr[pnum]._pRSplType;
		plr[pnum]._pSplFrom = SPL_FROMR;
	}

	return sizeof(TCmdLocParam3);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SPELLXY( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLocParam2 * p = (const TCmdLocParam2 *) pCmd;
		
		DROPLOG("CMD_SPELLXY(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		// PATCH2.JMM.3/5/97
		UINT nSplType = p->wParam1;

		if(currlevel == 0 && !spelldata[nSplType].sTownSpell) {
			IdentifyCheater("%s has cast an illegal spell.",plr[pnum]._pName);
			return sizeof(TCmdLocParam2);
		}
		// ENDPATCH2.JMM.3/5/97

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_SPELL;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
		plr[pnum].destParam3 = p->wParam2;
		plr[pnum]._pSpell = p->wParam1;
		plr[pnum]._pSplType = plr[pnum]._pRSplType;
		plr[pnum]._pSplFrom = SPL_FROMR;
	}
	return sizeof(TCmdLocParam2);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_TSPELLXY( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLocParam2 * p = (const TCmdLocParam2 *) pCmd;
		
		DROPLOG("CMD_TSPELLXY(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		// PATCH2.JMM.3/5/97
		UINT nSplType = p->wParam1;

		if(currlevel == 0 && !spelldata[nSplType].sTownSpell) {
			IdentifyCheater("%s has cast an illegal spell.",plr[pnum]._pName);
			return sizeof(TCmdLocParam2);
		}
		// ENDPATCH2.JMM.3/5/97

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_SPELL;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
		plr[pnum].destParam3 = p->wParam2;
		plr[pnum]._pSpell = p->wParam1;
		plr[pnum]._pSplType = plr[pnum]._pTSplType;
		plr[pnum]._pSplFrom = SPL_FROMT;
	}

	return sizeof(TCmdLocParam2);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_OPOBJXY( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLocParam1 * p = (const TCmdLocParam1 *) pCmd;

		DROPLOG("CMD_OPOBJXY(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		if (!object[p->wParam1]._oSolidFlag && !object[p->wParam1]._oDoorFlag)
			MakePlrPath(pnum, p->x, p->y, TRUE);
		else
			MakePlrPath(pnum, p->x, p->y, FALSE);
		plr[pnum].destAction = PCMD_OPOBJ;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(TCmdLocParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_DISARMXY( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLocParam1 * p = (const TCmdLocParam1 *) pCmd;
		DROPLOG("CMD_DISARMXY(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		if (!object[p->wParam1]._oSolidFlag && !object[p->wParam1]._oDoorFlag)
			MakePlrPath(pnum, p->x, p->y, TRUE);
		else
			MakePlrPath(pnum, p->x, p->y, FALSE);

		plr[pnum].destAction = PCMD_DISARM;
		plr[pnum].destParam1 = p->wParam1;
	}
	return sizeof(TCmdLocParam1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_OPOBJT( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_OPOBJT(%d)\n",pnum);
		
		plr[pnum].destAction = PCMD_TELEK;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ATTACKID( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_ATTACKID(%d):  id->%d\n",pnum,p->wParam1);

		int dx = abs(plr[pnum]._px - monster[p->wParam1]._mfutx);
		int dy = abs(plr[pnum]._py - monster[p->wParam1]._mfuty);
		if ((dx > 1) || (dy > 1))
			MakePlrPath(pnum, monster[p->wParam1]._mfutx, monster[p->wParam1]._mfuty, FALSE);
		plr[pnum].destAction = PCMD_ATTACKID;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ATTACKPID( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_ATTACKPID(%d):  id->%d\n",pnum,p->wParam1);

		MakePlrPath(pnum, plr[p->wParam1]._pfutx, plr[p->wParam1]._pfuty, FALSE);
		plr[pnum].destAction = PCMD_ATTACKPID;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(TCmdParam1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_RATTACKID( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		
		DROPLOG("CMD_RATTACKID(%d):  id->%d\n",pnum,p->wParam1);

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_RATTACKID;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_RATTACKPID( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		
		DROPLOG("CMD_RATTACKPID(%d):  id->%d\n",pnum,p->wParam1);

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_RATTACKPID;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SPELLID( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam3 * p = (const TCmdParam3 *) pCmd;

		DROPLOG("CMD_SPELLID(%d):  id->%d\n",pnum,p->wParam1);

		// PATCH2.JMM.3/5/97
		UINT nSplType = p->wParam2;

		if(currlevel == 0 && !spelldata[nSplType].sTownSpell) {
			IdentifyCheater("%s has cast an illegal spell.",plr[pnum]._pName);
			return sizeof(TCmdParam3);
		}
		// ENDPATCH2.JMM.3/5/97

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_SPELLID;
		plr[pnum].destParam1 = p->wParam1;
		plr[pnum].destParam2 = p->wParam3;
		plr[pnum]._pSpell = p->wParam2;
		plr[pnum]._pSplType = plr[pnum]._pRSplType;
		plr[pnum]._pSplFrom = SPL_FROMR;
	}

	return sizeof(TCmdParam3);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SPELLPID( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam3 * p = (const TCmdParam3 *) pCmd;
		
		DROPLOG("CMD_SPELLPID(%d):  id->%d\n",pnum,p->wParam1);

		// PATCH2.JMM.3/5/97
		UINT nSplType = p->wParam2;

		if(currlevel == 0 && !spelldata[nSplType].sTownSpell) {
			IdentifyCheater("%s has cast an illegal spell.",plr[pnum]._pName);
			return sizeof(TCmdParam3);
		}
		// ENDPATCH2.JMM.3/5/97

		
		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_SPELLPID;
		plr[pnum].destParam1 = p->wParam1;
		plr[pnum].destParam2 = p->wParam3;
		plr[pnum]._pSpell = p->wParam2;
		plr[pnum]._pSplType = plr[pnum]._pRSplType;
		plr[pnum]._pSplFrom = SPL_FROMR;
	}

	return sizeof(TCmdParam3);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_TSPELLID( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam3 * p = (const TCmdParam3 *) pCmd;

		DROPLOG("CMD_TSPELLID(%d):  id->%d\n",pnum,p->wParam1);

		// PATCH2.JMM.3/5/97
		UINT nSplType = p->wParam2;

		if(currlevel == 0 && !spelldata[nSplType].sTownSpell) {
			IdentifyCheater("%s has cast an illegal spell.",plr[pnum]._pName);
			return sizeof(TCmdParam3);
		}
		// ENDPATCH2.JMM.3/5/97

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_SPELLID;
		plr[pnum].destParam1 = p->wParam1;
		plr[pnum].destParam2 = p->wParam3;
		plr[pnum]._pSpell = p->wParam2;
		plr[pnum]._pSplType = plr[pnum]._pTSplType;
		plr[pnum]._pSplFrom = SPL_FROMT;
	}

	return sizeof(TCmdParam3);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_TSPELLPID( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam3 * p = (const TCmdParam3 *) pCmd;

		DROPLOG("CMD_TSPELLPID(%d):  id->%d\n",pnum,p->wParam1);

		// PATCH2.JMM.3/5/97
		UINT nSplType = p->wParam2;

		if(currlevel == 0 && !spelldata[nSplType].sTownSpell) {
			IdentifyCheater("%s has cast an illegal spell.",plr[pnum]._pName);
			return sizeof(TCmdParam3);
		}
		// ENDPATCH2.JMM.3/5/97

		ClrPlrPath(pnum);
		plr[pnum].destAction = PCMD_SPELLPID;
		plr[pnum].destParam1 = p->wParam1;
		plr[pnum].destParam2 = p->wParam3;
		plr[pnum]._pSpell = p->wParam2;
		plr[pnum]._pSplType = plr[pnum]._pTSplType;
		plr[pnum]._pSplFrom = SPL_FROMT;
	}
	
	return sizeof(TCmdParam3);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_KNOCKBACK( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		
		DROPLOG("CMD_KNOCKBACK(%d):  id->%d\n",pnum,p->wParam1);

		if (currlevel == plr[pnum].plrlevel) {
			M_GetKnockback(p->wParam1);
			M_StartHit(p->wParam1, pnum, 0);
		}
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_RESURRECT( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_RESURRECT(%d):  id->%d\n",pnum,p->wParam1);

		DoResurrect(pnum, p->wParam1);

		// update character file to prevent cheating
		check_update_plr(pnum);
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_HEALOTHER( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_HEALOTHER(%d):  id->%d\n",pnum,p->wParam1);

		DoHealOther(pnum, p->wParam1);
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_TALKXY( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else if (currlevel == plr[pnum].plrlevel) {
		const TCmdLocParam1 * p = (const TCmdLocParam1 *) pCmd;
		
		DROPLOG("CMD_TALKXY(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		MakePlrPath(pnum, p->x, p->y, FALSE);
		plr[pnum].destAction = PCMD_TALK;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(TCmdLocParam1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_NEWLVL( const TCmd* pCmd, int pnum ) {

	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam2));
	}
	else if (pnum != myplr) {
		const TCmdParam2 * p = (const TCmdParam2 *) pCmd;

		DROPLOG("CMD_NEWLVL(%d)\n",pnum);

		StartNewLvl(pnum,p->wParam1,p->wParam2);
	}

	return sizeof(TCmdParam2);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_WARP( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;

		DROPLOG("CMD_WARP(%d)\n",pnum);

		StartWarpLvl(pnum, p->wParam1);

		// drb.patch1.start.02/15/97
		if ((pnum == myplr) && (curs >= ICSTART)) {
			item[TEMPAVAIL] = plr[myplr].HoldItem;
			AutoGetItem(myplr, TEMPAVAIL);
		}
		// drb.patch1.end.02/15/97

	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_MONSTDEATH( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdLocParam1));
	}
	else if (pnum != myplr) {
		const TCmdLocParam1 * p = (const TCmdLocParam1 *) pCmd;
		DROPLOG("CMD_MONSTDEATH(%d):  id->%d xy->(%d,%d)\n",pnum,p->wParam1,p->x,p->y);
		if (currlevel == plr[pnum].plrlevel)
			M_SyncStartKill(p->wParam1, p->x, p->y, pnum);
		delta_kill_monster(p->wParam1, p->x, p->y,plr[pnum].plrlevel);
	}

	return sizeof(TCmdLocParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_KILLGOLEM( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdLocParam1));
	}
	else if (pnum != myplr) {
		const TCmdLocParam1 * p = (const TCmdLocParam1 *) pCmd;
		DROPLOG("CMD_KILLGOLEM(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);
		if (currlevel == p->wParam1)
			M_SyncStartKill(pnum, p->x, p->y, pnum);
		delta_kill_monster(pnum, p->x, p->y, plr[pnum].plrlevel);
	}

	return sizeof(TCmdLocParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_AWAKEGOLEM( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) 
		mega_add_data(pnum, pCmd, sizeof(TCmdGolem));
	else {
		DROPLOG("CMD_AWAKENGOLEM(%d)\n",pnum);
		if (currlevel != plr[pnum].plrlevel)
			delta_sync_golem((const TCmdGolem *) pCmd, pnum, ((const TCmdGolem *) pCmd)->_currlevel);
		else {
			if (pnum != myplr) {
				BOOL addok = TRUE;
				for (int i = 0; i < nummissiles; i++) {
					int mi = missileactive[i];
					if ((missile[mi]._mitype == MIT_GOLEM) && (missile[mi]._misource == pnum)) addok = FALSE;
				}
				const TCmdGolem * pG = (const TCmdGolem *) pCmd;
				if (addok) AddMissile(plr[pnum]._px, plr[pnum]._py, pG->_mx, pG->_my, pG->_mdir, MIT_GOLEM, MI_ENEMYMONST, pnum, 0, 1);
			}
		}
	}

	return sizeof(TCmdGolem);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_MONSTDAMAGE( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam2));
	}
	else if (pnum != myplr) {
		const TCmdMonstDamage * p = (const TCmdMonstDamage *) pCmd;

		DROPLOG("CMD_MONSTDAMAGE(%d):  id->%d\n",pnum,p->wMonst);

		if (currlevel == plr[pnum].plrlevel) {
			monster[p->wMonst].mWhoHit |= (1 << pnum);
			if (monster[p->wMonst]._mhitpoints >= 0) {
				monster[p->wMonst]._mhitpoints -= p->dwDam;
				// take damage but don't kill him
				if ((monster[p->wMonst]._mhitpoints >> HP_SHIFT) < 1)
					monster[p->wMonst]._mhitpoints = 1 << HP_SHIFT;
				delta_monster_hp(p->wMonst,monster[p->wMonst]._mhitpoints,plr[pnum].plrlevel);
			}
		}
		else {
			// @@@@ Dave -- need some way to calculate new monster hp
			// since we don't have its info handy
		}
	}

	return sizeof(TCmdMonstDamage);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_PLRDEAD( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else if (pnum != myplr) {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		DROPLOG("CMD_PLAYERDEAD(%d):  id->%d\n",pnum,p->wParam1);
		SyncPlrKill(pnum, p->wParam1);
	}
	else {
		DROPLOG("CMD_PLAYERDEAD(I'm dead!!!)\n",pnum);
		check_update_plr(pnum);
	}

	return sizeof(TCmdParam1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_PLRDAMAGE( const TCmd* pCmd, int pnum ) {
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	const TCmdDamage * p = (const TCmdDamage *) pCmd;
	DROPLOG("CMD_PLAYERDAMAGE(%d)\n",pnum);
	
	if (p->bPlr != myplr) {
		// ignore message
	}
	// JMM.PATCH1
	else if (currlevel == 0) {
		// illegal message!
	}
	// JMM.ENDPATCH1
	else if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered, because
		// the player isn't active yet, so he
		// can't be taking any damage
	}
	// PATCH2.JMM.3/5/97
	else if (currlevel != plr[pnum].plrlevel) {
		// illegal message!		(player must be on same level)
	}
	else if ( p->dwDam > (3000<<HP_SHIFT)) {
		// illegal message!
	}
	// ENDPATCH2
	else {
		DROPLOG("  I'm taking damage.\n");
		DROPLOG("  Current level->%d\n",currlevel);
		// If I am already dead, don't kill me again.
		if ((plr[myplr]._pHitPoints >> HP_SHIFT) > 0) {
			// not dead yet
			drawhpflag = TRUE;
			plr[myplr]._pHitPoints -= p->dwDam;
			plr[myplr]._pHPBase -= p->dwDam;
			if (plr[myplr]._pHitPoints > plr[myplr]._pMaxHP) {
				plr[myplr]._pHitPoints = plr[myplr]._pMaxHP;
				plr[myplr]._pHPBase = plr[myplr]._pMaxHPBase;
			}
			if ((plr[myplr]._pHitPoints >> HP_SHIFT) <= 0) {
				// rjs - manashld fix? - plr[myplr]._pHitPoints = 0;
				StartPlrKill(myplr, TRUE);
			}
		}
	}

	return sizeof(TCmdDamage);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_OPENDOOR( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		DROPLOG("CMD_OPENDOOR(%d):  id->%d\n",pnum,p->wParam1);
		if (currlevel == plr[pnum].plrlevel)
			SyncOpObject(pnum, CMD_OPENDOOR, p->wParam1);
		delta_sync_object(p->wParam1,CMD_OPENDOOR,plr[pnum].plrlevel);
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**----------------------------------------------------------------------*/
static DWORD On_CLOSEDOOR( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		DROPLOG("CMD_CLOSEDOOR(%d):  id->%d\n",pnum,p->wParam1);
		if (currlevel == plr[pnum].plrlevel)
			SyncOpObject(pnum, CMD_CLOSEDOOR, p->wParam1);
		delta_sync_object(p->wParam1,CMD_CLOSEDOOR,plr[pnum].plrlevel);
	}

	return sizeof(TCmdParam1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_OPERATEOBJ( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		DROPLOG("CMD_OPERATEOBJ(%d):  id->%d\n",pnum,p->wParam1);
		if (currlevel == plr[pnum].plrlevel)
			SyncOpObject(pnum, CMD_OPERATEOBJ, p->wParam1);
		delta_sync_object(p->wParam1,CMD_OPERATEOBJ,plr[pnum].plrlevel);
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_PLROPOBJ( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam2));
	}
	else {
		const TCmdParam2 * p = (const TCmdParam2 *) pCmd;
		DROPLOG("CMD_PLAYEROPOBJ(%d):  plrid->%d objid->%d\n",pnum,p->wParam1,p->wParam2);
		if (currlevel == plr[pnum].plrlevel)
			SyncOpObject(p->wParam1, CMD_PLROPOBJ, p->wParam2);
		delta_sync_object(p->wParam2,CMD_PLROPOBJ,plr[pnum].plrlevel);
	}

	return sizeof(TCmdParam2);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_BREAKOBJ( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam2));
	}
	else {
		const TCmdParam2 * p = (const TCmdParam2 *) pCmd;
		DROPLOG("CMD_BREAKOBJ(%d):  id1->%d id2->%d\n",pnum,p->wParam1,p->wParam2);
		if (currlevel == plr[pnum].plrlevel)
			SyncBreakObj(p->wParam1, p->wParam2);
		delta_sync_object(p->wParam2,CMD_BREAKOBJ,plr[pnum].plrlevel);
	}

	return sizeof(TCmdParam2);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_CHANGEPLRITEMS( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdChItem));
	}
	else {
		const TCmdChItem * p = (const TCmdChItem *) pCmd;
		DROPLOG("CMD_CHANGEPLAYERITEMS(%d):  index->%8.8x ci->%8.8x seed->%8.8x\n",pnum,p->wIndx, p->wCI, p->dwSeed);
		// drb.patch1.start.02/10/97
		// if (pnum != myplr) SyncInvPaste(pnum, p->bLoc, p->wIndx, p->wCI, p->dwSeed);
		if (pnum != myplr) SyncInvPaste(pnum, p->bLoc, p->wIndx, p->wCI, p->dwSeed, p->bId);
		// drb.patch1.end.02/10/97
	}

	return sizeof(TCmdChItem);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_DELPLRITEMS( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdDelItem));
	}
	else {
		const TCmdDelItem * p = (const TCmdDelItem *) pCmd;
		DROPLOG("CMD_DELPLAYERITEMS(%d):  loc->%d\n",pnum, p->bLoc);
		if (pnum != myplr) SyncInvCut(pnum, p->bLoc);
	}

	return sizeof(TCmdDelItem);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_PLRLEVEL( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		DROPLOG("CMD_PLAYERLEVEL(%d)\n",pnum);

		if ((p->wParam1 <= MAX_LEVEL) && pnum != myplr) 
			plr[pnum]._pLevel = (char) p->wParam1;
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_DROPITEM( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdPItem));
	}
	else {
		// item dropped by a dead monster/chest/etc.
		// doesn't have to do anything to level data
		// only to delta info
		const TCmdPItem * p = (const TCmdPItem *) pCmd;
		DROPLOG("CMD_DROPITEM(%d):  index->%8.8x ci->%8.8x seed->%8.8x xy->(%d,%d)\n",pnum,p->wIndx, p->wCI, p->dwSeed, p->x, p->y);
		delta_put_item(p,p->x,p->y,plr[pnum].plrlevel);
	}

	return sizeof(TCmdPItem);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SEND_PLRINFO( const TCmd* pCmd, int pnum ) {
	const TCmdPlrInfoHdr * p = (const TCmdPlrInfoHdr *) pCmd;
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,p,p->wBytes + sizeof(TCmdPlrInfoHdr));
	}
	else {
		DROPLOG("CMD_ACK\\SENDPLRINFO(%d)\n",pnum);
		recv_plrinfo(pnum,p,p->bCmd == CMD_ACK_PLRINFO);
	}

	return p->wBytes + sizeof(TCmdPlrInfoHdr);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ACK_PLRINFO( const TCmd* pCmd, int pnum ) {
	return On_SEND_PLRINFO( pCmd, pnum );
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_PLAYER_JOINLEVEL( const TCmd* pCmd, int pnum ) {

 	if (gbBufferMsgs == BUFFER_ON) {
 		mega_add_data(pnum,pCmd,sizeof(TCmdLocParam1));
 	}
 	else {
 		app_assert((DWORD)pnum < MAX_PLRS);
 		DROPLOG("CMD_PLAYERJOINLEVEL(%d)\n",pnum);

 		plr[pnum]._pLvlChanging = FALSE;

 		// joining game?
 		if (! plr[pnum]._pName[0]) {
 			// we are probably unbuffering messages,
 			// and we received this message before we
 			// received the message that tells us who
 			// this player is.  Just ignore this message.
 			// Eventually we will get a PLRINFO message
 			// which will activate this player
 			#if TRACEOUT
 			TraceOut("(%d) received %d joinlevel before plrdata",myplr,pnum);
 			#endif
 		}
 		else if (! plr[pnum].plractive) {
 			plr[pnum].plractive = 1;
 			gbActivePlayers++;
 			sysmsg_add("Player '%s' (level %d) just joined the game",plr[pnum]._pName,plr[pnum]._pLevel);
 			#if TRACEOUT
 			TraceOut("(%d) activating %d on joinlevel",myplr,pnum);
 			#endif
 		}

 		// activate player on this level
 		if (plr[pnum].plractive && myplr != pnum) {
 			const TCmdLocParam1 * p = (const TCmdLocParam1 *) pCmd;
 			plr[pnum]._px = p->x;
 			plr[pnum]._py = p->y;
 			plr[pnum].plrlevel = p->wParam1;
 			plr[pnum]._pGFXLoad = 0;
 			if (currlevel == plr[pnum].plrlevel) {
 				LoadPlrGFX(pnum,PGL_STAND);
 				SyncInitPlr(pnum);
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
 				plr[pnum]._pvid = AddVision(plr[pnum]._px, plr[pnum]._py,
 											plr[pnum]._pLightRad, pnum==myplr);
 				plr[pnum]._plid = -1;
 			}
 		}
 	}
	return sizeof(TCmdLocParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ACTIVATEPORTAL( const TCmd* pCmd, int pnum ) {

	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdLocParam3));
	}
	else {
		const TCmdLocParam3 * p = (const TCmdLocParam3 *) pCmd;

		DROPLOG("CMD_ACTIVATEPORTAL(%d):  xy->(%d,%d)\n",pnum,p->x,p->y);

		ActivatePortal(pnum, p->x, p->y, p->wParam1, p->wParam2, p->wParam3);
		if (pnum != myplr) {
			if (currlevel == 0) {
				AddInTownPortal(pnum);
			} else {
				if (currlevel == plr[pnum].plrlevel) {
					// If on same level make sure portal appears
					int i,mi;
					BOOL addok = TRUE;
					for (i = 0; i < nummissiles; i++) {
						mi = missileactive[i];
						if ((missile[mi]._mitype == MIT_TOWN) && (missile[mi]._misource == pnum)) addok = FALSE;
					}
					if (addok) AddWarpMissile(pnum, p->x, p->y);
				} else {
					// Remove portal if it was on my level before
					RemovePortalMissile(pnum);
				}
			}
		}
		delta_open_portal(pnum,p->x,p->y,(BYTE) p->wParam1,(BYTE) p->wParam2, (BYTE) p->wParam3);
	}

	return sizeof(TCmdLocParam3);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_DEACTIVATEPORTAL( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmd));
	}
	else {
		DROPLOG("CMD_DEACTIVEPORTAL(%d)\n",pnum);
		if (PortalOnLevel(pnum)) RemovePortalMissile(pnum);
		DeactivatePortal(pnum);
		delta_close_portal(pnum);
	}

	return sizeof(TCmd);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_RETOWN( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmd));
	}
	else {
		DROPLOG("CMD_RETOWN(%d)\n",pnum);
		if (pnum == myplr) {
			deathflag = FALSE;
			gamemenu_off();
		}
		RestartTownLvl(pnum);
	}

	return sizeof(TCmd);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SETSTR( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		DROPLOG("CMD_SETSTR(%d)\n",pnum);

		if( p->wParam1 > MAX_STAT )
			return sizeof( TCmdParam1 );

		if (pnum != myplr) {
			SetPlrStr(pnum, p->wParam1);
		}
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SETDEX( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		DROPLOG("CMD_SETDEX(%d)\n",pnum);

		if( p->wParam1 > MAX_STAT )
			return sizeof( TCmdParam1 );

		if (pnum != myplr) {
			SetPlrDex(pnum, p->wParam1);
		}
	}

	return sizeof(TCmdParam1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SETMAG( const TCmd* pCmd, int pnum ) {

	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		DROPLOG("CMD_SETMAG(%d)\n",pnum);

		if( p->wParam1 > MAX_STAT )
			return sizeof( TCmdParam1 );


		if (pnum != myplr) {
			SetPlrMag(pnum, p->wParam1);
		}
	}

	return sizeof(TCmdParam1);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SETVIT( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdParam1));
	}
	else {
		const TCmdParam1 * p = (const TCmdParam1 *) pCmd;
		DROPLOG("CMD_SETVIT(%d)\n",pnum);
		
		if( p->wParam1 > MAX_STAT )
			return sizeof( TCmdParam1 );


		if (pnum != myplr) {
			SetPlrVit(pnum, p->wParam1);
		}
	}

	return sizeof(TCmdParam1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_STRING( const TCmd* pCmd, int pnum ) {

		DROPLOG("CMD_STRING(%d)\n",pnum);
		// cmd_string handles gbBufferMsgs
		return cmd_string(pnum,(const TCmdString *) pCmd);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_SYNCQUEST( const TCmd* pCmd, int pnum ) {
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmdQuest));
	}
	else {
		DROPLOG("CMD_SYNCQUEST(%d)\n",pnum);
		if (pnum != myplr) {
			const TCmdQuest * p = (const TCmdQuest *) pCmd;
			SetMultiQuest(p->q, p->qstate, p->qlog, p->qvar1);
		}

		sgbDeltaChanged = TRUE;
	}

	return sizeof(TCmdQuest);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ENDSHIELD( const TCmd* pCmd, int pnum ) {

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else {
		DROPLOG("CMD_ENDSHIELD(%d)\n",pnum);
		if ((pnum != myplr) && (currlevel == plr[pnum].plrlevel)) {
			void DeleteMissile(int, int);
			for (int i = 0; i < nummissiles; i++) {
				int mi = missileactive[i];
				if ((missile[mi]._mitype == MIT_MANASHIELD) && (missile[mi]._misource == pnum)) {
					ClearMissileSpot(mi);
					DeleteMissile(mi, i);
				}
			}

		}
	}

	return sizeof(TCmd);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_ENDREFLECT( const TCmd* pCmd, int pnum ) {

	if (gbBufferMsgs == BUFFER_ON) {
		// doesn't need to be buffered
	}
	else {
		DROPLOG("CMD_ENDREFLECT(%d)\n",pnum);
		if ((pnum != myplr) && (currlevel == plr[pnum].plrlevel)) {
			void DeleteMissile(int, int);
			for (int i = 0; i < nummissiles; i++) {
				int mi = missileactive[i];
				if ((missile[mi]._mitype == MIT_REFLECT) && (missile[mi]._misource == pnum)) {
					ClearMissileSpot(mi);
					DeleteMissile(mi, i);
				}
			}

		}
	}

	return sizeof(TCmd);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_CHEAT_EXPERIENCE( const TCmd* pCmd, int pnum ) {

#if _DEBUG
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmd));
	}
	else {
		DROPLOG("CMD_CHEATEXPERIENCE(%d)\n",pnum);
		if (plr[pnum]._pLevel < 50) {
			plr[pnum]._pExperience = plr[pnum]._pNextExper;
			NextPlrLevel(pnum);
		}
	}
#endif

	return sizeof(TCmd);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_CHEAT_SPELL_LEVEL( const TCmd* pCmd, int pnum ) {

#if _DEBUG
	if (gbBufferMsgs == BUFFER_ON) {
		mega_add_data(pnum,pCmd,sizeof(TCmd));
	}
	else {
		DROPLOG("CMD_CHEATSPELLLEVEL(%d)\n",pnum);
		plr[pnum]._pSplLvl[plr[pnum]._pRSpell]++;
	}
#endif

	return sizeof(TCmd);
}
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_OPEN_NAKRUL(const TCmd* pCmd, int pnum)
{
	if (gbBufferMsgs == BUFFER_ON) 
	{
		// doesn't need to be buffered
	}
	else 
	{
		DROPLOG("CMD_OPEN_NAKRUL(%d)\n",pnum);

		OpenNaKrul();
		Na_Krul.Books = TRUE;
		quests[Q_NA_KRUL]._qactive = QUEST_DONE;
		Hose_NaKrul();

	}
	return sizeof(TCmd);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_OPEN_NEST(const TCmd * pCmd, int pnum)
{
	if (gbBufferMsgs == BUFFER_ON) 
	{
		// doesn't need to be buffered
	}
	else 
	{
		DROPLOG("CMD_OPEN_NEST(%d)\n",pnum);
		const TCmdLocParam2 * p = (const TCmdLocParam2 *) pCmd;
		
		AddMissile (p->x, p->y, p->wParam1, p->wParam2, 0, MIT_OPENNEST, MI_PLR, pnum, 0, 0);
		OpenNest();
	}
	return sizeof(TCmdLocParam2);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_OPEN_CRYPT(const TCmd* pCmd, int pnum)
{
	if (gbBufferMsgs == BUFFER_ON) 
	{
		// doesn't need to be buffered
	}
	else 
	{
		DROPLOG("CMD_OPEN_CRYPT(%d)\n",pnum);

		OpenCrypt();
		InitTownTriggers();
		if (currlevel == TLVL_START)	// In town
			PlaySFX(IS_SARC);

	}
	return sizeof(TCmd);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static DWORD On_DEBUG( const TCmd* pCmd, int pnum ) {
	return sizeof(TCmd);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
// PATCH3.JMM
extern WORD sgwPackPlrOffsetTbl[MAX_PLRS];
// ENDPATCH3.JMM
#define HANDLE_CMD( c ) case CMD_##c : return On_##c( pCmd, pnum )
DWORD ParseCmd(int pnum,const TCmd * pCmd) {
	app_assert((DWORD)pnum < MAX_PLRS);
	app_assert(pCmd != NULL);

	static BYTE sbLastCmd;
	sbLastCmd = pCmd->bCmd;

	// patch3.jmm
	if( sgwPackPlrOffsetTbl[pnum] && 
		sbLastCmd != CMD_ACK_PLRINFO &&
		sbLastCmd != CMD_SEND_PLRINFO ) 
		return 0;
	// endpatch3.jmm

	switch(pCmd->bCmd) {
		HANDLE_CMD( SYNCDATA );
		HANDLE_CMD( WALKXY );
		HANDLE_CMD( ADDSTR );
		HANDLE_CMD( ADDDEX );
		HANDLE_CMD( ADDMAG );
		HANDLE_CMD( ADDVIT );
		HANDLE_CMD( SBSPELL );
		HANDLE_CMD( GOTOGETITEM );
		HANDLE_CMD( REQUESTGITEM );
		HANDLE_CMD( GETITEM );
		HANDLE_CMD( GOTOAGETITEM );
		HANDLE_CMD( REQUESTAGITEM );
		HANDLE_CMD( AGETITEM );
		HANDLE_CMD( ITEMEXTRA );
		HANDLE_CMD( PUTITEM );
		HANDLE_CMD( SYNCPUTITEM );
		HANDLE_CMD( RESPAWNITEM );
		HANDLE_CMD( ATTACKXY );
		HANDLE_CMD( SATTACKXY );
		HANDLE_CMD( RATTACKXY );
		HANDLE_CMD( SPELLXYD );
		HANDLE_CMD( SPELLXY );
		HANDLE_CMD( TSPELLXY );
		HANDLE_CMD( OPOBJXY );
		HANDLE_CMD( DISARMXY );
		HANDLE_CMD( OPOBJT );
		HANDLE_CMD( ATTACKID );
		HANDLE_CMD( ATTACKPID );
		HANDLE_CMD( RATTACKID );
		HANDLE_CMD( RATTACKPID );
		HANDLE_CMD( SPELLID );
		HANDLE_CMD( SPELLPID );
		HANDLE_CMD( TSPELLID );
		HANDLE_CMD( TSPELLPID );
		HANDLE_CMD( KNOCKBACK );
		HANDLE_CMD( RESURRECT );
		HANDLE_CMD( HEALOTHER );
		HANDLE_CMD( TALKXY );
		HANDLE_CMD( DEBUG );
		HANDLE_CMD( NEWLVL );
		HANDLE_CMD( WARP );
		HANDLE_CMD( MONSTDEATH );
		HANDLE_CMD( KILLGOLEM );
		HANDLE_CMD( AWAKEGOLEM );
		HANDLE_CMD( MONSTDAMAGE );
		HANDLE_CMD( PLRDEAD );
		HANDLE_CMD( PLRDAMAGE );
		HANDLE_CMD( OPENDOOR );
		HANDLE_CMD( CLOSEDOOR );
		HANDLE_CMD( OPERATEOBJ );
		HANDLE_CMD( PLROPOBJ );
		HANDLE_CMD( BREAKOBJ );
		HANDLE_CMD( CHANGEPLRITEMS );
		HANDLE_CMD( DELPLRITEMS );
		HANDLE_CMD( PLRLEVEL );
		HANDLE_CMD( DROPITEM );
		HANDLE_CMD( ACK_PLRINFO );
		HANDLE_CMD( SEND_PLRINFO );
		HANDLE_CMD( PLAYER_JOINLEVEL );
		HANDLE_CMD( ACTIVATEPORTAL );
		HANDLE_CMD( DEACTIVATEPORTAL );
		HANDLE_CMD( RETOWN );
		HANDLE_CMD( SETSTR );
		HANDLE_CMD( SETMAG );
		HANDLE_CMD( SETDEX );
		HANDLE_CMD( SETVIT );
		HANDLE_CMD( STRING );
		HANDLE_CMD( SYNCQUEST );
		HANDLE_CMD( ENDSHIELD );
		HANDLE_CMD( CHEAT_EXPERIENCE );
		HANDLE_CMD( CHEAT_SPELL_LEVEL );
		HANDLE_CMD( ENDREFLECT );
		HANDLE_CMD( OPEN_NAKRUL );
		HANDLE_CMD( OPEN_NEST );
		HANDLE_CMD( OPEN_CRYPT );
		
		default:
			// dreceive_level handles gbBufferMsgs
			if (pCmd->bCmd >= CMD_DLEVEL_0 && pCmd->bCmd <= CMD_DLEVEL_END)
				return dreceive_chunk(pnum,(const TCmdPlrInfoHdr *) pCmd);
// PATCH3.JMM
//			app_fatal("Unknown PCMD %d (last cmd %d)\n",pCmd->bCmd,sbLastCmd);
			SNetDropPlayer( pnum, SNET_EXIT_NOTRESPONDING );
			return 0;		// 0 is error condition:  ditch rest of packet
// ENDPATCH3.JMM
	}


	// NO final return statement -- each case
	// should have its own return statement
}
// ENDPATCH2.JMM.3/5/97

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*
int GetLDeltaItem(int itm)
{
	TCmdPItem * pD = &sgLevels[currlevel].item[0];
	for (int i = 0; i < MAXITEMS; i++,pD++) {
		// find item
		if (pD->bCmd == ITEM_FREE) continue;
		if (pD->wIndx != item[itm].IDidx) continue;
		if (pD->wCI != item[itm]._iCreateInfo) continue;
		if (pD->dwSeed != (DWORD)item[itm]._iSeed) continue;
		return(i);
	}
	return(-1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
/*
old old old
void test_delta() {
	static DLevel lvl[NUMLEVELS];

	// randomize the level delta data
	sgbDeltaChanged = TRUE;
	srand(time(NULL));
	WORD * p = (WORD *) &sgLevels[0];
	for (int i = sizeof(sgLevels)/2; i--; )
		*p++ = rand();

	// save level data for later comparison
	CopyMemory(&lvl[0],&sgLevels[0],sizeof lvl);

	// send level delta info to myself
	DeltaSendAllLevels(myplr);

	// reinit level delta info
	ZeroMemory(&sgLevels[0],sizeof sgLevels);

	// get level delta packets
	NetReceivePackets();

	// make sure everything comps ok
	for (i = 0; i < NUMLEVELS; i++) {
		for (int j = 0; j < MAXITEMS; j++) {
			if (sgLevels[i].item[j].bCmd == INIT_VAL) {
				if (lvl[i].item[j].bCmd != INIT_VAL)
					DebugBreak();
			}
			else if (memcmp(&sgLevels[i].item[j],&lvl[i].item[j],sizeof(TCmdPItem))) {
				DebugBreak();
			}
		}
		for (j = 0; j < MAXOBJECTS; j++) {
			if (memcmp(&sgLevels[i].object[j],&lvl[i].object[j],sizeof(DObjectStr)))
				DebugBreak();
		}
		for (j = 0; j < MAXMONSTERS; j++) {
			if (sgLevels[i].monster[j]._mx == INIT_VAL) {
				if (lvl[i].monster[j]._mx != INIT_VAL)
					DebugBreak();
			}
			else if (memcmp(&sgLevels[i].monster[j],&lvl[i].monster[j],sizeof(DMonsterStr))) {
				DebugBreak();
			}
		}
	}

	// reinit delta info
	delta_init();
}
*/
