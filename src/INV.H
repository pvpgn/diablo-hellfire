/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/INV.H 3     2/14/97 11:23a Dbrevik $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/
extern BOOL invflag;
extern BOOL drawsbarflag;


/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
void InitInv();

void DrawInv();
void DrawSpdBar();

void FreeInvGFX();

void CheckInvScrn();
void CheckSpdBar();

void InvGetItem(int, int);
void AutoGetItem(int, int);
int FindGetItem(int, WORD, int);
void SyncGetItem(int, int, int, WORD, int);

int InvPutItem(int, int, int);
int SyncPutItem(int, 
			int, 
			int, 
			int, 
			WORD, 
			int, 
			BOOL, 
			int, 
			int, 
			int, 
			int, 
			int, 
			DWORD,
			int,
			int,
			int,
			int,
			int,
			int);
BOOL TryInvPut();

char CheckInvHLight();
void RemoveInvItem(int, int);

void CheckInvPaste(int, int, int);
// drb.patch1.start.02/10/97
//void SyncInvPaste(int pnum, BYTE bLoc, int idx, WORD icreateinfo, int iseed);
void SyncInvPaste(int pnum, BYTE bLoc, int idx, WORD icreateinfo, int iseed, BOOL Id);
// drb.patch1.end.02/10/97
void CheckInvCut(int, int, int);
void SyncInvCut(int pnum, BYTE bLoc);

BOOL CheckUsable(int, int);
BOOL UseInvItem(int, int);

void RemoveScroll(int);
BOOL UseScroll();

void UseStaffCharge(int);
BOOL UseStaff();
BOOL UseStaffSBook(int);

BOOL AutoPlace(int, int, int, int, BOOL);
BOOL SpecialAutoPlace(int, int, int, int, BOOL);
void DoTelekinesis();

long CalculateGold(int pnum);
void RemoveSpdBarItem(int pnum, int iv);

BOOL DropItemBeforeTrig();
int GetHighRingValue(int /* myPlr */);
int GetHighBowValue(int /* myPlr */);
int GetHighStaffValue(int /* myPlr */);
int GetHighSwordValue(int /* myPlr */);
int GetHighHelmValue(int /* myPlr */);
int GetHighArmorValue(int /* myPlr */);
int GetHighMaceValue(int /* myPlr */);
int GetHighAmuletValue(int /* myPlr */);
int GetHighAxeValue(int /* myPlr */);
int GetHighShieldValue(int /* myPlr */);
