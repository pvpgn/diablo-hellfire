/*-----------------------------------------------------------------------**
** 								Diablo
**
**							Map of Doom file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DOOM.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"doom.h"
#include	"engine.h"
#include	"control.h"
#include	"gendung.h"

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

#define DOOM_NUMFRAMES	30
#define DOOM_ENDFRAME	31
#define DOOM_TICK		1200	// Every 20*60 (1 min) frames is next doom map cycle
#define DOOM_TOTAL		(DOOM_NUMFRAMES*DOOM_TICK)+1
#define DOOM_ANIMSPD	5

bool drawmapofdoom = false;
static BYTE *pDoomCel = 0;
int doomtime = 0;
static int currdoom = 0;
static int animdoomdelay = 0;

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitMapOfDoomTime()
{
	if (doomtime > 0) return;
	doomtime = 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DoMapOfDoomTime()
{
	if (doomtime < DOOM_TOTAL) {
		doomtime++;
		if (doomtime == DOOM_TOTAL) {
			#if !IS_VERSION(SHAREWARE)
			PlayInGameMovie("gendata\\doom.smk");
			#endif
			doomtime++;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

int CheckMapOfDoomTime()
{
	if (doomtime == DOOM_TOTAL) return(DOOM_ENDFRAME);
	return(doomtime / DOOM_TICK);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void FreeDoomMem()
{
	if (pDoomCel != NULL)
	{
		DiabloFreePtr(pDoomCel);
		pDoomCel = NULL;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static bool GetDoomMem()
{
	FreeDoomMem();
	pDoomCel = DiabloAllocPtrSig((227 + 1) * 1024,'DOOM');
	return (pDoomCel != 0);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

static bool LoadDoomFrame()
{
	bool result = false;
	
//	if (currlevel < HIVESTART)
//	{
//		if (currdoom == DOOM_ENDFRAME) {
//			strcpy(tempstr, "Items\\Map\\MapZDoom.CEL");
//		}
//		else if (currdoom < 10) {
//			sprintf (tempstr, "Items\\Map\\MapZ000%i.CEL", currdoom);
//		}
//		else sprintf (tempstr, "Items\\Map\\MapZ00%i.CEL", currdoom);
//		LoadFileWithMem(tempstr, pDoomCel);
//	}
//	else
	{
		strcpy(tempstr, "Items\\Map\\MapZtown.CEL");
		if (LoadFileWithMem(tempstr, pDoomCel)) {
			result = true;
		}
	}
	return result;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitMapOfDoomView()
{
	if (GetDoomMem()) {
		if (CheckMapOfDoomTime() == DOOM_ENDFRAME) currdoom = DOOM_ENDFRAME;
		else currdoom = 0;
		if (LoadDoomFrame())
			drawmapofdoom = true;
		else
			EndMapOfDoomView();
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void EndMapOfDoomView()
{
	//if (!drawmapofdoom) return;
	drawmapofdoom = false;
	FreeDoomMem();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawMapOfDoom()
{
	if (!drawmapofdoom) return;

//	if (currdoom != DOOM_ENDFRAME) {
//		animdoomdelay++;
//		if (animdoomdelay >= DOOM_ANIMSPD) {
//			animdoomdelay = 0;
//			currdoom++;
//			if (currdoom > CheckMapOfDoomTime()) currdoom = 0;
//			LoadDoomFrame();
//		}
//	}
	DrawCel(64, 511, pDoomCel, 1, 640);
}
