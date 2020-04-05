/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/STORES.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/


#define MAXSMITHITEMS	25
#define MAXWITCHITEMS	25
#define MAXHEALITEMS	20
#define MAXPREMIUM		15

#define NUMSTLINES		((MAXSMITHITEMS + 1) * 4) 	//24

#define	STORE_NONE		0
#define STORE_SMITH		1
#define STORE_SBUY		2
#define STORE_SSELL		3
#define STORE_SREPAIR	4
#define STORE_WITCH		5
#define STORE_WBUY		6
#define STORE_WSELL		7
#define STORE_WRECHARGE	8
#define STORE_NOMONEY	9
#define STORE_NOROOM	10
#define STORE_CONFIRM	11
#define STORE_BOY		12
#define STORE_BBUY		13
#define STORE_HEALER	14
#define STORE_STORYTLR	15
#define STORE_HBUY		16
#define STORE_STORYID	17
#define STORE_SPBUY		18
#define STORE_TALK		19
#define STORE_IDSHOW	20
#define STORE_TAVERN	21
#define STORE_DRUNK		22
#define STORE_BARMAID	23

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	int	_sx;			// X justify
	int _syoff;			// Y offset (usually 0)
	char _sstr[128];	// string to print
	BOOL _sjust;		// Center, left, or right
	char _sclr;			// color
	BOOL _sline;		// just a line
	BOOL _ssel;			// selectable
	//BOOL _shigh;		// Highlighted
	int _sval;			// displayable value
} STextStruct;

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern BYTE *pSTextBoxCels;
extern BYTE *pSTextSpinCels;
extern BYTE *pSTextSlidCels;

extern int SStringY[];

extern char stextflag;
extern ItemStruct smithitem[];
extern ItemStruct premiumitem[];
extern int numpremium, premiumlevel;
extern ItemStruct witchitem[];
extern ItemStruct boyitem;
extern int boylevel;
extern ItemStruct golditem;
extern ItemStruct healitem[];

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitStores();
void SetupTownStores();
void FreeStoreMem();

void DrawSText();
void STextESC();
void STextUp();
void STextDown();
void STextPgUp();
void STextPgDown();
void STextEnter();

void StartStore(char);

void DrawSTextBack();
void PrintSString(int x, int y, BOOL cjustflag, char str[], char col, int val);
void DrawSLine(int y);

void SetupSTextWin();
void SetupSTextLWin();

void CheckStoreBtn();
void ReleaseStoreBtn();

void TakePlrsMoney(long);
void SetGoldCurs(int, int);

void SetSpdbarGoldCurs(int pnum, int i);
