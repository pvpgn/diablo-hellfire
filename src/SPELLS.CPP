/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Spells file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/SPELLS.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"engine.h"
#include	"sound.h"
#include	"spells.h"
#include	"gendung.h"
#include	"items.h"
#include	"player.h"
#include	"monster.h"
#include	"cursor.h"
#include	"missiles.h"
#include	"control.h"
#include	"inv.h"
#include	"spelldat.h"
#include	"gamemenu.h"

extern void StartStand(int, int);

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int GetManaAmount(int id, int sn)
{
	int i;
	int sl, ma, adj;

	// get total mana adjustments
	adj = 0;
	sl = plr[id]._pSplLvl[sn] + plr[id]._pISplLvlAdd - 1;
	if (sl < 0 ) sl = 0;
	for (i = sl; i > 0; i--) adj += spelldata[sn].sManaAdj;

	// special cases for adjustments
	//if (sn == SPL_CBOLT) adj = -adj;
	if (sn == SPL_FIREBOLT) adj = adj >> 1;
	if (sn == SPL_RESURRECT && sl > 0) {
		adj = 0;
		for (i = sl; i > 0; i--) adj += spelldata[sn].sManaCost >> 3;
	}

	// calc mana cost with adj
	if (spelldata[sn].sManaCost == MA_MAX)
		ma = (((BYTE)plr[id]._pMaxManaBase) - adj) << MANA_SHIFT;
	else
		ma = (spelldata[sn].sManaCost - adj) << MANA_SHIFT;

	// special cases for mana cost
	if (sn == SPL_HEAL) ma = ((plr[id]._pLevel << 1) + spelldata[SPL_HEAL].sManaCost - adj) << MANA_SHIFT;
	if (sn == SPL_HEALOTHER) ma = ((plr[id]._pLevel << 1) + spelldata[SPL_HEAL].sManaCost - adj) << MANA_SHIFT;
	if (sn == SPL_RESURRECT) {
		adj = 0;
		for (i = sl; i > 0; i--) adj += spelldata[sn].sManaCost;
	}

	// calc mana cost with class adj
	if (plr[id]._pClass == CLASS_SORCEROR)
	{ 
		ma >>= 1;
	}
	else if (plr[id]._pClass == CLASS_ROGUE ||
		     plr[id]._pClass == CLASS_MONK  ||
		     plr[id]._pClass == CLASS_BARD)
    { 
	    ma -= (ma >> 2);
    }
	
	// total final mana amount
	if (spelldata[sn].sMinMana > (ma >> MANA_SHIFT)) ma = spelldata[sn].sMinMana << MANA_SHIFT;
	//if (ma <= 0) ma = 1 << MANA_SHIFT;
	ma = (ma * (100 - plr[id]._pISplCost)) / 100;

	return(ma);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void UseMana(int id, int sn)
{
	int ma;

	if (id != myplr) return;
	// calc mana amounts seperated by the way spell was cast
	switch (plr[id]._pSplType) {
		case SPT_ABILITY:
		case SPT_NONE:
			break;
		
		case SPT_SCROLL:
			RemoveScroll(id);
			break;
		
		case SPT_ITEM:
			UseStaffCharge(id);
			break;
		
		case SPT_MEMORIZED:
#if CHEATS
			if (!cheatflag) {
				ma = GetManaAmount(id, sn);
				plr[id]._pMana -= ma;
				plr[id]._pManaBase -= ma;
				drawmanaflag = TRUE;
			}
#else
			ma = GetManaAmount(id, sn);
			plr[id]._pMana -= ma;
			plr[id]._pManaBase -= ma;
			drawmanaflag = TRUE;
#endif
			break;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL CheckSpell(int id, int sn, char st, BOOL manaonly)
{
	int ma;

	// various checks to see if a player can cast a spell (assumes they
	// already have casting available by some means, staff, scroll, etc.)
#if CHEATS
	if (cheatflag) return(TRUE);
#endif
	if ((!manaonly) && (curs != GLOVE_CURS)) return(FALSE);
	if (st == SPT_ABILITY) return(TRUE);
	
	if ((GetSpellLevel(id, sn)) <= 0) return(FALSE);
	
	ma = GetManaAmount(id, sn);
	if (plr[id]._pMana < ma) return(FALSE);
	
	return(TRUE);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int caster, int spllvl)
{
	int i;
	int dir;

	switch (caster) {
		case MI_PLR:
			dir = plr[id]._pdir;
			caster = MI_ENEMYMONST; // delete later
			if (spl == SPL_WALL || spl == SPL_LTWALL) dir = plr[id]._pVar3;
			break;
		case MI_MONST:
			dir = monster[id]._mdir;
			break;
	}

	for (i = 0; (spelldata[spl].sMissiles[i] != NULL) && (i < 3); i++) 
		AddMissile(sx, sy, dx, dy, dir, spelldata[spl].sMissiles[i], caster, id, 0, spllvl);

	if (spelldata[spl].sMissiles[0] == MIT_TOWN) UseMana(id, SPL_TOWN);
	if (spelldata[spl].sMissiles[0] == MIT_CBOLT) {
		UseMana(id, SPL_CBOLT);
		for (i = (spllvl >> 1) + 3; i > 0; i--) 
			AddMissile(sx, sy, dx, dy, dir, MIT_CBOLT, caster, id, 0, spllvl);
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoResurrect(int pnum, int rid)
{
	if ((char) rid != -1) 
		AddMissile(plr[rid]._px, plr[rid]._py, plr[rid]._px, plr[rid]._py, 0, MIT_RESURRECTBEAM, MI_PLR, pnum, 0, 0);

	if (pnum == myplr) {
		NewCursor(GLOVE_CURS);
	}

	if (((char) rid) != -1) {
		// if I am casting on a live person bail out
		if (plr[rid]._pHitPoints != 0) return;
		if (rid == myplr) {
			deathflag = FALSE;
			gamemenu_off();
			drawhpflag = TRUE;
			drawmanaflag = TRUE;
		}
		ClrPlrPath(rid);
		plr[rid].destAction = PCMD_NOTHING;
		plr[rid]._pInvincible = FALSE;
		SetPlayerHitPoints(rid, 10 << HP_SHIFT);
		plr[rid]._pHPBase = plr[rid]._pHitPoints - (plr[rid]._pMaxHP - plr[rid]._pMaxHPBase);
		plr[rid]._pMana = 0;
		plr[rid]._pManaBase = plr[rid]._pMana - (plr[rid]._pMaxMana - plr[rid]._pMaxManaBase);
		CalcPlrInv(rid, TRUE);
		if (plr[rid].plrlevel == currlevel) StartStand(rid, plr[rid]._pdir);
		else plr[rid]._pmode = PM_STAND;
	}
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void DoHealOther(int pnum, int rid)
{
	int i;
	long l;

	if (pnum == myplr) {
		NewCursor(GLOVE_CURS);
	}

	if (((char) rid) != -1) {
		// if I am casting on a dead person bail out
		if ((plr[rid]._pHitPoints >> HP_SHIFT) <= 0) return;

		l = (random(57, 10) + 1) << HP_SHIFT;
		for (i = 0; i < plr[pnum]._pLevel; i++) l += ((random(57, 4) + 1) << HP_SHIFT);
		for (i = 0; i < GetSpellLevel(pnum, SPL_HEALOTHER); i++) l += ((random(57, 6) + 1) << HP_SHIFT);
		if (plr[pnum]._pClass == CLASS_WARRIOR
			|| plr[pnum]._pClass == CLASS_BARBARIAN)
		{ 
			l = l << 1;
		}
		else if (plr[pnum]._pClass == CLASS_ROGUE ||
				 plr[pnum]._pClass == CLASS_BARD)
		{
			 l += (l >> 1);
		}
		else if (plr[pnum]._pClass == CLASS_MONK)
		{
			 l += (l << 1);
		}
		plr[rid]._pHitPoints += l;
		if (plr[rid]._pHitPoints > plr[rid]._pMaxHP) plr[rid]._pHitPoints = plr[rid]._pMaxHP;
		plr[rid]._pHPBase += l;
		if (plr[rid]._pHPBase > plr[rid]._pMaxHPBase) plr[rid]._pHPBase = plr[rid]._pMaxHPBase;
		drawhpflag = TRUE;
	}
}
