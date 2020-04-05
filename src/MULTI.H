/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MULTI.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/


//******************************************************************
// constants
//******************************************************************
// for every X async messages, we send X/8 sync messages
#define ASYNC_CYCLES_PER_SYNC		4

// Near house
#define STARTX	75
#define STARTY	68

#define TURN_REQUEST_DELTA_FLAG		0x80000000
#define TURN_COUNTER_MASK			0x7fffffff
#define TURN_COUNTER_RESET_MASK		0x0000ffff


//******************************************************************
// variables
//******************************************************************
extern BYTE gbMaxPlayers;
extern BYTE gbActivePlayers;
extern BYTE gbDropInactive;
extern BYTE gbGameDestroyed;
extern BYTE gbGameLoopsPerPacket;
extern DWORD gdwTurnsInTransit;
extern BYTE gbDeltaSender;
extern char gszGameName[128];
extern char gszGamePass[128];


enum {
	// msg buffering off when program running in normal game mode
	BUFFER_OFF = 0,

	// msg buffering on when program waiting for level delta info,
	// because program cannot handle any other messages besides deltas
	BUFFER_ON,

	// processing msgs which were stored in the message buffer
	BUFFER_PROCESS
};
extern BYTE gbBufferMsgs;


// results from receiving the last synchronous turn
extern DWORD gdwMsgLenTbl[MAX_PLRS];
extern LPVOID glpMsgTbl[MAX_PLRS];
extern DWORD gdwMsgStatTbl[MAX_PLRS];


//******************************************************************
// functions
//******************************************************************
// multi.cpp
BOOL NetInit(BOOL bSinglePlayer,BOOL * pfExitProgram);
void NetClose();
void NetReceivePackets();
BOOL NetEndSendCycle();
void NetDropInactive();
void process_turn();

void NetSendLoPri(const BYTE * pbMsg,BYTE bLen);
void NetSendHiPri(const BYTE * pbMsg,BYTE bLen);
void NetSendMyselfPri(const BYTE * pbMsg,BYTE bLen);
void NetSendMask(DWORD dwSendMask,const BYTE * pbMsg,BYTE bLen);

// sync.cpp
void sync_init();
DWORD sync_get(BYTE * pbBuf,DWORD dwMaxLen);
DWORD sync_update(int pnum,const BYTE * pbBuf);

// nthread.cpp
void nthread_init(BOOL bRequestDelta);
void nthread_free();
BOOL nthread_run_gameloop(BOOL bReloop);
void nthread_perform_keepalive(BOOL bStart);
DWORD nthread_fill_sync_queue(DWORD dwCounter,DWORD dwIncrement);
BOOL nthread_msg_check(BOOL * pfSendAsync);
