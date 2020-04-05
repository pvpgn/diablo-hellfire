/*-----------------------------------------------------------------------**
** 								Diablo
**
**							  sync.cpp
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/SYNC.CPP 3     2/10/97 6:23p Dbrevik2 $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/


#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"msg.h"
#include	"multi.h"
#include	"gendung.h"
#include	"engine.h"
#include	"items.h"
#include	"player.h"
#include	"error.h"
#include	"debug.h"
#include	"monster.h"
#include	"monstint.h"
#include	"inv.h"

//******************************************************************
// externs
//******************************************************************
void M_ClearSquares(int nMonster);
void delta_sync_monster(const TSyncMonster * p,BYTE bLevel);


//******************************************************************
// private
//******************************************************************
static int sgnMonsters;
static WORD sgwDelta[MAXMONSTERS];
static WORD sgwLRU[MAXMONSTERS];
static int sgnLRUScan;

#define LRU_NEVER	0xffff
#define LRU_REINIT	0xfffe
#define LRU_INIT	0xff

static int sgnSyncItem = 0;
static int sgnSyncObj = 0;
static int sgnSyncPInv = 0;

//******************************************************************
//******************************************************************
static void prep_monster_list(void) {
	for (int i = 0; i < nummonsters; i++) {
		int mi = monstactive[i];
		sgwDelta[mi] =	abs(plr[myplr]._px - monster[mi]._mx) +
						abs(plr[myplr]._py - monster[mi]._my);

		// make all the squelched monsters particularly unattractive
		// make all the unsquelched monsters decrement their LRU
		// counters so that they will get sent eventually
		if (! monster[mi]._msquelch)
			sgwDelta[mi] += 0x1000;
		else if (sgwLRU[mi])
			sgwLRU[mi]--;
	}
}


//******************************************************************
//******************************************************************
static void get_monster(TSyncMonster * p,int mi) {
	// get the true monster number
	// and save monster's parameters
	p->_mndx = mi;
	p->_mx = monster[mi]._mx;
	p->_my = monster[mi]._my;
	// encode player/monster enemy
	p->_menemy = encode_enemy(mi);
	p->_mdelta = sgwDelta[mi] > 0xff ? 0xff : sgwDelta[mi];
	// cant have this since menemy maybe a monster id
	//app_assert((DWORD) p->_menemy < MAX_PLRS);

	// reset delta table so we don't find this monster again
	sgwDelta[mi] = 0xffff;

	// reset LRU count so we don't resend this monster unnecessarily
	sgwLRU[mi] = monster[mi]._msquelch ? LRU_REINIT : LRU_NEVER;
}


//******************************************************************
//******************************************************************
static BOOL get_best_monster(TSyncMonster * p) {
	// search all the monsters for the one closest to the player
	int nFound = -1;
	DWORD dwMin = 0xffffffff;
	for (int i = 0; i < nummonsters; i++) {
		int mi = monstactive[i];

		// is this monster close to the local player?
		if (sgwDelta[mi] >= dwMin) continue;

		// is this a monster which has already been sent LRU?
		if (sgwLRU[mi] >= LRU_REINIT) continue;

		// found better candidate
		dwMin = sgwDelta[mi];
		nFound = mi;
	}
	if (nFound == -1) return FALSE;
	get_monster(p,nFound);
	return TRUE;
}


//******************************************************************
//******************************************************************
static BOOL get_lru_monster(TSyncMonster * p) {
	// search all the monsters for the lowest LRU number
	int nFound = -1;
	DWORD dwMin = LRU_REINIT;
	for (int i = 0; i < nummonsters; i++,sgnLRUScan++) {
		if (sgnLRUScan >= nummonsters) sgnLRUScan = 0;
		int mi = monstactive[sgnLRUScan];

		// is this a monster which hasn't been sent in a while?
		if (sgwLRU[mi] >= dwMin) continue;

		dwMin = sgwLRU[mi];
		nFound = mi;
	}
	if (nFound == -1) return FALSE;
	get_monster(p,nFound);
	return TRUE;
}

//******************************************************************
//******************************************************************
static void FillHeaderSync(TSyncHeader * pHdr)
{
	// Fill one item
	int ii;
	if (numitems > 0) {
		if (sgnSyncItem >= numitems) sgnSyncItem = 0;
		ii = itemactive[sgnSyncItem++];
		pHdr->bItemI = ii;
		pHdr->bItemX = item[ii]._ix;
		pHdr->bItemY = item[ii]._iy;
		pHdr->wItemIndx = item[ii].IDidx;
		if (item[ii].IDidx == IDI_EAR) {
			pHdr->wItemCI = (item[ii]._iName[7] << 8) | item[ii]._iName[8];
			pHdr->dwItemSeed = (item[ii]._iName[9] << 24) |
								(item[ii]._iName[10] << 16) |
								(item[ii]._iName[11] << 8) |
								item[ii]._iName[12];
			pHdr->bItemId = item[ii]._iName[13];
			pHdr->bItemDur = item[ii]._iName[14];
			pHdr->bItemMDur = item[ii]._iName[15];
			pHdr->bItemCh = item[ii]._iName[16];
			pHdr->bItemMCh = item[ii]._iName[17];
			pHdr->wItemVal = (item[ii]._iName[18] << 8) | ((item[ii]._iCurs - ITEM_EAR1) << 6) | item[ii]._ivalue;
			pHdr->dwItemBuff = (item[ii]._iName[19] << 24) |
								(item[ii]._iName[20] << 16) |
								(item[ii]._iName[21] << 8) |
								item[ii]._iName[22];
		} else {
			pHdr->wItemCI = item[ii]._iCreateInfo;
			pHdr->dwItemSeed = item[ii]._iSeed;
			pHdr->bItemId = item[ii]._iIdentified;
			pHdr->bItemDur = item[ii]._iDurability;
			pHdr->bItemMDur = item[ii]._iMaxDur;
			pHdr->bItemCh = item[ii]._iCharges;
			pHdr->bItemMCh = item[ii]._iMaxCharges;
			if (item[ii].IDidx == IDI_GOLD) pHdr->wItemVal = item[ii]._ivalue;
		}
	}
	else {
		pHdr->bItemI = 0xff;
	}

	// Fill with player inv info
	app_assert((DWORD) sgnSyncPInv < NUM_INVLOC);
	ItemStruct *itm = &plr[myplr].InvBody[sgnSyncPInv];
	if (itm->_itype != -1) {
		pHdr->bPInvLoc = sgnSyncPInv;
		pHdr->wPInvIndx = itm->IDidx;
		pHdr->wPInvCI = itm->_iCreateInfo;
		pHdr->dwPInvSeed = itm->_iSeed;
		// drb.patch1.start.02/10/97
		pHdr->bPInvId = itm->_iIdentified;
		// drb.patch1.end.02/10/97
	}
	else {
		pHdr->bPInvLoc = 0xff;
	}

	// next item
	sgnSyncPInv++;
	if (sgnSyncPInv >= NUM_INVLOC)
		sgnSyncPInv = 0;
}


//******************************************************************
//******************************************************************
DWORD sync_get(BYTE * pbBuf,DWORD dwMaxLen) {
	// if there are no monsters to sync, exit
	if (nummonsters < 1) return dwMaxLen;

	// is there enough space for at header + one monster sync
	if (dwMaxLen < sizeof(TSyncMonster) + sizeof(TSyncHeader))
		return dwMaxLen;

	// setup header
	TSyncHeader * pHdr = (TSyncHeader *) pbBuf;
	pbBuf += sizeof(TSyncHeader);
	dwMaxLen -= sizeof(TSyncHeader);
	pHdr->bCmd = CMD_SYNCDATA;
	pHdr->bLevel = currlevel;
	pHdr->wLen = 0;
	// Put one object, one item and one plr inv item into sync packets
	FillHeaderSync(pHdr);

	// make sure we don't overrun header maximum length
	app_assert(dwMaxLen <= 0xffff);

	prep_monster_list();
	for (int nMonsters = 0; nMonsters < nummonsters; nMonsters++) {
		if (dwMaxLen < sizeof(TSyncMonster)) break;

		BOOL bGotOne = FALSE;
		if (nMonsters < 2) bGotOne = get_lru_monster((TSyncMonster *) pbBuf);
		if (! bGotOne) bGotOne = get_best_monster((TSyncMonster *) pbBuf);
		if (! bGotOne) break;

		pbBuf += sizeof(TSyncMonster);
		pHdr->wLen += sizeof(TSyncMonster);
		dwMaxLen -= sizeof(TSyncMonster);
	}

	// return bytes left in buffer
	return dwMaxLen;
}


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*

byte monstupdate[MAXMONSTERS];

void DaveMonstMap(BOOL initupdate, int pnum)
{
	int i,j,m;
	char tempstr[512], tempstr2[256];

	if (initupdate)
		for (i = 0; i < MAXMONSTERS; i++) monstupdate[i] = 0;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dFlags[i][j] & BFLAG_MONSTLR) {
				if (dMonster[i-1][j] != dMonster[i][j-1]) {
					sprintf(tempstr, "Error in map @ %i,%i    x-1,j = %i,    x,j-1 = %i\n", i, j, dMonster[i-1][j], dMonster[i][j-1]);
					if (dMonster[i-1][j] == 0) sprintf(tempstr2, "Null monster\n @ %i,%i", i-1, j);
					else {
						if (dMonster[i-1][j] > 0) m = dMonster[i-1][j] - 1;
						else m = -(dMonster[i-1][j] + 1);
						sprintf(tempstr2, "Monst %i   xy = %i,%i   oxy = %i,%i  mode = %i  Updated = %i\n",
							m, monster[m]._mx, monster[m]._my, monster[m]._moldx, monster[m]._moldy, monster[m]._mmode, monstupdate[m]);
					}
					strcat(tempstr, tempstr2);
					if (dMonster[i][j-1] == 0) sprintf(tempstr2, "Null monster @ %i,%i\n", i, j-1);
					else {
						if (dMonster[i][j-1] > 0) m = dMonster[i][j-1] - 1;
						else m = -(dMonster[i][j-1] + 1);
						sprintf(tempstr2, "Monst %i   xy = %i,%i   oxy = %i,%i  mode = %i  Updated = %i\n",
							m, monster[m]._mx, monster[m]._my, monster[m]._moldx, monster[m]._moldy, monster[m]._mmode, monstupdate[m]);
					}
					strcat(tempstr, tempstr2);
					sprintf(tempstr2, "Info from plr %i = %s", pnum, plr[pnum]._pName);
					strcat(tempstr, tempstr2);
					app_fatal(tempstr);
				}
			}
		}
	}
}
*/
//******************************************************************
//******************************************************************
static void sync_monster(int pnum,const TSyncMonster * p) {
	
	// get monster index
	int ndx = p->_mndx;
	
	// did I just kill this monster?
	if (monster[ndx]._mhitpoints <= 0) return;

	// check for valid index
	for (int i = 0; i < nummonsters && monstactive[i] != ndx; i++);
	//app_assert(i < nummonsters);

	// calc distance I think he is from me
	DWORD delta =
		abs(plr[myplr]._px - monster[ndx]._mx) +
		abs(plr[myplr]._py - monster[ndx]._my);
	if (delta > 0xff) delta = 0xff;

	// if my delta is less than the other player's delta, use mine
	if (delta < p->_mdelta) return;
	// if the deltas are the same use the lowest player num
	else if ((delta == p->_mdelta) && (pnum > myplr)) return;

	// if the monster is going to be there soon, don't do anything
	if (monster[ndx]._mfutx == p->_mx && monster[ndx]._mfuty == p->_my) return;

	// Snake crash fix. Don't mess with missile monsters while they are warping
	if (monster[ndx]._mmode == MM_MISSILE) return;
	// Don't mess with stone cursed monsters
	if (monster[ndx]._mmode == MM_STONE) return;

	// if the monster is far away, jump him into position
	int mdx = abs(monster[ndx]._mx - p->_mx);
	int mdy = abs(monster[ndx]._my - p->_my);
	if (mdx > 2 || mdy > 2) {
		if (! dMonster[p->_mx][p->_my]) {
			// remove where monster is now
			M_ClearSquares(ndx);

			// put him in new location
			dMonster[p->_mx][p->_my] = ndx + 1;
			monster[ndx]._mx = p->_mx;
			monster[ndx]._my = p->_my;
			decode_enemy(ndx, p->_menemy);
			M_StartStand(ndx, GetDirection(p->_mx,p->_my, monster[ndx]._menemyx, monster[ndx]._menemyy));

			// make sure he's awake
			monster[ndx]._msquelch = 255;
		}
	}
	else {
		// if the monster is not already on the way to a nearby
		// square, start him immediately on his way
		if ((monster[ndx]._mmode < MM_WALK) || (monster[ndx]._mmode > MM_WALK3)) {
			int md = GetDirection(monster[ndx]._mx, monster[ndx]._my, p->_mx, p->_my);

			if (DirOK(ndx, md)) {
				// remove where monster is now
				M_ClearSquares(ndx);

				// make monster walk into square where he should be
				dMonster[monster[ndx]._mx][monster[ndx]._my] = ndx + 1;
				M_WalkDir(ndx, md);

				// make sure he's awake
				monster[ndx]._msquelch = 255;
			}
		}
	}

	// sync enemy
	// cant have this since menemy maybe a monster id
	//app_assert((DWORD) p->_menemy < MAX_PLRS);
	decode_enemy(ndx, p->_menemy);
}


//******************************************************************
//******************************************************************
static void UpdateHeaderSync(int pnum, const TSyncHeader * pHdr)
{
	// Fill one item
	if (pHdr->bItemI != 0xff) {
		int ii = pHdr->bItemI;
		if ((pHdr->wItemIndx != item[ii].IDidx) ||
			(pHdr->wItemCI != item[ii]._iCreateInfo) ||
			(pHdr->dwItemSeed != item[ii]._iSeed)) {
			ii = FindGetItem(pHdr->wItemIndx, pHdr->wItemCI, pHdr->dwItemSeed);
			if (ii == -1) {
				SyncPutItem(pnum, 
							pHdr->bItemX, 
							pHdr->bItemY,
							pHdr->wItemIndx, 
							pHdr->wItemCI, 
							pHdr->dwItemSeed,
							pHdr->bItemId, 
							pHdr->bItemDur, 
							pHdr->bItemMDur, 
							pHdr->bItemCh, 
							pHdr->bItemMCh, 
							pHdr->wItemVal, 
							pHdr->dwItemBuff,
							pHdr->wPLToHit,
							pHdr->wMaxDam,
							pHdr->bMinStr,
							pHdr->bMinMag,
							pHdr->bMinDex,
							pHdr->bAC
							);
			}
		}
	}
	// Fill with player inv info
	if (pHdr->bPInvLoc != 0xff) {
		ItemStruct *itm = &plr[myplr].InvBody[pHdr->bPInvLoc];
		if ((pHdr->wPInvIndx != itm->IDidx) ||
			(pHdr->wPInvCI != itm->_iCreateInfo) ||
			// drb.patch1.start.02/10/97
			// (pHdr->dwPInvSeed != itm->_iSeed)) {
			(pHdr->dwPInvSeed != itm->_iSeed) || 
			(pHdr->bPInvId != itm->_iIdentified)) {
			// SyncInvPaste(pnum, pHdr->bPInvLoc, pHdr->wPInvIndx, pHdr->wPInvCI, pHdr->dwPInvSeed,);
			SyncInvPaste(pnum, pHdr->bPInvLoc, pHdr->wPInvIndx, pHdr->wPInvCI, pHdr->dwPInvSeed, pHdr->bPInvId);
			// drb.patch1.end.02/10/97
		}
	}
}


//******************************************************************
//******************************************************************
DWORD sync_update(int pnum,const BYTE * pbBuf) {
	const TSyncHeader * pHdr = (const TSyncHeader *) pbBuf;
	pbBuf += sizeof(TSyncHeader);

	// make sure we have a valid sync record
	if (pHdr->bCmd != CMD_SYNCDATA)
		app_fatal("bad sync command");

	// don't resync while we're in buffer mode
	app_assert(gbBufferMsgs != BUFFER_PROCESS);
	if (gbBufferMsgs == BUFFER_ON)
		return pHdr->wLen + sizeof(TSyncHeader);

	// we don't resync using our own information
	if (pnum == myplr) return pHdr->wLen + sizeof(TSyncHeader);

	// Make sure we are in sync with others on the level
	//if (currlevel == pHdr->bLevel) UpdateHeaderSync(pnum, pHdr);

	WORD wLen = pHdr->wLen;
	while (wLen >= sizeof(TSyncMonster)) {
		if (currlevel == pHdr->bLevel)
			sync_monster(pnum,(const TSyncMonster *) pbBuf);
		delta_sync_monster((const TSyncMonster *) pbBuf,pHdr->bLevel);
		pbBuf += sizeof(TSyncMonster);
		wLen -= sizeof(TSyncMonster);
	}

	// make sure we used all the bytes
	app_assert(wLen == 0);

	// return true number of bytes in our sync section
	return pHdr->wLen + sizeof(TSyncHeader);
}


//******************************************************************
//******************************************************************
void sync_init() {
	// don't let all the players start scanning in the same location.
	// if each player is scanning from a different location, it is
	// more likely that multiple monsters can be synced every turn
	sgnLRUScan = myplr * 16;
	FillMemory(sgwLRU,sizeof sgwLRU,LRU_INIT);
}
