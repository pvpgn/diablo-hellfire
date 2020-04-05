/*-----------------------------------------------------------------------**
** 								Diablo
**
**							   Town file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/TOWNERS.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"engine.h"
#include	"gendung.h"
#include	"palette.h"

#include	"items.h"
#include	"itemdat.h"
#include	"inv.h"

#include	"player.h"
#include	"monster.h"

#include	"town.h"
#include	"quests.h"
#include	"towners.h"
#include	"minitext.h"
#include	"textdat.h"
#include	"stores.h"
#include	"effects.h"
#include	"multi.h"

#include	"cursor.h"
#include	"msg.h"

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SpawnSomething(int, int, int);
TownerStruct towner[MAX_TOWNERS];
int numtowners;
BOOL storeflag;
BOOL boyloadflag;
BOOL bannerflag;

char AnimOrder[6][148] = {
{ 5,6,7,8,9,10,11,12,13,14,14,13,12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12,13,14,14,13,12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12,13,14,14,13,12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12,13,14,14,13,12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12,13,14,14,13,12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12,13,14,15,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,3,4,-1 },  // Blacksmith
{ 1,2,3,3,2,1,20,19,19,20,1,2,3,3,2,1,20,19,19,20,1,2,3,3,2,1,20,19,19,20,1,2,3,3,2,1,20,19,19,20,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,15,14,13,12,11,10,9,8,7,6,5,4,5,6,7,8,9,10,11,12,13,14,15,16,15,14,13,12,11,10,9,8,7,6,5,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,-1 },  // Healer
{ 1,1,25,25,24,23,22,21,20,19,18,17,16,15,16,17,18,19,20,21,22,23,24,25,25,25,1,1,1,25,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,-1 },  // Storyteller
{ 1,2,3,3,2,1,16,15,14,14,16,1,2,3,3,2,1,16,15,14,14,15,16,1,2,3,3,2,1,16,15,14,14,15,16,1,2,3,3,2,1,16,15,14,14,15,16,1,2,3,3,2,1,16,15,14,14,15,16,1,2,3,3,2,1,16,15,14,14,15,16,1,2,3,3,2,1,16,15,14,14,15,16,1,2,3,2,1,16,15,14,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,-1 },  // Innkeeper
{ 1,1,1,2,3,4,5,6,7,8,9,10,11,11,11,11,12,13,14,15,16,17,18,18,1,1,1,18,17,16,15,14,13,12,11,10,11,12,13,14,15,16,17,18,1,2,3,4,5,5,5,4,3,2,-1 },  // Towndrunk
{ 4,4,4,5,6,6,6,5,4,15,14,13,13,13,14,15,4,5,6,6,6,5,4,4,4,5,6,6,6,5,4,15,14,13,13,13,14,15,4,5,6,6,6,5,4,4,4,5,6,6,6,5,4,15,14,13,13,13,14,15,4,5,6,6,6,5,4,3,2,1,19,18,19,1,2,1,19,18,19,1,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,14,13,13,13,13,14,15,15,15,14,13,12,12,12,11,10,10,10,9,8,9,10,10,11,12,13,14,15,16,17,18,19,1,2,1,19,18,19,1,2,1,2,3,-1 }  // Witch
};

/*-----------------------------------------------------------------------*/

#define TOWN_NUMCOWS	3


#define COW				// turns off cow quest JKECOW

BYTE *pCowCels;
int TownCowX[] = { 58, 56, 59 };
int TownCowY[] = { 16, 14, 20 };
int TownCowDir[] = { DIR_DL, DIR_UL, DIR_U };

int cowoffx[8] = { -1, 0, -1, -1, -1, 0, -1, -1 };
int cowoffy[8] = { -1, -1, -1, 0, -1, -1, -1, 0 };

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
int Qtalklist[14][MAXQUESTS] = {
//		Q_ROCK			Q_BKMUSHRM		Q_GARBUD	Q_ZHAR		Q_VEIL			Q_DIABLO		Q_BUTCHER		Q_LTBANNER		Q_BLIND			Q_BLOOD			Q_ANVIL			Q_WARLORD		Q_SKELKING		Q_PWATER	Q_SCHAMB		Q_BETRAYER		Q_CRYPTMAP		Q_FARMER	Q_THEO	Q_TRADER	Q_DEFILER	Q_NA_KRUL	Q_CORNERSTONE	Q_COWSUIT
	{	TXT_INFRABS2,	TXT_BLKMBS1,	-1,			-1,			TXT_VEILBS1,	-1,				TXT_BUTCHBS1,	TXT_BOLBS1,		TXT_BLINDBS1,	TXT_BLOODBS1,	TXT_ANVILBS2,	TXT_WARLRDBS1,	TXT_KINGBS1,	TXT_PWBS1,	TXT_BONEBS1,	TXT_VBBS1,		TXT_CRYPTMAP2,	-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// black smith
	{	TXT_INFRAH1,	-1,				-1,			-1,			TXT_VEILH1,		-1,				TXT_BUTCHH1,	TXT_BOLH1,		TXT_BLINDH1,	TXT_BLOODH1,	TXT_ANVILH1,	TXT_WARLRDH1,	TXT_KINGH1,		TXT_PWH2,	TXT_BONEH1,		TXT_VBH1,		TXT_CRYPTMAP3,	-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// healer
	{	-1,				-1,				-1,			-1,			-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,			-1,				-1,				-1,				-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// dead guy
	{	TXT_INFRATO1,	TXT_BLKMTO1,	-1,			-1,			TXT_VEILTO1,	-1,				TXT_BUTCHTO1,	-1,				TXT_BLINDTO1,	TXT_BLOODTO1,	TXT_ANVILTO1,	TXT_WARLRDTO1,	TXT_KINGTO2,	TXT_PWTO1,	TXT_BONETO1,	TXT_VBTO1,		TXT_CRYPTMAP5,	-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// tavern owner
	{	TXT_INFRAST1,	TXT_BLKMST1,	-1,			-1,			TXT_VEILST1,	TXT_VBST3,		TXT_BUTCHST1,	TXT_BOLST1,		TXT_BLINDST1,	TXT_BLOODST1,	TXT_ANVILST1,	TXT_WARLRDST1,	TXT_KINGST1,	TXT_PWST1,	TXT_BONEST1,	TXT_VBST2,		TXT_CRYPTMAP6,	-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// storyteller
	{	TXT_INFRATD1,	TXT_BLKMTD1,	-1,			-1,			TXT_VEILTD1,	-1,				TXT_BUTCHTD1,	TXT_BOLTD1,		TXT_BLINDTD1,	TXT_BLOODTD1,	TXT_ANVILTD1,	TXT_WARLRDTD1,	TXT_KINGTD1,	TXT_PWTD1,	TXT_BONETD1,	TXT_VBTD1,		TXT_CRYPTMAP7,	-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// town drunk
	{	TXT_INFRAW1,	TXT_BLKMW2,		-1,			-1,			TXT_VEILW1,		-1,				TXT_BUTCHW1,	TXT_BOLW1,		TXT_BLINDW1,	TXT_BLOODW1,	TXT_ANVILW1,	TXT_WARLRDW1,	TXT_KINGW1,		TXT_PWW1,	TXT_BONEW1,		TXT_VBW1,		TXT_CRYPTMAP1,	-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// witch
	{   TXT_INFRABM1,	TXT_BLKMBM1,	-1,			-1,			TXT_VEILBM1,	-1,				TXT_BUTCHBM1,	TXT_BOLBM1,		TXT_BLINDBM1,	TXT_BLOODBM1,	TXT_ANVILBM1,	TXT_WARLRDBM1,	TXT_KINGBM1,	TXT_PWBM1,	TXT_BONEBM1,	TXT_VBBM1,		TXT_CRYPTMAP8,	-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// barmaid 
	{	TXT_INFRAPB1,	TXT_BLKMPB1,	-1,			-1,			TXT_VEILPB1,	-1,				TXT_BUTCHPB1,	TXT_BOLPB1,		TXT_BLINDPB1,	TXT_BLOODPB1,	TXT_ANVILPB1,	TXT_WARLRDPB1,	TXT_KINGPB1,	TXT_PWPB1,	TXT_BONEPB1,	TXT_VBPB1,		TXT_CRYPTMAP9,	-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// peg-legged boy
	{	-1,				-1,				-1,			-1,			-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,			-1,				-1,				-1,				-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// cow
	{	-1,				-1,				-1,			-1,			-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,			-1,				-1,				-1,				-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// Farmer
	{	-1,				-1,				-1,			-1,			-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,			-1,				-1,				-1,				-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// Little Girl
	{	-1,				-1,				-1,			-1,			-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,			-1,				-1,				-1,				-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// cow suit guy
	{	-1,				-1,				-1,			-1,			-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,				-1,			-1,				-1,				-1,				-1,			-1,		-1,			-1,			-1,			-1,				-1			},	// traveling salesman
};

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
int GetActiveTowner(int t) {
	for (int i = 0; i < numtowners; i++) {
		if (towner[i]._ttype == t) return(i);
	}
	return(-1);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void SetTownerGPtrs(BYTE *pData, BYTE *pAnim[]) {
	BYTE *p;
	int i;

	for (i = 0; i < 8; i++) {
		p = pData;
		__asm {
			mov		eax,dword ptr [p]
			mov		ebx,eax
			mov		edx,dword ptr [i]
			shl		edx,2
			add		ebx,edx
			mov		edx,dword ptr [ebx]
			add		eax,edx
			mov		dword ptr [p],eax
		}
		pAnim[i] = p;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void NewTownerAnim(int tnum, BYTE *pAnim, int numFrames, int Delay)
{
	app_assert(pAnim);
	towner[tnum]._tAnimData = pAnim;
	towner[tnum]._tAnimLen = numFrames;
	towner[tnum]._tAnimFrame = 1;
	towner[tnum]._tAnimCnt = 0;
	towner[tnum]._tAnimDelay = Delay;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void InitTownerInfo(int i, long w, BOOL sel, int t, int x, int y, char ao, int tp)
{
	ZeroMemory(&towner[i],sizeof(TownerStruct));
	towner[i]._tSelFlag = sel;
	towner[i]._tAnimWidth = w;
	towner[i]._tAnimWidth2 = (w - 64) >> 1;
	towner[i]._tMsgSaid = FALSE;
	towner[i]._ttype = t;
	towner[i]._tx = x;
	towner[i]._ty = y;
	dMonster[x][y] = i + 1;
	towner[i]._tAnimOrder = ao;
	towner[i]._tTenPer = tp;	
	towner[i]._tSeed = GetRndSeed();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitQstSnds(int i)
{
	int j = i;

	if (boyloadflag) j = i+1;
	for (int quest = 0; quest < MAXQUESTS; quest++) {
		towner[i].qsts[quest]._qsttype = quests[quest]._qtype;
		towner[i].qsts[quest]._qstmsg = Qtalklist[j][quest];
		if (Qtalklist[j][quest] != -1) towner[i].qsts[quest]._qstmsgact = QSTMSG_AVAIL;
			else towner[i].qsts[quest]._qstmsgact = QSTMSG_NOTAVAIL;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitSmith()
{
	InitTownerInfo(numtowners, 96, TRUE, TWN_BLKSMITH, 62, 63, 0, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Smith\\SmithN.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 16;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_DL], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Griswold the Blacksmith");
	numtowners++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitBarOwner()
{
	bannerflag = FALSE;
	InitTownerInfo(numtowners, 96, TRUE, TWN_BAROWNER, 55, 62, 3, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\TwnF\\TwnFN.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 16;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_DL], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Ogden the Tavern owner");
	numtowners++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitTownDead() {
	InitTownerInfo(numtowners, 96, TRUE, TWN_DEAD, 24, 32, -1, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Butch\\Deadguy.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 8;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_U], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Wounded Townsman");
	numtowners++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitWitch() {
	InitTownerInfo(numtowners, 96, TRUE, TWN_WITCH, 80, 20, 5, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\TownWmn1\\Witch.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 19;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_D], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Adria the Witch");
	numtowners++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitBarmaid() {
	InitTownerInfo(numtowners, 96, TRUE, TWN_BARMAID, 43, 66, -1, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\TownWmn1\\WmnN.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 18;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_D], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Gillian the Barmaid");
	numtowners++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitBoy() {
	boyloadflag = TRUE;
	InitTownerInfo(numtowners, 96, TRUE, TWN_BOY, 11, 53, -1, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners); 
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\TownBoy\\PegKid1.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_D], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Wirt the Peg-legged boy");
	numtowners++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitHealer() {
	InitTownerInfo(numtowners, 96, TRUE, TWN_HEALER, 55, 79, 1, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Healer\\Healer.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_DR], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Pepin the Healer");
	numtowners++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitTeller()
{
	InitTownerInfo(numtowners, 96, TRUE, TWN_TELLER, 62, 71, 2, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Strytell\\Strytell.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 25;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_D], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Cain the Elder");
	numtowners++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitDrunk() {
	InitTownerInfo(numtowners, 96, TRUE, TWN_DRUNK, 71, 84, 4, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Drunk\\TwnDrunk.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 18;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_D], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Farnham the Drunk");
	numtowners++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitCows()
{
	int i, x, y, d, x2, y2;

	app_assert(! pCowCels);
	pCowCels = LoadFileInMemSig("Towners\\Animals\\Cow.CEL",NULL,'TOWN');

	for (i = 0; i < TOWN_NUMCOWS; i++) {
		x = TownCowX[i];
		y = TownCowY[i];
		d = TownCowDir[i];
		InitTownerInfo(numtowners, 128, FALSE, TWN_COW, x, y, -1, 10);
		towner[numtowners]._tNData = pCowCels;
		SetTownerGPtrs(towner[numtowners]._tNData, towner[numtowners]._tNAnim);
		towner[numtowners]._tNFrames = 12;
		NewTownerAnim(numtowners, towner[numtowners]._tNAnim[d], towner[numtowners]._tNFrames, 3);
		towner[numtowners]._tAnimFrame = random(0,11) + 1;
		towner[numtowners]._tSelFlag = TRUE;
		strcpy(towner[numtowners]._tName, "Cow");
		x2 = x + cowoffx[d];
		y2 = y + cowoffy[d];
		if (dMonster[x][y2] == 0) dMonster[x][y2] = -(numtowners + 1);
		if (dMonster[x2][y] == 0) dMonster[x2][y] = -(numtowners + 1);
		if (dMonster[x2][y2] == 0) dMonster[x2][y2] = -(numtowners + 1);
		numtowners++;
	}
}

// JKENEWTOWNER INIT INFO HERE
void InitFarmer()
{
	InitTownerInfo(numtowners, 96, TRUE, TWN_FARMER, 62, 16, -1, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Farmer\\Farmrn2.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 15;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_D], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Lester the farmer");
	numtowners++;
}
void InitCowsuit()
{
	InitTownerInfo(numtowners, 96, TRUE, TWN_COWSUIT, 61, 22, -1, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	if(quests[Q_COWSUIT]._qactive != QUEST_DONE)
		towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Farmer\\cfrmrn2.CEL",NULL,'TOWN');
	else
		towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Farmer\\mfrmrn2.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 15;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_DL], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Complete Nut");
	numtowners++;

}
void InitGirl()
{
	InitTownerInfo(numtowners, 96, TRUE, TWN_GIRL, 77, 43, -1, 10);
	app_assert(! towner[numtowners]._tNData);
	InitQstSnds(numtowners);
	if (quests[Q_THEO]._qactive != QUEST_DONE)
		towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Girl\\Girlw1.CEL",NULL,'TOWN');
	else
		towner[numtowners]._tNData = LoadFileInMemSig("Towners\\Girl\\Girls1.CEL",NULL,'TOWN');
	for (int i = 0; i < 8; i++) towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	towner[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_D], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Celia");
	numtowners++;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitTowners()	//when adding any towners add the init in order of the defines
{
	numtowners = 0;
	boyloadflag = FALSE;
	InitSmith();
	InitHealer();
	if (quests[Q_BUTCHER]._qactive != QUEST_NOTAVAIL && quests[Q_BUTCHER]._qactive != QUEST_DONE)
		InitTownDead();
	InitBarOwner();
	InitTeller();
	InitDrunk();
	InitWitch();
	InitBarmaid();
	InitBoy();
	// From here down, not controled, only animated
	InitCows();
	// HELLFIRE INIT JKENEWTOWNERS

	if (gbCowsuit)
	{
			InitCowsuit();
	}
	else
	{
		if (quests[Q_FARMER]._qactive != QUEST_REALLYDONE)
			InitFarmer();
	}

	if (gbTheo)
		if (plr[0]._pLvlVisited[HIVESTART])
			InitGirl();

}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void FreeTownerGFX() {
	int i;

	for (i = 0; i < MAX_TOWNERS; i++) {
		if (towner[i]._tNData == pCowCels) {
			// don't free
			towner[i]._tNData = NULL;
		}
		else if (towner[i]._tNData) {
			DiabloFreePtr(towner[i]._tNData);
		}
	}

	// now free cow cels
	DiabloFreePtr(pCowCels);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownCtrlMsg(int i)
{
	if (towner[i]._tbtcnt != 0) {
		int const p = towner[i]._tVar1;
		int const dx = abs(towner[i]._tx - plr[p]._px);
		int const dy = abs(towner[i]._ty - plr[p]._py);
		if ((dx >= 2) || (dy >= 2))
		{ 
			towner[i]._tbtcnt = 0;
			qtextflag = FALSE;
			stream_stop();
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownBlackSmith()
{
	int const tidx = GetActiveTowner(TWN_BLKSMITH);
	TownCtrlMsg(tidx);
#if 0
	if ((!qtextflag) && quests[Q_ROCK]._qactive == QUEST_DONE) {
		int const x = towner[tidx]._tx;
		int const y = towner[tidx]._ty + 1;
		if (dPlayer[x][y] > 0) x++;
	}
#endif
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownBarOwner()
{
	int const tidx = GetActiveTowner(TWN_BAROWNER);
	TownCtrlMsg(tidx);
#if 0
	if ((!qtextflag) && (quests[Q_LTBANNER]._qactive == QUEST_DONE) && (bannerflag)) {
		int const x = towner[tidx]._tx;
		int const y = towner[tidx]._ty + 1;
		if (dPlayer[x][y] > 0) x++;
	}
#endif
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownDead()
{
	int const tidx = GetActiveTowner(TWN_DEAD);
	TownCtrlMsg(tidx);
	if (!qtextflag) {
		if ((quests[Q_BUTCHER]._qactive == QUEST_NOTDONE) && (!quests[Q_BUTCHER]._qlog)) return;
		if (quests[Q_BUTCHER]._qactive != QUEST_NOTACTIVE) {
			towner[tidx]._tAnimDelay = 1000;
			towner[tidx]._tAnimFrame = 1;
			strcpy(towner[tidx]._tName, "Slain Townsman");
		}
 	}
	if (quests[Q_BUTCHER]._qactive != QUEST_NOTACTIVE) towner[tidx]._tAnimCnt = 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownHealer()
{
	int const tidx = GetActiveTowner(TWN_HEALER);
	TownCtrlMsg(tidx);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownStory()
{

	int const tidx = GetActiveTowner(TWN_TELLER);
	TownCtrlMsg(tidx);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownDrunk()
{

	int const tidx = GetActiveTowner(TWN_DRUNK);
	TownCtrlMsg(tidx);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownBoy()
{

	int const tidx = GetActiveTowner(TWN_BOY);
	TownCtrlMsg(tidx);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownWitch()
{

	int const tidx = GetActiveTowner(TWN_WITCH);
	TownCtrlMsg(tidx);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownBarMaid()
{
	int const tidx = GetActiveTowner(TWN_BARMAID);
	TownCtrlMsg(tidx);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TownCow()
{

	int const tidx = GetActiveTowner(TWN_COW);
	TownCtrlMsg(tidx);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
// add helfire characters here

void Farmer()
{
	int const tidx = GetActiveTowner(TWN_FARMER);
	TownCtrlMsg(tidx);
}

void Cowsuit()
{
	int const tidx = GetActiveTowner(TWN_COWSUIT);
	TownCtrlMsg(tidx);
}

void Girl()
{
	int const tidx = GetActiveTowner(TWN_GIRL);
	TownCtrlMsg(tidx);
}


void ProcessTowners()
{
	int		i;

	for (i = 0; i < MAX_TOWNERS; ++i) {
		switch (towner[i]._ttype) {
			case TWN_BLKSMITH :
				TownBlackSmith();
				break;
			case TWN_HEALER :
				TownHealer();
				break;
			case TWN_DEAD :
				TownDead();
				break;
			case TWN_BAROWNER :
				TownBarOwner();
				break;
			case TWN_TELLER :
				TownStory();
				break;
			case TWN_DRUNK :
				TownDrunk();
				break;
			case TWN_BOY :
				TownBoy();
				break;
			case TWN_WITCH :
				TownWitch();
				break;
			case TWN_BARMAID :
				TownBarMaid();
				break;
			case TWN_COW :
				TownCow();
				break;
			case TWN_FARMER:
				Farmer();
				break;
			case TWN_GIRL:
				Girl();
				break;
			case TWN_COWSUIT:
				Cowsuit();
				break;
		}

		// Animate Towner
		++towner[i]._tAnimCnt;
		if (towner[i]._tAnimCnt >= towner[i]._tAnimDelay) {
			towner[i]._tAnimCnt = 0;
			if (towner[i]._tAnimOrder >= 0) {
				int const ao = towner[i]._tAnimOrder;
				++towner[i]._tAnimFrameCnt;
				if (AnimOrder[ao][towner[i]._tAnimFrameCnt] == -1) towner[i]._tAnimFrameCnt = 0;
				towner[i]._tAnimFrame = AnimOrder[ao][towner[i]._tAnimFrameCnt];
			} else {
				++towner[i]._tAnimFrame;
				if (towner[i]._tAnimFrame > towner[i]._tAnimLen) towner[i]._tAnimFrame = 1;
			}
		}
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
ItemStruct *PlrHasItem(int pnum, int item, int &i)
{
	for (i = 0; i < plr[pnum]._pNumInv; ++i) {
		if (plr[pnum].InvList[i].IDidx == item)
			return &plr[pnum].InvList[i];
	}
	return FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
static DWORD sgdwCowClicks;
static int sgnCowMsg;
static void CowSFX(int pnum) {
	#if !IS_VERSION(SHAREWARE)
	#define NUM_COW_SFX 3
	static const int snSFX[NUM_COW_SFX][NUM_CLASSES] = {
		{ PS_WARR52, PS_ROGUE52, PS_MAGE52, PS_MONK52, PS_BARD52 }, // yup, that's a cow
		{ PS_WARR49, PS_ROGUE49, PS_MAGE49, PS_MONK49, PS_BARD49 }, // I'm not thirsty
		{ PS_WARR50, PS_ROGUE50, PS_MAGE50, PS_MONK50, PS_BARD50 }, // I'm no milkmaid
	};
	#endif

	// make sure we're not still playing the last cow SFX
	static int snLastCowSFX = -1;
	BOOL effect_is_playing(int nSFX);
	if (snLastCowSFX != -1 && effect_is_playing(snLastCowSFX))
		return;

	// next cow sound
	sgdwCowClicks++;
	#if !IS_VERSION(SHAREWARE)
	if (sgdwCowClicks >= 8) {
		// play cow in background
		PlaySfxLoc(TSFX_COW1, plr[pnum]._px, plr[pnum]._py + 5);
	
		// reset cow counter
		sgdwCowClicks = 4;

		// choose player cow sfx
		snLastCowSFX = snSFX[sgnCowMsg][plr[pnum]._pClass];
		if (++sgnCowMsg >= NUM_COW_SFX) sgnCowMsg = 0;
	}
	else
	#endif
	if (sgdwCowClicks == 4) {
		#if IS_VERSION(SHAREWARE)
		sgdwCowClicks = 0;
		#endif
		snLastCowSFX = TSFX_COW2;
	}
	else {
		snLastCowSFX = TSFX_COW1;
	}

	PlaySfxLoc(snLastCowSFX,plr[pnum]._px,plr[pnum]._py);
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void TownerTalk(int first, int t)
{
	// reset cow counter
	sgdwCowClicks = 0;
	sgnCowMsg = 0;

	storeflag = TRUE;
	InitQTextMsg(first);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void TalkToTowner(int p, int t)
{
	int i;
	int dx, dy;
	int r3, r4, r5;
	ItemStruct *Item;

	r3 = random(6, 3);
	r4 = random(6, 4);
	r5 = random(6, 5);
	dx = abs(plr[p]._px - towner[t]._tx);
	dy = abs(plr[p]._py - towner[t]._ty);
#if CHEATS
	if ((!davedebug) && ((dx >= 2) || (dy >= 2))) return;
#else
	if ((dx >= 2) || (dy >= 2)) return;
#endif
	if (qtextflag) return;
	towner[t]._tMsgSaid = FALSE;

	//Does player have item in hand? If so, drop it before talking to town person.
	if (curs >= ICSTART) {
		if(!DropItemBeforeTrig()) return;
	}

/*-----Tavern Owner------------------------------------------------------*/
	if (t == GetActiveTowner(TWN_BAROWNER)) {
		if (!(plr[p]._pLvlVisited[0]) && !(towner[t]._tMsgSaid)) {
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			InitQTextMsg(TXT_INTRO);
			towner[t]._tMsgSaid = TRUE;
		}
		if ((plr[p]._pLvlVisited[2] || plr[p]._pLvlVisited[4]) && (quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL)) {
		// skel king init
			if ((quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) && (quests[Q_SKELKING]._qvar2 == 0) && (!towner[t]._tMsgSaid)) {
				quests[Q_SKELKING]._qvar2 = 1;
				quests[Q_SKELKING]._qlog = TRUE;
				if (quests[Q_SKELKING]._qactive == QUEST_NOTACTIVE) {
					quests[Q_SKELKING]._qactive = QUEST_NOTDONE;
					quests[Q_SKELKING]._qvar1 = 1;	// state of quest
				}
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TXT_KINGTO1);
				towner[t]._tMsgSaid = TRUE;			
				NetSendCmdQuest(TRUE, Q_SKELKING);
			}
		// skel king finished
			if ((quests[Q_SKELKING]._qactive == QUEST_DONE) && (quests[Q_SKELKING]._qvar2 == 1) && (!towner[t]._tMsgSaid)) {
				quests[Q_SKELKING]._qvar2 = 2;
				quests[Q_SKELKING]._qvar1 = 2;	// state of quest
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TXT_KINGTO3);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, Q_SKELKING);
			}
		}			

		if (gbMaxPlayers == 1) {
			// banner of light init
			if ((plr[p]._pLvlVisited[3]) && (quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL)) {
				if ((quests[Q_LTBANNER]._qactive == QUEST_NOTACTIVE) || (quests[Q_LTBANNER]._qactive == QUEST_NOTDONE)) {
					if ((quests[Q_LTBANNER]._qvar2 == 0) && (!towner[t]._tMsgSaid)) {
						quests[Q_LTBANNER]._qvar2 = 1;
						if (quests[Q_LTBANNER]._qactive == QUEST_NOTACTIVE) {
							quests[Q_LTBANNER]._qvar1 = 1;	// state of quest
							quests[Q_LTBANNER]._qactive = QUEST_NOTDONE;
						}
						quests[Q_LTBANNER]._qlog = TRUE;
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TXT_BOLTO1);
						towner[t]._tMsgSaid = TRUE;
					}
				}
			}
			// banner of light finished
			if ((!towner[t]._tMsgSaid) && PlrHasItem(p, IDI_BANNER, i)) {
				quests[Q_LTBANNER]._qactive = QUEST_DONE;
				quests[Q_LTBANNER]._qvar1 = 3;
				RemoveInvItem(p, i);
				CreateItem(UID_HARCREST, towner[t]._tx, towner[t]._ty + 1); // Harlequin Crest
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TXT_BOLTO2);
				towner[t]._tMsgSaid = TRUE;
			}
		}
		if (!qtextflag) {
			TownerTalk(TXT_OGDEN1, t);
			if(storeflag) StartStore(STORE_TAVERN);
		}
	}
	
/*-----Nearly dead guy---------------------------------------------------*/
	else if (t == GetActiveTowner(TWN_DEAD)) {
		if ((quests[Q_BUTCHER]._qactive == QUEST_NOTDONE) && (quests[Q_BUTCHER]._qvar1 == 1)) {
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			quests[Q_BUTCHER]._qvar1 = 1;
			#if !IS_VERSION(SHAREWARE)
			BOOL effect_is_playing(int nSFX);
			if ((plr[p]._pClass == CLASS_WARRIOR) && !effect_is_playing(PS_WARR8)) PlaySFX(PS_WARR8);
			else if ((plr[p]._pClass == CLASS_ROGUE) && !effect_is_playing(PS_ROGUE8)) PlaySFX(PS_ROGUE8);
			else if ((plr[p]._pClass == CLASS_SORCEROR) && !effect_is_playing(PS_MAGE8)) PlaySFX(PS_MAGE8);
			else if ((plr[p]._pClass == CLASS_MONK) && !effect_is_playing(PS_MONK8)) PlaySFX(PS_MONK8);
			else if ((plr[p]._pClass == CLASS_BARD) && !effect_is_playing(PS_BARD8)) PlaySFX(PS_BARD8);
			else if ((plr[p]._pClass == CLASS_BARBARIAN) && !effect_is_playing(PS_BARBARIAN8)) PlaySFX(PS_BARBARIAN8);
			#endif
			towner[t]._tMsgSaid = TRUE;
		}
		else if ((quests[Q_BUTCHER]._qactive == QUEST_DONE) && (quests[Q_BUTCHER]._qvar1 == 1)) {
	/*		#if !IS_VERSION(SHAREWARE)
			BOOL effect_is_playing(int nSFX);
			if ((plr[p]._pClass == CLASS_WARRIOR) && !effect_is_playing(PS_WARR9)) PlaySFX(PS_WARR9);
			else if ((plr[p]._pClass == CLASS_ROGUE) && !effect_is_playing(PS_ROGUE9)) PlaySFX(PS_ROGUE9);
			else if ((plr[p]._pClass == CLASS_SORCEROR) && !effect_is_playing(PS_MAGE9)) PlaySFX(PS_MAGE9);
			else if ((plr[p]._pClass == CLASS_MONK) && !effect_is_playing(PS_MONK9)) PlaySFX(PS_MONK9);
			else if ((plr[p]._pClass == CLASS_BARD) && !effect_is_playing(PS_BARD9)) PlaySFX(PS_BARD9);
			else if ((plr[p]._pClass == CLASS_BARBARIAN) && !effect_is_playing(PS_BARBARIAN9)) PlaySFX(PS_BARBARIAN9);
			#endif
	*/
		    //InitQTextMsg(5);
			quests[Q_BUTCHER]._qvar1 = 1;
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			towner[t]._tMsgSaid = TRUE;
		}

		else if ((quests[Q_BUTCHER]._qactive == QUEST_NOTACTIVE) ||
			((quests[Q_BUTCHER]._qactive == QUEST_NOTDONE) && (!quests[Q_BUTCHER]._qvar1))) {
			quests[Q_BUTCHER]._qactive = QUEST_NOTDONE;
			quests[Q_BUTCHER]._qlog = TRUE;
			quests[Q_BUTCHER]._qmsg = TXT_BUTCH1;
			quests[Q_BUTCHER]._qvar1 = 1;
			towner[t]._tbtcnt = 50;
			towner[t]._tVar1 = p;
			towner[t]._tVar2 = 3;
			InitQTextMsg(TXT_BUTCH1);
			towner[t]._tMsgSaid = TRUE;
			NetSendCmdQuest(TRUE, Q_BUTCHER);
		}
	}
/*-----Blacksmith-------------------------------------------------------*/
	else if (t == GetActiveTowner(TWN_BLKSMITH)) {
		if (gbMaxPlayers == 1) { 
			// magic rock init
			if ((plr[p]._pLvlVisited[4]) && (quests[Q_ROCK]._qactive != QUEST_NOTAVAIL)) {
				if ((quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) && (quests[Q_ROCK]._qvar2 == 0)) {
					quests[Q_ROCK]._qvar2 = 1;
					quests[Q_ROCK]._qlog = TRUE;
					if (quests[Q_ROCK]._qactive == QUEST_NOTACTIVE) {
						quests[Q_ROCK]._qactive = QUEST_NOTDONE;
						quests[Q_ROCK]._qvar1 = 1;	// state
					}
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TXT_INFRABS1);
					towner[t]._tMsgSaid = TRUE;
				}
			}
			// magic rock finished
			if ((!towner[t]._tMsgSaid) && PlrHasItem(p, IDI_ROCK, i))
			{
				quests[Q_ROCK]._qactive = QUEST_DONE;
				quests[Q_ROCK]._qvar2 = 2;
				quests[Q_ROCK]._qvar1 = 2;
				RemoveInvItem(p, i);
				CreateItem(UID_INFRARING, towner[t]._tx, towner[t]._ty + 1); // Ring of Infravision
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TXT_INFRABS3);
				towner[t]._tMsgSaid = TRUE;
			}

			// anvil init
			if ((plr[p]._pLvlVisited[9]) && (quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL)) {
				if ((quests[Q_ANVIL]._qactive == QUEST_NOTACTIVE) || (quests[Q_ANVIL]._qactive == QUEST_NOTDONE)) {
					if ((quests[Q_ANVIL]._qvar2 == 0) && (!towner[t]._tMsgSaid)) {
						if ((quests[Q_ROCK]._qvar2 == 2)
							|| ((quests[Q_ROCK]._qactive == QUEST_NOTDONE) && (quests[Q_ROCK]._qvar2 == 1))) {
							quests[Q_ANVIL]._qvar2 = 1;
							quests[Q_ANVIL]._qlog = TRUE;
							if (quests[Q_ANVIL]._qactive == QUEST_NOTACTIVE) {
								quests[Q_ANVIL]._qactive = QUEST_NOTDONE;
								quests[Q_ANVIL]._qvar1 = 1;
							}
							towner[t]._tbtcnt = 150;
							towner[t]._tVar1 = p;
							InitQTextMsg(TXT_ANVILBS1);
							towner[t]._tMsgSaid = TRUE;			
						}
					}
				}
			}
			// anvil finished
			if ((!towner[t]._tMsgSaid) && PlrHasItem(p, IDI_ANVIL, i)) { 
				quests[Q_ANVIL]._qactive = QUEST_DONE;
				quests[Q_ANVIL]._qvar2 = 2;
				quests[Q_ANVIL]._qvar1 = 2;
				RemoveInvItem(p, i);
				CreateItem(UID_GRISWOLD, towner[t]._tx, towner[t]._ty + 1); // Griswolds edge
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TXT_ANVILBS3);
				towner[t]._tMsgSaid = TRUE;
			}
		}
		if (!qtextflag) {
			TownerTalk(TXT_GRIS1, t);
			if(storeflag) StartStore(STORE_SMITH);
		}
	}

/*-----Witch-------------------------------------------------------------*/
	else if (t == GetActiveTowner(TWN_WITCH)) {
		if (quests[Q_BKMUSHRM]._qactive == QUEST_NOTACTIVE
			&& PlrHasItem(p, IDI_FUNGALTM, i)) {
			// got fungal tome
			RemoveInvItem(p, i);
			quests[Q_BKMUSHRM]._qactive = QUEST_NOTDONE;
			quests[Q_BKMUSHRM]._qlog = TRUE;
			quests[Q_BKMUSHRM]._qvar1 = QS_TOMEGIVEN;
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			InitQTextMsg(TXT_BLKMW1);
			towner[t]._tMsgSaid = TRUE;
		} else 	if (quests[Q_BKMUSHRM]._qactive == QUEST_NOTDONE) {
			if (quests[Q_BKMUSHRM]._qvar1 >= QS_TOMEGIVEN
				&& quests[Q_BKMUSHRM]._qvar1 <= QS_MUSHPICKED) {
				// waiting for mushroom
				if (PlrHasItem(p, IDI_MUSHROOM, i)) {
					RemoveInvItem(p, i);
					quests[Q_BKMUSHRM]._qvar1 = QS_MUSHGIVEN;
					Qtalklist[TWN_HEALER][Q_BKMUSHRM] = TXT_BLKMH1;
					Qtalklist[TWN_WITCH][Q_BKMUSHRM] = -1;
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[Q_BKMUSHRM]._qmsg = TXT_BLKMW3;
					InitQTextMsg(TXT_BLKMW3);
					towner[t]._tMsgSaid = TRUE;
				}
				else if (quests[Q_BKMUSHRM]._qmsg != TXT_BLKMW2) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[Q_BKMUSHRM]._qmsg = TXT_BLKMW2;
					InitQTextMsg(TXT_BLKMW2);
					towner[t]._tMsgSaid = TRUE;
				}
			}
			else {
				// waiting for elixir
				if (Item = PlrHasItem(p, IDI_SPECELIX, i)) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TXT_BLKMW5);
					quests[Q_BKMUSHRM]._qactive = QUEST_DONE;
					towner[t]._tMsgSaid = TRUE;
					AllItemsList[Item->IDidx].iUsable = TRUE;
				}
				else if (PlrHasItem(p, IDI_BRAIN, i)
						 && quests[Q_BKMUSHRM]._qvar2 != TXT_BLKMW4) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[Q_BKMUSHRM]._qvar2 = TXT_BLKMW4;
					InitQTextMsg(TXT_BLKMW4);
					towner[t]._tMsgSaid = TRUE;
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(TXT_ADRIA1, t);
			if(storeflag) StartStore(STORE_WITCH);
		}
	}

/*-----BarMaid-------------------------------------------------------------*/
	else if (t == GetActiveTowner(TWN_BARMAID)) {
		if ((!plr[p]._pLvlVisited[CRYPTSTART]) && (PlrHasItem(p, IDI_MAPOFDOOM, i)))
		{
			quests[Q_CRYPTMAP]._qactive = QUEST_NOTDONE;
			quests[Q_CRYPTMAP]._qlog = TRUE;
			quests[Q_CRYPTMAP]._qmsg = TXT_CRYPTMAP8;
			InitQTextMsg(TXT_CRYPTMAP8);
			towner[t]._tMsgSaid = TRUE;
		}
		if (!qtextflag) {
			TownerTalk(TXT_GILIAN1, t);
			if(storeflag) StartStore(STORE_BARMAID);
		}
	}
/*-----Drunk-------------------------------------------------------------*/
	else if (t == GetActiveTowner(TWN_DRUNK)) {
		if (!qtextflag) {
			TownerTalk(TXT_FARN1, t);
			if(storeflag) StartStore(STORE_DRUNK);
		}
	}
/*-----Healer------------------------------------------------------------*/
	else if (t == GetActiveTowner(TWN_HEALER)) {
		if (gbMaxPlayers == 1) { 
			if (plr[p]._pLvlVisited[1] || plr[p]._pLvlVisited[5]) {
				if (!towner[t]._tMsgSaid) {
					if (quests[Q_PWATER]._qactive == QUEST_NOTACTIVE) {
						quests[Q_PWATER]._qactive = QUEST_NOTDONE;
						quests[Q_PWATER]._qlog = TRUE;
						quests[Q_PWATER]._qmsg = TXT_PWH1;
						quests[Q_PWATER]._qvar1 = 1;
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TXT_PWH1);
						towner[t]._tMsgSaid = TRUE;
					} 
					else if ((quests[Q_PWATER]._qactive == QUEST_DONE) && (quests[Q_PWATER]._qvar1 != 2)) {
						quests[Q_PWATER]._qvar1 = 2;
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TXT_PWH3);
						CreateItem(UID_TRING, towner[t]._tx, towner[t]._ty + 1); // Ring of truth
						towner[t]._tMsgSaid = TRUE;
					}
				}
			}
			if (quests[Q_BKMUSHRM]._qactive == QUEST_NOTDONE
				&& quests[Q_BKMUSHRM]._qmsg == TXT_BLKMW3) {
					if (PlrHasItem(p, IDI_BRAIN, i)) {
						RemoveInvItem(p, i);
						// pop out spectral elixir
						SpawnQuestItem(IDI_SPECELIX, towner[t]._tx, towner[t]._ty + 1, FALSE, ISEL_NONE);
						InitQTextMsg(TXT_BLKMH2);
						quests[Q_BKMUSHRM]._qvar1 = QS_BRAINGIVEN;
						Qtalklist[TWN_HEALER][Q_BKMUSHRM] = -1;
					}
			}
		}
		if (!qtextflag) {
			TownerTalk(TXT_PEPIN1, t);
			if(storeflag) StartStore(STORE_HEALER);
		}
	}
/*-----Peg-legged Boy----------------------------------------------------*/
	else if (t == GetActiveTowner(TWN_BOY)) {
		if (!qtextflag) {
			TownerTalk(TXT_WIRT1, t);
			if(storeflag) StartStore(STORE_BOY);
		}
	}
/*-----Story Teller------------------------------------------------------*/
	else if (t == GetActiveTowner(TWN_TELLER)) {
		if (gbMaxPlayers == 1) { 
			if ((quests[Q_BETRAYER]._qactive == QUEST_NOTACTIVE) && (PlrHasItem(p, IDI_LAZSTAFF, i))) {
				RemoveInvItem(p, i);
				quests[Q_BETRAYER]._qvar1 = 2;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TXT_VBST1);
				towner[t]._tMsgSaid = TRUE;
				quests[Q_BETRAYER]._qactive = QUEST_NOTDONE;
				quests[Q_BETRAYER]._qlog = TRUE;
			}
			else if ((quests[Q_BETRAYER]._qactive == QUEST_DONE) && (quests[Q_BETRAYER]._qvar1 == 7)) {
				quests[Q_BETRAYER]._qvar1 = 8;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TXT_VBST3);
				towner[t]._tMsgSaid = TRUE;
				quests[Q_DIABLO]._qlog = TRUE;
			}
		}
		if (gbMaxPlayers != 1) { 
			if ((quests[Q_BETRAYER]._qactive == QUEST_NOTDONE) && (quests[Q_BETRAYER]._qlog == FALSE)) {
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TXT_VBST1);
				towner[t]._tMsgSaid = TRUE;
				quests[Q_BETRAYER]._qlog = TRUE;
				NetSendCmdQuest(TRUE, Q_BETRAYER);
			}
			else if ((quests[Q_BETRAYER]._qactive == QUEST_DONE) && (quests[Q_BETRAYER]._qvar1 == 7)) {
				quests[Q_BETRAYER]._qvar1 = 8;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TXT_VBST3);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, Q_BETRAYER);
				quests[Q_DIABLO]._qlog = TRUE;
				NetSendCmdQuest(TRUE, Q_DIABLO);
			}
		}
		if (!qtextflag) {
			TownerTalk(TXT_STORY1, t);
			if(storeflag) StartStore(STORE_STORYTLR);
		}
	}
/*-----Cow-------------------------------------------------------------------------*/

	else if (towner[t]._ttype == TWN_COW) {
		if (! qtextflag) CowSFX(p);
	} 
/*-------------N E W   T O W N E R S   JKENEWTOWNER -------------------------------- */
	
	else if (towner[t]._ttype == TWN_FARMER) 
	{
		if (! qtextflag) 
		{
			int message = TXT_FARMER1;
//			int textflag = FALSE;
			int textflag = TRUE;
			switch (quests[Q_FARMER]._qactive)
			{
				case QUEST_NOTAVAIL:
					if (PlrHasItem(p, IDI_RUNEBOMB, i))
					{
						message = TXT_FARMER2;
						quests[Q_FARMER]._qactive = QUEST_NOTDONE;
						quests[Q_FARMER]._qvar1 = 1;
						quests[Q_FARMER]._qlog = TRUE;
						quests[Q_FARMER]._qmsg = TXT_FARMER1;
						break;
					}
					else
					if (!(plr[myplr]._pLvlVisited[9] || 
							plr[myplr]._pLevel >= 15))
					{
						message = TXT_FARMER7;
						if (plr[myplr]._pLvlVisited[2]) 
							message = TXT_FARMER5;
						if (plr[myplr]._pLvlVisited[5]) 
							message = TXT_FARMER6;
						if (plr[myplr]._pLvlVisited[7]) 
							message = TXT_FARMER8;

							
/*						int respond;
						respond = random(0,5) + 5;
						switch (respond)
						{
							case 5:
								message = TXT_FARMER5;
								break;
							case 6:
								message = TXT_FARMER6;
								break;
							case 7:
								message = TXT_FARMER7;
								break;
							case 8:
								message = TXT_FARMER8;
								break;
							default:
								message = TXT_FARMER9;
						}*/
					}
					else
					{
						message = TXT_FARMER1;
						quests[Q_FARMER]._qactive = QUEST_NOTDONE;
						quests[Q_FARMER]._qvar1 = 1;
						quests[Q_FARMER]._qlog = TRUE;
						quests[Q_FARMER]._qmsg = TXT_FARMER1;
						SpawnBomb(towner[t]._tx + 1, towner[t]._ty);
						textflag = TRUE;
						break;
					}
				case QUEST_NOTDONE:
					if(PlrHasItem(p, IDI_RUNEBOMB, i))
						message = TXT_FARMER2;
					else
						message = TXT_FARMER3;
					break;
				case QUEST_NOTACTIVE:
					if (PlrHasItem(p, IDI_RUNEBOMB, i))
					{
						message = TXT_FARMER2;
						quests[Q_FARMER]._qactive = QUEST_NOTDONE;
						quests[Q_FARMER]._qvar1 = 1;
						quests[Q_FARMER]._qmsg = TXT_FARMER1;
						quests[Q_FARMER]._qlog = TRUE;
						break;
					}
					else if (!(plr[myplr]._pLvlVisited[9] ||
								plr[myplr]._pLevel >= 15))
					{
						
						message = TXT_FARMER7;
						if (plr[myplr]._pLvlVisited[2]) 
							message = TXT_FARMER5;
						if (plr[myplr]._pLvlVisited[5]) 
							message = TXT_FARMER6;
						if (plr[myplr]._pLvlVisited[7]) 
							message = TXT_FARMER8;

/*						int respond;
						respond = random(0,5) + 5;
						switch (respond)
						{
							case 5:
								message = TXT_FARMER5;
								break;
							case 6:
								message = TXT_FARMER6;
								break;
							case 7:
								message = TXT_FARMER7;
								break;
							case 8:
								message = TXT_FARMER8;
								break;
							default:
								message = TXT_FARMER9;
						}*/
						break;
					}
					else
					{
						message = TXT_FARMER1;
						quests[Q_FARMER]._qactive = QUEST_NOTDONE;
						quests[Q_FARMER]._qvar1 = 1;
						quests[Q_FARMER]._qlog = TRUE;
						quests[Q_FARMER]._qmsg = TXT_FARMER1;
						SpawnBomb(towner[t]._tx + 1, towner[t]._ty);
						textflag = TRUE;
						break;
					}
				case QUEST_DONE:
					message = TXT_FARMER4;
					//SpawnUnique(UID_INFRARING, towner[t]._tx, towner[t]._ty + 1);
					SpawnSomething(IDI_AURIC, towner[t]._tx + 1, towner[t]._ty);
					quests[Q_FARMER]._qactive = QUEST_REALLYDONE;
					quests[Q_FARMER]._qlog = FALSE;
					textflag = TRUE;
					break;
				case QUEST_REALLYDONE:
					message = -1;
					break;
				default:
					message = TXT_FARMER4;
					quests[Q_FARMER]._qactive = QUEST_NOTAVAIL;


			}

			if (message != -1)
			{
				if (textflag)
					InitQTextMsg(message);
				else
					PlaySFX(alltext[message].sfxnr);
			}
			if (gbMaxPlayers != 1)
				NetSendCmdQuest(TRUE, Q_FARMER);
			
		}			// JKENEWTOWNER
	}

	else if (towner[t]._ttype == TWN_COWSUIT) 
	{
		if (! qtextflag) 
		{
			int message = TXT_COWSUIT1;
			int textflag = TRUE;
			if (PlrHasItem(p, IDI_SUITGREY, i))
			{
				message = TXT_COWSUIT7;
				RemoveInvItem(p, i);
			}
			else if (PlrHasItem(p, IDI_SUITBRWN, i))
			{
				CreateItem(UID_ARMRCOW,towner[t]._tx + 1, towner[t]._ty); 
				RemoveInvItem(p, i);
				message = TXT_COWSUIT8;
				quests[Q_COWSUIT]._qactive = QUEST_DONE;
			}
			else if (PlrHasItem(p, IDI_RUNEBOMB, i))
			{
				message = TXT_COWSUIT5;
				quests[Q_COWSUIT]._qactive = QUEST_NOTDONE;
				quests[Q_COWSUIT]._qvar1 = 1;
				quests[Q_COWSUIT]._qmsg = TXT_COWSUIT4;
				quests[Q_COWSUIT]._qlog = TRUE;
			}
			else
			{
				switch (quests[Q_COWSUIT]._qactive)
				{
					case QUEST_NOTAVAIL:
						message = TXT_COWSUIT1;
						quests[Q_COWSUIT]._qactive = QUEST_PASS1;
						break;
					case QUEST_PASS1:
						message = TXT_COWSUIT2;
						quests[Q_COWSUIT]._qactive = QUEST_PASS2;
						break;
					case QUEST_PASS2:
						message = TXT_COWSUIT3;
						quests[Q_COWSUIT]._qactive = QUEST_PASS3;
						break;
					case QUEST_PASS3:
						if (!(plr[myplr]._pLvlVisited[9] || plr[myplr]._pLevel >= 15))
						{
							int respond;
							respond = random(0,4) + 9;
							switch (respond)
							{
								case 9:
									message = TXT_COWSUIT9;
									break;
								case 10:
									message = TXT_COWSUIT10;
									break;
								case 11:
									message = TXT_COWSUIT11;
									break;
								default:
									message = TXT_COWSUIT12;
							}
							break;
						}
						else
						{
							message = TXT_COWSUIT4;
							quests[Q_COWSUIT]._qactive = QUEST_NOTDONE;
							quests[Q_COWSUIT]._qvar1 = 1;
							quests[Q_COWSUIT]._qmsg = TXT_COWSUIT4;
							quests[Q_COWSUIT]._qlog = TRUE;
							SpawnBomb(towner[t]._tx + 1, towner[t]._ty);
							textflag = TRUE;
							break;
						}
					case QUEST_NOTDONE:
						message = TXT_COWSUIT5;
						break;
					case QUEST_DONE:
						message = TXT_COWSUIT1;
						break;
					case QUEST_NOTACTIVE:
						message = TXT_COWSUIT1;
						quests[Q_COWSUIT]._qactive = QUEST_PASS1;
						break;
					default:
						message = TXT_COWSUIT5;
						quests[Q_COWSUIT]._qactive = QUEST_NOTAVAIL;


				}
			}

			if (message != -1)
			{
				if (textflag)
					InitQTextMsg(message);
				else
					PlaySFX(alltext[message].sfxnr);
			}
			if (gbMaxPlayers != 1)
				NetSendCmdQuest(TRUE, Q_COWSUIT);
			
		}				// borrow this JKENEWTOWNER
	}
	else if (towner[t]._ttype == TWN_GIRL) 
	{
		if (! qtextflag) 
		{
			int message = TXT_THEO1;
			int textflag = FALSE;
			if((PlrHasItem(p, IDI_THEODORE, i)) && (quests[Q_THEO]._qactive != QUEST_DONE))
			{
				message = TXT_THEO4;
				RemoveInvItem(p, i);
				//SpawnSomething(IDI_AURIC, towner[t]._tx, towner[t]._ty + 1);
				CreateAmulet(towner[t]._tx, towner[t]._ty, 13, FALSE, TRUE);
				quests[Q_THEO]._qactive = QUEST_DONE;
				quests[Q_THEO]._qlog = FALSE;
				textflag = TRUE;
			}
			else
				switch (quests[Q_THEO]._qactive)
				{
					case QUEST_NOTAVAIL:
						message = TXT_THEO2;
						quests[Q_THEO]._qactive = QUEST_NOTDONE;
						quests[Q_THEO]._qvar1 = 1;
						quests[Q_THEO]._qlog = TRUE;
						quests[Q_THEO]._qmsg = TXT_THEO2;
						textflag = TRUE;
						break;
					case QUEST_NOTDONE:
						message = TXT_THEO3;
						textflag = TRUE;
						break;
					case QUEST_DONE:
						message = -1;
						break;
					case QUEST_NOTACTIVE:
						message = TXT_THEO2;
						quests[Q_THEO]._qvar1 = 1;
						quests[Q_THEO]._qlog = TRUE;
						quests[Q_THEO]._qmsg = TXT_THEO2;
						quests[Q_THEO]._qactive = QUEST_NOTDONE;
						textflag = TRUE;
						break;
					default:
						message = TXT_THEO1;
						quests[Q_THEO]._qactive = QUEST_NOTAVAIL;


				}

			if (message != -1)
			{
				if (textflag)
					InitQTextMsg(message);
				else
					PlaySFX(alltext[message].sfxnr);
			}
			if (gbMaxPlayers != 1)
				NetSendCmdQuest(TRUE, Q_THEO);

			
		}			// JKENEWTOWNER
	}
}

