/*-----------------------------------------------------------------------**
** 								Diablo
**
**							Triggers file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"portal.h"
#include	"gendung.h"
#include	"items.h"
#include	"player.h"
#include	"misdat.h"
#include	"missiles.h"
#include	"effects.h"
#include	"msg.h"
#include	"lighting.h"

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

PortalStruct portal[MAXPORTAL];

int WarpDropX[MAXPORTAL] = { 57, 59, 61, 63 };
int WarpDropY[MAXPORTAL] = { 40, 40, 40, 40 };

int portalindex;		// current portal in use for myplr

/*-----------------------------------------------------------------------**
** externs
**-----------------------------------------------------------------------*/
void SetMissDir(int mi, int dir);
void DeleteMissile(int mi, int i);
BOOL delta_portal_inited(int i);

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitPortals()
{
	for (int i = 0; i < MAXPORTAL; i++) {
		// Check to see if needs initing or parsed from level delta
		if (delta_portal_inited(i))
			portal[i].open = FALSE;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SyncPortal(int i, BOOL open, int x, int y, int level, int ltype)
{
	app_assert((DWORD)i < MAXPORTAL);
	portal[i].open = open;
	portal[i].x = x;
	portal[i].y = y;
	portal[i].level = level;
	portal[i].ltype = ltype;
	portal[i].setlvl = FALSE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
void AddWarpMissile(int i, int x, int y)
{
	// Don't play sound when loading level
	missiledata[MIT_TOWN].mlSFX = -1;

	// Fix because portal cant be drawn on top of each other
	dMissile[x][y] = 0;

	// Put in map and activate
	int mi = AddMissile(0, 0, x, y, 0, MIT_TOWN, MI_ENEMYMONST, i, 0, 0);

	//check if AddMissile unsuccessful
	if (mi == -1)
		return;	//too many missiles already, so quit

	// Force open
	SetMissDir(mi, 1);

	// Give it a lightsource
//DL - 11/9/97
// This is a serious bug.  Under certain situations, missile[i] will be
// uninitialized, causing coordinate (0, 0) to be passed to AddLight().
// When the light is processed later, and the radius is subtracted from
// the (0, 0) coordinate, the light table will be written to with negative
// indicies, causing an array underwrite.  This was causing a crash on the
// Mac because it was overwriting the memory block header, trashing the
// application's heap.  I don't know what is getting overwritten on the PC
// version. An example of when this will happen is if player 1 creates a
// new game, and player 2 joins it.  Both players go to level 1.  Player 2
// casts town portal (with cheats on). On player 1's machine,
// AddWarpMissile( will be called with i = 1 (player 2's player index).
// Since there are no missles yet, AddMissile() will use missle index 0, so
// mi wil be 0. But since i is 1, AddLight() will be passed the coordinates
// of missle[1], which doesn't exist, so its coordinates are undefined.  In
// this case, they are uninitialized, so they are zero.

//	if (currlevel != 0) missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, 15);
	if (currlevel != 0) missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 15);

	// play sound while in dungeon
	missiledata[MIT_TOWN].mlSFX = LS_SENTINEL;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SyncPortals()
{
	// Add the warp fields
	for (int i = 0; i < MAXPORTAL; i++) {
		if (portal[i].open) {
			if (currlevel == 0) {
				AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
			} else {
				if (! setlevel) {
					if (portal[i].level == currlevel) AddWarpMissile(i, portal[i].x, portal[i].y);
				} else {
					if (portal[i].level == setlvlnum) AddWarpMissile(i, portal[i].x, portal[i].y);
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AddInTownPortal(int i)
{
	AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void ActivatePortal(int i, int x, int y, int lvl, int lvltype, BOOL sp)
{
	app_assert((DWORD)i < MAXPORTAL);
	portal[i].open = TRUE;
	if (lvl != 0) {
		portal[i].x = x;
		portal[i].y = y;
		portal[i].level = lvl;
		portal[i].ltype = lvltype;
		portal[i].setlvl = sp;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DeactivatePortal(int i)
{
	app_assert((DWORD)i < MAXPORTAL);
	portal[i].open = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL PortalOnLevel(int i)
{
	app_assert((DWORD)i < MAXPORTAL);
	if (portal[i].level == currlevel) return(TRUE);
	if (currlevel == 0) return(TRUE);
	else return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void RemovePortalMissile(int id)
{
	int i, mi;

	// Remove current portal
	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		if ((missile[mi]._mitype == MIT_TOWN) && (missile[mi]._misource == id)) {
			dFlags[missile[mi]._mix][missile[mi]._miy] &= BFMASK_MISSILE;
			dMissile[missile[mi]._mix][missile[mi]._miy] = 0;
			if (portal[id].level != 0) AddUnLight(missile[mi]._mlid);
			DeleteMissile(mi, i);
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void SetCurrentPortal(int p)
{
	portalindex = p;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void GetPortalLevel() {
	if (currlevel) {
		setlevel = FALSE;
		currlevel = 0;
		plr[myplr].plrlevel = 0;
		leveltype = 0;
		return;
	}
	app_assert((DWORD)portalindex < MAXPORTAL);
	if (portal[portalindex].setlvl) {
		setlevel = TRUE;
		setlvlnum = portal[portalindex].level;
		currlevel = portal[portalindex].level;
		plr[myplr].plrlevel = setlvlnum;
		leveltype = portal[portalindex].ltype;
	}
	else {
		setlevel = FALSE;
		currlevel = portal[portalindex].level;
		plr[myplr].plrlevel = currlevel;
		leveltype = portal[portalindex].ltype;
	}
	if (portalindex == myplr) {
		NetSendCmd(TRUE, CMD_DEACTIVATEPORTAL);
		DeactivatePortal(portalindex);
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void GetPortalLvlPos()
{
	app_assert((DWORD)portalindex < MAXPORTAL);
	if (!currlevel) {
		ViewX = WarpDropX[portalindex] + 1;
		ViewY = WarpDropY[portalindex] + 1;
		return;
	}
	ViewX = portal[portalindex].x;
	ViewY = portal[portalindex].y;
	if (portalindex != myplr) {
		ViewX++;
		ViewY++;
	}
}

