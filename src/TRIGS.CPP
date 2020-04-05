/*-----------------------------------------------------------------------**
** 								Diablo
**
**							Triggers file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/TRIGS.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"trigs.h"
#include	"palette.h"
#include	"gendung.h"
#include	"items.h"
#include	"player.h"
#include	"quests.h"
#include	"cursor.h"
#include	"control.h"
#include	"effects.h"
#include	"msg.h"
#include	"multi.h"
#include	"error.h"
#include	"inv.h"
#include	"setmaps.h"

//#define JIM


/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

TriggerStruct trigs[MAXTRIGGERS];
int numtrigs;
int TWarpFrom;

int TownDownList[] = { 716, 715, 719, 720, 721, 723, 724, 725, 726, 727, -1 };
int TownWarp1List[] = { 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1181, 1183, 1185, -1 };

//HellFire patch JKE
int TownCryptList[] = { 1331,1332,1333,1334,1335,1336,1337,1338, -1};	// Tag for info box
int TownHiveList[] = { 1307,1308,1309,1310, -1};
//end of patch JKE

int L1UpList[] = { 127, 129, 130, 131, 132, 133, 135, 137, 138, 139, 140, -1 };
int L1DownList[] = { 106, 107, 108, 109, 110, 112, 114, 115, 118, -1 };
int L2UpList[] = { 266, 267, -1 };
int L2DownList[] = { 269, 270, 271, 272, -1 };
int L2TWarpUpList[] = { 558, 559, -1 };
int L3UpList[] = { 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, -1 };
int L3DownList[] = { 162, 163, 164, 165, 166, 167, 168, 169, -1 };
int L3TWarpUpList[] = { 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, -1 };
//int L4UpList[] = { 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113,  -1 };
//int L4DownList[] = { 118, 120, 126, 127, 131, 132, 133, 134, 135, 137, 139, 140, 142, -1 };
//int L4TWarpUpList[] = { 425, 427, 429, 430, 431, 432, 433, 434, 435, 439, -1 };
int L4UpList[] = { 82, 83, 90, -1 };
int L4DownList[] = { 120, 130, 131, 132, 133, -1 };
int L4TWarpUpList[] = { 421, 422, 429, -1 };
int L4PentaList[] = { 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, -1 };

// JKE new warp list for L5 CRYPT
int L5TWarpUpList[] = {172,173,174,175,176,177,178,179,184,-1};
//int L5UpList[] = {159,-1};
int L5UpList[] = {149,150,151,152,153,154,155,157,158,159,-1};
//int L5DownList[] = {125,126,129,130,131,132,135,136,140,142,-1};
int L5DownList[] = {125, 126, 129, 131, 132, 135, 136, 140, 142,-1};

// JKE new warp list for L6
int L6TWarpUpList[] = {79,80,81,82,83,84,85,86,87,88,89,90,91,92,-1};
int L6UpList[] = {65,66,67,68,69,70,71,72,73,74,75,76,77,78,-1};
int L6DownList[] = {57,58,59,60,61,62,63,64,-1};

BOOL trigflag;
BOOL townwarps[3];

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitNoTriggers()
{
	numtrigs = 0;
	trigflag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitTownTriggers()
{
	trigs[0]._tx = 25;
	trigs[0]._ty = 29;
	trigs[0]._tmsg = WM_DIABNEXTLVL;
	numtrigs = 1;
#if IS_VERSION(SHAREWARE)
	for (int i = 0; i < (MAX_PLRS - 1); i++) townwarps[i] = FALSE;
	trigflag = FALSE;
	return;
#endif
	if (gbMaxPlayers == MAX_PLRS) {
		for (int i = 0; i < 3; i++) townwarps[i] = TRUE;
		trigs[1]._tx = 49;
		trigs[1]._ty = 21;
		trigs[1]._tmsg = WM_DIABTOWNWARP;
		trigs[1]._tlvl = 5;
#if CHEATS
		extern BOOL trigdebug;
		if (trigdebug)
			trigs[1]._tlvl = trigdebug;
#endif
		numtrigs++;
		trigs[2]._tx = 17;
		trigs[2]._ty = 69;
		trigs[2]._tmsg = WM_DIABTOWNWARP;
		trigs[2]._tlvl = 9;
		numtrigs++;
		trigs[3]._tx = 41;
		trigs[3]._ty = 80;
		trigs[3]._tmsg = WM_DIABTOWNWARP;
		trigs[3]._tlvl = 13;
		numtrigs++;
		trigs[4]._tx = 36;
		trigs[4]._ty = 24;
		trigs[4]._tmsg = WM_DIABTOWNWARP;
		trigs[4]._tlvl = CRYPTSTART;
		numtrigs++;
		trigs[5]._tx = 80;
		trigs[5]._ty = 62;
		trigs[5]._tmsg = WM_DIABTOWNWARP;
		trigs[5]._tlvl = HIVESTART;
		numtrigs++;
	} else {
		for (int i = 0; i < (MAX_PLRS - 1); i++) townwarps[i] = FALSE;
		if (plr[myplr].pTownWarps & 0x01 || plr[myplr]._pLevel >= 10) {
			trigs[numtrigs]._tx = 49;
			trigs[numtrigs]._ty = 21;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 5;			
			numtrigs++;
			townwarps[0] = TRUE;
		}
		if (plr[myplr].pTownWarps & 0x02 || plr[myplr]._pLevel >= 15) {
			trigs[numtrigs]._tx = 17;
			trigs[numtrigs]._ty = 69;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 9;
			numtrigs++;
			townwarps[1] = TRUE;
		}
		if (plr[myplr].pTownWarps & 0x04 || plr[myplr]._pLevel >= 20) {
			trigs[numtrigs]._tx = 41;
			trigs[numtrigs]._ty = 80;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 13;
			numtrigs++;
			townwarps[2] = TRUE;
		}
// This will all have to be ifed with whatever we want to trigger with.
		// for now I've assumed it is last and always active. JKE

		if (quests[Q_CRYPTMAP]._qactive == QUEST_DONE)
		{
			trigs[numtrigs]._tx = 36;
			trigs[numtrigs]._ty = 24;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = CRYPTSTART;
			++numtrigs;
		}

		trigs[numtrigs]._tx = 80;
		trigs[numtrigs]._ty = 62;
		trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
		trigs[numtrigs]._tlvl = HIVESTART;
		++numtrigs;
	}
	trigflag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitL1Triggers()
{
	int		i,j;

	numtrigs = 0;
	if (currlevel < HIVESTART)
	{
		for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
	//			if ((dPiece[i][j] == 129) && (currlevel == CRYPTSTART)) {		// cheat to add level up for crypt very temporary JKE
	//				trigs[numtrigs]._tx = i;
	//				trigs[numtrigs]._ty = j;
	//				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
	//				trigs[numtrigs]._tlvl = 0;
	//				numtrigs++;
	//			} else {
				if (dPiece[i][j] == 129) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}
	//			}
				if (dPiece[i][j] == 115) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
			}
		}
	}
	else
	{
		for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
				if (dPiece[i][j] == 184) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
					trigs[numtrigs]._tlvl = 0;
					numtrigs++;
				}
				if (dPiece[i][j] == 158) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}

				if (dPiece[i][j] == 126) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
			}
		}
	}

	
	trigflag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitL2Triggers()
{
	int		i,j;

	numtrigs = 0;
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dPiece[i][j] == 267) {
				if ((i != quests[Q_SCHAMB]._qtx) || (j != quests[Q_SCHAMB]._qty)) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}
			}
			if (dPiece[i][j] == 559) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				trigs[numtrigs]._tlvl = 0;
				numtrigs++;
			}
			if (dPiece[i][j] == 271) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = FALSE;
}
                               
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitL3Triggers()
{
	int		i,j;

	if (currlevel < HIVESTART)
	{
		numtrigs = 0;
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++) {
				if (dPiece[i][j] == 171) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}
				if (dPiece[i][j] == 168) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
				if (dPiece[i][j] == 549) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
					numtrigs++;
				}
			}
		}
	}
	else
	{
		numtrigs = 0;
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++) {
				if (dPiece[i][j] == 66) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}
				if (dPiece[i][j] == 63) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
				if (dPiece[i][j] == 80) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
					numtrigs++;
				}
			}
		}
	}

	trigflag = FALSE;
}
                               
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitL4Triggers()
{
	int		i,j;

	numtrigs = 0;
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dPiece[i][j] == 83) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}
			if (dPiece[i][j] == 422) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				trigs[numtrigs]._tlvl = 0;
				numtrigs++;
			}
			if (dPiece[i][j] == 120) { 
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	// this will ensure that the first trigger is not the trigger down to 16
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
	trigflag = FALSE;
}
                               
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitSKingTriggers()
{
	numtrigs = 1;
	trigs[0]._tx = 82;
	trigs[0]._ty = 42;
	trigs[0]._tmsg = WM_DIABRTNLVL;
	trigflag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitSChambTriggers()
{
	numtrigs = 1;
	trigs[0]._tx = 70;
	trigs[0]._ty = 39;
	trigs[0]._tmsg = WM_DIABRTNLVL;
	trigflag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitPWaterTriggers()
{
	numtrigs = 1;
	trigs[0]._tx = 30;
	trigs[0]._ty = 83;
	trigs[0]._tmsg = WM_DIABRTNLVL;
	trigflag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitVPTriggers()
{
	numtrigs = 1;
	trigs[0]._tx = 35;
	trigs[0]._ty = 32;
	trigs[0]._tmsg = WM_DIABRTNLVL;
	trigflag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL ForceTownTrig()
{
	int i;
#ifdef JIM	// debug only
	char mystr[10];
#endif

// HellFire patch JKE
	for (i = 0; TownCryptList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == TownCryptList[i]) {
			strcpy(infostr, "Down to Crypt");
#ifdef JIM	// debug only
				_itoa(dPiece[cursmx][cursmy],mystr,10);
				strcat(infostr,mystr);

#endif		// END
			cursmx = 36;
			cursmy = 24;
			return(TRUE);
		}
	}
	for (i = 0; TownHiveList [i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == TownHiveList[i]) {
			strcpy(infostr, "Down to Hive");
			cursmx = 80;
			cursmy = 62;
			return(TRUE);
		}
	}
// end patch JKE	

	for (i = 0; TownDownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == TownDownList[i]) {
			strcpy(infostr, "Down to dungeon");

			// JIM info bit
#ifdef JIM
				_itoa(dPiece[cursmx][cursmy],mystr,10);
				strcat(infostr,mystr);
#endif
			// END
			cursmx = 25;
			cursmy = 29;
			return(TRUE);
		}
	}
	if (townwarps[0]) {
		for (i = 0; TownWarp1List[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == TownWarp1List[i]) {
				strcpy(infostr, "Down to catacombs");
			// JIM info bit
#ifdef JIM
				_itoa(dPiece[cursmx][cursmy],mystr,10);
				strcat(infostr,mystr);
#endif
			// END
				cursmx = 49;
				cursmy = 21;
				return(TRUE);
			}
		}
	}
	if (townwarps[1]) {
		for (i = 1199; i <= 1220; i++) {
			if (dPiece[cursmx][cursmy] == i) {
				strcpy(infostr, "Down to caves");
			// JIM info bit
#ifdef JIM
				_itoa(dPiece[cursmx][cursmy],mystr,10);
				strcat(infostr,mystr);
#endif
			// END
				cursmx = 17;
				cursmy = 69;
				return(TRUE);
			}
		}
	}
	if (townwarps[2]) {
		for (i = 1240; i <= 1255; i++) {
			if (dPiece[cursmx][cursmy] == i) {
				strcpy(infostr, "Down to hell");
			// JIM info bit
#ifdef JIM
				_itoa(dPiece[cursmx][cursmy],mystr,10);
				strcat(infostr,mystr);
#endif
			// END
				cursmx = 41;
				cursmy = 80;
				return(TRUE);
			}
		}
	}
			// JIM info bit
#ifdef JIM
	sprintf(infostr,"X: %d, Y: %d, dPiece: %d", cursmx, cursmy,dPiece[cursmx][cursmy]);
	return (TRUE);
#endif
			// END
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BOOL ForceL1Trig() {
	int i, j, dx, dy;

	if (currlevel < HIVESTART)
	{
		for (i = 0; L1UpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L1UpList[i]) {
				if (currlevel > 1) sprintf(infostr, "Up to level %i",currlevel - 1);
				else strcpy(infostr, "Up to town");
// Total hack up just cause I aint got art. JKE
//			if (currlevel == 17) {
//				strcpy(infostr, "Warp to town");
//				if (trigs[j]._tmsg == WM_DIABTWARPUP) {
//					cursmx = trigs[j]._tx;
//					cursmy = trigs[j]._ty;
//					return(TRUE);
//				}
//			}
// end of hack			
			
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABPREVLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}

		for (i = 0; L1DownList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L1DownList[i]) {
				sprintf(infostr, "Down to level %i", currlevel + 1);
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}
	}
	else
	{
		for (i = 0; L5UpList[i] != -1; i++) 
		{
			if (dPiece[cursmx][cursmy] == L5UpList[i]) 
			{
				sprintf(infostr, "Up to Crypt level %i", currlevel - 21);
				for (j = 0; j < numtrigs; j++) 
				{
					if (trigs[j]._tmsg == WM_DIABPREVLVL) 
					{
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}
		if (dPiece[cursmx][cursmy] == 317)	// cornerstone
		{
			strcpy (infostr, "Cornerstone of the World");
			return(TRUE);
		}
		for (i = 0; L5DownList[i] != -1; i++) 
		{
			if (dPiece[cursmx][cursmy] == L5DownList[i])
			{
				sprintf(infostr, "Down to Crypt level %i", currlevel - 19);
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}

		if (currlevel == CRYPTSTART) {
			for (i = 0; L5TWarpUpList[i] != -1; i++) {
				if (dPiece[cursmx][cursmy] == L5TWarpUpList[i]) {
					for (j = 0; j < numtrigs; j++) {
						if (trigs[j]._tmsg == WM_DIABTWARPUP) {
							dx = abs(trigs[j]._tx - cursmx);
							dy = abs(trigs[j]._ty - cursmy);
							if ((dx < 4) && (dy < 4)) {
								strcpy(infostr, "Up to town");
								cursmx = trigs[j]._tx;
								cursmy = trigs[j]._ty;
								return(TRUE);
							}
						}
					}
				}
			}
		}
	}
			// JIM info bit
#ifdef JIM
	sprintf(infostr,"X: %d, Y: %d, dPiece: %d", cursmx, cursmy,dPiece[cursmx][cursmy]);
	return (TRUE);
#endif
			// END
	return(FALSE);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL ForceL2Trig()
{
	int i, j, dx, dy;

	for (i = 0; L2UpList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L2UpList[i]) {
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					dx = abs(trigs[j]._tx - cursmx);
					dy = abs(trigs[j]._ty - cursmy);
					if ((dx < 4) && (dy < 4)) {
						sprintf(infostr, "Up to level %i", currlevel - 1);
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}
	}
	for (i = 0; L2DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L2DownList[i]) {
			sprintf(infostr, "Down to level %i", currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return(TRUE);
				}
			}
		}
	}
	if (currlevel == 5) {
		for (i = 0; L2TWarpUpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L2TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						dx = abs(trigs[j]._tx - cursmx);
						dy = abs(trigs[j]._ty - cursmy);
						if ((dx < 4) && (dy < 4)) {
							strcpy(infostr, "Up to town");
							cursmx = trigs[j]._tx;
							cursmy = trigs[j]._ty;
							return(TRUE);
						}
					}
				}
			}
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL ForceL3Trig()
{
	int i, j, dx, dy;

// JKE level 6 stuff
	if (currlevel < HIVESTART)
	{
		for (i = 0; L3UpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L3UpList[i]) {
				sprintf(infostr, "Up to level %i", currlevel - 1);
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABPREVLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}
		for (i = 0; L3DownList[i] != -1; i++) {
			if ((dPiece[cursmx][cursmy] == L3DownList[i]) ||
				(dPiece[cursmx+1][cursmy] == L3DownList[i]) ||
				(dPiece[cursmx+2][cursmy] == L3DownList[i])) {
				sprintf(infostr, "Down to level %i", currlevel + 1);
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}
	}
	else
	{
		for (i = 0; L6UpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L6UpList[i]) {
				sprintf(infostr, "Up to Nest level %i", currlevel - 17);
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABPREVLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}
		for (i = 0; L6DownList[i] != -1; i++) {
			if ((dPiece[cursmx][cursmy] == L6DownList[i]) ||
				(dPiece[cursmx+1][cursmy] == L6DownList[i]) ||
				(dPiece[cursmx+2][cursmy] == L6DownList[i])) {
				sprintf(infostr, "Down to level %i", currlevel - 15);
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}
	}

	if (currlevel == 9) {
		for (i = 0; L3TWarpUpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L3TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						dx = abs(trigs[j]._tx - cursmx);
						dy = abs(trigs[j]._ty - cursmy);
						if ((dx < 4) && (dy < 4)) {
							strcpy(infostr, "Up to town");
							cursmx = trigs[j]._tx;
							cursmy = trigs[j]._ty;
							return(TRUE);
						}
					}
				}
			}
		}
	}
	if (currlevel == HIVESTART) {
		for (i = 0; L6TWarpUpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L6TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						dx = abs(trigs[j]._tx - cursmx);
						dy = abs(trigs[j]._ty - cursmy);
						if ((dx < 4) && (dy < 4)) {
							strcpy(infostr, "Up to town");
							cursmx = trigs[j]._tx;
							cursmy = trigs[j]._ty;
							return(TRUE);
						}
					}
				}
			}
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL ForceL4Trig()
{
	int i, j, dx, dy;

	for (i = 0; L4UpList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L4UpList[i]) {
			sprintf(infostr, "Up to level %i", currlevel - 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return(TRUE);
				}
			}
		}
	}
	for (i = 0; L4DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L4DownList[i]) {
			sprintf(infostr, "Down to level %i", currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return(TRUE);
				}
			}
		}
	}
	if (currlevel == 13) {
		for (i = 0; L4TWarpUpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L4TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						dx = abs(trigs[j]._tx - cursmx);
						dy = abs(trigs[j]._ty - cursmy);
						if ((dx < 4) && (dy < 4)) {
							strcpy(infostr, "Up to town");
							cursmx = trigs[j]._tx;
							cursmy = trigs[j]._ty;
							return(TRUE);
						}
					}
				}
			}
		}
	}
	if (currlevel == 15) {
		for (i = 0; L4PentaList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L4PentaList[i]) {
				strcpy(infostr, "Down to Diablo");
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return(TRUE);
					}
				}
			}
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void Freeupstairs()
{
	int j, tx, ty, xx, yy;
	
	for (j = 0; j < numtrigs; j++) {
		tx = trigs[j]._tx;
		ty = trigs[j]._ty;
		for (yy = -2; yy <= 2; yy++) {
			for (xx = -2; xx <= 2; xx++)
				dFlags[tx+xx][ty+yy] |= BFLAG_SETPC;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL ForceSKingTrig()
{
	int i;

	for (i = 0; L1UpList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L1UpList[i]) {
			sprintf(infostr, "Back to Level %i", quests[Q_SKELKING]._qlevel);
			cursmx = trigs[0]._tx;
			cursmy = trigs[0]._ty;
			return(TRUE);
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL ForceSChambTrig()
{
	int i;

	for (i = 0; L2DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L2DownList[i]) {
			sprintf(infostr, "Back to Level %i", quests[Q_SCHAMB]._qlevel);
			cursmx = trigs[0]._tx;
			cursmy = trigs[0]._ty;
			return(TRUE);
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL ForcePWaterTrig()
{
	int i;

	for (i = 0; L3DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L3DownList[i]) {
			sprintf(infostr, "Back to Level %i", quests[Q_PWATER]._qlevel);
			cursmx = trigs[0]._tx;
			cursmy = trigs[0]._ty;
			return(TRUE);
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CheckTrigForce()
{
	trigflag = FALSE;
	if (MouseY > 351) return;
	if (!setlevel) {
		switch (leveltype) {
			case 0:
				trigflag = ForceTownTrig();
				break;
			case 1:
				trigflag = ForceL1Trig();
				break;
			case 2:
				trigflag = ForceL2Trig();
				break;
			case 3:
				trigflag = ForceL3Trig();
				break;
			case 4:
				trigflag = ForceL4Trig();
				break;
		}
		if (leveltype && !trigflag)
			trigflag = ForceQuests();
	} else {
		switch (setlvlnum) {
			case SL_SKELKING:
				trigflag = ForceSKingTrig();
				break;
			case SL_BONECHAMB:
				trigflag = ForceSChambTrig();
				break;
			case SL_POISONWATER:
				trigflag = ForcePWaterTrig();
				break;
/*			case SL_VILEBETRAYER:
				trigflag = ForceVBetTrig();
				break;*/
		}
	}
	if (trigflag) ClearPanel();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void CheckTriggers() {

	// player can only work triggers in stand mode
	if (plr[myplr]._pmode != PM_STAND)
		return;

	for (int i = 0; i < numtrigs; i++) {
		if (plr[myplr]._px != trigs[i]._tx) continue;
		if (plr[myplr]._py != trigs[i]._ty) continue;

		switch (trigs[i]._tmsg) {
			case WM_DIABNEXTLVL:
				#if IS_VERSION(SHAREWARE)
				if (currlevel >= 2) {
					NetSendCmdLoc(TRUE,CMD_WALKXY,plr[myplr]._px,plr[myplr]._py+1);
					PlaySFX(PS_WARR18);	// "no way"
					InitDiabloMsg(MSG_SHAREWARE);
					break;
				}
				#endif
				//Does player have item in hand? If so, drop it before going down level.
				if (curs >= ICSTART) {
					if(DropItemBeforeTrig()) return;
				}
				StartNewLvl(myplr,trigs[i]._tmsg,currlevel + 1);
			break;

			case WM_DIABPREVLVL:
				//Does player have item in hand? If so, drop it before going up level.
				if (curs >= ICSTART) {
					if(DropItemBeforeTrig()) return;
				}
				StartNewLvl(myplr,trigs[i]._tmsg,currlevel - 1);
			break;

			case WM_DIABTOWNWARP:
				if (gbMaxPlayers != 1) {
					BOOL abortflag = FALSE;
					int dx,dy;
					char m;
					if ((trigs[i]._tlvl == 5) && (plr[myplr]._pLevel < 8)) {
						abortflag = TRUE;
						dx = plr[myplr]._px;
						dy = plr[myplr]._py+1;
						m = MSG_TRIG1;
					}
					if ((trigs[i]._tlvl == 9) && (plr[myplr]._pLevel < 13)) {
						abortflag = TRUE;
						dx = plr[myplr]._px+1;
						dy = plr[myplr]._py;
						m = MSG_TRIG2;
					}
					if ((trigs[i]._tlvl == 13) && (plr[myplr]._pLevel < 17)) {
						abortflag = TRUE;
						dx = plr[myplr]._px;
						dy = plr[myplr]._py+1;
						m = MSG_TRIG3;
					}
					if (abortflag) {
						if (plr[myplr]._pClass == CLASS_WARRIOR) PlaySFX(PS_WARR43);
						#if !IS_VERSION(SHAREWARE)
						else if (plr[myplr]._pClass == CLASS_ROGUE) PlaySFX(PS_ROGUE43);
						else if (plr[myplr]._pClass == CLASS_SORCEROR) PlaySFX(PS_MAGE43);
						else if (plr[myplr]._pClass == CLASS_MONK) PlaySFX(PS_MONK43);
						else if (plr[myplr]._pClass == CLASS_BARD) PlaySFX(PS_BARD43);
						else if (plr[myplr]._pClass == CLASS_BARBARIAN) PlaySFX(PS_BARBARIAN43);
						#endif
						InitDiabloMsg(m);
						NetSendCmdLoc(TRUE,CMD_WALKXY,dx,dy);
						return;
					}
				}
				StartNewLvl(myplr,trigs[i]._tmsg,trigs[i]._tlvl);
			break;

			case WM_DIABTWARPUP:
				TWarpFrom = currlevel;
				StartNewLvl(myplr,trigs[i]._tmsg,0);
			break;

			case WM_DIABRTNLVL:
				// doesn't work for multiplayer
				app_assert(gbMaxPlayers == 1);
				StartNewLvl(myplr,trigs[i]._tmsg,ReturnLvl);
			break;

			default:
				app_fatal("Unknown trigger msg");
			break;
		}
	}

}
 