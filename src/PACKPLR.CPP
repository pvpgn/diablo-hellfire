/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/PACKPLR.CPP 3     2/13/97 5:00p Dbrevik2 $
**-----------------------------------------------------------------------*/

#include "diablo.h"
#pragma hdrstop
#include	"storm.h"
#include	"msg.h"
#include	"multi.h"
#include	"gendung.h"
#include	"items.h"
#include	"player.h"
#include	"spells.h"
#include	"packplr.h"
#include	"stores.h"
#include	"engine.h"

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define INVALID_ITEM	0xffff


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void PackItem(PkItemStruct *id, const ItemStruct *is) {
	if (is->_itype == -1) {
		id->idx = INVALID_ITEM;
	}
	else {
		id->idx = is->IDidx;
		if (is->IDidx == IDI_EAR) {
			id->iCreateInfo = (is->_iName[7] << 8) | is->_iName[8];
			id->iSeed = (is->_iName[9] << 24) |
							(is->_iName[10] << 16) |
							(is->_iName[11] << 8) |
							is->_iName[12];
			id->bId = is->_iName[13];
			id->bDur = is->_iName[14];
			id->bMDur = is->_iName[15];
			id->bCh = is->_iName[16];
			id->bMCh = is->_iName[17];
			id->wValue = (is->_iName[18] << 8) | ((is->_iCurs - ITEM_EAR1) << 6) | is->_ivalue;
			id->dwBuff = (is->_iName[19] << 24) |
							(is->_iName[20] << 16) |
							(is->_iName[21] << 8) |
							is->_iName[22];
		} else {
			id->iSeed = is->_iSeed;
			id->iCreateInfo = is->_iCreateInfo;
			id->bId = is->_iIdentified + (is->_iMagical << 1);
			id->bDur = is->_iDurability;
			id->bMDur = is->_iMaxDur;
			id->bCh = is->_iCharges;
			id->bMCh = is->_iMaxCharges;
			if (is->IDidx == IDI_GOLD) id->wValue = is->_ivalue;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void PackPlayer(PkPlayerStruct * pPack, int pnum) {
	ZeroMemory(pPack,sizeof PkPlayerStruct);

	int i;
	PkItemStruct * pki;
	const ItemStruct * pi;
	const PlayerStruct * pPlayer = &plr[pnum];

	pPack->destAction = pPlayer->destAction;
	pPack->destParam1 = pPlayer->destParam1;
	pPack->destParam2 = pPlayer->destParam2;
	pPack->plrlevel = pPlayer->plrlevel;
	pPack->px = pPlayer->_px;
	pPack->py = pPlayer->_py;
	pPack->targx = pPlayer->_ptargx;
	pPack->targy = pPlayer->_ptargy;

	strcpy(pPack->pName,pPlayer->_pName);
	pPack->pClass = pPlayer->_pClass;

	pPack->pBaseStr = pPlayer->_pBaseStr;
	pPack->pBaseMag = pPlayer->_pBaseMag;
	pPack->pBaseDex = pPlayer->_pBaseDex;
	pPack->pBaseVit = pPlayer->_pBaseVit;
	pPack->pLevel = pPlayer->_pLevel;
	pPack->pStatPts = pPlayer->_pStatPts;

	pPack->pExperience = pPlayer->_pExperience;
	pPack->pGold = pPlayer->_pGold;

	pPack->pHPBase = pPlayer->_pHPBase;
	pPack->pMaxHPBase = pPlayer->_pMaxHPBase;
	pPack->pManaBase = pPlayer->_pManaBase;
	pPack->pMaxManaBase = pPlayer->_pMaxManaBase;

	pPack->pMemSpells = pPlayer->_pMemSpells;

//  hack hack hack; will have to fix later --donald
	for (i = 0; i <= SPL_BONESPIRIT; i++)
		pPack->pSplLvl[i] = pPlayer->_pSplLvl[i];
	unsigned char *p = (unsigned char *) ((void *) &(pPack->wReserved3));
	for (i = SPL_MANA; i < SPL_RUNEOFFIRE; i++)	
	{
		p[i - SPL_MANA] = pPlayer->_pSplLvl[i];
	}

	pki = &pPack->InvBody[0];
	pi = &pPlayer->InvBody[0];
	for (i = NUM_INVLOC; i--; pki++,pi++)
		PackItem(pki,pi);

	pki = &pPack->InvList[0];
	pi = &pPlayer->InvList[0];
	for (i = MAXINV; i--; pki++,pi++)
		PackItem(pki,pi);
	for (i = 0; i < MAXINV; i++)
		pPack->InvGrid[i] = pPlayer->InvGrid[i];
	pPack->_pNumInv = pPlayer->_pNumInv;

	pki = &pPack->SpdList[0];
	pi = &pPlayer->SpdList[0];
	for (i = MAXSPD; i--; pki++,pi++)
		PackItem(pki,pi);

	pPack->_pReflectCount = pPlayer->_pReflectCount;
	pPack->pDiabloKillLevel = pPlayer->pDiabloKillLevel;
	pPack->_gnDifficulty = pPlayer->_gnDifficulty;
	pPack->_pIFlags2 = pPlayer->_pIFlags2;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void UnPackItem(const PkItemStruct *is, ItemStruct *id) {
	if (is->idx == INVALID_ITEM) {
		id->_itype = -1;
	}
	else {
		if (is->idx == IDI_EAR) {
			RecreateEar(TEMPAVAIL,
				is->iCreateInfo,
				is->iSeed,
				is->bId,
				is->bDur,
				is->bMDur,
				is->bCh,
				is->bMCh,
				is->wValue,
				is->dwBuff);
		} else {
			RecreateItem(TEMPAVAIL, is->idx, is->iCreateInfo, is->iSeed, is->wValue);
			item[TEMPAVAIL]._iMagical = is->bId >> 1;
			item[TEMPAVAIL]._iIdentified = is->bId & 1;
			item[TEMPAVAIL]._iDurability = is->bDur;
			item[TEMPAVAIL]._iMaxDur = is->bMDur;
			item[TEMPAVAIL]._iCharges = is->bCh;
			item[TEMPAVAIL]._iMaxCharges = is->bMCh;
		}
		*id = item[TEMPAVAIL];
	}
}

// drb.patch1.start.2/13/97
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void VerifyGoldSeeds(PlayerStruct * pPlayer)
{
	int i, j;

	for (i = 0; i < pPlayer->_pNumInv; i++) {
		if (pPlayer->InvList[i].IDidx == IDI_GOLD) {
			for (j = 0; j < pPlayer->_pNumInv; j++) {
				if ((i != j) && (pPlayer->InvList[j].IDidx == IDI_GOLD) &&
					(pPlayer->InvList[i]._iSeed == pPlayer->InvList[j]._iSeed)) {

					pPlayer->InvList[i]._iSeed = GetRndSeed();
					j = -1;
				}
			}
		}
	}
}
// drb.patch1.end.2/13/97

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void UnPackPlayer(const PkPlayerStruct *pPack, int pnum, BOOL killok) {
	ItemStruct * pi;
	const PkItemStruct * pki;
	PlayerStruct * pPlayer = &plr[pnum];

	void PlrInitReserved(PlayerStruct * p);
	PlrInitReserved(pPlayer);

	// position player
	pPlayer->_px = pPack->px;
	pPlayer->_py = pPack->py;
	pPlayer->_pfutx = pPack->px;
	pPlayer->_pfuty = pPack->py;
	pPlayer->_ptargx = pPack->targx;
	pPlayer->_ptargy = pPack->targy;
	pPlayer->plrlevel = pPack->plrlevel;

	// reset command to NOTHING
	ClrPlrPath(pnum);
	pPlayer->destAction = PCMD_NOTHING;

	strcpy(pPlayer->_pName,pPack->pName);
	pPlayer->_pClass = pPack->pClass;

	InitPlayer(pnum,TRUE);

	pPlayer->_pBaseStr = pPack->pBaseStr;
	pPlayer->_pStrength = pPack->pBaseStr;
	pPlayer->_pBaseMag = pPack->pBaseMag;
	pPlayer->_pMagic = pPack->pBaseMag;
	pPlayer->_pBaseDex = pPack->pBaseDex;
	pPlayer->_pDexterity = pPack->pBaseDex;
	pPlayer->_pBaseVit = pPack->pBaseVit;
	pPlayer->_pVitality = pPack->pBaseVit;
	pPlayer->_pLevel = pPack->pLevel;
	pPlayer->_pStatPts = pPack->pStatPts;

	pPlayer->_pExperience = pPack->pExperience;
	pPlayer->_pGold = pPack->pGold;

	pPlayer->_pMaxHPBase = pPack->pMaxHPBase;
	pPlayer->_pHPBase = pPack->pHPBase;
	if (!killok && ((pPlayer->_pHPBase >> HP_SHIFT) < 1)) pPlayer->_pHPBase = 1 << HP_SHIFT;
	pPlayer->_pMaxManaBase = pPack->pMaxManaBase;
	pPlayer->_pManaBase = pPack->pManaBase;

	pPlayer->_pMemSpells = pPack->pMemSpells;

// hack hack hack -- donald

	for (int i = 0; i <= SPL_BONESPIRIT; i++)
		pPlayer->_pSplLvl[i] = pPack->pSplLvl[i];
	
	unsigned char *p = (unsigned char *) ((void *)&(pPack->wReserved3));
	for (i = SPL_MANA; i < SPL_RUNEOFFIRE; i++)	
	{
		pPlayer->_pSplLvl[i] = p[i - SPL_MANA];
	}

// end hack -- donald

	pki = &pPack->InvBody[0];
	pi = &pPlayer->InvBody[0];
	for (i = NUM_INVLOC; i--; pki++,pi++)
		UnPackItem(pki,pi);

	pki = &pPack->InvList[0];
	pi = &pPlayer->InvList[0];
	for (i = MAXINV; i--; pki++,pi++)
		UnPackItem(pki,pi);
	for (i = 0; i < MAXINV; i++)
		pPlayer->InvGrid[i] = pPack->InvGrid[i];
	pPlayer->_pNumInv = pPack->_pNumInv;
	// drb.patch1.start.2/13/97
	VerifyGoldSeeds(pPlayer);
	// drb.patch1.end.2/13/97

	pki = &pPack->SpdList[0];
	pi = &pPlayer->SpdList[0];
	for (i = MAXSPD; i--; pki++,pi++)
		UnPackItem(pki,pi);

	// this is a fix below for witch store items - rjs
	if (pnum == myplr)
		for (i = 0; i < MAXWITCHITEMS; i++) witchitem[i]._itype = -1;

	CalcPlrInv(pnum, FALSE);

	pPlayer->_pReflectCount = pPack->_pReflectCount;
	pPlayer->pTownWarps = 0;
	pPlayer->pDungMsgs = 0;
	pPlayer->pHellfireMsgs = 0;
	pPlayer->pLvlLoad = 0;
	pPlayer->pDiabloKillLevel = pPack->pDiabloKillLevel;
	pPlayer->_gnDifficulty = pPack->_gnDifficulty;
	pPlayer->_pIFlags2 = pPack->_pIFlags2;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void PackClearCheater(PkPlayerStruct *pPack) {
	pPack->pGold = 0;
	FillMemory(pPack->InvBody,sizeof pPack->InvBody,0xff);
	FillMemory(pPack->InvList,sizeof pPack->InvList,0xff);
	FillMemory(pPack->InvGrid,sizeof pPack->InvGrid,0);
	pPack->_pNumInv = 0;
	FillMemory(pPack->SpdList,sizeof pPack->SpdList,0xff);
}
