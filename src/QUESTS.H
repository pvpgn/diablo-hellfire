/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/QUESTS.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/
#ifndef MAXQUESTS		// only one copy in existsence	JKE



/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXQUESTS		24		// JKEQUEST add to this when adding new quests
#define MAXMULTIQUESTS	9

#define Q_ROCK			0
#define Q_BKMUSHRM		1
#define Q_GARBUD		2
#define Q_ZHAR			3
#define Q_VEIL			4
#define Q_DIABLO		5
#define Q_BUTCHER		6
#define Q_LTBANNER		7
#define Q_BLIND			8
#define Q_BLOOD			9
#define	Q_ANVIL			10
#define Q_WARLORD		11
#define Q_SKELKING		12
#define Q_PWATER		13
#define Q_SCHAMB		14
#define Q_BETRAYER		15
#define Q_CRYPTMAP		16	// JKEQUEST
#define Q_FARMER		17
#define Q_THEO			18
#define Q_TRADER		19
#define Q_DEFILER		20
#define Q_NA_KRUL		21
#define Q_CORNERSTONE	22
#define Q_COWSUIT		23

#define QUEST_NOTAVAIL	0
#define QUEST_NOTACTIVE	1
#define QUEST_NOTDONE	2
#define QUEST_DONE		3
#define QUEST_TOOWEAK1	4
#define QUEST_TOOWEAK2	5
#define QUEST_TOOWEAK3	6
#define QUEST_PASS1		7
#define QUEST_PASS2		8
#define QUEST_PASS3		9
#define QUEST_REALLYDONE 10


#define QFLAG_MULTI		1

// Vile Betrayer Red Portal
#define QS_VBRPOFF		0		//	Red Portal not init
#define QS_VBRP1		1		//	Red Portal lvl 15 on
#define QS_VBRP2		2		//	Red Portal lvl 15 off
#define QS_VBRP3		3		//	Red Portal set lvl on
#define	QS_VBRP4		4		//	Red Portal set lvl off

// Mushroom Quest states
enum {
	QS_INIT=0,
	QS_TOMESPAWNED,
	QS_TOMEGIVEN,
	QS_MUSHSPAWNED,
	QS_MUSHPICKED,
	QS_MUSHGIVEN,
	QS_BRAINSPAWNED,
	QS_BRAINGIVEN,			// == elixir spawned
};

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	byte _qlevel;		// Which level the quest will appear on
	byte _qtype;		// Which quest it is
	byte _qactive;		// Not avail, not active, not completed, done flag
	byte _qlvltype;		// Quest level type (255 is current)
	int _qtx;			// X trigger to level
	int _qty;			// Y trigger to level
	byte _qslvl;		// Which set level is it?
	byte _qidx;			// quest data index
	int _qmsg;			// current quest msg
	BYTE _qvar1;		// quest-dependent state variable
	BYTE _qvar2;
	BOOL _qlog;			// display in quest log
} QuestStruct;
#define SAVE_QUEST_SIZE sizeof(QuestStruct)

typedef struct {
	byte _qdlvl;		// level the quest will appear on
	char _qdmultlvl;	// Which level the quest will appear on in multiplayer
	byte _qlvlt;		// Level gfx type
	byte _qdtype;		// Which quest it is
	byte _qdrnd;		// Random percent of quest appearing
	byte _qslvl;		// Set level number
	BOOL _qflags;		// Quest flags
	int _qdmsg;			// initial quest message
	char *_qlstr;		// String description for log
} QuestData;


/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern QuestStruct quests[MAXQUESTS];

extern BOOL questlog;

extern BYTE *pQLogCel;

extern int ReturnLvlX, ReturnLvlY, ReturnLvl, ReturnLvlT;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitQuests();
void DRLG_CheckQuests(int, int);
void CheckQuests();
void CheckQuestKill(int, BOOL);

void SetReturnLvlPos();
void GetReturnLvlPos();

void ResyncQuests();
void ResyncMPQuests();

void DrawQuestLog();

void StartQuestlog();
void QuestlogUp();
void QuestlogDown();
void QuestlogEnter();
void QuestlogESC();
void CheckQLogBtn();

BOOL ForceQuests();
BOOL QuestStatus(int);

void SetMultiQuest(int, int, BOOL, int);


#endif