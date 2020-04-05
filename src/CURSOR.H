/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/CURSOR.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define NO_CURSOR	0
#define VIEW_CURSOR	1

#define GLOVE_CURS		1
#define IDENTIFY_CURS	2
#define REPAIR_CURS		3
#define RECHARGE_CURS	4
#define DISARM_CURS		5
#define OIL_CURS		6
#define TELE_CURS		7
#define RESURRECT_CURS	8
#define TARGET_CURS		9
#define HEALOTHER_CURS	10
#define WATCH_CURS		11
#define ICSTART			12		// Cursor where items start at
#define ICLAST			179		// last of the original cursors

#define MSEL_NONE	0		// No selection (not used)
#define MSEL_FLR	1		// Floor / single square
#define MSEL_MID	2		// Square above monster base
#define MSEL_REG	3		// Normal floor + 1 square above
#define MSEL_TOP	4		// Square 2 above monster base
#define MSEL_FLY	6		// Square 1 and 2 above base (gargolye, bat)
#define MSEL_BIG	7		// Large monsters floor, 1, and 2 squares


/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/
extern int curs;
extern int cursW, cursH;
extern int icursW, icursH;
extern int icursW28, icursH28;
extern int cursmx, cursmy;
extern int cursmonst;
extern char cursobj;
extern char cursitem;
extern char cursinvitem;
extern char cursplr;
extern BYTE *pCursCels;
extern BYTE *pCursCels2;
extern const int CursorWidth[];
extern const int CursorHeight[];


/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
void InitCursor();
void InitLevelCursor();
void CheckCursMove();
void SetICursor(int i);
void SetCursor(int i);
void NewCursor(int i);
