/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/CONTROL.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define	SB1X1	121
#define	SB1X2	176

#define	SB2X1	196
#define	SB2X2	251

#define	SBY1	408
#define	SBY2	463

#define	SBSY1	SBY1-55
#define	SBSY2	SBY2-55

#define	TEXT_LEFT		0
#define	TEXT_CENTER		1
#define TEXT_RIGHT		2

#define	ICOLOR_WHITE	0
#define ICOLOR_BLUE		1
#define ICOLOR_RED		2
#define ICOLOR_GOLD		3

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern BYTE *pBtmBuff;		// Offscreen control panel buffer
extern BYTE *pStatusPanel;

extern BYTE *pGBoxBuff;		// Drop gold control panel buffer
extern BOOL dropGoldFlag;

extern const BYTE fonttrans[];
extern const BYTE fontkern[];

extern BOOL pinfoflag;

extern char infostr[256];
extern char infoclr;

extern char tempstr[256];

extern BOOL drawhpflag;
extern BOOL drawmanaflag;

extern BOOL chrflag;

extern BOOL drawbtnflag, panbtndown;
extern BOOL panelflag;

extern BOOL spselflag;

extern BOOL chrbtndown;

extern BOOL lvlbtndown;

extern BOOL sbookflag;

extern BOOL talkflag;

extern int dropGoldValue;

extern int initialDropGoldValue;

extern int initialDropGoldIndex;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitControlPan();
void CopyCtrlPan(int, int, int, int, int, int);
void DrawCtrlPan();
void FreeControlPan();

void AddPanelString(const char *, int);
void ClearPanel();

void DrawHealthTop();
void DrawHealthBar();
void DrawManaTop();
void DrawManaBar();
void CalcInitBallPer();

BOOL InfoFit(const char *);
void DrawInfoBox();

void DrawSpellIcon();
void DrawSpellList();
void SetSpell();
void SetupSpellSel();
void SetSpellHK(int);
void GetSpellHK(int);

void DrawChr();

void CheckLvlBtn();
void ReleaseLvlBtn();
void DrawLevelUpIcon();

void CheckPanelBtns();
void ReleasePanelBtn();
void DrawButtons();
void CheckPanelInfo();
void CheckDeadButtons();

void CheckChrBtns();
void ReleaseChrBtn();

void DrawDurIcon();

void DrawPanelFont (long, long, char);

void RedBack();
void DrawPause();

void DrawSpellBook();
void CheckSBook();
//BOOL CheckSBookCast();

void TalkStart();
void TalkEnd();
BOOL Talk_wm_char(WPARAM wKey);
BOOL Talk_wm_keydown(WPARAM wKey);
void DrawTalkBox();

void PrintStringXY(int x, int y,const char * pszStr, char col);

void DrawGoldBox(int gold);
void DropGoldType(char c);
void DropGold(int pnum, int cii);
void SetDropGoldCursor(int pnum);
