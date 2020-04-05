/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/SCROLLRT.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define NBUFFWSL4	12288
#define NBUFFWSL5	24576

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern "C" {
	extern long nLVal;
	extern DWORD gdwPNum;
	extern BOOL nTrans;
}
extern long nBuffWTbl[1024];
extern bool HighLightAllItems;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

extern "C" void _fastcall DrawBlankMTile(BYTE *);

void DrawAndBlit (void);

void FullBlit(BOOL);
void ClrDraw();

void CheckForScroll();

void DrawMissile(int, int, int, int, int, int, BOOL);
void CDrawMissile(int, int, int, int, int, int, BOOL);
void DrawDeadPlr(int sx, int sy, int xp, int yp, int ostart, int oend, BOOL clip);
