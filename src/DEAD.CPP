/*-----------------------------------------------------------------------**
** 								Diablo
**
**							   Dead file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DEAD.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"dead.h"
#include	"monster.h"
#include	"monstint.h"
#include	"missiles.h"
#include	"misdat.h"
#include	"gendung.h"
#include	"lighting.h"

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

DeadStruct dead[MAXDEAD];
int spurtndx;
int stonendx;

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void InitDead ()
{
	int		nd,i,j,mi;
	int		mtypes[MONSTERTYPES];

	for (i = 0; i < MONSTERTYPES; i++) mtypes[i] = 0;

	nd = 0;
	for (i = 0; i < nummtypes; i++) {
		if (mtypes[Monsters[i].mtype] == 0) {
			for (j = 0; j < 8; j++) dead[nd]._deadData[j] = Monsters[i].Anims[MA_DEATH].Cels[j];
			dead[nd]._deadFrame = Monsters[i].Anims[MA_DEATH].Frames;
			dead[nd]._deadWidth = Monsters[i].mAnimWidth;
			dead[nd]._deadWidth2 = Monsters[i].mAnimWidth2;
			dead[nd]._deadtrans = 0;
			Monsters[i].mdeadval = nd + 1;
			mtypes[Monsters[i].mtype] = nd + 1;
			nd++;
		}
	}

	// set blood burst dead frames
	for (j = 0; j < 8; j++) dead[nd]._deadData[j] = misfiledata[MF_SPURT].mAnimData[0];
	dead[nd]._deadFrame = 8;
	dead[nd]._deadWidth = 128;
	dead[nd]._deadWidth2 = (128 - 64) >> 1;
	dead[nd]._deadtrans = 0;
	spurtndx = nd + 1;
	nd++;
	// set stone dead frames
	for (j = 0; j < 8; j++) dead[nd]._deadData[j] = misfiledata[MF_STONE].mAnimData[0];
	dead[nd]._deadFrame = 12;
	dead[nd]._deadWidth = 128;
	dead[nd]._deadWidth2 = (128 - 64) >> 1;
	dead[nd]._deadtrans = 0;
	stonendx = nd + 1;
	nd++;

	// set unique monster dead frames
	for (i = 0; i < nummonsters; i++) {
		mi = monstactive[i];
		if (monster[mi]._uniqtype != 0) {
			for (j = 0; j < 8; j++) dead[nd]._deadData[j] = monster[mi].MType->Anims[MA_DEATH].Cels[j];
			dead[nd]._deadFrame = monster[mi].MType->Anims[MA_DEATH].Frames;
			dead[nd]._deadWidth = monster[mi].MType->mAnimWidth;
			dead[nd]._deadWidth2 = monster[mi].MType->mAnimWidth2;
			dead[nd]._deadtrans = LIGHT_U + monster[mi]._uniqtrans;
			monster[mi]._udeadval = nd + 1;
			nd++;
		}
	}
	app_assert(nd <= MAXDEAD);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void AddDead(int dx, int dy, char dv, int ddir)
{
	char	tdv;

	tdv = (dv & 0x1f) + ((ddir & 0x7) << 5);
	dDead[dx][dy] = tdv;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void SyncUniqDead()
{
	int i,mi;
	int x,y;

	// See function InitDead() to understand what this is all about

	for (i = 0; i < nummonsters; i++) {
		mi = monstactive[i];
		if (monster[mi]._uniqtype != 0) {
			
			// Search dDead array for this dead type, and place a light source there
			for(x=0; x < DMAXX; x++)
				for(y=0; y < DMAXY; y++)
					if((dDead[x][y] & 0x1f) == monster[mi]._udeadval)
						ChangeLightXY(monster[mi].mlid, x, y);
		}
	}
}
