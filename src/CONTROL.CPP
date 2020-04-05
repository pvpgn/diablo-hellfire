/*-----------------------------------------------------------------------**
** 								Diablo
**
**						 Control panel file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/CONTROL.CPP 2     2/05/97 10:41a Dbrevik2 $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"engine.h"
#include	"control.h"
#include	"gendung.h"
#include	"scrollrt.h"
#include	"msg.h"

#include	"items.h"
#include	"itemdat.h"
#include	"player.h"
#include	"monster.h"
#include	"objects.h"
#include	"cursor.h"
#include	"spells.h"
#include	"missiles.h"

#include	"town.h"
#include	"towners.h"
#include	"trigs.h"
#include	"gamemenu.h"
#include	"inv.h"
#include	"minitext.h"
#include	"lighting.h"
#include	"stores.h"
#include	"automap.h"
#include	"quests.h"

#include	"multi.h"

#include	"error.h"
#include	"spelldat.h"

/*-----------------------------------------------------------------------**
** Registration info
**-----------------------------------------------------------------------*/
#include "regconst.h"
char sgszRegSig5[REG_LEN] = "REGISTRATION_BLOCK";

/*-----------------------------------------------------------------------**
** Local defines
**-----------------------------------------------------------------------*/

#define MANABUFFSIZE	7744		// 88x88
#define LIFEBUFFSIZE	7744

#define STRN_GOLD	0
#define STRN_BLUE	1
#define STRN_RED	2
#define STRN_ORANGE	3
#define STRN_GREY	4

// old version was 4/76
#define NUMSPBKBTNS 5
#define SPBKBTNWDTH 61
/*-----------------------------------------------------------------------**
** Global variables
**-----------------------------------------------------------------------*/

BYTE *pBtmBuff;		// Offscreen control panel buffer
BYTE *pStatusPanel;
BYTE *pPanelButtons;
BYTE *pPanelText;
BYTE *pManaBuff;
BYTE *pLifeBuff;
BYTE *pChrPanel;
BYTE *pChrButtons;
BYTE *pSpellCels;
BYTE *pGBoxBuff;

char panelstr[4][64];
int pstrjust[4];
BOOL pinfoflag;
int pnumlines;

char infostr[256];
char infoclr;

char tempstr[256];

int	pentaspin;
int	dropGoldValue;
int	initialDropGoldValue;
int initialDropGoldIndex;

const BYTE fonttrans[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					// 0-15
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					// 16-31
	0, 54, 44, 57, 58, 56, 55, 47, 40, 41, 59, 39, 50, 37, 51, 52,	// 32-47
	36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 48, 49, 60, 38, 61, 53,	// 48-63
	62, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,			// 64-79
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42, 63, 43, 64, 65,	// 80-95
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,			// 96-111
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 40, 66, 41, 67, 0 };	// 112-127

const BYTE fontkern[68] = { 8,													// Space/Invalid
	10, 7, 9, 8, 7, 6, 8, 8, 3, 3, 8, 6, 11, 9, 10, 6,						// a-p
	9, 9, 6, 9, 11, 10, 13, 10, 11, 7,     									// q-z
	5, 7, 7, 8, 7, 7, 7, 7, 7, 10, 											// 1-0
	4, 5, 6, 3, 3, 4, 3, 6, 6, 3, 3, 3, 3, 3, 2, 7, 6, 3, 10, 10, 6,		// misc
	6, 7, 4, 4, 9, 6, 6, 12, 3, 7 };										// misc

static const long fontofs[5][5] = {
	{ 456433, 24576, 24576, 24576, 24756 },
	{ 447217, 465649, 24576, 24576, 24576 },
	{ 442609, 456433, 470257, 24576, 24576 },
	{ 439537, 451057, 461809, 473329, 24576 },
	{ 438001, 447217, 456433, 465649, 474097 } };

BOOL drawhpflag;
BOOL drawmanaflag;
BOOL chrflag;

/*-----------------------------------------------------------------------*/
typedef enum {
	SI_INVALID,
	SI_FIREBOLT,
	SI_HEAL,
	SI_LIGHTNING,
	SI_FLASH,
	SI_IDENTIFY,
	SI_FIREWALL,
	SI_TOWNPORTAL,
	SI_STONECURSE,
	SI_INFRAVISION,
	SI_HEALOTHER,
	SI_NOVA,
	SI_FIREBALL,
	SI_MANASHIELD,
	SI_FLAMEWAVE,
	SI_INFERNO,
	SI_CHAINLIGHTNING,
	SI_JABBERWOCK,		// with eyes of flame...
	SI_GUARDIAN,
	SI_BLOODRITUAL,		// unused, looks like Krull
	SI_INVISIBILITY,
	SI_GOLEM,
	SI_ETHEREALIZE,
	//SI_BLOODBOIL,		// unused
	SI_RAGE,			// Replaced bloodboil.
	SI_TELEPORT,
	SI_APOCALYPSE,
	SI_REPAIR,
	SI_EMPTY,
	SI_PHASE,
	SI_RECHARGE,
	SI_BONESPIRIT,
	SI_REDSKULL,		// unused
	SI_PENTAGRAM,		// unused
	SI_FIRECLOUD,		// unused
	SI_LONGHORN,		// unused
	SI_PENTASTAR,		// unused
	SI_BLOODSTAR,
	SI_DISARM,
	SI_ELEMENTAL,
	SI_CHARGEDBOLT,
	SI_TELEKINESIS,
	SI_RESURRECT,
	SI_HOLYBOLT,
	
#if 1	
	// new icons
	SI_WARP,
	SI_SEARCH,
	SI_REFLECT,
	SI_LIGHTNINGWALL,
	SI_IMMOLATION,
	SI_BERSERK,
	SI_RINGOFFIRE,
	SI_JESTER,
	SI_MANA,
	
	SI_LAST

#else
	// faked icons
	SI_WARP = SI_PHASE,
	SI_SEARCH = SI_INFRAVISION,
	SI_REFLECT = SI_MANASHIELD,
	SI_LIGHTNINGWALL = SI_FIREWALL,
	SI_IMMOLATION = SI_NOVA,
	SI_BERSERK = SI_REDSKULL,
	SI_RINGOFFIRE = SI_PENTAGRAM,
	SI_MANA = SI_BLOODRITUAL,
	SI_JESTER = SI_ETHEREALIZE,
	
	SI_LAST = SI_HOLYBOLT+1
#endif	
} SPICONTYPE;

BOOL spselflag = FALSE;
int pSpell, pSplType;
byte SpellTrans[256];
static char SpellITbl[MAXSPELLS] = {
		SI_EMPTY,			// Invalid
		SI_FIREBOLT,		// SPL_FIREBOLT
		SI_HEAL,			// SPL_HEAL
		SI_LIGHTNING,		// SPL_LIGHTNING
		SI_FLASH,			// SPL_FLASH
		SI_IDENTIFY,		// SPL_IDENTIFY
		SI_FIREWALL,		// SPL_WALL
		SI_TOWNPORTAL,		// SPL_TOWN
		SI_STONECURSE,		// SPL_STONE
		SI_INFRAVISION,		// SPL_INFRA
		SI_PHASE,			// SPL_PHASE
		SI_MANASHIELD,		// SPL_MANASHLD
		SI_FIREBALL,		// SPL_FIREBALL
		SI_GUARDIAN,		// SPL_GUARDIAN
		SI_CHAINLIGHTNING,	// SPL_CHAIN
		SI_FLAMEWAVE,		// SPL_WAVE
		SI_GUARDIAN,		// SPL_DOOM
		SI_BLOODRITUAL,		// SPL_BLOODR
		SI_NOVA,			// SPL_NOVA
		SI_INVISIBILITY,	// SPL_INVIS
		SI_INFERNO,			// SPL_FLAME
		SI_GOLEM,			// SPL_GOLEM
		//SI_BLOODBOIL,		// SPL_BLOODB
		SI_RAGE,			// SPL_RAGE
		SI_TELEPORT,		// SPL_TELE
		SI_APOCALYPSE,		// SPL_APOCA
		SI_ETHEREALIZE,		// SPL_ETHER
		SI_REPAIR,			// SPL_REPAIR
		SI_RECHARGE,		// SPL_RECHARGE
		SI_DISARM,			// SPL_DISARM
		SI_ELEMENTAL,		// SPL_ELEMENT
		SI_CHARGEDBOLT,		// SPL_CBOLT
		SI_HOLYBOLT,		// SPL_HBOLT
		SI_RESURRECT,		// SPL_RESURRECT
		SI_TELEKINESIS,		// SPL_TELEKINESIS
		SI_HEALOTHER,		// SPL_HEALOTHER
		SI_BLOODSTAR, 		// SPL_BSTAR
		SI_BONESPIRIT,		// SPL_BONESPIRT
		SI_MANA,			// SPL_MANA
		SI_MANA,			// SPL_FMANA
		SI_JESTER,			// SPL_RANDOM
		SI_LIGHTNINGWALL,	// SPL_LTWALL
		SI_IMMOLATION,		// SPL_IMMOLATION
		SI_WARP,			// SPL_TELESTAIRS
		SI_REFLECT,			// SPL_REFLECT
		SI_BERSERK,			// SPL_BERSERK
		SI_RINGOFFIRE,		// SPL_RINGOFFIRE
		SI_SEARCH,			// SPL_SHOWMAGITEMS
		SI_PENTASTAR,		// SPL_RUNEOFFIRE
		SI_PENTASTAR,		// SPL_RUNEOFLIGHT
		SI_PENTASTAR,		// SPL_RUNEOFNOVA
		SI_PENTASTAR,		// SPL_RUNEOFIMMOLATION
		SI_PENTASTAR,		// SPL_RUNEOFSTONE

#if defined (HELLFIRE2)
		SI_RINGOFFIRE,		// SPL_RINGOFLIGHT
		SI_REDSKULL,		// SPL_AURA
		SI_ETHERALIZE,		// SPL_SPIRALFIREBALL
#endif
	};

/*-----------------------------------------------------------------------*/

#define NUMPBTNS		8
#define SINGLE_PBTNS	6
#define MULTI_PBTNS		NUMPBTNS

#define PBTN_CHR	0
#define PBTN_TPLR1	0
#define PBTN_QUEST	1
#define PBTN_AMAP	2
#define PBTN_TPLR2	2
#define PBTN_MENU	3
#define PBTN_INV	4
#define PBTN_TPLR3	4
#define PBTN_SBOOK	5
#define PBTN_TALK	6
#define PBTN_ATTACK	7

// x1, y1, width, height, talk pushable
int PanBtnPos[NUMPBTNS][5] = {
	{   9, 361, 71, 19, TRUE  },
	{   9, 387, 71, 19, FALSE },
	{   9, 427, 71, 19, TRUE  },
	{   9, 453, 71, 19, FALSE },
	{ 560, 361, 71, 19, TRUE  },
	{ 560, 387, 71, 19, FALSE },
	{  87, 443, 33, 32, TRUE  },
	{ 527, 443, 33, 32, TRUE  },
};

char *PanBtnHotKey[NUMPBTNS] = {
	"'c'",
	"'q'",
	"Tab",
	"Esc",
	"'i'",
	"'b'",
	"Enter",
	NULL };

char *PanBtnStr[NUMPBTNS] = {
	"Character Information",
	"Quests log",
	"Automap",
	"Main Menu",
	"Inventory",
	"Spell book",
	"Send Message",
	"Player Attack" };

BOOL panbtn[NUMPBTNS];
BOOL drawbtnflag, panbtndown;
BOOL panelflag;		// panel info draw
int numpanbtns;

BYTE *pDurIcons;

BOOL drawdurflag;
BOOL dropGoldFlag;

/*-----------------------------------------------------------------------*/

#define NUMCBTNS	4

#define CBTN_STR	0
#define CBTN_MAG	1
#define CBTN_DEX	2
#define CBTN_VIT	3

// x1, y1, width, height
int ChrBtnPos[NUMCBTNS][4] = {
	{ 137, 138, 41, 22 },
	{ 137, 166, 41, 22 },
	{ 137, 195, 41, 22 },
	{ 137, 223, 41, 22 } };

BOOL chrbtn[NUMCBTNS];
BOOL chrbtndown;

/*-----------------------------------------------------------------------*/

BOOL lvlbtndown;

/*-----------------------------------------------------------------------*/

BYTE *pSpellBkCel;
BYTE *pSBkBtnCel;
BYTE *pSBkIconCels;

int sbooktab;
BOOL sbookflag;

// The first entry will be filled during init with the player types skill
int SpellPages[6][7] = {
	{ 0, SPL_FIREBOLT, SPL_CBOLT, SPL_HBOLT, SPL_HEAL, SPL_HEALOTHER, SPL_FLAME },
	{ SPL_RESURRECT, SPL_WALL, SPL_TELEKINESIS, SPL_LIGHTNING, SPL_TOWN, SPL_FLASH, SPL_STONE },
	{ SPL_PHASE, SPL_MANASHLD, SPL_ELEMENT, SPL_FIREBALL, SPL_WAVE, SPL_CHAIN, SPL_GUARDIAN},
	{ SPL_NOVA, SPL_GOLEM, SPL_TELE, SPL_APOCA, SPL_BONESPIRIT, SPL_BSTAR, SPL_ETHER },
	{ SPL_LTWALL, SPL_IMMOLATION, SPL_TELESTAIRS, SPL_REFLECT, SPL_BERSERK, SPL_RINGOFFIRE, SPL_SHOWMAGITEMS },
#if defined (HELLFIRE2)
	{ SPL_RINGOFLIGHT, SPL_AURA, SPL_SPIRALFIREBALL, -1, -1, -1, -1 }
#else
	{ -1, -1, -1, -1, -1, -1, -1 }
#endif
};

/*-----------------------------------------------------------------------*/
#define MAX_TALK_SAVES	8	// must be pow2
BOOL talkflag;
static int tspin;
static long talkofs;
static char sgszTalkMsg[MAX_SEND_STR_LEN];
static BYTE sgbTalkSavePos;
static BYTE sgbNextTalkSave;
static char sgszTalkSaveMsg[MAX_TALK_SAVES][MAX_SEND_STR_LEN];
static BYTE sgbPlrTalkTbl[MAX_PLRS];
static BYTE *pTalkPanel;
static BYTE *pMultiBtns;
static BYTE *pTalkBtns;
static BOOL talkbtndown[3];

void TalkStart();
void TalkEnd();
void PlrStringXY(int x1, int y, int x2,const char * pszStr,char col);


/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
void SetPlrHandItem(ItemStruct *h, int idata);
void GetPlrHandSeed(ItemStruct *h);
void GetGoldSeed(int pnum, ItemStruct *h);
void SetSpellTrans(char);

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawSpellCel(long xp, long yp, BYTE *pCels, long nCel, long w)
{
	BYTE *pTo;
	byte *ttbl;
	long RLELen;

	ttbl = &SpellTrans[0];
	app_assert(gpBuffer);
	pTo = gpBuffer + nBuffWTbl[yp] + xp;
	__asm {
		mov		ebx,dword ptr [pCels]
		mov		eax,dword ptr [nCel]
		shl		eax,2
		add		ebx,eax
		mov		eax,dword ptr [ebx+4]
		sub		eax,dword ptr [ebx]
		mov		dword ptr [RLELen],eax

		mov		esi,dword ptr [pCels]			// Source
		add		esi,dword ptr [ebx]

		mov		edi,dword ptr [pTo]				// Dest

		mov		eax,dword ptr [RLELen]
		add		eax,esi
		mov		dword ptr [RLELen],eax

		mov		ebx,dword ptr [ttbl]

_T1Lp1:	mov		edx,dword ptr [w]

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax
		mov		ecx,eax
		shr		ecx,1
		jnc		_T1w
		lodsb
		xlatb
		stosb
		jecxz	_T1x
_T1w:	shr		ecx,1
		jnc		_T1Lp3
		lodsw
		xlatb
		ror		ax,8
		xlatb
		ror		ax,8
		stosw
		jecxz	_T1x
_T1Lp3:	lodsd
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		xlatb
		ror		eax,8
		stosd
		loop	_T1Lp3
_T1x:	or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	sub		edi,768
		sub		edi,dword ptr [w]
		cmp		esi,dword ptr [RLELen]
		jnz		_T1Lp1
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetSpellTrans(char t)
{
	int i;

	if (t == STRN_GOLD) {
		for (i = 0; i < 128; i++) SpellTrans[i] = i;
	}
	for (i = 128; i < 256; i++) SpellTrans[i] = i;
	SpellTrans[255] = 0;

	switch (t) {
		case STRN_BLUE :
			SpellTrans[144] = 177;
			SpellTrans[145] = 179;
			SpellTrans[146] = 181;
			for (i = 176; i < 192; i++) {
				SpellTrans[i - 16] = i;		// 160-175
				SpellTrans[i + 16] = i;		// 192-207
				SpellTrans[i + 32] = i;		// 208-223
			}
			break;
		case STRN_ORANGE :
			SpellTrans[144] = 209;
			SpellTrans[145] = 211;
			SpellTrans[146] = 213;
			for (i = 208; i < 224; i++) {
				SpellTrans[i - 48] = i;		// 160-175
				SpellTrans[i - 16] = i;		// 192-207
			}
			break;
		case STRN_RED :
			SpellTrans[144] = 161;
			SpellTrans[145] = 163;
			SpellTrans[146] = 165;
			for (i = 160; i < 176; i++) {
				SpellTrans[i + 32] = i;		// 192-207
				SpellTrans[i + 48] = i;		// 208-223
			}
			break;
		case STRN_GREY :
			SpellTrans[144] = 241;
			SpellTrans[145] = 243;
			SpellTrans[146] = 245;
			for (i = 240; i < 255; i++) {
				SpellTrans[i - 80] = i;		// 160-174
				SpellTrans[i - 48] = i;		// 192-206
				SpellTrans[i - 32] = i;		// 208-222
			}
			SpellTrans[175] = 0;
			SpellTrans[207] = 0;
			SpellTrans[223] = 0;
			break;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawSpellIcon() {
	char t, sn;
	int sl;

	t = plr[myplr]._pRSplType;
	sn = plr[myplr]._pRSpell;
	sl = plr[myplr]._pSplLvl[sn] + plr[myplr]._pISplLvlAdd;
	if ((t == SPT_MEMORIZED) && (sn != -1)) {
		if (!CheckSpell(myplr, sn, t, TRUE)) t = STRN_GREY;
		if (sl <= 0 ) t = STRN_GREY;
	}

	if ((currlevel == 0) && (t != STRN_GREY) && (spelldata[sn].sTownSpell == FALSE)) t = STRN_GREY;
	if (plr[myplr]._pRSpell < 0) t = STRN_GREY;
	SetSpellTrans(t);
	if (sn != -1) DrawSpellCel(629, 631, pSpellCels, SpellITbl[sn], 56);
	else DrawSpellCel(629, 631, pSpellCels, 27, 56);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#define SPLICONSIZE		56
#define SPLICONRIGHT	636		// (640 - (trunc(640 / 56) * 56)) / 2) + 640 + 64 - SPLICONSIZE
#define SPLICONLEFT		20		// SPLICONRIGHT - (trunc(640 / 56) * 56) - SPLICONSIZE

#define SPELLICON_BLANK	SI_LAST
#define SPELLICON_F5	(SI_LAST + 5)

void DrawSpellList()
{
	int mx,my,x,y,i,j,t;
	__int64 mask,spl;
	int s,c,hk;
	int v, selbox;

	x = SPLICONRIGHT;
	y = 495;

	pSpell = -1;
	infostr[0] = 0;
	ClearPanel();
	for (j = 0; j < 4; j++) {
		switch(j) {
			case 0:
				SetSpellTrans(STRN_GOLD);
				spl = plr[myplr]._pAblSpells;
				selbox = SPELLICON_BLANK + 3;
				break;
			case 1:
				spl = plr[myplr]._pMemSpells;
				selbox = SPELLICON_BLANK + 4;
				break;
			case 2:
				SetSpellTrans(STRN_RED);
				spl = plr[myplr]._pScrlSpells;
				selbox = SPELLICON_BLANK + 1;
				break;
			case 3:
				SetSpellTrans(STRN_ORANGE);
				spl = plr[myplr]._pISpells;
				selbox = SPELLICON_BLANK + 2;
				break;
		}
		mask = 1;
		for (i = 1; i < SPL_LAST; i++) {
			if ((spl & mask) != 0)
			{
				if (j == 1) {
					v = plr[myplr]._pSplLvl[i] + plr[myplr]._pISplLvlAdd;
					if ( v < 0 ) v = 0;
					if (v > 0)	t = STRN_BLUE;
					else t = STRN_GREY;
					SetSpellTrans(t);
				}
				
				if ((currlevel == 0) && (spelldata[i].sTownSpell == FALSE)) SetSpellTrans(STRN_GREY);

				DrawSpellCel(x, y, pSpellCels, SpellITbl[i], SPLICONSIZE);
				mx = x - 64;
				my = y - (160 + SPLICONSIZE);
				if ((MouseX >= mx) && (MouseX < (mx+SPLICONSIZE)) && (MouseY >= my) && (MouseY < (my+SPLICONSIZE))) {
					pSpell = i;
					pSplType = j;
					if (plr[myplr]._pClass == CLASS_MONK && i == SPL_SHOWMAGITEMS)
						pSplType = 0;
					DrawSpellCel(x, y, pSpellCels, selbox, SPLICONSIZE);
					switch (pSplType) {
						case 0:
							sprintf(infostr, "%s Skill", spelldata[pSpell].sSkillText);
							break;
						case 1:
							sprintf(infostr, "%s Spell", spelldata[pSpell].sNameText);
							if (pSpell == SPL_HBOLT) {
								sprintf(tempstr, "Damages undead only");
								AddPanelString(tempstr, TEXT_CENTER);
							}
							if (v == 0) sprintf(tempstr, "Spell Level 0 - Unusable");
							else sprintf(tempstr, "Spell Level %i", v);
							AddPanelString(tempstr, TEXT_CENTER);
							break;
						case 2:
							sprintf(infostr, "Scroll of %s", spelldata[pSpell].sNameText);
							c = 0;
							for (s = 0; s < plr[myplr]._pNumInv; s++) {
								if ((plr[myplr].InvList[s]._itype != -1) &&
									((plr[myplr].InvList[s]._iMiscId == IMID_SCROLL) ||
									(plr[myplr].InvList[s]._iMiscId == IMID_TSCROLL))) {
										if (plr[myplr].InvList[s]._iSpell == pSpell) c++;
								}
							}
							for (s = 0; s < MAXSPD; s++) {
								if ((plr[myplr].SpdList[s]._itype != -1) &&
									((plr[myplr].SpdList[s]._iMiscId == IMID_SCROLL) ||
									(plr[myplr].SpdList[s]._iMiscId == IMID_TSCROLL))) {
										if (plr[myplr].SpdList[s]._iSpell == pSpell) c++;
								}
							}
							if (c == 1) strcpy(tempstr, "1 Scroll");
							else sprintf(tempstr, "%i Scrolls", c);
							AddPanelString(tempstr, TEXT_CENTER);
							break;
						case 3:
							sprintf(infostr, "Staff of %s", spelldata[pSpell].sNameText);
							if (plr[myplr].Hand1Item._iCharges == 1) strcpy(tempstr, "1 Charge");
							else sprintf(tempstr, "%i Charges", plr[myplr].Hand1Item._iCharges);
							AddPanelString(tempstr, TEXT_CENTER);
							break;
					}
					for (hk = 0; hk < 4; hk++) {
						if ((plr[myplr]._pSplHotKey[hk] == pSpell) && (plr[myplr]._pSplTHotKey[hk] == pSplType)) {
							DrawSpellCel(x, y, pSpellCels, SPELLICON_F5+hk, SPLICONSIZE);
							sprintf(tempstr, "Spell Hot Key #F%i", hk+5);
							AddPanelString(tempstr, TEXT_CENTER);
						}
					}
				}
				x -= SPLICONSIZE;
				if (x == SPLICONLEFT) {
					x = SPLICONRIGHT;
					y -= SPLICONSIZE;
				}
			}
			mask = mask << 1;
		}
		if (spl && (x != SPLICONRIGHT)) x -= SPLICONSIZE;
		if (x == SPLICONLEFT) {
			x = SPLICONRIGHT;
			y -= SPLICONSIZE;
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void SetSpell()
{
	spselflag = FALSE;
	if (pSpell != -1) {
		ClearPanel();
		plr[myplr]._pRSpell = pSpell;
		plr[myplr]._pRSplType = pSplType;
		force_redraw = FULLDRAW;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetSpellHK(int hk)
{
	if (pSpell != -1) {
		for (int i = 0; i < 4; i++) {
			if ((plr[myplr]._pSplHotKey[i] == pSpell) && (plr[myplr]._pSplTHotKey[i] == pSplType))
				plr[myplr]._pSplHotKey[i] = -1;
		}
		plr[myplr]._pSplHotKey[hk] = pSpell;
		plr[myplr]._pSplTHotKey[hk] = pSplType;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void GetSpellHK(int hk)
{
	__int64 spl;

	if (plr[myplr]._pSplHotKey[hk] == -1) return;

	switch(plr[myplr]._pSplTHotKey[hk]) {
		case 0:
			spl = plr[myplr]._pAblSpells;
			break;
		case 1:
			spl = plr[myplr]._pMemSpells;
			break;
		case 2:
			spl = plr[myplr]._pScrlSpells;
			break;
		case 3:
			spl = plr[myplr]._pISpells;
			break;
	}
	spl &= (((__int64)1) << (plr[myplr]._pSplHotKey[hk]-1));
	if (spl) {
		plr[myplr]._pRSpell = plr[myplr]._pSplHotKey[hk];
		plr[myplr]._pRSplType = plr[myplr]._pSplTHotKey[hk];
		force_redraw = FULLDRAW;
	}
}

/*-----------------------------------------------------------------------**
** Draws a small font letter
**-----------------------------------------------------------------------*/

void DrawPanelFont (long poffset, long nCel, char clr)
{
	app_assert(gpBuffer);
	__asm {
		mov		ebx,dword ptr [pPanelText]
		mov		eax,dword ptr [nCel]
		shl		eax,2
		add		ebx,eax

		mov		edx,dword ptr [ebx+4]		// Length
		sub		edx,dword ptr [ebx]

		mov		esi,dword ptr [pPanelText]	// Source
		add		esi,dword ptr [ebx]

		mov		edi,dword ptr [gpBuffer]	// Dest
		add		edi,dword ptr [poffset]

		mov		ebx,edx
		add		ebx,esi

		xor		edx,edx
		mov		dl,byte ptr [clr]
		cmp		edx,ICOLOR_WHITE
		jz		_T1Lp1				// Normal / White
		cmp		edx,ICOLOR_BLUE
		jz		_T2Lp1				// Blue
		cmp		edx,ICOLOR_RED
		jz		_T3Lp1				// Red
		jmp		_T4Lp1				// Gold

/*- White ---------------------------------------------------------------*/

_T1Lp1:	mov		edx,13

_T1Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T1J

		sub		edx,eax				// Draw
		mov		ecx,eax
		shr		ecx,1
		jnc		_T1w
		movsb
		jecxz	_T1x
_T1w:	shr		ecx,1
		jnc		_T1Lp3
		movsw
		jecxz	_T1x
_T1Lp3:	rep movsd
_T1x:	or		edx,edx
		jz		_T1Nxt
		jmp		_T1Lp2

_T1J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T1Lp2
_T1Nxt:	sub		edi,781
		cmp		ebx,esi
		jnz		_T1Lp1
		jmp		_Done

/*- Blue ----------------------------------------------------------------*/

_T2Lp1:	mov		edx,13

_T2Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T2J

		sub		edx,eax				// Draw
		mov		ecx,eax
_T2Lp3:	lodsb
		cmp		al,253
		ja		_T2F
		cmp		al,240
		jb		_T2Sv
		sub		al,62
		jmp		_T2Sv
_T2F:	mov		al,191
_T2Sv:	stosb
		loop	_T2Lp3
		or		edx,edx
		jz		_T2Nxt
		jmp		_T2Lp2

_T2J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T2Lp2
_T2Nxt:	sub		edi,781
		cmp		ebx,esi
		jnz		_T2Lp1
		jmp		_Done

/*- Red -----------------------------------------------------------------*/

_T3Lp1:	mov		edx,13

_T3Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T3J

		sub		edx,eax				// Draw
		mov		ecx,eax
_T3Lp3:	lodsb
		cmp		al,240
		jb		_T3Sv
		sub		al,16
_T3Sv:	stosb
		loop	_T3Lp3
		or		edx,edx
		jz		_T3Nxt
		jmp		_T3Lp2

_T3J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T3Lp2
_T3Nxt:	sub		edi,781
		cmp		ebx,esi
		jnz		_T3Lp1
		jmp		_Done

/*- Gold ----------------------------------------------------------------*/

_T4Lp1:	mov		edx,13

_T4Lp2:	xor		eax,eax				// Load control byte
		lodsb
		or		al,al
		js		_T4J

		sub		edx,eax				// Draw
		mov		ecx,eax
_T4Lp3:	lodsb
		cmp		al,240
		jb		_T4Sv
		cmp		al,254
		jae		_T4val
		sub		al,46
		jmp		_T4Sv
_T4val:	mov		al,207
_T4Sv:	stosb
		loop	_T4Lp3
		or		edx,edx
		jz		_T4Nxt
		jmp		_T4Lp2

_T4J:	neg		al					// Do jump
		add		edi,eax
		sub		edx,eax
		jnz		_T4Lp2
_T4Nxt:	sub		edi,781
		cmp		ebx,esi
		jnz		_T4Lp1

/*-----------------------------------------------------------------------*/

_Done:
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void AddPanelString(const char * str, int just)
{
	strcpy(panelstr[pnumlines],str);
	pstrjust[pnumlines] = just;
	if (pnumlines < 4) pnumlines++;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void ClearPanel()
{
	pnumlines = 0;
	pinfoflag = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CopyCtrlPan(int sx, int sy, int deltax, int deltay, int dx, int dy)
{
	long		src, dest;

	app_assert(gpBuffer);
	src = (sy * 640) + sx;
	dest = (dy * 768) + dx;
	__asm {
		mov		esi,dword ptr [pBtmBuff]
		add		esi,dword ptr [src]
		mov		edi,dword ptr [gpBuffer]
		add		edi,dword ptr [dest]

		xor		ebx,ebx
		mov		bx,word ptr [deltax]
		xor		edx,edx
		mov		dx,word ptr [deltay]
_CLp:	mov		ecx,ebx
		shr		ecx,1
		jnc		_Tw
		movsb
		jecxz	_Tx
_Tw:	shr		ecx,1
		jnc		_TLp
		movsw
		jecxz	_Tx
_TLp:	rep movsd
_Tx:	add		esi,640
		sub		esi,ebx
		add		edi,768
		sub		edi,ebx
		dec		edx
		jnz		_CLp
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitPanelStr()
{
	ClearPanel();
	//AddPanelString("Welcome to Diablo", TEXT_CENTER);	// This was ok (pre Demo) now its not
	//AddPanelString("Press F1 for help", TEXT_CENTER);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void BuffCopy(BYTE *pSrc, int y1, int y2, int dx, int dy)
{
	long	srco, desto, deltay;

	app_assert(gpBuffer);
	srco = y1 * 88;
	desto = (dy * 768) + dx;
	deltay = y2 - y1;
	__asm {
		mov		esi,dword ptr [pSrc]
		add		esi,dword ptr [srco]
		mov		edi,dword ptr [gpBuffer]
		add		edi,dword ptr [desto]

		mov		edx,dword ptr [deltay]
_YLp:	mov		ecx,22
		rep movsd
		add		edi,680
		dec		edx
		jnz		_YLp
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void TransBuffCopy(BYTE *pSrc, long srcwidth, long srcoff, BYTE *pDest, long destoff, long dy)
{
	__asm {
		mov		esi,dword ptr [pSrc]
		add		esi,dword ptr [srcoff]
		mov		edi,dword ptr [pDest]
		add		edi,dword ptr [destoff]

		mov		edx,dword ptr [dy]
_YLp:	mov		ecx,59
_XLp:	lodsb
		or		al,al
		jz		_Skip
		mov		byte ptr [edi],al
_Skip:	inc		edi
		loop	_XLp
		add		esi,dword ptr [srcwidth]
		sub		esi,59
		add		edi,709
		dec		edx
		jnz		_YLp
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawHealthTop()
{
	double	v;
	int		dv;

	if (plr[myplr]._pMaxHP <= 0)
		v = 0.0;
	else
		v = ((double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP) * 80;
	dv = (int)v;
	plr[myplr]._pHPPer = dv;

	
	long dy;

	dy = 80 - plr[myplr]._pHPPer;
	if (dy > 11) dy = 11;
	dy += 2;

	app_assert(gpBuffer);
	TransBuffCopy(pLifeBuff, 88, 277, gpBuffer, 383405, dy);
	if (dy != 13) TransBuffCopy(pBtmBuff, 640, (dy * 640) + 2029, gpBuffer, (dy * 768) + 383405, 13 - dy);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawHealthBar()
{
	double	v;
	int		dv;

	if (plr[myplr]._pMaxHP <= 0)
		v = 0.0;
	else
		v = ((double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP) * 80;
	dv = (int)v;
	plr[myplr]._pHPPer = dv;

	if (dv > 69) dv = 69;
	if (dv != 69) BuffCopy(pLifeBuff, 16, 85-dv, 160, 512);
	if (dv != 0) CopyCtrlPan(96, 85-dv, 88, dv, 160, 581-dv);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawManaTop()
{
	long dy;

	dy = 80 - plr[myplr]._pManaPer;
	if (dy > 11) dy = 11;
	dy += 2;

	app_assert(gpBuffer);
	TransBuffCopy(pManaBuff, 88, 277, gpBuffer, 383771, dy);
	if (dy != 13) TransBuffCopy(pBtmBuff, 640, (dy * 640) + 2395, gpBuffer, (dy * 768) + 383771, 13 - dy);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CalcInitBallPer()
{
	double	v;
	int		dv;
	long	m,mm;

	mm = plr[myplr]._pMaxMana;
	m = plr[myplr]._pMana;
	if (mm < 0) mm = 0;
	if (m < 0) m = 0;
	if (mm == 0) dv = 0;
	else {
		v = ((double)m / (double)mm) * 80;
		dv = (int)v;
	}
	plr[myplr]._pManaPer = dv;
	v = ((double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP) * 80;
	dv = (int)v;
	plr[myplr]._pHPPer = dv;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawManaBar()
{
	double	v;
	int		dv;
	long	m,mm;

	mm = plr[myplr]._pMaxMana;
	m = plr[myplr]._pMana;
	if (mm < 0) mm = 0;
	if (m < 0) m = 0;
	if (mm == 0) dv = 0;
	else {
		v = ((double)m / (double)mm) * 80;
		dv = (int)v;
	}
	plr[myplr]._pManaPer = dv;

	if (dv > 69) dv = 69;
	if (dv != 69) BuffCopy(pManaBuff, 16, 85-dv, 528, 512);
	if (dv != 0) CopyCtrlPan(464, 85-dv, 88, dv, 528, 581-dv);

	DrawSpellIcon();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void InitControlPan()
{
	int i;

	app_assert(! pBtmBuff);
	if (gbMaxPlayers == 1) {
		pBtmBuff = DiabloAllocPtrSig(BTMBUFFSIZE,'CTRL');
		ZeroMemory(pBtmBuff,BTMBUFFSIZE);
	} else {
		pBtmBuff = DiabloAllocPtrSig(BTMBUFFMULTISIZE,'CTRL');
		ZeroMemory(pBtmBuff,BTMBUFFMULTISIZE);
	}
	pManaBuff = DiabloAllocPtrSig(MANABUFFSIZE,'CTRL');
	ZeroMemory(pManaBuff,MANABUFFSIZE);
	pLifeBuff = DiabloAllocPtrSig(LIFEBUFFSIZE,'CTRL');
	ZeroMemory(pLifeBuff,LIFEBUFFSIZE);
	pPanelText = LoadFileInMemSig("CtrlPan\\SmalText.CEL",NULL,'CTRL');
	pChrPanel = LoadFileInMemSig("Data\\Char.CEL",NULL,'CTRL');
	pSpellCels = LoadFileInMemSig("Data\\SpelIcon.CEL",NULL,'CTRL');	// was in CtrlPan
	SetSpellTrans(STRN_GOLD);

	// Init Control panel offscreen buffer
	pStatusPanel = LoadFileInMemSig("CtrlPan\\Panel8.CEL",NULL,'CTRL');
	DrawBuffCel(pBtmBuff, 0, 143, BTMBUFFX, pStatusPanel, 1, 640);
	DiabloFreePtr (pStatusPanel);
	pStatusPanel = LoadFileInMemSig("CtrlPan\\P8Bulbs.CEL",NULL,'CTRL');
	DrawBuffCel(pLifeBuff, 0, 87, 88, pStatusPanel, 1, 88);
	DrawBuffCel(pManaBuff, 0, 87, 88, pStatusPanel, 2, 88);
	DiabloFreePtr (pStatusPanel);
	talkflag = FALSE;
	if (gbMaxPlayers != 1) {
		pTalkPanel = LoadFileInMemSig("CtrlPan\\TalkPanl.CEL",NULL,'CTRL');
		DrawBuffCel(pBtmBuff, 0, 287, BTMBUFFX, pTalkPanel, 1, 640);
		DiabloFreePtr (pTalkPanel);
		pMultiBtns = LoadFileInMemSig("CtrlPan\\P8But2.CEL",NULL,'CTRL');
		pTalkBtns = LoadFileInMemSig("CtrlPan\\TalkButt.CEL",NULL,'CTRL');
		talkofs = 0;
		sgszTalkMsg[0] = 0;
		for (i = 0; i < MAX_PLRS; i++) sgbPlrTalkTbl[i] = TRUE;
		for (i = 0; i < 3; i++) talkbtndown[i] = FALSE;
	}
	panelflag = FALSE;

	lvlbtndown = FALSE;

	pPanelButtons = LoadFileInMemSig("CtrlPan\\Panel8bu.CEL",NULL,'CTRL');
	for (i = 0; i < NUMPBTNS; i++) panbtn[i] = FALSE;
	panbtndown = FALSE;
	if (gbMaxPlayers == 1) numpanbtns = SINGLE_PBTNS;
	else numpanbtns = MULTI_PBTNS;

	pChrButtons = LoadFileInMemSig("Data\\CharBut.CEL",NULL,'CTRL');
	for (i = 0; i < NUMCBTNS; i++) chrbtn[i] = FALSE;
	chrbtndown = FALSE;

	pDurIcons = LoadFileInMemSig("Items\\DurIcons.CEL",NULL,'CTRL');

	strcpy(infostr,"");
	InitPanelStr();
	drawhpflag = TRUE;
	drawmanaflag = TRUE;
	chrflag = FALSE;
	spselflag = FALSE;

	pSpellBkCel = LoadFileInMemSig("Data\\SpellBk.CEL",NULL,'CTRL');
	pSBkBtnCel = LoadFileInMemSig("Data\\SpellBkB.CEL",NULL,'CTRL');
	pSBkIconCels = LoadFileInMemSig("Data\\SpellI2.CEL",NULL,'CTRL');
	sbooktab = 0;
	sbookflag = FALSE;
	if (plr[myplr]._pClass == CLASS_WARRIOR) SpellPages[0][0] = SPL_REPAIR;
	#if !IS_VERSION(SHAREWARE)
	else if (plr[myplr]._pClass == CLASS_ROGUE) SpellPages[0][0] = SPL_DISARM;
	else if (plr[myplr]._pClass == CLASS_SORCEROR) SpellPages[0][0] = SPL_RECHARGE;
	else if (plr[myplr]._pClass == CLASS_MONK) SpellPages[0][0] = SPL_SHOWMAGITEMS;
	else if (plr[myplr]._pClass == CLASS_BARD) SpellPages[0][0] = SPL_IDENTIFY;
	else if (plr[myplr]._pClass == CLASS_BARBARIAN) SpellPages[0][0] = SPL_RAGE;
	#endif

	pQLogCel = LoadFileInMemSig("Data\\Quest.CEL",NULL,'CTRL');

	pGBoxBuff = LoadFileInMemSig("CtrlPan\\Golddrop.cel",NULL,'CTRL');
	// Initialize gold drop variables
	dropGoldFlag = FALSE;
	dropGoldValue = 0;
	initialDropGoldValue = 0;
	initialDropGoldIndex = 0;
	pentaspin = 1;
	
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DrawCtrlPan() {
	CopyCtrlPan(0, 16+talkofs, 640, 128, 64, 512);
	DrawInfoBox();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawButtons()
{
	for (int i = 0; i < SINGLE_PBTNS; i++) {
		if (!panbtn[i])
			CopyCtrlPan(PanBtnPos[i][0], PanBtnPos[i][1]-336, 71, 20, PanBtnPos[i][0] + 64, PanBtnPos[i][1] + 160);
		else
			DrawCel(PanBtnPos[i][0]+64, PanBtnPos[i][1]+178, pPanelButtons, i+1, 71);
	}

	if (numpanbtns == MULTI_PBTNS) {
		// Draw talk button
		DrawCel(151, 634, pMultiBtns, 1+panbtn[PBTN_TALK], 33);
		// Draw Friend or foe button
		if (FriendlyMode) {
			DrawCel(591, 634, pMultiBtns, 3+panbtn[PBTN_ATTACK], 33);
		} else {
			DrawCel(591, 634, pMultiBtns, 5+panbtn[PBTN_ATTACK], 33);
		}
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetupSpellSel()
{
	int x,y,i,j;
	__int64 mask,spl;
	int cx, cy;

	spselflag = TRUE;
	x = SPLICONRIGHT;
	y = 495;
	cx = x - (64 - (SPLICONSIZE >> 1));
	cy = y - (160 + (SPLICONSIZE >> 1));
	if (plr[myplr]._pRSpell != -1) {
		for (j = 0; j < 4; j++) {
			switch(j) {
				case 0:
					spl = plr[myplr]._pAblSpells;
					break;
				case 1:
					spl = plr[myplr]._pMemSpells;
					break;
				case 2:
					spl = plr[myplr]._pScrlSpells;
					break;
				case 3:
					spl = plr[myplr]._pISpells;
					break;
			}
			mask = 1;
			for (i = 1; i < SPL_LAST; i++) {
				if ((spl & mask) != 0) {
					if ((i == plr[myplr]._pRSpell) && (j == plr[myplr]._pRSplType)) {
						cx = x - (64 - (SPLICONSIZE >> 1));
						cy = y - (160 + (SPLICONSIZE >> 1));
					}
					x -= SPLICONSIZE;
					if (x == SPLICONLEFT) {
						x = SPLICONRIGHT;
						y -= SPLICONSIZE;
					}
				}
				mask = mask << 1;
			}
			if (spl && (x != SPLICONRIGHT)) x -= SPLICONSIZE;
			if (x == SPLICONLEFT) {
				x = SPLICONRIGHT;
				y -= SPLICONSIZE;
			}
		}
	}
	SetCursorPos(cx,cy);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CheckPanelBtns()
{
	for (int i = 0; i < numpanbtns; i++) {
		int x2 = PanBtnPos[i][0] + PanBtnPos[i][2];
		int y2 = PanBtnPos[i][1] + PanBtnPos[i][3];
		if ((MouseX >= PanBtnPos[i][0]) && (MouseX <= x2) && (MouseY >= PanBtnPos[i][1]) && (MouseY <= y2)) {
			panbtn[i] = TRUE;
			drawbtnflag = TRUE;
			panbtndown = TRUE;
		}
	}

	if (!spselflag) {
		if (MouseX >= 565 && MouseX < 621 && MouseY >= 416 && MouseY < 472) {
			SetupSpellSel();
			gamemenu_off();
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void ActivatePanelBtn(int i)
{
	panbtn[i] = TRUE;
	drawbtnflag = TRUE;
	panbtndown = TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CheckDeadButtons()
{
	int x2, y2;

	x2 = PanBtnPos[PBTN_MENU][0] + PanBtnPos[PBTN_MENU][2];
	y2 = PanBtnPos[PBTN_MENU][1] + PanBtnPos[PBTN_MENU][3];
	if ((MouseX >= PanBtnPos[PBTN_MENU][0]) && (MouseX <= x2) && (MouseY >= PanBtnPos[PBTN_MENU][1]) && (MouseY <= y2))
		ActivatePanelBtn(PBTN_MENU);
	x2 = PanBtnPos[PBTN_TALK][0] + PanBtnPos[PBTN_TALK][2];
	y2 = PanBtnPos[PBTN_TALK][1] + PanBtnPos[PBTN_TALK][3];
	if ((MouseX >= PanBtnPos[PBTN_TALK][0]) && (MouseX <= x2) && (MouseY >= PanBtnPos[PBTN_TALK][1]) && (MouseY <= y2))
		ActivatePanelBtn(PBTN_TALK);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DoAutoMap() {
	// if we are multiplayer on the town level, DO NOT display
	// the error message.  Instead, the automap will show the
	// game name and password
	if (currlevel == 0 && gbMaxPlayers == 1) {
		InitDiabloMsg(MSG_AMAPTWN);
		return;
	}

	// toggle automap
	if (!automapflag) StartAutomap();
	else automapflag = FALSE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CheckPanelInfo()
{
	int i;
	int x2, y2;
	int pSpell, c, s;
	int v;

	panelflag = FALSE;
	ClearPanel();

	for (i = 0; i < numpanbtns; i++) {
		x2 = PanBtnPos[i][0] + PanBtnPos[i][2];
		y2 = PanBtnPos[i][1] + PanBtnPos[i][3];
		if ((MouseX >= PanBtnPos[i][0]) && (MouseX <= x2) && (MouseY >= PanBtnPos[i][1]) && (MouseY <= y2)) {
			if (i != PBTN_ATTACK)
				strcpy(infostr, PanBtnStr[i]);
			else if (FriendlyMode)
				strcpy(infostr, "Player friendly");
			else
				strcpy(infostr, "Player attack");
			if (PanBtnHotKey[i] != NULL) {
				sprintf(tempstr, "Hotkey : %s", PanBtnHotKey[i]);
				AddPanelString(tempstr, TEXT_CENTER);
			}
			infoclr = ICOLOR_WHITE;
			panelflag = TRUE;
			pinfoflag = TRUE;
		}
	}

	if (!spselflag) {
		if ((MouseX >= 565) && (MouseX < 621) && (MouseY >= 416) && (MouseY < 472)) {
			strcpy(infostr, "Select current spell button");
			infoclr = ICOLOR_WHITE;
			panelflag = TRUE;
			pinfoflag = TRUE;
			strcpy(tempstr, "Hotkey : 's'");
			AddPanelString(tempstr, TEXT_CENTER);
			pSpell = plr[myplr]._pRSpell;
			if (pSpell != -1) {
				switch (plr[myplr]._pRSplType) {
					case 0:
						sprintf(tempstr, "%s Skill", spelldata[pSpell].sSkillText);
						AddPanelString(tempstr, TEXT_CENTER);
						break;
					case 1:
						sprintf(tempstr, "%s Spell", spelldata[pSpell].sNameText);
						AddPanelString(tempstr, TEXT_CENTER);
						v =  plr[myplr]._pSplLvl[pSpell]+plr[myplr]._pISplLvlAdd;
						if ( v < 0 ) v = 0;
						if (v == 0) sprintf(tempstr, "Spell Level 0 - Unusable");
						else sprintf(tempstr, "Spell Level %i", v);
						AddPanelString(tempstr, TEXT_CENTER);
						break;
					case 2:
						sprintf(tempstr, "Scroll of %s", spelldata[pSpell].sNameText);
						AddPanelString(tempstr, TEXT_CENTER);
						c = 0;
						for (s = 0; s < plr[myplr]._pNumInv; s++) {
							if ((plr[myplr].InvList[s]._itype != -1) &&
								((plr[myplr].InvList[s]._iMiscId == IMID_SCROLL) ||
								(plr[myplr].InvList[s]._iMiscId == IMID_TSCROLL))) {
									if (plr[myplr].InvList[s]._iSpell == pSpell) c++;
							}
						}
						for (s = 0; s < MAXSPD; s++) {
							if ((plr[myplr].SpdList[s]._itype != -1) &&
								((plr[myplr].SpdList[s]._iMiscId == IMID_SCROLL) ||
								(plr[myplr].SpdList[s]._iMiscId == IMID_TSCROLL))) {
									if (plr[myplr].SpdList[s]._iSpell == pSpell) c++;
							}
						}
						if (c == 1) strcpy(tempstr, "1 Scroll");
						else sprintf(tempstr, "%i Scrolls", c);
						AddPanelString(tempstr, TEXT_CENTER);
						break;
					case 3:
						sprintf(tempstr, "Staff of %s", spelldata[pSpell].sNameText);
						AddPanelString(tempstr, TEXT_CENTER);
						if (plr[myplr].Hand1Item._iCharges == 1) strcpy(tempstr, "1 Charge");
						else sprintf(tempstr, "%i Charges", plr[myplr].Hand1Item._iCharges);
						AddPanelString(tempstr, TEXT_CENTER);
						break;
				}
			}
		}
	}

	if ((MouseX > 190) && (MouseX < 437) && (MouseY > 356) && (MouseY < 385))
		cursinvitem = CheckInvHLight();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void ReleasePanelBtn() {
	BYTE bMenuOff = TRUE;

	drawbtnflag = TRUE;
	panbtndown = FALSE;
	for (int i = 0; i < NUMPBTNS; i++) {
		if (! panbtn[i]) continue;
		panbtn[i] = FALSE;

		// was the mouseup in this button?
		if (MouseX < PanBtnPos[i][0]) continue;
		if (MouseX > PanBtnPos[i][0] + PanBtnPos[i][2]) continue;
		if (MouseY < PanBtnPos[i][1]) continue;
		if (MouseY > PanBtnPos[i][1] + PanBtnPos[i][3]) continue;

		switch(i) {
			case PBTN_CHR :
				questlog = FALSE;
				chrflag = !chrflag;
			break;
			case PBTN_QUEST:
				chrflag = FALSE;
				if (!questlog) StartQuestlog();
				else questlog = FALSE;
			break;
			case PBTN_AMAP :
				DoAutoMap();
			break;
			case PBTN_MENU :
				qtextflag = FALSE;
				gamemenu_toggle();
				bMenuOff = FALSE;
			break;
			case PBTN_INV :
				sbookflag = FALSE;
				invflag = !invflag;
				if (dropGoldFlag) {
					dropGoldFlag = FALSE;
					dropGoldValue = 0;
				}
			break;
			case PBTN_SBOOK :
				invflag = FALSE;
				if (dropGoldFlag) {
					dropGoldFlag = FALSE;
					dropGoldValue = 0;
				}
				sbookflag = !sbookflag;
			break;
			case PBTN_TALK:
				if (talkflag) TalkEnd();
				else TalkStart();
			break;
			case PBTN_ATTACK:
				FriendlyMode = !FriendlyMode;
			break;
		}
	}

	if (bMenuOff) gamemenu_off();
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void FreeControlPan()
{
	DiabloFreePtr(pBtmBuff);
	DiabloFreePtr(pManaBuff);
	DiabloFreePtr(pLifeBuff);
	DiabloFreePtr(pPanelText);
	DiabloFreePtr(pChrPanel);
	DiabloFreePtr(pSpellCels);
	DiabloFreePtr(pPanelButtons);
	DiabloFreePtr(pMultiBtns);
	DiabloFreePtr(pTalkBtns);
	DiabloFreePtr(pChrButtons);
	DiabloFreePtr(pDurIcons);
	DiabloFreePtr(pQLogCel);
	DiabloFreePtr(pSpellBkCel);
	DiabloFreePtr(pSBkBtnCel);
	DiabloFreePtr(pSBkIconCels);
	DiabloFreePtr(pGBoxBuff);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

BOOL InfoFit(const char * p) {
	long tw = 0;
	while (*p) {
		BYTE c = char2print(*p++);
		c = fonttrans[c];
		tw += fontkern[c];
		if (tw >= 125) return FALSE;
	}

	return TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void CPrintString(int l,const char * pszStr, int just, int pnl) {
	long boffset = fontofs[pnl][l];

	int w = 0;
	if (just == TEXT_CENTER) {
		long tw = 0;
		const char * pszTemp = pszStr;
		while (*pszTemp) {
			BYTE c = char2print(*pszTemp++);
			c = fonttrans[c];
			tw += fontkern[c] + 2;
		}

		if (tw < 288) w = (288 - tw) >> 1;
		boffset += w;
	}

	while (*pszStr) {
		BYTE c = char2print(*pszStr++);
		c = fonttrans[c];
		w += fontkern[c] + 2;
		if (c && (w < 288)) DrawPanelFont(boffset, c, infoclr);
		boffset += fontkern[c] + 2;
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void PrintInfo() {
	if (talkflag) return;

	int nOffset1 = 0;
	int nOffset2 = 1;
	if (infostr[0] != 0) {
		CPrintString(0, infostr, TEXT_CENTER, pnumlines);
		nOffset1 = 1;
		nOffset2 = 0;
	}

	for (int i = 0; i < pnumlines; i++)
		CPrintString(i+nOffset1, panelstr[i], pstrjust[i], pnumlines-nOffset2);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DrawInfoBox()
{
	// Erase old box
	CopyCtrlPan(177, 62, 288, 60, 241, 558);

	if ((!panelflag) && (!trigflag) && (cursinvitem == -1) && (!spselflag)) {
		infostr[0] = 0;
		infoclr = ICOLOR_WHITE;
		ClearPanel();
	}

	if ((spselflag) || (trigflag)) {
		infoclr = ICOLOR_WHITE;
	} else {
		if (curs >= ICSTART) {
			if (plr[myplr].HoldItem._itype == IT_GOLD) {
				int nGold = plr[myplr].HoldItem._ivalue;
				const char * get_pieces_str(int nGold);
				sprintf(infostr,"%i gold %s",nGold,get_pieces_str(nGold));
			}
			else if (!plr[myplr].HoldItem._iStatFlag) {
				ClearPanel();
				AddPanelString("Requirements not met", TEXT_CENTER);
				pinfoflag = TRUE;
			}
			else {
				if (plr[myplr].HoldItem._iIdentified) strcpy(infostr, plr[myplr].HoldItem._iIName);
				else strcpy(infostr, plr[myplr].HoldItem._iName);
				if (plr[myplr].HoldItem._iMagical == IMAGIC_MAGIC) infoclr = ICOLOR_BLUE;
				if (plr[myplr].HoldItem._iMagical == IMAGIC_UNIQUE) infoclr = ICOLOR_GOLD;
			}
		} else {
			// if cursinvitem != -1 then the string will already be in infostr
			// if trigflag then the string will already be set as well
			if (cursitem != -1) {
	// @@@ drb debug
#if 0

int GetLDeltaItem();

	ClearPanel();
	ItemStruct *pi = &item[cursitem];
	strcpy(infostr, pi->_iName);
	sprintf(tempstr, "Delta # = %i", GetLDeltaItem());
	AddPanelString(tempstr, TEXT_CENTER);
	//sprintf(tempstr, "seed = %i", pi->_iSeed);
	//AddPanelString(tempstr, TEXT_CENTER);
	// @@@ drb end debug
#else
				GetItemStr(cursitem);
#endif
			}
			if (cursobj != -1) GetObjectStr(cursobj);
			if (cursmonst != -1) {
				if (leveltype != 0) {
					infoclr = ICOLOR_WHITE;
					strcpy(infostr, monster[cursmonst].mName);
					ClearPanel();
#if DISABLED_CHEATS
					if (1) {
						sprintf(tempstr, "HP: %08X/%08X",
							monster[cursmonst]._mhitpoints,
							monster[cursmonst]._mmaxhp);
						AddPanelString(tempstr, TEXT_CENTER);
						sprintf(tempstr, "Res: %02X",
							monster[cursmonst].mMagicRes);
						pinfoflag = TRUE;
					} else
#endif
					if (monster[cursmonst]._uniqtype != 0) {
						infoclr = ICOLOR_GOLD;
						void PrintUniqueHistory();
						PrintUniqueHistory();
					} else PrintMonstHistory(monster[cursmonst].MType->mtype);
				} else strcpy(infostr, towner[cursmonst]._tName);
			}
			if (cursplr != -1) {
				infoclr = ICOLOR_GOLD;
				strcpy(infostr, plr[cursplr]._pName);
				ClearPanel();
				sprintf(tempstr, "%s, Level : %i", ClassStrTbl[plr[cursplr]._pClass], plr[cursplr]._pLevel);
				AddPanelString(tempstr, TEXT_CENTER);
				sprintf(tempstr, "Hit Points %i of %i", (plr[cursplr]._pHitPoints >> HP_SHIFT), (plr[cursplr]._pMaxHP >> HP_SHIFT));
				AddPanelString(tempstr, TEXT_CENTER);
			}
		}
	}
	//if ((pinfoflag) && (cursmonst == -1) && (cursinvitem == -1) && (curs < ICSTART)) ClearPanel();
	if ((infostr[0] != 0) || (pnumlines != 0)) PrintInfo();
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void PlrStringXY(int x1, int y, int x2,const char * pszStr,char col) {
	long boffset = nBuffWTbl[y + 160] + x1 + 64;
	int aw = x2 - x1 + 1;
	int w = 0;

	// calculate string width
	int tw = 0;
	const char * pszTemp = pszStr;
	while (*pszTemp) {
		BYTE c = char2print(*pszTemp++);
		c = fonttrans[c];
		tw += fontkern[c] + 1;
	}

	if (tw < aw) w = (aw - tw) >> 1;
	boffset += w;

	while (*pszStr) {
		BYTE c = char2print(*pszStr++);
		c = fonttrans[c];
		w += fontkern[c] + 1;
		if (c && w < aw) DrawPanelFont(boffset, c, col);
		boffset += fontkern[c] + 1;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void PrintStringXY(int x, int y,const char * pszStr, char col) {
	long boffset = nBuffWTbl[y + 160] + x + 64;
	while (*pszStr) {
		BYTE c = char2print(*pszStr++);
		c = fonttrans[c];
		if (c) DrawPanelFont(boffset, c, col);
		boffset += fontkern[c] + 1;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void PlrStringXY2(int x1, int y, int x2,const char * pszStr,char col,int a) {
	long boffset = nBuffWTbl[y + 160] + x1 + 64;
	int aw = x2 - x1 + 1;
	int w = 0;

	// calculate string width
	int tw = 0;
	const char * pszTemp = pszStr;
	while (*pszTemp) {
		BYTE c = char2print(*pszTemp++);
		c = fonttrans[c];
		tw += fontkern[c] + a;
	}

	if (tw < aw) w = (aw - tw) >> 1;
	boffset += w;

	while (*pszStr) {
		BYTE c = char2print(*pszStr++);
		c = fonttrans[c];
		w += fontkern[c] + a;
		if (c && w < aw) DrawPanelFont(boffset, c, col);
		boffset += fontkern[c] + a;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DrawChr()
{
	char c;
	char chrstr[64];
	int pc;
	long mind, maxd;
	int hper, ac;

	DrawCel(64, 511, pChrPanel, 1, 320);

	PlrStringXY(20, 32, 151, plr[myplr]._pName, ICOLOR_WHITE);
	
	PlrStringXY(168, 32, 299, ClassStrTbl[plr[myplr]._pClass], ICOLOR_WHITE);
	
	#if 0
	if (plr[myplr]._pClass == CLASS_WARRIOR) PlrStringXY(168, 32, 299, "Warrior", ICOLOR_WHITE);
	#if !IS_VERSION(SHAREWARE)
	else if (plr[myplr]._pClass == CLASS_ROGUE) PlrStringXY(168, 32, 299, "Rogue", ICOLOR_WHITE);
	else if (plr[myplr]._pClass == CLASS_SORCEROR) PlrStringXY(168, 32, 299, "Sorceror", ICOLOR_WHITE);
	else if (plr[myplr]._pClass == CLASS_MONK) PlrStringXY(168, 32, 299, "Monk", ICOLOR_WHITE);
	else if (plr[myplr]._pClass == CLASS_BARD) PlrStringXY(168, 32, 299, "Bard", ICOLOR_WHITE);
	else if (plr[myplr]._pClass == CLASS_BARBARIAN) PlrStringXY(168, 32, 299, "Barbarian", ICOLOR_WHITE);
	#endif
	#endif

	sprintf(chrstr, "%i", plr[myplr]._pLevel);
	PlrStringXY(66, 69, 109, chrstr, ICOLOR_WHITE);

	sprintf(chrstr, "%li", plr[myplr]._pExperience);
	PlrStringXY(216, 69, 300, chrstr, ICOLOR_WHITE);

	if (plr[myplr]._pLevel == 50) {
		strcpy(chrstr, "None");
		c = ICOLOR_GOLD;
	} else {
		sprintf(chrstr, "%li", plr[myplr]._pNextExper);
		c = ICOLOR_WHITE;
	}
	PlrStringXY(216, 97, 300, chrstr, c);

	sprintf(chrstr, "%i", plr[myplr]._pGold);
	PlrStringXY(216, 146, 300, chrstr, ICOLOR_WHITE);

	c = ICOLOR_WHITE;
	if (plr[myplr]._pIBonusAC > 0) c = ICOLOR_BLUE;
	if (plr[myplr]._pIBonusAC < 0) c = ICOLOR_RED;
	// rjs ac = (byte)plr[myplr]._pArmorClass+plr[myplr]._pIAC+plr[myplr]._pIBonusAC;
	ac = plr[myplr]._pIAC+plr[myplr]._pIBonusAC;
	ac += (plr[myplr]._pDexterity / 5);
	sprintf(chrstr, "%i", ac);
	PlrStringXY(258, 183, 301, chrstr, c);

	c = ICOLOR_WHITE;
	if (plr[myplr]._pIBonusToHit > 0) c = ICOLOR_BLUE;
	if (plr[myplr]._pIBonusToHit < 0) c = ICOLOR_RED;
	hper = BASE_TO_HIT + (plr[myplr]._pDexterity >> 1) + plr[myplr]._pIBonusToHit;
	sprintf(chrstr, "%i%%", hper);
	PlrStringXY(258, 211, 301, chrstr, c);

	c = ICOLOR_WHITE;
	if (plr[myplr]._pIBonusDam > 0) c = ICOLOR_BLUE;
	if (plr[myplr]._pIBonusDam < 0) c = ICOLOR_RED;
	mind = plr[myplr]._pIMinDam;
	mind += (mind * plr[myplr]._pIBonusDam) / 100;
	mind += plr[myplr]._pIBonusDamMod;
	if (plr[myplr].Hand1Item._itype == IT_BOW) {
		if (plr[myplr]._pClass == CLASS_ROGUE) mind += plr[myplr]._pDamageMod;
		else mind += (plr[myplr]._pDamageMod >> 1);
	} else mind += plr[myplr]._pDamageMod;
	maxd = plr[myplr]._pIMaxDam;
	maxd += (maxd * plr[myplr]._pIBonusDam) / 100;
	maxd += plr[myplr]._pIBonusDamMod;
	if (plr[myplr].Hand1Item._itype == IT_BOW) {
		if (plr[myplr]._pClass == CLASS_ROGUE) maxd += plr[myplr]._pDamageMod;
		else maxd += (plr[myplr]._pDamageMod >> 1);
	} else maxd += plr[myplr]._pDamageMod;
	sprintf(chrstr, "%i-%i", mind, maxd);
	if ((mind >= 100) || (maxd >= 100))
		PlrStringXY2(254, 239, 305, chrstr, c, -1);
	else
		PlrStringXY2(258, 239, 301, chrstr, c, 0);

	// Magic Resist
	if (plr[myplr]._pMagResist == 0) c = ICOLOR_WHITE;
	else c = ICOLOR_BLUE;
	if (plr[myplr]._pMagResist < RESIST_MAX) sprintf(chrstr, "%i%%", plr[myplr]._pMagResist);
	else {
		c = ICOLOR_GOLD;
		sprintf(chrstr, "MAX");
	}
	PlrStringXY(257, 276, 300, chrstr, c);

	// Fire Resist
	if (plr[myplr]._pFireResist == 0) c = ICOLOR_WHITE;
	else c = ICOLOR_BLUE;
	if (plr[myplr]._pFireResist < RESIST_MAX) sprintf(chrstr, "%i%%", plr[myplr]._pFireResist);
	else {
		c = ICOLOR_GOLD;
		sprintf(chrstr, "MAX");
	}
	PlrStringXY(257, 304, 300, chrstr, c);

	// Lightning Resist
	if (plr[myplr]._pLghtResist == 0) c = ICOLOR_WHITE;
	else c = ICOLOR_BLUE;
	if (plr[myplr]._pLghtResist < RESIST_MAX) sprintf(chrstr, "%i%%", plr[myplr]._pLghtResist);
	else {
		c = ICOLOR_GOLD;
		sprintf(chrstr, "MAX");
	}
	PlrStringXY(257, 332, 300, chrstr, c);

	c = ICOLOR_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseStr);
	if (MaxStats[plr[myplr]._pClass][0] == plr[myplr]._pBaseStr) c = ICOLOR_GOLD;
	PlrStringXY( 95, 155, 126, chrstr, c);

	c = ICOLOR_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseMag);
	if (MaxStats[plr[myplr]._pClass][1] == plr[myplr]._pBaseMag) c = ICOLOR_GOLD;
	PlrStringXY( 95, 183, 126, chrstr, c);

	c = ICOLOR_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseDex);
	if (MaxStats[plr[myplr]._pClass][2] == plr[myplr]._pBaseDex) c = ICOLOR_GOLD;
	PlrStringXY( 95, 211, 126, chrstr, c);

	c = ICOLOR_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseVit);
	if (MaxStats[plr[myplr]._pClass][3] == plr[myplr]._pBaseVit) c = ICOLOR_GOLD;
	PlrStringXY( 95, 239, 126, chrstr, c);

	c = ICOLOR_WHITE;
	if (plr[myplr]._pStrength > plr[myplr]._pBaseStr) c = ICOLOR_BLUE;
	if (plr[myplr]._pStrength < plr[myplr]._pBaseStr) c = ICOLOR_RED;
	sprintf(chrstr, "%i", plr[myplr]._pStrength);
	PlrStringXY(143, 155, 173, chrstr, c);

	c = ICOLOR_WHITE;
	if (plr[myplr]._pMagic > plr[myplr]._pBaseMag) c = ICOLOR_BLUE;
	if (plr[myplr]._pMagic < plr[myplr]._pBaseMag) c = ICOLOR_RED;
	sprintf(chrstr, "%i", plr[myplr]._pMagic);
	PlrStringXY(143, 183, 173, chrstr, c);

	c = ICOLOR_WHITE;
	if (plr[myplr]._pDexterity > plr[myplr]._pBaseDex) c = ICOLOR_BLUE;
	if (plr[myplr]._pDexterity < plr[myplr]._pBaseDex) c = ICOLOR_RED;
	sprintf(chrstr, "%i", plr[myplr]._pDexterity);
	PlrStringXY(143, 211, 173, chrstr, c);

	c = ICOLOR_WHITE;
	if (plr[myplr]._pVitality > plr[myplr]._pBaseVit) c = ICOLOR_BLUE;
	if (plr[myplr]._pVitality < plr[myplr]._pBaseVit) c = ICOLOR_RED;
	sprintf(chrstr, "%i", plr[myplr]._pVitality);
	PlrStringXY(143, 239, 173, chrstr, c);

	// drb.patch1.start.2/05/97
	if (plr[myplr]._pStatPts > 0) {
		int CalcStatDiff(int);
		if (CalcStatDiff(myplr) < plr[myplr]._pStatPts) plr[myplr]._pStatPts = CalcStatDiff(myplr);
	}
	// drb.patch1.end.2/05/97
	// Points to distibute
	if (plr[myplr]._pStatPts > 0) {
		sprintf(chrstr, "%i", plr[myplr]._pStatPts);
		PlrStringXY(95, 266, 126, chrstr, ICOLOR_RED);// check x y  
		pc = plr[myplr]._pClass;
		if (plr[myplr]._pBaseStr < MaxStats[pc][0]) DrawCel(201, 319, pChrButtons, 2+chrbtn[0], 41);
		if (plr[myplr]._pBaseMag < MaxStats[pc][1]) DrawCel(201, 347, pChrButtons, 4+chrbtn[1], 41);
		if (plr[myplr]._pBaseDex < MaxStats[pc][2]) DrawCel(201, 376, pChrButtons, 6+chrbtn[2], 41);
		if (plr[myplr]._pBaseVit < MaxStats[pc][3]) DrawCel(201, 404, pChrButtons, 8+chrbtn[3], 41);
	}

	if (plr[myplr]._pMaxHP > plr[myplr]._pMaxHPBase) c = ICOLOR_BLUE;
	else c = ICOLOR_WHITE;
	sprintf(chrstr, "%i", (plr[myplr]._pMaxHP >> HP_SHIFT));
	PlrStringXY( 95, 304, 126, chrstr, c);
	if (plr[myplr]._pHitPoints != plr[myplr]._pMaxHP) c = ICOLOR_RED;
	sprintf(chrstr, "%i", (plr[myplr]._pHitPoints >> HP_SHIFT));
	PlrStringXY(143, 304, 174, chrstr, c);

	if (plr[myplr]._pMaxMana > plr[myplr]._pMaxManaBase) c = ICOLOR_BLUE;
	else c = ICOLOR_WHITE;
	sprintf(chrstr, "%i", (plr[myplr]._pMaxMana >> MANA_SHIFT));
	PlrStringXY( 95, 332, 126, chrstr, c);
	if (plr[myplr]._pMana != plr[myplr]._pMaxMana) c = ICOLOR_RED;
	sprintf(chrstr, "%i", (plr[myplr]._pMana >> MANA_SHIFT));
	PlrStringXY(143, 332, 174, chrstr, c);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void CheckLvlBtn() {
	if (lvlbtndown) return;
	if ((MouseX >= 40) && (MouseX <= 81) && (MouseY >= 313) && (MouseY <= 335))
		lvlbtndown = TRUE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void ReleaseLvlBtn() {
	if ((MouseX >= 40) && (MouseX <= 81) && (MouseY >= 313) && (MouseY <= 335))
		chrflag = TRUE;
	lvlbtndown = FALSE;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DrawLevelUpIcon() {
	int c;

	if (stextflag != STORE_NONE) return;
	if (lvlbtndown) c = 3;
	else c = 2;
	PlrStringXY(0, 303, 120, "Level Up", ICOLOR_WHITE);
	DrawCel(104, 495, pChrButtons, c, 41);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void CheckChrBtns() {
	if (chrbtndown) return;
	if (! plr[myplr]._pStatPts) return;

	int pc = plr[myplr]._pClass;
	for (int i = 0; i < NUMCBTNS; i++) {
		switch(i) {
			case 0:
				if (plr[myplr]._pBaseStr >= MaxStats[pc][i])
					continue;
			break;

			case 1:
				if (plr[myplr]._pBaseMag >= MaxStats[pc][i])
					continue;
			break;

			case 2:
				if (plr[myplr]._pBaseDex >= MaxStats[pc][i])
					continue;
			break;

			case 3:
				if (plr[myplr]._pBaseVit >= MaxStats[pc][i])
					continue;
			break;

			default:
			continue;
		}

		int x2 = ChrBtnPos[i][0] + ChrBtnPos[i][2];
		int y2 = ChrBtnPos[i][1] + ChrBtnPos[i][3];
		if ((MouseX >= ChrBtnPos[i][0]) && (MouseX <= x2) && (MouseY >= ChrBtnPos[i][1]) && (MouseY <= y2)) {
			chrbtn[i] = TRUE;
			chrbtndown = TRUE;
		}
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void ReleaseChrBtn() {
	chrbtndown = FALSE;
	for (int i = 0; i < NUMCBTNS; i++) {
		if (! chrbtn[i]) continue;
		chrbtn[i] = FALSE;

		// was mouseup inside button?
		if (MouseX < ChrBtnPos[i][0]) continue;
		if (MouseX > ChrBtnPos[i][0] + ChrBtnPos[i][2]) continue;
		if (MouseY < ChrBtnPos[i][1]) continue;
		if (MouseY > ChrBtnPos[i][1] + ChrBtnPos[i][3]) continue;

		switch(i) {
			case CBTN_STR :
				NetSendCmdParam1(TRUE,CMD_ADDSTR,1);
				plr[myplr]._pStatPts--;
			break;
			case CBTN_MAG :
				NetSendCmdParam1(TRUE,CMD_ADDMAG,1);
				plr[myplr]._pStatPts--;
			break;
			case CBTN_DEX :
				NetSendCmdParam1(TRUE,CMD_ADDDEX,1);
				plr[myplr]._pStatPts--;
			break;
			case CBTN_VIT :
				NetSendCmdParam1(TRUE,CMD_ADDVIT,1);
				plr[myplr]._pStatPts--;
			break;
		}
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static int DrawDurIcon4Item(const ItemStruct * pItem, int x, int c) {

	// don't need to draw icon if there is no item
	if (pItem->_itype == -1) return x;

	// don't need to draw icon if durability is high
	if (pItem->_iDurability > 5) return x;

	if (c == 0) {
		if (pItem->_iClass == IC_WEAP) switch(pItem->_itype) {
			case IT_SWORD:
				c = 2;
			break;
			case IT_AXE:
				c = 6;
			break;
			case IT_BOW:
				c = 7;
			break;
			case IT_MACE:
				c = 5;
			break;
			case IT_STAFF:
				c = 8;
			break;
		}
		else {
			c = 1;
		}
	}

	// choose cel to draw based on durability
	if (pItem->_iDurability > 2) c += 8;

	// draw it
	DrawCel(x, 495, pDurIcons, c, 32);

	// adjust position for next durability icon
	return x - 40;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DrawDurIcon() {
	if ((chrflag || questlog) && (invflag || sbookflag)) return;

	int x = 656;
	if (invflag || sbookflag) x -= 320;
	const PlayerStruct * p = &plr[myplr];
	x = DrawDurIcon4Item(&p->InvBody[INVLOC_HEAD],x,4);
	x = DrawDurIcon4Item(&p->InvBody[INVLOC_BODY],x,3);
	x = DrawDurIcon4Item(&p->InvBody[INVLOC_HAND1],x,0);
	x = DrawDurIcon4Item(&p->InvBody[INVLOC_HAND2],x,0);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void RedBack() {
	long ltaboff;

	if (light4flag) ltaboff = 1536;
	else ltaboff = 4608;

	app_assert(gpBuffer);
	if (leveltype != 4) {
		__asm {
			mov		edi,dword ptr [gpBuffer]
			add		edi,122944

			mov		ebx,dword ptr [pLightTbl]
			add		ebx,dword ptr [ltaboff]

			mov		edx,352
_YLp:		mov		ecx,640
_XLp:		mov		al,byte ptr [edi]
			xlatb
			stosb
			loop	_XLp
			add		edi,128
			dec		edx
			jnz		_YLp
		}
	} else {
		__asm {
			mov		edi,dword ptr [gpBuffer]
			add		edi,122944

			mov		ebx,dword ptr [pLightTbl]
			add		ebx,dword ptr [ltaboff]

			mov		edx,352
_YLp2:		mov		ecx,640
_XLp2:		mov		al,byte ptr [edi]
			cmp		al,32
			jb		_Skip
			xlatb
_Skip:		stosb
			loop	_XLp2
			add		edi,128
			dec		edx
			jnz		_YLp2
		}
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void PrintSBookStr(int x, int y, BOOL cjustflag,const char * pszStr, char col) {
	long boffset = nBuffWTbl[y] + x + 440;
	int w = 0;

	if (cjustflag) {
		int tw = 0;
		const char * pszTemp = pszStr;
		while (*pszTemp) {
			BYTE c = char2print(*pszTemp++);
			c = fonttrans[c];
			tw += fontkern[c] + 1;
		}

		if (tw < 222) w = (222 - tw) >> 1;
		boffset += w;
	}

	while (*pszStr) {
		BYTE c = char2print(*pszStr++);
		c = fonttrans[c];
		w += fontkern[c] + 1;
		if (c && w <= 222) DrawPanelFont(boffset, c, col);
		boffset += fontkern[c] + 1;
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
char GetSBookTrans(int ii, BOOL townok) {
	char st, sl;

	// hack hack hack
	if (plr[myplr]._pClass == CLASS_MONK && ii == SPL_SHOWMAGITEMS)
		return STRN_GOLD;
		
	st = STRN_BLUE;
	if (plr[myplr]._pISpells & (((__int64)1) << (ii-1))) st = STRN_ORANGE;
	if (plr[myplr]._pAblSpells & (((__int64)1) << (ii-1))) st = STRN_GOLD;
	if (st == STRN_BLUE) {
		if (!CheckSpell(myplr, ii, SPT_MEMORIZED, TRUE)) st = STRN_GREY;
		sl = plr[myplr]._pSplLvl[ii] + plr[myplr]._pISplLvlAdd;
		if (sl <= 0 ) st = STRN_GREY;
	}
	if ((townok) && (currlevel == 0) && (st != STRN_GREY) && (spelldata[ii].sTownSpell == FALSE)) st = STRN_GREY;
	return(st);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DrawSpellBook()
{
	int i, ii, y, mind, maxd;
	__int64 tspls;
	char st;
	int v;

	// Drawbackground
	DrawCel(384, 511, pSpellBkCel, 1, 320);

	// Draw tab
	//x = (sbooktab * 51) + 391;
	//DrawCel(x, 508, pSBkBtnCel, 1 + sbooktab, 51);
//	x = (sbooktab * 76) + 391;
//	DrawCel(x, 508, pSBkBtnCel, 1 + sbooktab, 76);
	
	if (sbooktab < NUMSPBKBTNS)
	{
		long x = (sbooktab * SPBKBTNWDTH) + 391;
		DrawCel(x, 508, pSBkBtnCel, sbooktab + 1, SPBKBTNWDTH);
	}

	// Draw Spell Icons
	y = 215;
	tspls = plr[myplr]._pISpells | plr[myplr]._pMemSpells | plr[myplr]._pAblSpells;
	for (i = 1; i < 8; i++) {
		ii = SpellPages[sbooktab][i-1];
		if (ii != -1) {
			if (tspls & (((__int64) 1) << (ii-1))) {
				st = GetSBookTrans(ii, TRUE);
				SetSpellTrans(st);
				DrawSpellCel(395, y, pSBkIconCels, SpellITbl[ii], 37);
				if ((ii == plr[myplr]._pRSpell) && (st == plr[myplr]._pRSplType)) {
					SetSpellTrans(STRN_GOLD);
					DrawSpellCel(395, y, pSBkIconCels, SI_LAST, 37);
				}
				PrintSBookStr(10, y-23, FALSE, spelldata[ii].sNameText, ICOLOR_WHITE);
				st = GetSBookTrans(ii, FALSE);
				switch (st) {
					case STRN_GOLD:
						strcpy(tempstr, "Skill");
						break;
					case STRN_ORANGE:
						sprintf(tempstr, "Staff (%i charges)", plr[myplr].Hand1Item._iCharges);
						break;
					default:
						v = GetManaAmount(myplr, ii) >> MANA_SHIFT;
						GetDamageAmt(ii, &mind, &maxd);
						if (mind != -1) sprintf(tempstr, "Mana: %i  Dam: %i - %i", v, mind, maxd);
						else sprintf(tempstr, "Mana: %i   Dam: n/a", v);
						if (ii == SPL_BONESPIRIT) sprintf(tempstr, "Mana: %i  Dam: 1/3 tgt hp", v);
						PrintSBookStr(10, y-1, FALSE, tempstr, ICOLOR_WHITE);

						v =  plr[myplr]._pSplLvl[ii]+plr[myplr]._pISplLvlAdd;
						if ( v < 0 ) v = 0;
						if (v == 0) sprintf(tempstr, "Spell Level 0 - Unusable");
						else sprintf(tempstr, "Spell Level %i", v);
						break;
				}
				PrintSBookStr(10, y-12, FALSE, tempstr, ICOLOR_WHITE);
			}
		}
		y += 43;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void CheckSBook()
{
	int spl;
	__int64 tspls;
	char st;

	// Check spell icon
	if ((MouseX >= 331) && (MouseX < 368) && (MouseY >= 18) && (MouseY < 314)) {
		spl = (MouseY - 18) / 43;
		spl = SpellPages[sbooktab][spl];
		tspls = plr[myplr]._pISpells | plr[myplr]._pMemSpells | plr[myplr]._pAblSpells;
		if (spl != -1) {
			if (tspls & (((__int64)1) << (spl-1))) {
				st = SPT_MEMORIZED;
				if (plr[myplr]._pISpells & (((__int64)1) << (spl-1))) st = SPT_ITEM;
				if (plr[myplr]._pAblSpells & (((__int64)1) << (spl-1))) st = SPT_ABILITY;

				plr[myplr]._pRSpell = spl;
				plr[myplr]._pRSplType = st;
				force_redraw = FULLDRAW;
			}
		}
	}
	// Check spell tabs
	if ((MouseX >= 327) && (MouseX < 327 + SPBKBTNWDTH * NUMSPBKBTNS) &&
	    (MouseY >= 320) && (MouseY < 349))
	{
		sbooktab = (MouseX - 327) / SPBKBTNWDTH;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*

BOOL CheckSBookCast()
{
	int spl;
	__int64 tspls;
	char st;
	BOOL okflag;

	if (currlevel == 0) return(FALSE);
	// Check spell icon
	if ((MouseX >= 327) && (MouseX < 368) && (MouseY >= 29) && (MouseY < 346)) {
		spl = (MouseY - 29) / 46;
		spl = SpellPages[sbooktab][spl];
		tspls = plr[myplr]._pISpells | plr[myplr]._pMemSpells | plr[myplr]._pAblSpells;
		if (spl != -1) {
			if (tspls & (1 << (spl-1))) {
				st = SPT_MEMORIZED;
				if (plr[myplr]._pISpells & (((__int64)1) << (spl-1))) st = SPT_ITEM;
				if (plr[myplr]._pAblSpells & (1 << (spl-1))) st = SPT_ABILITY;
				okflag = FALSE;
				switch (st) {
					case SPT_ABILITY :
					case SPT_MEMORIZED :
						okflag = CheckSpell(myplr, spl, st, FALSE);
						break;
					case SPT_ITEM:
						okflag = UseStaffSBook(spl);
						break;
				}
				if (okflag) {
					if (spelldata[spl].sTargeted) {
						plr[myplr]._pTSpell = spl;
						plr[myplr]._pTSplType = st;
						NewCursor(TARGET_CURS);
					}
					else {
						plr[myplr]._pSBkSpell = spl;
						plr[myplr]._pSBkSplType = st;
						NetSendCmdParam1(TRUE,CMD_SBSPELL,plr[myplr]._pSBkSpell);
					}
					return(TRUE);
				}
			}
		}
	}
	return(FALSE);
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
const char * get_pieces_str(int nGold) {
	if (nGold == 1) return "piece";
	return "pieces";
}

	
/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void DrawGoldBox(int gold)
{
	int		i;
	long	xOffset;

	// Initialize
	xOffset = 0;
	
	// Draw the box
	DrawCel(415, 338, pGBoxBuff, 1, 261);
	// Output the strings
	sprintf(tempstr, "You have %u gold",initialDropGoldValue);
	PlrStringXY(366, 87, 600, tempstr, ICOLOR_GOLD);
	sprintf(tempstr, "%s.  How many do", get_pieces_str(initialDropGoldValue));
	PlrStringXY(366, 103, 600, tempstr, ICOLOR_GOLD);
	PlrStringXY(366, 121, 600, "you want to remove?", ICOLOR_GOLD);

	if (gold > 0) {
		sprintf(tempstr, "%u", gold);
		PrintStringXY(388, 140, tempstr, ICOLOR_WHITE);
	} 
	
	// Get x offset for pentagram
	if (gold > 0) {
		for (i = 0; i < tempstr[i] != 0; i++) {
			BYTE c = char2print(tempstr[i]);
			c = fonttrans[c];
			xOffset += fontkern[c]+1;
		} 
		xOffset += 452;
	} else xOffset = 450;

	// Draw the pentagram
	DrawCel(xOffset, 300, pSTextSpinCels, pentaspin, 12);
	// Increment the pentagram spinner
	pentaspin = (pentaspin & 0x7) + 1;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

// macro to convert char to int
#define	ATON(c) (c - 0x30)
// keyboard constants
#define DG_CR	0x0d
#define DG_ESC	0x1b
#define DG_BACK	0x08

void DropGoldType(char c) //, int ivalue)
{
	char	dGoldStr[6];

	// Check if player is dead. This should prevent the multi player death frame
	// gold cheat.
	if ((plr[myplr]._pHitPoints >> HP_SHIFT) <= 0) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
		return;
	}

	// Clear the string
	memset(dGoldStr, 0x00, sizeof(dGoldStr));
	// Convert number to string
	_itoa(dropGoldValue, dGoldStr, 10);	

	// Carriage Return was pressed so the player wants to drop some gold
	if (c == DG_CR)	{
		if (dropGoldValue > 0) DropGold(myplr, initialDropGoldIndex);
		dropGoldFlag = FALSE;
		return;
	}
	// Escape was pressed so the player wants to quit w/out dropping gold
	if (c == DG_ESC) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
		return;
	} 
	// Backspace was pressed so clear the last char in the string
	if (c == DG_BACK) {
        dGoldStr[strlen(dGoldStr)-1] = NULL;
        dropGoldValue = atoi(dGoldStr);
        return;
    } 
	// Check to see if the player typed a number and if so update the string
	if ((ATON(c) >= 0) && (ATON(c) <= 9)) {
		if ((dropGoldValue == 0) && (atoi(dGoldStr) > initialDropGoldValue))
			dGoldStr[0] = c;
		else {
			dGoldStr[strlen(dGoldStr)] = c;
			if ((atoi(dGoldStr) > initialDropGoldValue) || (strlen(dGoldStr) > strlen(dGoldStr)))
				return;
		} 
		dropGoldValue = atoi(dGoldStr);
		return;
	}
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DropGold(int pnum, int cii)
{
	int	c;

	if (cii <= 46) {
		// Item is in the InvList
		c = cii - 7;
		plr[pnum].InvList[c]._ivalue -= dropGoldValue;
		// Modify item cursor in the InvList
		if (plr[pnum].InvList[c]._ivalue > 0) SetGoldCurs(pnum, c);
		else RemoveInvItem(pnum, c);
	} else {
		// Item is in the SpdList
		c = cii - 47;
		plr[pnum].SpdList[c]._ivalue -= dropGoldValue;
		// Modify item cursor in the SpdList
		if (plr[pnum].SpdList[c]._ivalue > 0) SetSpdbarGoldCurs(pnum, c);
		else RemoveSpdBarItem(pnum, c);
	} 

	// Initialize hold item to gold type
	SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
	GetGoldSeed(pnum, &plr[pnum].HoldItem);
	// Set hold item values
	plr[pnum].HoldItem._ivalue = dropGoldValue;
	plr[pnum].HoldItem._iStatFlag = TRUE;
	// Set cursor arrow to gold 
	SetDropGoldCursor(pnum);
		
	// Recalculate players gold
	plr[pnum]._pGold = CalculateGold(pnum);

	dropGoldValue = 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void SetDropGoldCursor(int pnum)
{
	if (plr[pnum].HoldItem._ivalue >= GOLD_VT2) 
		plr[pnum].HoldItem._iCurs = ITEM_5GOLD;
	else {
		if (plr[pnum].HoldItem._ivalue <= GOLD_VT1) 
			plr[pnum].HoldItem._iCurs = ITEM_1GOLD;
		else 
			plr[pnum].HoldItem._iCurs = ITEM_3GOLD;
	}

	NewCursor(plr[pnum].HoldItem._iCurs + ICSTART);
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
/*
void WhiteCtrlPan(int sx, int sy, int deltax, int deltay, int dx, int dy)
{
	long		dest;

	dest = (dy * 768) + dx;
	__asm {
		mov		edi,dword ptr [pBuffer]
		add		edi,dword ptr [dest]

		xor		ebx,ebx
		mov		bx,word ptr [deltax]
		xor		edx,edx
		mov		dx,word ptr [deltay]
		mov		eax, 0ffffffffh
_CLp:	mov		ecx,ebx
		shr		ecx,1
		jnc		_Tw
		stosb
		jecxz	_Tx
_Tw:	shr		ecx,1
		jnc		_TLp
		stosw
		jecxz	_Tx
_TLp:	rep stosd
_Tx:	add		edi,768
		sub		edi,ebx
		dec		edx
		jnz		_CLp
	}
}
*/


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static char * print_talk_string(
	char * pszStr,
	int x,
	int y,
	long * plOffset,
	int color
) {
	// move to top left of talk box
	x += 264;
	//y += 546;
	y += 534;

	int w = x;
	*plOffset = nBuffWTbl[y] + x;
	while (*pszStr) {
		// can we fit the next character on this row?
		BYTE c = char2print(*pszStr);
		c = fonttrans[c];
		w += fontkern[c] + 1;
		if (w > 250+264) return pszStr;
		pszStr++;

		// draw char
		if (c != 0) DrawPanelFont(*plOffset, c, color);
		*plOffset += fontkern[c] + 1;
	}

	return NULL;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define TALK_ROW_HGT	13
void DrawTalkBox() {

	if (!talkflag) return;
	app_assert(gpBuffer);

	// erase old box
	CopyCtrlPan(175, 20+talkofs, 294,  5, 239, 516);
	for (int i = 0; i < 10; i++)
		CopyCtrlPan(175+(i>>1), 25+i+talkofs, 293-i, 1, 239+(i>>1), 521+i);
	CopyCtrlPan(185, 35+talkofs, 274, 30, 249, 531);
	CopyCtrlPan(180, 65+talkofs, 284,  5, 244, 561);
	for (i = 0; i < 10; i++)
		CopyCtrlPan(180, 70+i+talkofs, 284+i,  1, 244, 566+i);
	CopyCtrlPan(170, 80+talkofs, 310, 55, 234, 576);

	// 200,373-450,456 (screen coords)
	long lOffset;
	char * pszStr = sgszTalkMsg;
	for (int row = 0; row < 3; row++) {
		pszStr = print_talk_string(pszStr,0,row * TALK_ROW_HGT,&lOffset,ICOLOR_WHITE);
		if (! pszStr) break;
	}
	
	// don't allow string to be too long
	if (pszStr) *pszStr = 0;

	// draw spinnies on last row
	DrawCelP(gpBuffer + lOffset, pSTextSpinCels, tspin, 12);
	tspin = (tspin & 0x7) + 1;

	// draw player names
	row = 0;
	for (i = 0; i < MAX_PLRS; i++) {
		if (i == myplr) continue;

		// are we "talking" to this player
		int nColor, nCel;
		if (sgbPlrTalkTbl[i]) {
			nColor = ICOLOR_GOLD;
			if (talkbtndown[row]) {
				if (row == 0) nCel = 3;
				else nCel = 4;
				DrawCel(236, 596 + (row * 18), pTalkBtns, nCel, 61);
			}
		} else {
			nColor = ICOLOR_RED;
			if (row == 0) nCel = 1;
			else nCel = 2;
			if (talkbtndown[row]) nCel += 4;
			DrawCel(236, 596 + (row * 18), pTalkBtns, nCel, 61);
		}

		if (plr[i].plractive)
			print_talk_string(plr[i]._pName,46,60 + (row * 18),&lOffset,nColor);
		row++;
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
#define TALK_NAME_TOP	421
#define TALK_NAME_LEFT	172
#define TALK_NAME_WDT	61
#define TALK_NAME_HGT	(18*(MAX_PLRS-1))

BOOL talk_click() {
	if (! talkflag) return FALSE;

	if (MouseX < TALK_NAME_LEFT) return FALSE;
	if (MouseY < TALK_NAME_TOP) return FALSE;
	if (MouseX > TALK_NAME_LEFT + TALK_NAME_WDT) return FALSE;
	if (MouseY > TALK_NAME_TOP + TALK_NAME_HGT) return FALSE;

	for (int i = 0; i < 3; i++) talkbtndown[i] = FALSE;
	talkbtndown[(MouseY - TALK_NAME_TOP) / 18] = TRUE;

	return TRUE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void talk_release() {
	if (! talkflag) return;

	for (int i = 0; i < 3; i++) talkbtndown[i] = FALSE;

	if (MouseX < TALK_NAME_LEFT) return;
	if (MouseY < TALK_NAME_TOP) return;
	if (MouseX > TALK_NAME_LEFT + TALK_NAME_WDT) return;
	if (MouseY > TALK_NAME_TOP + TALK_NAME_HGT) return;

	int pnum = (MouseY - TALK_NAME_TOP) / 18;
	for (i = 0; (i < MAX_PLRS) && (pnum != -1); i++)
		//if ((i != myplr) && plr[i].plractive) pnum--;
		if (i != myplr) pnum--;
	if (i <= MAX_PLRS) sgbPlrTalkTbl[i - 1] = !sgbPlrTalkTbl[i - 1];
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void TalkStart() {
	if (gbMaxPlayers == 1) return;
	talkflag = TRUE;
	talkofs = 144;
	sgszTalkMsg[0] = 0;
	tspin = 1;
	for (int i = 0; i < 3; i++) talkbtndown[i] = FALSE;
	force_redraw = FULLDRAW;

	// reset history position
	sgbTalkSavePos = sgbNextTalkSave;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
void TalkEnd() {
	talkflag = FALSE;
	talkofs = 0;
	force_redraw = FULLDRAW;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void TalkSendMsg() {

	if (sgszTalkMsg[0] != 0) {
		// send message to all players who have flag set
		DWORD sendmask = 0;
		for (int i = 0; i < MAX_PLRS; i++)
			if (sgbPlrTalkTbl[i]) sendmask |= 1 << i;
		NetSendString(sendmask,sgszTalkMsg);

		// save msg in history buffer if it is unique
		for (i = 0; i < MAX_TALK_SAVES; i++) {
			if (! strcmp(sgszTalkSaveMsg[i],sgszTalkMsg))
				break;
		}
		if (i >= MAX_TALK_SAVES) {
			// string is unique -- save in history buffer
			strcpy(sgszTalkSaveMsg[sgbNextTalkSave],sgszTalkMsg);
			sgbNextTalkSave++;
			sgbNextTalkSave &= MAX_TALK_SAVES - 1;
		}
		else {
			// string is not unique -- swap curr string with non-unique
			BYTE bTemp = sgbNextTalkSave - 1;
			bTemp &= MAX_TALK_SAVES - 1;
			if (i != bTemp) {
				strcpy(sgszTalkSaveMsg[i],sgszTalkSaveMsg[bTemp]);
				strcpy(sgszTalkSaveMsg[bTemp],sgszTalkMsg);
			}
		}

		// reset history position
		sgbTalkSavePos = sgbNextTalkSave;

		// reset talk string
		sgszTalkMsg[0] = 0;
	}

	TalkEnd();
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BOOL Talk_wm_char(WPARAM wKey) {
	if (gbMaxPlayers == 1) return FALSE;
	if (! talkflag) return FALSE;
	if (wKey < 32) return FALSE;

	int i = strlen(sgszTalkMsg);
	if (i < (MAX_SEND_STR_LEN-2)) {
		sgszTalkMsg[i] = (char) wKey;
		sgszTalkMsg[i+1] = 0;
	}

	return TRUE;
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
static void talk_history(int nDelta) {
	for (int i = 0; i < MAX_TALK_SAVES; i++) {
		sgbTalkSavePos += nDelta;
		sgbTalkSavePos &= MAX_TALK_SAVES - 1;
		if (! sgszTalkSaveMsg[sgbTalkSavePos][0]) continue;

		// we found a string in the history
		strcpy(sgszTalkMsg,sgszTalkSaveMsg[sgbTalkSavePos]);
		break;
	}
}


/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
BOOL Talk_wm_keydown(WPARAM wKey) {
	// only in multiplayer
	if (gbMaxPlayers == 1) return FALSE;
	if (! talkflag) return FALSE;

	if (wKey == VK_SPACE) {
		// grab keystroke so
		// program doesn't get it
	}
	else if (wKey == VK_ESCAPE) {
		TalkEnd();
	}
	else if (wKey == VK_RETURN) {
		TalkSendMsg();
	}
	else if (wKey == VK_BACK) {
		int i = strlen(sgszTalkMsg);
		if (i > 0) sgszTalkMsg[i-1] = 0;
	}
	else if (wKey == VK_DOWN) {
		talk_history(+1);
	}
	else if (wKey == VK_UP) {
		talk_history(-1);
	}
	else {
		return FALSE;
	}

	return TRUE;
}


//******************************************************************
// conversion table -- converts funky ANSI/OEM chars to ASCII
// NOTE: only '\0' is allowed to translate to zero
// map all other characters into range 32..127
//******************************************************************
const BYTE gbFontTransTbl[256] = {
	// control characters
	  0,   1,   1,   1,		  1,   1,   1,   1,	// 0x00 - 0x07
	  1,   1,   1,   1,		  1,   1,   1,   1,	// 0x08 - 0x0f
	  1,   1,   1,   1,		  1,   1,   1,   1,	// 0x10 - 0x17
	  1,   1,   1,   1,		  1,   1,   1,   1,	// 0x18 - 0x1f

	// punctuation/digits
	' ', '!', '"', '#',		'$', '%', '&', '\'',// 0x20 - 0x27
	'(', ')', '*', '+',		',', '-', '.', '/',	// 0x28 - 0x2f
	'0', '1', '2', '3',		'4', '5', '6', '7',	// 0x30 - 0x37
	'8', '9', ':', ';',		'<', '=', '>', '?',	// 0x38 - 0x3f

	// uppercase
	'@', 'A', 'B', 'C',		'D', 'E', 'F', 'G',	// 0x40 - 0x47
	'H', 'I', 'J', 'K',		'L', 'M', 'N', 'O',	// 0x48 - 0x4f
	'P', 'Q', 'R', 'S',		'T', 'U', 'V', 'W',	// 0x50 - 0x57
	'X', 'Y', 'Z', '[',		'\\',']', '^', '_',	// 0x58 - 0x5f

	// lowercase
	'`', 'a', 'b', 'c',		'd', 'e', 'f', 'g',	// 0x60 - 0x67
	'h', 'i', 'j', 'k',		'l', 'm', 'n', 'o',	// 0x68 - 0x6f
	'p', 'q', 'r', 's',		't', 'u', 'v', 'w',	// 0x70 - 0x77
	'x', 'y', 'z', '{',		'|', '}', '~',   1,	// 0x78 - 0x7f

	'C', 'u', 'e', 'a',		'a', 'a', 'a', 'c',	// 0x80 - 0x87
	'e', 'e', 'e', 'i',		'i', 'i', 'A', 'A',	// 0x88 - 0x8f
	'E', 'a', 'A', 'o',		'o', 'o', 'u', 'u',	// 0x90 - 0x97
	'y', 'O', 'U', 'c',		'L', 'Y', 'P', 'f',	// 0x98 - 0x9f

	'a', 'i', 'o', 'u',		'n', 'N', 'a', 'o',	// 0xa0 - 0xa7
	'?',   1,   1,   1,		  1, '!', '<', '>',	// 0xa8 - 0xaf
	'o', '+', '2', '3',		'\'','u', 'P', '.',	// 0xb0 - 0xb7
	',', '1', '0', '>',		  1,   1,   1, '?',	// 0xb8 - 0xbf

	'A', 'A', 'A', 'A',		'A', 'A', 'A', 'C',	// 0xc0 - 0xc7
	'E', 'E', 'E', 'E',		'I', 'I', 'I', 'I',	// 0xc8 - 0xcf
	'D', 'N', 'O', 'O',		'O', 'O', 'O', 'X',	// 0xd0 - 0xd7
	'0', 'U', 'U', 'U',		'U', 'Y', 'b', 'B',	// 0xd8 - 0xdf

	'a', 'a', 'a', 'a',		'a', 'a', 'a', 'c',	// 0xe0 - 0xe7
	'e', 'e', 'e', 'e',		'i', 'i', 'i', 'i',	// 0xe8 - 0xef
	'o', 'n', 'o', 'o',		'o', 'o', 'o', '/',	// 0xf0 - 0xf7
	'0', 'u', 'u', 'u',		'u', 'y', 'b', 'y',	// 0xf8 - 0xff
};
