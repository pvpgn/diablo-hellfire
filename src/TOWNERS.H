/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/TOWNERS.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

#include "quests.h"

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAX_TOWNERS		16		

//#define TALKQUESTS		17

// These are interactive people in town, so @ beginning of towner list
#define TWN_BLKSMITH	0
#define TWN_HEALER		1
#define TWN_DEAD		2
#define TWN_BAROWNER	3
#define TWN_TELLER		4
#define TWN_DRUNK		5
#define TWN_WITCH		6
#define TWN_BARMAID		7
#define TWN_BOY			8
#define TWN_COW			9
#define TWN_FARMER		10
#define TWN_GIRL		11
#define TWN_COWSUIT		12

#define QSTMSG_NOTAVAIL	0
#define QSTMSG_AVAIL	1
#define QSTMSG_SAID		2
#define QSTMSG_NOTSAID	3

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	byte _qsttype;		// Quest type
	byte _qstmsg;		// Quest message
	byte _qstmsgact;		// Quest message flag
} TNQ;

typedef struct {
	int _tmode;			// towners current mode
	int _ttype;			// towner type
	int _tx;			// plr map x
	int _ty;			// plr map y
	long _txoff;		// offset x from left of map tile
	long _tyoff;		// offset y from bottom of map tile
	long _txvel;		// current x rate
	long _tyvel;		// current y rate
	int _tdir;			// current towner direction
	BYTE *_tAnimData;	// Data pointer to anim tables
	int _tAnimDelay;	// anim delay amount
	int _tAnimCnt;		// current anim delay value
	int	_tAnimLen;		// number of anim frames
	int	_tAnimFrame;	// current anim frame
	int	_tAnimFrameCnt;	// current anim frame count into AnimOrder
	char _tAnimOrder;	// Animation order or not
	long _tAnimWidth;	// width of anim frames
	long _tAnimWidth2;	// (width - 64) / 2 of towner for drawing
	int _tTenPer;		// Ten percent of time (special message said)
	int _teflag;		// draw extra tile to left for walk fix (flag)
	int _tbtcnt;		// Big text count
	BOOL _tSelFlag;		// Is the town person selectable?
	BOOL _tMsgSaid;		// Has the towner given his talk this trip to town?
	TNQ	qsts[MAXQUESTS];// quest struct for talking about quests

	int _tSeed;

	long _tVar1;		// scratch var 1
	long _tVar2;		// scratch var 2
	long _tVar3;		// scratch var 3
	long _tVar4;		// scratch var 4

	char _tName[32];	// Towners name

	BYTE *_tNAnim[8];	// Neutral anims
	int	_tNFrames;		// Number of neutral frames

	// Anything below this will not be saved or sent during a sync
	BYTE *_tNData;		// Neutral anim memory
} TownerStruct;


typedef struct {
	int	_qinfra;	// Infravision
	int	_qblkm;		// Black Mushroom
	int	_qgarb;
	int	_qzhar;
	int	_qveil;		// Veil of Steel
	int	_qmod;		// Map of Doom
	int	_qbutch;	// Butcher
	int	_qbol;		// Banner of Light
	int	_qblind;	// Halls of Blind
	int	_qblood;	// Stones of Blood
	int	_qanvil;	// Anvil of Fury
	int	_qwarlrd;	// Warlord of Blood
	int	_qking;		// Skeleton King
	int	_qpw;		// Poison Water
	int	_qbone;		// Chamber of Bone
	int	_qvb;		// Vile Betrayer
} QuestTalkData;


/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern TownerStruct towner[MAX_TOWNERS];
extern int Qtalklist[14][MAXQUESTS];		// use define JKENEWTOWNERS
extern void SpawnBomb(int,int);

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitTowners();
void FreeTownerGFX();
void ProcessTowners();
void TalkToTowner(int, int);
ItemStruct *PlrHasItem(int pnum, int item, int &i);
