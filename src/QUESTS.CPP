/*-----------------------------------------------------------------------**
** 								Diablo
**
**							  Quests file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/QUESTS.CPP 3     1/23/97 4:51p Vdwel $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"quests.h"
#include	"gendung.h"
#include	"engine.h"
#include	"control.h"
#include	"items.h"
#include	"objects.h"
#include	"objdat.h"
#include	"player.h"
#include	"monster.h"
#include	"monstdat.h"
#include	"drlg_l1.h"
#include	"stores.h"
#include	"scrollrt.h"
#include	"minitext.h"
#include	"textdat.h"
#include	"effects.h"
#include	"cursor.h"
#include	"palette.h"
#include	"msg.h"
#include	"multi.h"
#include	"doom.h"
#include	"missiles.h"
#include	"trigs.h"
#include	"towners.h"
#include	"setmaps.h"

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL delta_quest_inited(int i);

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

QuestStruct quests[MAXQUESTS];

int ReturnLvlX, ReturnLvlY, ReturnLvl, ReturnLvlT;
BOOL rporttest;
BOOL questlog;
int qspin, qline, numqlines, qtopline;
int qlist[MAXQUESTS];			// index list of displayed quests in questlog
int qfade;


BYTE *pQLogCel;

#if CHEATS
int questcheat = -1;			// TEMP!!!!!! drb
#endif

/*-----------------------------------------------------------------------*
** private
**-----------------------------------------------------------------------*/
// level, multi lvl, lvl type, id, % chance, setlevel #, multi flag, qmsg, string
QuestData questlist[] = {
	// Contained
	{  5, -1, 255, Q_ROCK,		100, 0, 0,				TXT_INFRABS1,	"The Magic Rock" },			// 0
	{  9, -1, 255, Q_BKMUSHRM,	100, 0, 0,				TXT_BLKMW1,		"Black Mushroom" },			// 1 
	{  4, -1, 255, Q_GARBUD,	100, 0, 0,				TXT_GARB1,		"Gharbad The Weak" },		// 2
	{  8, -1, 255, Q_ZHAR,		100, 0, 0,				TXT_ZHAR1,		"Zhar the Mad" },			// 3 
	{ 14, -1, 255, Q_VEIL,		100, 0, 0,				TXT_VEIL1,		"Lachdanan" },				// 4 
	{ 15, -1, 255, Q_DIABLO,	100, 0, QFLAG_MULTI,	TXT_VBST3,		"Diablo" },					// 5 
	// Internal Set Pieces
	{  2,  2, 255, Q_BUTCHER,	100, 0, QFLAG_MULTI,	TXT_BUTCH1,		"The Butcher" },			// 6
	{  4, -1, 255, Q_LTBANNER,	100, 0, 0,				TXT_BOLTO1,		"Ogden's Sign" },			// 7
	{  7, -1, 255, Q_BLIND,		100, 0, 0,				TXT_WARBLIND,	"Halls of the Blind" },		// 8
	{  5, -1, 255, Q_BLOOD,		100, 0, 0,				TXT_WARBLOOD,	"Valor" },					// 9 
	{ 10, -1, 255, Q_ANVIL,		100, 0, 0,				TXT_ANVILBS1,	"Anvil of Fury" },			// 10 
	{ 13, -1, 255, Q_WARLORD,	100, 0, 0,				TXT_WARLORD,	"Warlord of Blood" },		// 11 
	// External Levels Maps	
	{  3,  3,   1, Q_SKELKING,	100, 1, QFLAG_MULTI,	TXT_KINGTO1,	"The Curse of King Leoric"},// 12
	{  2, -1,   3, Q_PWATER,	100, 4, 0,				TXT_PWH1,		"Poisoned Water Supply" },	// 13
	{  6, -1,   2, Q_SCHAMB,	100, 2, 0,				TXT_WARBONE,	"The Chamber of Bone" },	// 14
	{ 15, 15,   1, Q_BETRAYER,	100, 5, QFLAG_MULTI,	TXT_VBST1,		"Archbishop Lazarus" },		// 15 
	// New quests for Hellfire JKEQUEST
	{ 17, 17, 255, Q_CRYPTMAP,  100, 0, QFLAG_MULTI,    TXT_CRYPTMAP7,	"Grave Matters" },		// 16
	{  9,  9, 255, Q_FARMER,	100, 0, QFLAG_MULTI,	TXT_FARMER1,	"Farmer's Orchard" },		// 17
	{ 17, -1, 255, Q_THEO,		100, 0, 0,				TXT_THEO2,		"Little Girl" },			// 18
	{ 19, -1, 255, Q_TRADER,	100, 0, 0,				TXT_TRADER1,	"Wandering Trader" },		// 19
	{ 17, 17, 255, Q_DEFILER,	100, 0, QFLAG_MULTI,	TXT_DEFILER1,	"The Defiler" },			// 20
	{ 21, 21, 255, Q_NA_KRUL,	100, 0, QFLAG_MULTI,	TXT_NA_KRUL1,	"Na-Krul" },				// 21
	{ 21, -1, 255, Q_CORNERSTONE, 100, 0, 0,		  TXT_CORNERSTONE1,	"Cornerstone of the World"}, // 22
	{  9,  9, 255, Q_COWSUIT,		100, 0, QFLAG_MULTI,	TXT_COWSUIT4,		"The Jersey's Jersey" }		// 23
};
#define	ALLQUESTS (sizeof(questlist) / sizeof(questlist[0]))

#define QUEST_OFFSETS 7
static char questxoff[QUEST_OFFSETS] = { 0, -1, 0, -1, -2, -1, -2 };
static char questyoff[QUEST_OFFSETS] = { 0, 0, -1, -1, -1, -2, -2 };

char *questtrigstr[] = {
	"King Leoric's Tomb",
	"The Chamber of Bone",
	"Maze",
	"A Dark Passage",
	"Unholy Altar"
};

// Single player quest lists
int QuestGroup1[3] = { Q_BUTCHER, Q_LTBANNER, Q_GARBUD };	// 2 of 3
int QuestGroup2[3] = { Q_BLIND, Q_ROCK, Q_BLOOD };			// 2 of 3
int QuestGroup3[3] = { Q_BKMUSHRM, Q_ZHAR, Q_ANVIL };		// 2 of 3
int QuestGroup4[2] = { Q_VEIL, Q_WARLORD };					// 1 of 2

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void InitQuests() {
	int i, gri, deltaq;

	if (gbMaxPlayers == 1) {
		for (i = 0; i < MAXQUESTS; i++)
			quests[i]._qactive = QUEST_NOTAVAIL;
	} else {
		for (i = 0; i < MAXQUESTS; i++) {
			if (!(questlist[i]._qflags & QFLAG_MULTI))
				quests[i]._qactive = QUEST_NOTAVAIL;
		}
	}

	questlog = FALSE;
	qspin = 1;
	qfade = 0;
	deltaq = 0;
	for (i = 0; i < ALLQUESTS; i++) {
		if (gbMaxPlayers > 1 && !(questlist[i]._qflags & QFLAG_MULTI)) continue;

		quests[i]._qtype = questlist[i]._qdtype;
		if (gbMaxPlayers > 1) {
			quests[i]._qlevel = questlist[i]._qdmultlvl;
			if (! delta_quest_inited(deltaq)) {
				quests[i]._qactive = QUEST_NOTACTIVE;
				quests[i]._qvar1 = 0;
				quests[i]._qlog = FALSE;
			}
			deltaq++;
		} else {
			quests[i]._qactive = QUEST_NOTACTIVE;
			quests[i]._qlevel = questlist[i]._qdlvl;
			quests[i]._qvar1 = 0;
			quests[i]._qlog = FALSE;
		}
		quests[i]._qslvl = questlist[i]._qslvl;
		quests[i]._qtx = 0;
		quests[i]._qty = 0;
		quests[i]._qidx = i;
		quests[i]._qlvltype = questlist[i]._qlvlt;
		quests[i]._qvar2 = 0;
		quests[i]._qmsg = questlist[i]._qdmsg;
	}

	// Random select quests
	if (gbMaxPlayers == 1) {
		SetRndSeed(glSeedTbl[15]);
		if (random(0, 2)) quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;
		else quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL;
		gri = QuestGroup1[random(0, 3)];
		quests[gri]._qactive = QUEST_NOTAVAIL;
		gri = QuestGroup2[random(0, 3)];
		quests[gri]._qactive = QUEST_NOTAVAIL;
		gri = QuestGroup3[random(0, 3)];
		quests[gri]._qactive = QUEST_NOTAVAIL;
		gri = QuestGroup4[random(0, 2)];
		quests[gri]._qactive = QUEST_NOTAVAIL;
	}

#if CHEATS
	// New 12/01 drb
	if (questcheat != -1)
		quests[questcheat]._qactive = QUEST_NOTDONE;	///test area
#endif

	#if IS_VERSION(SHAREWARE)
	for (i = 0; i < ALLQUESTS; i++) {
		quests[i]._qactive = QUEST_NOTAVAIL;
	}
	#endif

// additional inits for the quests (not to be messed with)
	if (quests[Q_SKELKING]._qactive == QUEST_NOTAVAIL) quests[Q_SKELKING]._qvar2 = 2;
	if (quests[Q_ROCK]._qactive == QUEST_NOTAVAIL) quests[Q_ROCK]._qvar2 = 2;
	quests[Q_LTBANNER]._qvar1 = 1;  // so the door is open to snotspil for the lame gamers (LAME!!!)
	if (gbMaxPlayers != 1) quests[Q_BETRAYER]._qvar1 = 2;	// so that betrayer is on level 15
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CheckQuests()
{
// no quests in shareware version
#if !IS_VERSION(SHAREWARE)
	int i;
	int rportx, rporty;

	if ((QuestStatus(Q_BETRAYER)) && (gbMaxPlayers != 1) && (quests[Q_BETRAYER]._qvar1 == 2)) {
		AddObject(OBJ_ALTBOY, (setpc_x << 1) + DIRTEDGED2 + 4, (setpc_y << 1) + DIRTEDGED2 + 6);
		quests[Q_BETRAYER]._qvar1 = 3;
		NetSendCmdQuest(TRUE, Q_BETRAYER);
	}
	// If multiplayer return
	if (gbMaxPlayers != 1) return;

	if ((currlevel == quests[Q_BETRAYER]._qlevel) && (!(setlevel)) && (quests[Q_BETRAYER]._qvar1 >= 2)
		&& ((quests[Q_BETRAYER]._qactive == QUEST_NOTDONE) || (quests[Q_BETRAYER]._qactive == QUEST_DONE))) {
		if ((quests[Q_BETRAYER]._qvar2 == QS_VBRPOFF) || (quests[Q_BETRAYER]._qvar2 == QS_VBRP2)) {
			rportx = (quests[Q_BETRAYER]._qtx<<1)+DIRTEDGED2;
			rporty = (quests[Q_BETRAYER]._qty<<1)+DIRTEDGED2;
			quests[Q_BETRAYER]._qtx = rportx;
			quests[Q_BETRAYER]._qty = rporty;
			AddMissile(rportx, rporty, rportx, rporty, 0, MIT_RPORTAL, MI_ENEMYMONST, myplr, 0, 0);
			quests[Q_BETRAYER]._qvar2 = QS_VBRP1;
			if (quests[Q_BETRAYER]._qactive == QUEST_NOTDONE) 
				quests[Q_BETRAYER]._qvar1 = 3;
		}
	}
	if ((quests[Q_BETRAYER]._qactive == QUEST_DONE) && (setlevel && (setlvlnum == SL_VILEBETRAYER)) 
		&& (quests[Q_BETRAYER]._qvar2 == QS_VBRP4)) {
		AddMissile(35, 32, 35, 32, 0, MIT_RPORTAL, MI_ENEMYMONST, myplr, 0, 0);
		quests[Q_BETRAYER]._qvar2 = QS_VBRP3;
	}
	if (setlevel) {
		if ((setlvlnum == quests[Q_PWATER]._qslvl) && (quests[Q_PWATER]._qactive != QUEST_NOTACTIVE) && (leveltype == quests[Q_PWATER]._qlvltype)) {
			if (nummonsters == 4 && quests[Q_PWATER]._qactive != QUEST_DONE) {
				quests[Q_PWATER]._qactive = QUEST_DONE;
				PlaySfxLoc(IS_QUESTDN, plr[myplr]._px, plr[myplr]._py);
				LoadPalette("Levels\\L3Data\\L3pwater.pal");
				qfade = 32;
			}
		}

		if (qfade > 0) {
			MeshLavaPalette(qfade);
			qfade--;
		}

		return;
	}

	// only check quests in stand mode
	if (plr[myplr]._pmode != PM_STAND)
		return;

	for (i = 0; i < MAXQUESTS; i++) {
		if ((currlevel == quests[i]._qlevel) && (quests[i]._qslvl != 0) && (quests[i]._qactive != QUEST_NOTAVAIL)) {
			if ((plr[myplr]._px == quests[i]._qtx) && (plr[myplr]._py == quests[i]._qty)) {
				if (quests[i]._qlvltype != 255) setlvltype = quests[i]._qlvltype;
				StartNewLvl(myplr,WM_DIABSETLVL,quests[i]._qslvl);
			}
		}
	}
#endif
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BOOL ForceQuests() {
// no quests in shareware version
#if !IS_VERSION(SHAREWARE)
	// If multiplayer return
	if (gbMaxPlayers != 1) return(FALSE);

	for (int i = 0; i < MAXQUESTS; i++) {
		if (i == Q_BETRAYER) continue;
		if (currlevel != quests[i]._qlevel) continue;
//		if (quests[i]._qactive != QUEST_NOTDONE) continue;
		if (quests[i]._qslvl == 0) continue;

		int ql = quests[quests[i]._qidx]._qslvl - 1;
		int qx = quests[i]._qtx;
		int qy = quests[i]._qty;
		for (int j = 0; j < QUEST_OFFSETS; j++) {
			if (qx + questxoff[j] != cursmx) continue;
			if (qy + questyoff[j] != cursmy) continue;

			sprintf(infostr, "To %s", questtrigstr[ql]);
			cursmx = qx;
			cursmy = qy;
			return TRUE;
		}
	}
#endif

	return FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
BOOL QuestStatus(int i) {
	if (setlevel) return FALSE;
	app_assert((DWORD)i < MAXQUESTS);
	if (currlevel != quests[i]._qlevel) return FALSE;
	if (quests[i]._qactive == QUEST_NOTAVAIL) return FALSE;
	if ((gbMaxPlayers != 1) && !(questlist[i]._qflags & QFLAG_MULTI)) return FALSE;
	return TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void CheckQuestKill(int m, BOOL sendmsg)
{
// no quests in shareware version
#if !IS_VERSION(SHAREWARE)

	app_assert((DWORD)m < MAXMONSTERS);

	if (monster[m].MType->mtype == MT_SKING) {
		quests[Q_SKELKING]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR82;
		#if !IS_VERSION(SHAREWARE)
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE82;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE82;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK82;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD82;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN82;
		#endif
		if (sendmsg) NetSendCmdQuest(TRUE, Q_SKELKING);
	}
	else if (monster[m].MType->mtype == MT_CLEAVER) {
		quests[Q_BUTCHER]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR80;
		#if !IS_VERSION(SHAREWARE)
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE80;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE80;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK80;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD80;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN80;
		#endif
		if (sendmsg) NetSendCmdQuest(TRUE, Q_BUTCHER);
	}
	else if (monster[m].mName == UniqMonst[MU_GARBUD].mName) {
		quests[Q_GARBUD]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR61;
		#if !IS_VERSION(SHAREWARE)
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE61;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE61;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK61;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD61;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN61;
		#endif
	}
	else if (monster[m].mName == UniqMonst[MU_ZHAR].mName) {
		quests[Q_ZHAR]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR62;
		#if !IS_VERSION(SHAREWARE)
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE62;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE62;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK62;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD62;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN62;
		#endif
	}
	else if ((monster[m].mName == UniqMonst[MU_LAZARUS].mName ) && (gbMaxPlayers != 1)) {
		int i,j;
		quests[Q_BETRAYER]._qactive = QUEST_DONE;
		quests[Q_BETRAYER]._qvar1 = 7;
		sfxdelay = 30;
		quests[Q_DIABLO]._qactive = QUEST_NOTDONE;
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++) {
				if (dPiece[i][j] == 370) { 
					if (quests[Q_BETRAYER]._qactive == QUEST_DONE) {
						trigs[numtrigs]._tx = i;
						trigs[numtrigs]._ty = j;
						trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
						numtrigs++;
					}
				}
			}
		}
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR83;
		#if !IS_VERSION(SHAREWARE)
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE83;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE83;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK83;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD83;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN83;
		#endif
		if (sendmsg) {
			NetSendCmdQuest(TRUE, Q_BETRAYER);
			NetSendCmdQuest(TRUE, Q_DIABLO);
		}	
	}
	else if ((monster[m].mName == UniqMonst[MU_LAZARUS].mName) && (gbMaxPlayers == 1)) {
		quests[Q_BETRAYER]._qactive = QUEST_DONE;
		sfxdelay = 30;
		InitVPTriggers();
		quests[Q_BETRAYER]._qvar1 = 7;
		quests[Q_BETRAYER]._qvar2 = QS_VBRP4;
		quests[Q_DIABLO]._qactive = QUEST_NOTDONE;
		AddMissile(35, 32, 35, 32, 0, MIT_RPORTAL, MI_ENEMYMONST, myplr, 0, 0);
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR83;
		#if !IS_VERSION(SHAREWARE)
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE83;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE83;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK83;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD83;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN83;
		#endif
	}
	else if (monster[m].mName == UniqMonst[MU_WARLORD].mName) {
		app_assert(gbMaxPlayers == 1);
		quests[Q_WARLORD]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == CLASS_WARRIOR) sfxdnum = PS_WARR94;
		#if !IS_VERSION(SHAREWARE)
		else if (plr[myplr]._pClass == CLASS_ROGUE) sfxdnum = PS_ROGUE94;
		else if (plr[myplr]._pClass == CLASS_SORCEROR) sfxdnum = PS_MAGE94;
		else if (plr[myplr]._pClass == CLASS_MONK) sfxdnum = PS_MONK94;
		else if (plr[myplr]._pClass == CLASS_BARD) sfxdnum = PS_BARD94;
		else if (plr[myplr]._pClass == CLASS_BARBARIAN) sfxdnum = PS_BARBARIAN94;
		#endif
	}
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SetPostDungeon()
{
	int i,j;

	for (j = 0; j < MDMAXY; j++) {
		for (i = 0; i < MDMAXX; i++) pdungeon[i][j] = dungeon[i][j];
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawButcher()
{
	int x, y;

	x = (setpc_x << 1) + DIRTEDGED2;
	y = (setpc_y << 1) + DIRTEDGED2;
	DRLG_RectTrans(x+3, y+3, x+10, y+10);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawSkelKing(int q, int x, int y)
{
	app_assert((DWORD)q < MAXQUESTS);
	quests[q]._qtx = DIRTEDGED2 + (x << 1) + 12;
	quests[q]._qty = DIRTEDGED2 + (y << 1) + 7;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawWarLord(int x, int y)
{
	int		rw,rh;
	int		i,j;
	byte	*sp, *setp;

	setp = LoadFileInMemSig("Levels\\L4Data\\Warlord2.DUN",NULL,'QSTt');
	sp = setp;
 	rw = *sp;
	sp+=2;
	rh = *sp;
	sp+=2;

	setpc_w = rw;
	setpc_h = rh;
	setpc_x = x;
	setpc_y = y;

	app_assert((DWORD)(rw + x - 1) < MDMAXX);
	app_assert((DWORD)(rh + y - 1) < MDMAXY);
	for (j = y; j < (rh + y); j++) {
		for (i = x; i < (rw + x); i++) {
			dungeon[i][j] = *sp ? *sp : 6;
			sp+=2;
		}
	}
	DiabloFreePtr(setp);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawSChamber(int q, int x, int y)
{
	int		i, j, rw, rh, xx, yy;
	byte	*sp, *setp;

	setp = LoadFileInMemSig("Levels\\L2Data\\Bonestr1.DUN",NULL,'QSTt');
	sp = setp;
	rw = *sp;
	sp+=2;
	rh = *sp;
	sp+=2;

	setpc_w = rw;
	setpc_h = rh;
	setpc_x = x;
	setpc_y = y;

	app_assert((DWORD)(rw + x - 1) < MDMAXX);
	app_assert((DWORD)(rh + y - 1) < MDMAXY);
	for (j = y; j < (rh + y); j++) {
		for (i = x; i < (rw + x); i++) {
			dungeon[i][j] = *sp ? *sp : 3;
			sp+=2;
		}
	}

	xx = DIRTEDGED2 + (x << 1) + 6;
	yy = DIRTEDGED2 + (y << 1) + 7;
	app_assert((DWORD)q < MAXQUESTS);
	quests[q]._qtx = xx;
	quests[q]._qty = yy;
	DiabloFreePtr(setp);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawLTBanner(int x, int y)
{
	int		rw,rh;
	int		i,j;
	byte	*sp, *setp;

	setp = LoadFileInMemSig("Levels\\L1Data\\Banner1.DUN",NULL,'QSTt');
	sp = setp;
 	rw = *sp;
	sp+=2;
	rh = *sp;
	sp+=2;

	setpc_w = rw;
	setpc_h = rh;
	setpc_x = x;
	setpc_y = y;

	app_assert((DWORD)(x+rw-1) < MDMAXX);
	app_assert((DWORD)(y+rh-1) < MDMAXY);
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp) pdungeon[x+i][y+j] = *sp;
			sp+=2;
		}
	}
	DiabloFreePtr(setp);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawBlind(int x, int y)
{
	int		rw,rh;
	int		i,j;
	byte	*sp, *setp;

	setp = LoadFileInMemSig("Levels\\L2Data\\Blind1.DUN",NULL,'QSTt');	
	sp = setp;
 	rw = *sp;
	sp+=2;
	rh = *sp;
	sp+=2;

	setpc_x = x;
	setpc_y = y;
	setpc_w = rw;
	setpc_h = rh;

	app_assert((DWORD)(x+rw-1) < MDMAXX);
	app_assert((DWORD)(y+rh-1) < MDMAXY);
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp) pdungeon[x+i][y+j] = *sp;
			sp+=2;
		}
	}
	DiabloFreePtr(setp);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawBlood(int x, int y)
{
	int		rw,rh;
	int		i,j;
	byte	*sp, *setp;

	setp = LoadFileInMemSig("Levels\\L2Data\\Blood2.DUN",NULL,'QSTt');	
	sp = setp;
 	rw = *sp;
	sp+=2;
	rh = *sp;
	sp+=2;

	setpc_x = x;
	setpc_y = y;
	setpc_w = rw;
	setpc_h = rh;

	app_assert((DWORD)(rw + x - 1) < MDMAXX);
	app_assert((DWORD)(rh + y - 1) < MDMAXY);
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp) dungeon[x+i][y+j] = *sp;
			//else dungeon[x+i][x+j] = 3;
			sp+=2;			
		}
	}

	DiabloFreePtr(setp);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DRLG_CheckQuests(int x, int y)
{
	int i;

	for (i = 0; i < MAXQUESTS; i++) {
		if (!QuestStatus(i))
			continue;
		switch (quests[i]._qtype) {
			case Q_BUTCHER:
				DrawButcher();
				break;
			case Q_SKELKING :
				DrawSkelKing(i, x, y);
				break;
			case Q_SCHAMB :
				DrawSChamber(i, x, y);
				break;
			case Q_BLIND :
				DrawBlind(x, y);
				break;
			case Q_BLOOD :
				DrawBlood(x,y);
				break;
			case Q_LTBANNER :
				DrawLTBanner(x, y);
				break;
			case Q_WARLORD:
				DrawWarLord(x, y);
				break;
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetReturnLvlPos()
{
	switch (setlvlnum) {
		case SL_SKELKING:		// SKing
			ReturnLvlX = quests[Q_SKELKING]._qtx + 1;
			ReturnLvlY = quests[Q_SKELKING]._qty;
			ReturnLvl = quests[Q_SKELKING]._qlevel;
			ReturnLvlT = 1;
			break;
		case SL_BONECHAMB:		// Bone chamber
			ReturnLvlX = quests[Q_SCHAMB]._qtx + 1;
			ReturnLvlY = quests[Q_SCHAMB]._qty;
			ReturnLvl = quests[Q_SCHAMB]._qlevel;
			ReturnLvlT = 2;
			break;
		case SL_POISONWATER:	// Poison Water
			ReturnLvlX = quests[Q_PWATER]._qtx;
			ReturnLvlY = quests[Q_PWATER]._qty + 1;
			ReturnLvl = quests[Q_PWATER]._qlevel;
			ReturnLvlT = 1;
			break;
		case SL_VILEBETRAYER:	// Vile Betrayer
			ReturnLvlX = quests[Q_BETRAYER]._qtx + 1;
			ReturnLvlY = quests[Q_BETRAYER]._qty - 1;
			ReturnLvl = quests[Q_BETRAYER]._qlevel;
			ReturnLvlT = 4;
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void GetReturnLvlPos()
{
	if (quests[Q_BETRAYER]._qactive == QUEST_DONE) quests[Q_BETRAYER]._qvar2 = QS_VBRP2;
	ViewX = ReturnLvlX;
	ViewY = ReturnLvlY;
	currlevel = ReturnLvl;
	leveltype = ReturnLvlT;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ResyncMPQuests()
{
// no quests in shareware version
#if !IS_VERSION(SHAREWARE)
	if (quests[Q_SKELKING]._qactive == QUEST_NOTACTIVE) {
		if ((currlevel >= (quests[Q_SKELKING]._qlevel - 1)) && (currlevel <= (quests[Q_SKELKING]._qlevel + 1))) {
			quests[Q_SKELKING]._qactive = QUEST_NOTDONE;
			NetSendCmdQuest(TRUE, Q_SKELKING);
		}
	}
	if (quests[Q_BUTCHER]._qactive == QUEST_NOTACTIVE) {
		if ((currlevel >= (quests[Q_BUTCHER]._qlevel - 1)) && (currlevel <= (quests[Q_BUTCHER]._qlevel + 1))) {
			quests[Q_BUTCHER]._qactive = QUEST_NOTDONE;
			NetSendCmdQuest(TRUE, Q_BUTCHER);
		}
	}
	if (quests[Q_BETRAYER]._qactive == QUEST_NOTACTIVE) {
		if (currlevel == (quests[Q_BETRAYER]._qlevel - 1)) {
			quests[Q_BETRAYER]._qactive = QUEST_NOTDONE;
			NetSendCmdQuest(TRUE, Q_BETRAYER);
		}
	}
	if (QuestStatus(Q_BETRAYER))
		AddObject(OBJ_ALTBOY, (setpc_x << 1) + DIRTEDGED2 + 4, (setpc_y << 1) + DIRTEDGED2 + 6);

	//	Add Multi player Hellfire quests here
	if (quests[Q_CRYPTMAP]._qactive == QUEST_NOTACTIVE) 
		if (currlevel == (quests[Q_CRYPTMAP]._qlevel - 1)) {
			quests[Q_CRYPTMAP]._qactive = QUEST_NOTDONE;
			NetSendCmdQuest(TRUE, Q_CRYPTMAP);
		}

	if (quests[Q_DEFILER]._qactive == QUEST_NOTACTIVE) 
		if (currlevel == (quests[Q_DEFILER]._qlevel - 1)) {
			quests[Q_DEFILER]._qactive = QUEST_NOTDONE;
			NetSendCmdQuest(TRUE, Q_DEFILER);
		}

	if (quests[Q_NA_KRUL]._qactive == QUEST_NOTACTIVE) 
		if (currlevel == (quests[Q_NA_KRUL]._qlevel - 1)) {
			quests[Q_NA_KRUL]._qactive = QUEST_NOTDONE;
			NetSendCmdQuest(TRUE, Q_NA_KRUL);
		}

	if (quests[Q_COWSUIT]._qactive == QUEST_NOTACTIVE) 
		if (currlevel == (quests[Q_COWSUIT]._qlevel - 1)) {
			quests[Q_COWSUIT]._qactive = QUEST_NOTDONE;
			NetSendCmdQuest(TRUE, Q_COWSUIT);
		}

#endif		

}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void ResyncQuests()
{
// no quests in shareware version
#if !IS_VERSION(SHAREWARE)

	int i, tren;

	// for Poison Water quest only - inits the poison/not poison water pal
	if (setlevel) {
		if ((setlvlnum == quests[Q_PWATER]._qslvl) && (quests[Q_PWATER]._qactive != QUEST_NOTACTIVE) && (leveltype == quests[Q_PWATER]._qlvltype)) {
			if (quests[Q_PWATER]._qactive == QUEST_DONE) LoadPalette("Levels\\L3Data\\L3pwater.pal");
			else LoadPalette("Levels\\L3Data\\L3pfoul.pal");
			for (i = 0; i <= 32; i++) {
				MeshLavaPalette(i);
			}
		}
	}

	if (QuestStatus(Q_LTBANNER)) {
		if (quests[Q_LTBANNER]._qvar1 == 1) {
			ObjChangeMapResync(setpc_x + setpc_w - 2, setpc_y + setpc_h - 2, setpc_x + setpc_w+1, setpc_y + setpc_h+1);
		}
		if (quests[Q_LTBANNER]._qvar1 == 2) {
			ObjChangeMapResync(setpc_x + setpc_w - 2, setpc_y + setpc_h - 2, setpc_x + setpc_w+1, setpc_y + setpc_h+1);
			ObjChangeMapResync(setpc_x, setpc_y, setpc_x+(setpc_w>>1)+2, setpc_y+(setpc_h>>1)-2);
			for (i = 0; i < numobjects; i++) SyncObjectAnim(objectactive[i]);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, setpc_x+(setpc_w>>1)+4, setpc_y+(setpc_h>>1));
			TransVal = tren;
		}
		if (quests[Q_LTBANNER]._qvar1 == 3) {
			ObjChangeMapResync(setpc_x, setpc_y, setpc_x + setpc_w + 1, setpc_y + setpc_h + 1);
			for (i = 0; i < numobjects; i++) SyncObjectAnim(objectactive[i]);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, setpc_x+(setpc_w>>1)+4, setpc_y+(setpc_h>>1));
			TransVal = tren;
		}
	}

	if (currlevel == quests[Q_BKMUSHRM]._qlevel) {
		if (quests[Q_BKMUSHRM]._qactive == QUEST_NOTACTIVE
			&& quests[Q_BKMUSHRM]._qvar1 == QS_INIT)
		{
			SpawnQuestItem(IDI_FUNGALTM, 0,0,5, ISEL_FLR);
			quests[Q_BKMUSHRM]._qvar1 = QS_TOMESPAWNED;
		}
		else if (quests[Q_BKMUSHRM]._qactive == QUEST_NOTDONE)
		{
			if (quests[Q_BKMUSHRM]._qvar1 >= QS_MUSHGIVEN)	// Mushroom was given to Witch
			{
				Qtalklist[TWN_HEALER][Q_BKMUSHRM] = TXT_BLKMH1;
				Qtalklist[TWN_WITCH][Q_BKMUSHRM] = -1;
			}
			else if (quests[Q_BKMUSHRM]._qvar1 >= QS_BRAINGIVEN)	// Brain was given to Healer
			{
				Qtalklist[TWN_HEALER][Q_BKMUSHRM] = -1;
			}
		}
	}
	if (currlevel == (quests[Q_VEIL]._qlevel + 1)) {
		if ((quests[Q_VEIL]._qactive == QUEST_NOTDONE) && (quests[Q_VEIL]._qvar1 == 0)) {
			quests[Q_VEIL]._qvar1 = 1;
			SpawnQuestItem(IDI_GLDNELIX, 0,0,5, ISEL_FLR);
		}
	}

	if (setlevel && (setlvlnum == SL_VILEBETRAYER)) {
		if (quests[Q_BETRAYER]._qvar1 >= 4) 
			ObjChangeMapResync(1, 11, 20, 18);
		if (quests[Q_BETRAYER]._qvar1 >= 6) 
			ObjChangeMapResync(1, 18, 20, 24);
		if (quests[Q_BETRAYER]._qvar1 >= 7) 
			InitVPTriggers();
		for (i = 0; i < numobjects; i++) SyncObjectAnim(objectactive[i]);
	}
		
// rmw.patch1.start.1/23/97
// 	if ((currlevel == quests[Q_BETRAYER]._qlevel) && (quests[Q_BETRAYER]._qvar2 == QS_VBRP1)
	if ((currlevel == quests[Q_BETRAYER]._qlevel) && !setlevel
		&& ((quests[Q_BETRAYER]._qvar2 == QS_VBRP1) || (quests[Q_BETRAYER]._qvar2 >= QS_VBRP3))
// rmw.patch1.end.1/23/97
		&& ((quests[Q_BETRAYER]._qactive == QUEST_NOTDONE) || (quests[Q_BETRAYER]._qactive == QUEST_DONE))) {
		quests[Q_BETRAYER]._qvar2 = QS_VBRP2;		
	}

#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*

void DrawQLBack()
{
	DrawCel(408, 487, pSTextBoxCels, 1, 271);

	__asm {
		mov		edi,dword ptr [pBuffer]
		add		edi,372123

		xor		eax,eax
		mov		edx,148
_YLp:	mov		ecx,132
_XLp1:	stosb
		inc		edi
		loop	_XLp1
		stosb
		sub		edi,1033
		mov		ecx,132
_XLp2:	inc		edi
		stosb
		loop	_XLp2
		sub		edi,1032
		dec		edx
		jnz		_YLp
		mov		ecx,132
_XLp3:	stosb
		inc		edi
		loop	_XLp3
		stosb
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void PrintQLString(int x, int y, BOOL cjustflag, char str[], char col)
{
	long	boffset;
	int		sl,i,w,tw,xx,yy;

	yy = SStringY[y];
	boffset = nBuffWTbl[yy + 204] + x + 96;
	sl = strlen(str);
	w = 0;
	if (cjustflag) {
		tw = 0;
		for (i = 0; i < sl; i++) {
			BYTE c = char2print(str[i]);
			c = fonttrans[c];
			tw += fontkern[c]+1;
		}
		if (tw < 257) w = (257 - tw) >> 1;
		boffset += w;
	}
	if (qline == y) {
		if (cjustflag) xx = x + w + 76;
		else xx = x + 76;
		DrawCel(xx, yy + 205, pSTextSpinCels, qspin, 12);
	}
	for (i = 0; i < sl; i++) {
		BYTE c = char2print(str[i]);
		c = fonttrans[c];
		w += fontkern[c]+1;
		if ((c != 0) && (w <= 257)) DrawPanelFont(boffset, c, col);
		boffset += fontkern[c]+1;
	}
	if (qline == y) {
		if (cjustflag) xx = x + w + 100;
		else xx = 340 - x;
		DrawCel(xx, yy + 205, pSTextSpinCels, qspin, 12);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*

void DrawQLLine(int y)
{
	long	doffset;

	doffset = nBuffWTbl[SStringY[y] + 198] + 410;
	__asm {
		mov		esi,dword ptr [pBuffer]
		mov		edi,esi
		add		esi,142170
		add		edi,dword ptr [doffset]

		mov		ebx,502

		mov		edx,3
_YLp:	mov		ecx,66
		rep movsd
		movsw
		add		esi,ebx
		add		edi,ebx
		dec		edx
		jnz		_YLp
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void DrawQuestLog()
{
	int i, l, q;

	//DrawQLBack();
	PrintQLString(0, 2, TRUE, "Quest Log", ICOLOR_GOLD);
	//DrawQLLine(5);

	DrawCel(64, 511, pQLogCel, 1, 320);

	l = qtopline;
	for (i = 0; i < numqlines; i++) {
		q = qlist[i];
		PrintQLString(0, l , TRUE, questlist[q]._qlstr, ICOLOR_WHITE);
		l+=2;
	}
	PrintQLString(0, 22, TRUE, "Close Quest Log", ICOLOR_WHITE);
	qspin = (qspin & 0x7) + 1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void StartQuestlog()
{
	int i;

	numqlines = 0;
	for (i = 0; i < ALLQUESTS; i++) {
		if ((quests[i]._qactive == QUEST_NOTDONE) && (quests[i]._qlog)) {
			qlist[numqlines] = i;
			numqlines++;
		}
	}
	if (numqlines > 5) qtopline = 5 - (numqlines >> 1);
	else qtopline = 8;
	if (numqlines == 0) qline = 22;
	else qline = qtopline;
	questlog = TRUE;
	qspin = 1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void QuestlogUp()
{
	if (numqlines != 0) {
		if (qline == qtopline) qline = 22;
		else {
			if (qline == 22) qline = ((numqlines-1) << 1) + qtopline;
			else qline -= 2;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void QuestlogDown()
{
	if (numqlines != 0) {
		if (qline == 22) qline = qtopline;
		else {
			if (qline == (((numqlines-1) << 1) + qtopline)) qline = 22;
			else qline += 2;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void QuestlogEnter()
{
	int q;

	PlaySFX(IS_TITLSLCT);
	if ((numqlines != 0) && (qline != 22)) {
		q = qlist[(qline - qtopline) >> 1];
		InitQTextMsg(quests[q]._qmsg);
	}
	questlog = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CheckQLogBtn()
{
	int i, ql;

	ql = (MouseY - 32) / 12;
	if (numqlines != 0) {
		for (i = 0; i < numqlines; i++) {
			if (ql == ((i << 1) + qtopline)) {
				qline = ql;
				QuestlogEnter();
			}
		}
	} 
	if (ql == 22) {
		qline = 22;
		QuestlogEnter();
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void QuestlogESC()
{
	questlog = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SetMultiQuest(int q, int s, BOOL l, int v1)
{
#if !IS_VERSION(SHAREWARE)
	app_assert((DWORD)q < MAXQUESTS);
	app_assert(quests[q]._qactive != QUEST_NOTAVAIL);

	// Don't change if already done
	if (quests[q]._qactive == QUEST_DONE)
		return;
	
	// always next state
	if (s > quests[q]._qactive)
		quests[q]._qactive = s;
	
	// never change from TRUE to FALSE
	quests[q]._qlog |= l;
	
	// always next state
	if (v1 > quests[q]._qvar1)
		quests[q]._qvar1 = v1;
#endif
}
