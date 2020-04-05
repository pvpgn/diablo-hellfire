/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Game Menu file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/LOADSAVE.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	<stddef.h>
#include	"sound.h"
#include	"engine.h"
#include	"gendung.h"
#include	"palette.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"dead.h"
#include	"objects.h"
#include	"spells.h"
#include	"missiles.h"
#include	"quests.h"
#include	"trigs.h"
#include	"lighting.h"
#include	"control.h"
#include	"inv.h"
#include	"interfac.h"
#include	"town.h"
#include	"stores.h"
#include	"cursor.h"
#include	"automap.h"
#include	"multi.h"
#include	"doom.h"
#include	"portal.h"

/*-----------------------------------------------------------------------*
** extern
**-----------------------------------------------------------------------*/
DWORD CalcEncodeDstBytes(DWORD dwSrcBytes);
void CreateSaveLevelName(char szName[MAX_PATH]);
void CreateLoadLevelName(char szName[MAX_PATH]);
void CreateSaveGameName(char szName[MAX_PATH]);
void WriteSaveFile(const char * pszName,BYTE * pbData,DWORD dwLen,DWORD dwEncodeLen);
BYTE * ReadSaveFile(const char * pszName,DWORD * pdwLen);
void SyncPlrAnim(int);
void DestroyTempSaves();
void MoveTempSavesToPermanent();
void RedoPlayerVision();


/*-----------------------------------------------------------------------*
** private
**-----------------------------------------------------------------------*/
// Save only these bits in the dFlags
#define BFLAG_SAVEMASK	(BFLAG_AUTOMAP | BFLAG_VISIBLE | BFLAG_PLRLR | BFLAG_MONSTLR | BFLAG_SETPC)

// save file buffer size
#define FILEBUFF	362147

// pointer into save file buffer
static BYTE *tbuff;

#define SHAREWARE_ID	'SHAR'
#define BETA_ID			'BETA'
#define RETAIL_ID		'RETL'
#define HELLFIRE_ID		'HELF'

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static char BLoad() {
	return *tbuff++;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int ILoad() {
	int rv;
	// drb old because we used to think ints were 16 bit
/*	if ((*tbuff & 0x80) != 0) rv = -1 & 0xffff0000;
	else rv = 0;
	rv |= *tbuff++ << 8;
	rv |= *tbuff++;*/
	rv = *tbuff++ << 24;
	rv |= *tbuff++ << 16;
	rv |= *tbuff++ << 8;
	rv |= *tbuff++;
	return(rv);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static long LLoad() {
	long rv;

	rv = *tbuff++ << 24;
	rv |= *tbuff++ << 16;
	rv |= *tbuff++ << 8;
	rv |= *tbuff++;
	return(rv);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static BOOL OLoad() {
	if (*tbuff++ == 1) return(TRUE);
	else return(FALSE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadPlr(int i) {
	memcpy(&plr[i], tbuff, SAVE_PLAYER_SIZE);
	tbuff += SAVE_PLAYER_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadMonst(int i) {
	memcpy(&monster[i], tbuff, SAVE_MONSTER_SIZE);
	tbuff += SAVE_MONSTER_SIZE;
	SyncMonsterAnim(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadMissile(int i) {
	memcpy(&missile[i], tbuff, SAVE_MISSILE_SIZE);
	tbuff += SAVE_MISSILE_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadSpell(int i) {
	tbuff += 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadObject(int i) {
	memcpy(&object[i], tbuff, SAVE_OBJECT_SIZE);
	tbuff += SAVE_OBJECT_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadItem(int i) {
	memcpy(&item[i], tbuff, SAVE_ITEM_SIZE);
	tbuff += SAVE_ITEM_SIZE;
	SyncItemAnim(i);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadPremium(int i) {
	memcpy(&premiumitem[i], tbuff, SAVE_ITEM_SIZE);
	tbuff += SAVE_ITEM_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadQuest(int i) {
	memcpy(&quests[i], tbuff, SAVE_QUEST_SIZE);
	tbuff += SAVE_QUEST_SIZE;

	// where to go back to
	ReturnLvlX = ILoad();
	ReturnLvlY = ILoad();
	ReturnLvl = ILoad();
	ReturnLvlT = ILoad();

	// Map of doom
	doomtime = ILoad();
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadLight(int i) {
	memcpy(&LightList[i], tbuff, sizeof(LightListStruct));
	tbuff += sizeof(LightListStruct);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadVision(int i) {
	memcpy(&VisionList[i], tbuff, sizeof(LightListStruct));
	tbuff += sizeof(LightListStruct);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void LoadPortal(int i) {
	memcpy(&portal[i], tbuff, sizeof(PortalStruct));
	tbuff += sizeof(PortalStruct);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void GM_LoadGame(BOOL firstflag) {
#if !IS_VERSION(BETA)
	int	i,j;
	int hvx, hvy;
	int hnummonsters, hnumitems, hnummissiles, hnumobjects;

	app_assert(gbMaxPlayers == 1);
	FreeGameMem();

	// since we are loading an existing game, any temporary
	// save files which were created can be removed
	DestroyTempSaves();

	DWORD dwLen;
	char szName[MAX_PATH];
	CreateSaveGameName(szName);
	BYTE *LoadBuff = ReadSaveFile(szName,&dwLen);
	tbuff = LoadBuff;

	DWORD dwVersion = LLoad();
	#if IS_VERSION(SHAREWARE)
		if (dwVersion != SHAREWARE_ID) app_fatal("Invalid save file");
	#elif IS_VERSION(BETA)
		if (dwVersion != BETA_ID) app_fatal("Invalid save file");
	#elif IS_VERSION(RETAIL)
		//if (dwVersion != RETAIL_ID) app_fatal("Invalid save file");
		if (dwVersion != HELLFIRE_ID) app_fatal("Invalid save file");
	#else
		#error No version defined
	#endif

	setlevel = OLoad();
	setlvlnum = ILoad();
	currlevel = ILoad();
	leveltype = ILoad();
	hvx = ILoad();
	hvy = ILoad();
	invflag = OLoad();
	chrflag = OLoad();
	hnummonsters = ILoad();
	hnumitems = ILoad();
	hnummissiles = ILoad();
	hnumobjects = ILoad();

	for (i = 0; i < NUMLEVELS; i++) {
		glSeedTbl[i] = LLoad();
		gnLevelTypeTbl[i] = ILoad();
	}

	// Load player info
	LoadPlr(myplr);
	gnDifficulty = plr[myplr]._gnDifficulty;
	if (gnDifficulty < D_NORMAL || gnDifficulty > D_HELL)
		gnDifficulty = D_NORMAL;

	// Load quest info
	for (i = 0; i < MAXQUESTS; i++) LoadQuest(i);

	// Load town portal info
	for (i = 0; i < MAXPORTAL; i++) LoadPortal(i);

	LoadGameLevel(firstflag, LVL_NODIR);

	SyncInitPlr(myplr);
	SyncPlrAnim(myplr);

	ViewX = hvx;
	ViewY = hvy;
	nummonsters = hnummonsters;
	numitems = hnumitems;
	nummissiles = hnummissiles;
	numobjects = hnumobjects;

	for (i = 0; i < MONSTERTYPES; i++) monstkills[i] = LLoad();

	if (leveltype != 0) {
		// Load monster block
		for (i = 0; i < MAXMONSTERS; i++) monstactive[i] = ILoad();
		for (i = 0; i < nummonsters; i++) LoadMonst(monstactive[i]);
		// Load missile block
		for (i = 0; i < MAXMISSILES; i++) missileactive[i] = BLoad();
		for (i = 0; i < MAXMISSILES; i++) missileavail[i] = BLoad();
		for (i = 0; i < nummissiles; i++) LoadMissile(missileactive[i]);
		// Load object block
		for (i = 0; i < MAXOBJECTS; i++) objectactive[i] = BLoad();
		for (i = 0; i < MAXOBJECTS; i++) objectavail[i] = BLoad();
		for (i = 0; i < numobjects; i++) LoadObject(objectactive[i]);
		for (i = 0; i < numobjects; i++) SyncObjectAnim(objectactive[i]);
		// Load light and vision
		numlights = ILoad();
		for (i = 0; i < MAXLIGHTS; i++) lightactive[i] = BLoad();
		for (i = 0; i < numlights; i++) LoadLight(lightactive[i]);
		visionid = ILoad();
		numvision = ILoad();
		for (i = 0; i < numvision; i++) LoadVision(i);
	}
	// Load item block
	for (i = 0; i < MAXITEMS; i++) itemactive[i] = BLoad();
	for (i = 0; i < MAXITEMS; i++) itemavail[i] = BLoad();
	for (i = 0; i < numitems; i++) LoadItem(itemactive[i]);
	for (i = 0; i < MAXUITEMS; i++) UniqueItemFlag[i] = OLoad();

	// Load map info
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) dLight[i][j] = BLoad();
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) dFlags[i][j] = BLoad();
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) dPlayer[i][j] = BLoad();
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) dItem[i][j] = BLoad();

	if (leveltype != 0) {
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dMonster[i][j] = ILoad();
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dDead[i][j] = BLoad();
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dObject[i][j] = BLoad();
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dLight[i][j] = BLoad();
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dSaveLight[i][j] = BLoad();
		for (j = 0; j < AUTOMAPY; j++)
			for (i = 0; i < AUTOMAPX; i++) automapview[i][j] = OLoad();
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dMissile[i][j] = BLoad();
	}

	numpremium = ILoad();
	premiumlevel = ILoad();
	for (i = 0; i < MAXPREMIUM; i++) LoadPremium(i);

	automapflag = OLoad();
	automapscale = ILoad();

	DiabloFreePtr(LoadBuff);

	// Misc sync routines
	SyncAutomap();
	ResyncQuests();
	if (leveltype)
		ProcessLightList();
	RedoPlayerVision();
	ProcessVisionList();
	SyncMissAnim();

	ResetPal();
	SetCursor(GLOVE_CURS);
	gbProcessPlayers = TRUE;
#endif
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void BSave(char v) {
	*tbuff++ = v;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void ISave(int v) {
	// Changed because we used to thing ints were 16 bit, but they are 32
/*	*tbuff++ = (char) (v >> 8);
	*tbuff++ = (char) v;*/
	*tbuff++ = (char) (v >> 24);
	*tbuff++ = (char) (v >> 16);
	*tbuff++ = (char) (v >> 8);
	*tbuff++ = (char) (v >> 0);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void LSave(long v) {
	*tbuff++ = (char) (v >> 24);
	*tbuff++ = (char) (v >> 16);
	*tbuff++ = (char) (v >> 8);
	*tbuff++ = (char) (v >> 0);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void OSave(BOOL v) {
	if (v) *tbuff++ = 1;
	else *tbuff++ = 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SavePlr(int i) {
	memcpy(tbuff, &plr[i], SAVE_PLAYER_SIZE);
	tbuff += SAVE_PLAYER_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SaveMonst(int i) {
	memcpy(tbuff, &monster[i], SAVE_MONSTER_SIZE);
	tbuff += SAVE_MONSTER_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SaveMissile(int i) {
	memcpy(tbuff, &missile[i], SAVE_MISSILE_SIZE);
	tbuff += SAVE_MISSILE_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SaveSpell(int i) {
	tbuff += 0;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SaveObject(int i) {
	memcpy(tbuff, &object[i], SAVE_OBJECT_SIZE);
	tbuff += SAVE_OBJECT_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SaveItem(int i) {
	memcpy(tbuff, &item[i], SAVE_ITEM_SIZE);
	tbuff += SAVE_ITEM_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SavePremium(int i) {
	memcpy(tbuff, &premiumitem[i], SAVE_ITEM_SIZE);
	tbuff += SAVE_ITEM_SIZE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SaveQuest(int i) {
	memcpy(tbuff, &quests[i], SAVE_QUEST_SIZE);
	tbuff += SAVE_QUEST_SIZE;
	ISave(ReturnLvlX);
	ISave(ReturnLvlY);
	ISave(ReturnLvl);
	ISave(ReturnLvlT);
	ISave(doomtime);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SaveLight(int i) {
	memcpy(tbuff, &LightList[i], sizeof(LightListStruct));
	tbuff += sizeof(LightListStruct);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SaveVision(int i) {
	memcpy(tbuff, &VisionList[i], sizeof(LightListStruct));
	tbuff += sizeof(LightListStruct);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static void SavePortal(int i) {
	memcpy(tbuff, &portal[i], sizeof(PortalStruct));
	tbuff += sizeof(PortalStruct);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void GM_SaveGame() {
#if !IS_VERSION(BETA)
	BYTE *SaveBuff;
	int	i,j;

	// allocate a ptr large enough for save file + encode data
	app_assert(gbMaxPlayers == 1);
	SaveBuff = DiabloAllocPtrSig(CalcEncodeDstBytes(FILEBUFF),'SAVt');
	tbuff = SaveBuff;

	#if IS_VERSION(SHAREWARE)
		LSave(SHAREWARE_ID);
	#elif IS_VERSION(BETA)
		LSave(BETA_ID);
	#elif IS_VERSION(RETAIL)
		//LSave(RETAIL_ID);
		LSave(HELLFIRE_ID);
	#else
		#error No version defined
	#endif

	OSave(setlevel);
	ISave(setlvlnum);
	ISave(currlevel);
	ISave(leveltype);
	ISave(ViewX);
	ISave(ViewY);
	OSave(invflag);
	OSave(chrflag);
	ISave(nummonsters);
	ISave(numitems);
	ISave(nummissiles);
	ISave(numobjects);

	for (i = 0; i < NUMLEVELS; i++) {
		LSave(glSeedTbl[i]);
		ISave(gnLevelTypeTbl[i]);
	}

	// Save player info
	plr[myplr]._gnDifficulty = gnDifficulty;
	SavePlr(myplr);

	// Save quest info
	for (i = 0; i < MAXQUESTS; i++) SaveQuest(i);

	// Save town portal info
	for (i = 0; i < MAXPORTAL; i++) SavePortal(i);

	for (i = 0; i < MONSTERTYPES; i++) LSave(monstkills[i]);

	if (leveltype != 0) {
		// Save monster block
		for (i = 0; i < MAXMONSTERS; i++) ISave(monstactive[i]);
		for (i = 0; i < nummonsters; i++) SaveMonst(monstactive[i]);
		// Save missile block
		for (i = 0; i < MAXMISSILES; i++) BSave(missileactive[i]);
		for (i = 0; i < MAXMISSILES; i++) BSave(missileavail[i]);
		for (i = 0; i < nummissiles; i++) SaveMissile(missileactive[i]);
		// Save object block
		for (i = 0; i < MAXOBJECTS; i++) BSave(objectactive[i]);
		for (i = 0; i < MAXOBJECTS; i++) BSave(objectavail[i]);
		for (i = 0; i < numobjects; i++) SaveObject(objectactive[i]);
		// Save light and vision
		ISave(numlights);
		for (i = 0; i < MAXLIGHTS; i++) BSave(lightactive[i]);
		for (i = 0; i < numlights; i++) SaveLight(lightactive[i]);
		ISave(visionid);
		ISave(numvision);
		for (i = 0; i < numvision; i++) SaveVision(i);
	}
	// Save item block
	for (i = 0; i < MAXITEMS; i++) BSave(itemactive[i]);
	for (i = 0; i < MAXITEMS; i++) BSave(itemavail[i]);
	for (i = 0; i < numitems; i++) SaveItem(itemactive[i]);
	for (i = 0; i < MAXUITEMS; i++) OSave(UniqueItemFlag[i]);

	// Save map info
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) BSave(dLight[i][j]);
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) BSave(dFlags[i][j] & BFLAG_SAVEMASK);
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) BSave(dPlayer[i][j]);
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) BSave(dItem[i][j]);

	if (leveltype != 0) {
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) ISave(dMonster[i][j]);
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dDead[i][j]);
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dObject[i][j]);
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dLight[i][j]);
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dSaveLight[i][j]);
		for (j = 0; j < AUTOMAPY; j++)
			for (i = 0; i < AUTOMAPX; i++) OSave(automapview[i][j]);
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dMissile[i][j]);
	}
	ISave(numpremium);
	ISave(premiumlevel);
	for (i = 0; i < MAXPREMIUM; i++) SavePremium(i);

	OSave(automapflag);
	ISave(automapscale);

	char szName[MAX_PATH];
	CreateSaveGameName(szName);

	// when we allocated SaveBuff, we made sure to include enough
	// bytes for the encryption information
	WriteSaveFile(
		szName,
		SaveBuff,
		tbuff - SaveBuff,
		CalcEncodeDstBytes(tbuff - SaveBuff)
	);

	DiabloFreePtr(SaveBuff);
	gbValidSaveFile = TRUE;

	// take all the temporary save files which have accumulated
	// during the course of gameplay and make them part of the
	// permanent save game
	MoveTempSavesToPermanent();

	void UpdatePlayerFile();
	UpdatePlayerFile();
#endif
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void SaveLevel()
{
#if !IS_VERSION(BETA)
	BYTE *SaveBuff;
	int	i,j;

	// make sure this code doesn't get called in multiplayer
	// or everyone will have different results...
	app_assert(gbMaxPlayers == 1);
	if (currlevel == 0) glSeedTbl[0] = GetRndSeed();

	// allocate a ptr large enough for save file + encode data
	SaveBuff = DiabloAllocPtrSig(CalcEncodeDstBytes(FILEBUFF),'SAVt');
	tbuff = SaveBuff;

	// Moved here to sync dead unique monsters drb 12/15
	if (leveltype != 0) {
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dDead[i][j]);
	}

	ISave(nummonsters);
	ISave(numitems);
	ISave(numobjects);

	if (leveltype != 0) {
		// Save monster block
		for (i = 0; i < MAXMONSTERS; i++) ISave(monstactive[i]);
		for (i = 0; i < nummonsters; i++) SaveMonst(monstactive[i]);
		// Save object block
		for (i = 0; i < MAXOBJECTS; i++) BSave(objectactive[i]);
		for (i = 0; i < MAXOBJECTS; i++) BSave(objectavail[i]);
		for (i = 0; i < numobjects; i++) SaveObject(objectactive[i]);
	}
	// Save item block
	for (i = 0; i < MAXITEMS; i++) BSave(itemactive[i]);
	for (i = 0; i < MAXITEMS; i++) BSave(itemavail[i]);
	for (i = 0; i < numitems; i++) SaveItem(itemactive[i]);

	// Save map info
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++)
			BSave(dFlags[i][j] & BFLAG_SAVEMASK);
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) BSave(dItem[i][j]);

	if (leveltype != 0) {
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) ISave(dMonster[i][j]);
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dObject[i][j]);
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dLight[i][j]);
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dSaveLight[i][j]);
		for (j = 0; j < AUTOMAPY; j++)
			for (i = 0; i < AUTOMAPX; i++) OSave(automapview[i][j]);
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) BSave(dMissile[i][j]);
	}

	app_assert(FILEBUFF >= tbuff - SaveBuff);
	char szName[MAX_PATH];
	CreateSaveLevelName(szName);

	// when we allocated SaveBuff, we made sure to include enough
	// bytes for the encryption information
	WriteSaveFile(
		szName,
		SaveBuff,
		tbuff - SaveBuff,
		CalcEncodeDstBytes(tbuff - SaveBuff)
	);

	DiabloFreePtr(SaveBuff);
	if (!setlevel) plr[myplr]._pLvlVisited[currlevel] = TRUE;
	else plr[myplr]._pSLvlVisited[setlvlnum] = TRUE;
#endif
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void LoadLevel() {
#if !IS_VERSION(BETA)
	int	i,j;

	DWORD LoadSize;
	char szName[MAX_PATH];
	CreateLoadLevelName(szName);
	BYTE * LoadBuff = ReadSaveFile(szName,&LoadSize);
	tbuff = LoadBuff;

	if (leveltype != 0) {
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dDead[i][j] = BLoad();
		SyncUniqDead();
	}

	nummonsters = ILoad();
	numitems = ILoad();
	numobjects = ILoad();

	if (leveltype != 0) {
		// Load monster block
		for (i = 0; i < MAXMONSTERS; i++) monstactive[i] = ILoad();
		for (i = 0; i < nummonsters; i++) LoadMonst(monstactive[i]);
		// Load object block
		for (i = 0; i < MAXOBJECTS; i++) objectactive[i] = BLoad();
		for (i = 0; i < MAXOBJECTS; i++) objectavail[i] = BLoad();
		for (i = 0; i < numobjects; i++) LoadObject(objectactive[i]);
		for (i = 0; i < numobjects; i++) SyncObjectAnim(objectactive[i]);
	}
	// Load item block
	for (i = 0; i < MAXITEMS; i++) itemactive[i] = BLoad();
	for (i = 0; i < MAXITEMS; i++) itemavail[i] = BLoad();
	for (i = 0; i < numitems; i++) LoadItem(itemactive[i]);

	// Load map info
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) dFlags[i][j] = BLoad();
	for (j = 0; j < MAXDUNY; j++)
		for (i = 0; i < MAXDUNX; i++) dItem[i][j] = BLoad();

	if (leveltype != 0) {
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dMonster[i][j] = ILoad();
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dObject[i][j] = BLoad();
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dLight[i][j] = BLoad();
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dSaveLight[i][j] = BLoad();
		for (j = 0; j < AUTOMAPY; j++)
			for (i = 0; i < AUTOMAPX; i++) automapview[i][j] = OLoad();
		for (j = 0; j < MAXDUNY; j++)
			for (i = 0; i < MAXDUNX; i++) dMissile[i][j] = 0;
	}

	// Misc sync routines
	SyncAutomap();
	ResyncQuests();
	SyncPortals();

	// player lighting needs to be reset because players enter level at different
	// location than where they leave
	dolighting = TRUE;
	for (i = 0; i < MAX_PLRS; i++) {
		if (plr[i].plractive && currlevel == plr[i].plrlevel)
			LightList[plr[i]._plid]._lunflag = TRUE;
	}

	DiabloFreePtr(LoadBuff);
#endif
}
