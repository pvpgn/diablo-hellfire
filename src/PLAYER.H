/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/PLAYER.H 3     2/22/97 12:54p Pwyatt $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/
#define MAXPACKLEN	256

#define PM_STAND		0
#define PM_WALK			1
#define PM_WALK2		2
#define PM_WALK3		3
#define PM_ATTACK		4
#define PM_RATTACK		5
#define PM_BLOCK		6
#define PM_GOTHIT		7
#define PM_DEATH		8
#define PM_SPELL		9
#define PM_NEWLVL		10
#define PM_QUIT			11

#define PCMD_NOTHING		-1
#define PCMD_STAND			0
#define PCMD_WALKUR			1
#define PCMD_WALKUL			2
#define PCMD_WALKDR			3
#define PCMD_WALKDL			4
#define PCMD_WALKU			5
#define PCMD_WALKR			6
#define PCMD_WALKD			7
#define PCMD_WALKL			8
#define PCMD_ATTACK			9
#define PCMD_RATTACK		10
#define PCMD_BLOCK			11
#define PCMD_SPELL			12
#define PCMD_OPOBJ			13
#define PCMD_DISARM			14
#define PCMD_REQGETITEM		15
#define PCMD_REQAGETITEM	16
#define PCMD_TALK			17
#define PCMD_TELEK			18
// free space
#define PCMD_ATTACKID		20
#define PCMD_ATTACKPID		21
#define PCMD_RATTACKID		22
#define PCMD_RATTACKPID		23
#define PCMD_SPELLID		24
#define PCMD_SPELLPID		25
#define PCMD_SPELLXYD		26

#define DIR_U	4
#define DIR_UR	5
#define DIR_R	6
#define DIR_DR	7
#define DIR_D	0
#define DIR_DL	1
#define DIR_L	2
#define DIR_UL	3

#define PGFX_MASK	0x0f
#define PGFX_CMASK	0xf0
#define PGFX_CSHIFT	4

#define PGFX_NGUY	0			// Nothing in hands
#define PGFX_SGUY	1			// Shield only
#define PGFX_XGUY	2			// Sword only
#define PGFX_GUY	3			// Sword and shield
#define PGFX_BGUY	4			// Bow
#define PGFX_FGUY	5			// Axe
#define PGFX_ZGUY	6			// Mace
#define PGFX_CGUY	7			// Mace and shield
#define PGFX_TGUY	8			// Staff

#define PGFX_NMGUY	16			// Same as above, medium armor = 0x10 + plain version
#define PGFX_SMGUY	17
#define PGFX_XMGUY	18
#define PGFX_MGUY	19
#define PGFX_BMGUY	20
#define PGFX_FMGUY	21
#define PGFX_ZMGUY	22
#define PGFX_CMGUY	23
#define PGFX_TMGUY	24

#define PGFX_NHGUY	32			// Same as above, heavy armor = 0x20 + plain version
#define PGFX_SHGUY	33
#define PGFX_XHGUY	34
#define PGFX_HGUY	35
#define PGFX_BHGUY	36
#define PGFX_FHGUY	37
#define PGFX_ZHGUY	38
#define PGFX_CHGUY	39
#define PGFX_THGUY	40

#define PGL_STAND	0x0001			// Player gfx load
#define PGL_WALK	0x0002
#define PGL_ATTACK	0x0004
#define PGL_HIT		0x0008
#define PGL_LMAG	0x0010
#define PGL_FMAG	0x0020
#define PGL_TMAG	0x0040
#define PGL_DEAD	0x0080
#define PGL_BLOCK	0x0100
#define PGL_ALL		0x017f

#define E_SINGLE	0
#define E_DOUBLE	1


#define CLASS_WARRIOR		0
#define CLASS_ROGUE			1
#define CLASS_SORCEROR		2
#define CLASS_MONK			3
#define CLASS_BARD			4
#define CLASS_BARBARIAN		5

#define NUM_CLASSES		(1 + CLASS_BARBARIAN)

#define HP_SHIFT		6		// number of fractional bits for hit points
#define MANA_SHIFT		6		// number of fractional bits for mana points

#define WEAP_H2H		0
#define WEAP_RANGE		1

#define MAXPATHLEN		25

#define MAXINV			40		// maximum inventory items per player
#define MAXSPD			8		// maximum invertory speed items per player

#define SPL_FROMR		0		// Current spell from readied spell
#define SPL_FROMBK		1		// Current spell from spell book
#define SPL_FROMT		2		// Current spell from targeted spell
#define SPL_FROMSB		3		// Current spell from scroll in speed bar

#define PLR_NAME_LEN	32

#define BASE_TO_HIT		50

#define SF_ETHER		0x01		// Etheralize spell on
#define SF_RAGE			0x02		// Rage spell on.
#define SF_LETHERGY		0x04		// Lethergy spell on.
#define SF_FLAG4		0x08		// 
#define SF_FLAG5		0x10		// 
#define SF_FLAG6		0x20		// 
#define SF_FLAG7		0x40		// 
#define SF_FLAG8		0x80		// 


#define INVLOC_HEAD		0
#define INVLOC_RING1	1
#define INVLOC_RING2	2
#define INVLOC_NECK		3

#define INVLOC_HAND1	4
#define INVLOC_HAND2	5
#define INVLOC_BODY		6
#define NUM_INVLOC		7

#define DUNGMSG_1		0x01
#define DUNGMSG_2		0x02
#define DUNGMSG_3		0x04
#define DUNGMSG_4		0x08
#define DUNGMSG_5		0x10
#define DUNGMSG_6		0x20	// JKE CRYPT
#define DUNGMSG_7		0x40	// JKE HIVE
#define DUNGMSG_8		0x80	// JKEQUEST HIVE2

#define STAT_STR		0
#define STAT_MAG		1
#define STAT_DEX		2
#define STAT_VIT		3

#define LVLCHANGE_OFF	0
#define LVLCHANGE_TIME	10

#define KILL_UNKNOWN	-1

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/
typedef struct {
	int _pmode;			// players current mode

	char walkpath[MAXPATHLEN];
	BYTE plractive;

	int destAction;
	int destParam1;
	int destParam2;
	int destParam3;
	int destParam4;
	int plrlevel;

	int _px;			// plr map x
	int _py;			// plr map y
	int _pfutx;			// plr future map x
	int _pfuty;			// plr future map y
	int _ptargx;		// final target x
	int _ptargy;		// final target y
	int _pownerx;		// x coord from computer which owns this player
	int _pownery;		// x coord from computer which owns this player
	int _poldx;			// plr old x position
	int _poldy;			// plr old y position

	long _pxoff;		// offset x from left of map tile
	long _pyoff;		// offset y from bottom of map tile
	long _pxvel;		// current x rate
	long _pyvel;		// current y rate
	int _pdir;			// current player direction
	int _nextdir;		// Next direction wanted
	int _pgfxnum;		// Graphic set player number
	BYTE *_pAnimData;	// Data pointer to anim tables
	int _pAnimDelay;	// anim delay amount
	int _pAnimCnt;		// current anim delay value
	int	_pAnimLen;		// number of anim frames
	int	_pAnimFrame;	// current anim frame
	long _pAnimWidth;	// current width of anim
	long _pAnimWidth2;	// (width - 64) / 2
	int _peflag;		// draw extra tile to left for walk fix (flag)

	int _plid;			// player light id
	int _pvid;			// player vision id

	int	_pSpell;		// Spell to cast (from book or readied)
	char _pSplType;		// Spell type of spell to cast
	char _pSplFrom;		// Spell from Readied, book, or speed bar

	int _pTSpell;		// Targeted spell
	char _pTSplType;	// Targeted spell type

	int _pRSpell;		// Readied spell
	char _pRSplType;	// Readied spell type (Memorized, ability, staff/item)

	int _pSBkSpell;		// Current spell book spell
	char _pSBkSplType;	// Spell book spell type

	char _pSplLvl[64];		// Spell level corresponding to SplType bits
	__int64 _pMemSpells;	// Flags for which spell the char has from memorizing books
	__int64 _pAblSpells;	// Flags for which spell the char has class abilities
	__int64 _pScrlSpells;	// Flags for which spell the char has from scrolls
	char	_pSpellFlags;	// Flags for spells, these can be used for anything, see defines above
	
	int	_pSplHotKey[4];		// Spell hotkeys
	char _pSplTHotKey[4];	// Spell type hotkeys

	int _pwtype;		// Weapon type (ranged, h2h)

	BYTE _pBlockFlag;	// Do I have a shield to block with?
	BYTE _pInvincible;	// Can I be hit?
	char _pLightRad;	// Player light radius
	BYTE _pLvlChanging;		// Am I in the process of changing levels?

	// Player attributes
	char _pName[PLR_NAME_LEN];
	char _pClass;

	int _pStrength;
	int _pBaseStr;
	int _pMagic;
	int _pBaseMag;
	int _pDexterity;
	int _pBaseDex;
	int _pVitality;
	int _pBaseVit;

	int _pStatPts;

	int _pDamageMod;		// Damage modifier
	int _pBaseToBlk;		// Block %

	long _pHPBase;
	long _pMaxHPBase;
	long _pHitPoints;
	long _pMaxHP;
	int _pHPPer;
	long _pManaBase;
	long _pMaxManaBase;
	long _pMana;
	long _pMaxMana;
	int _pManaPer;

	char _pLevel;
	char _pMaxLvl;
	long _pExperience;
	long _pMaxExp;
	long _pNextExper;

	char _pArmorClass;

	char _pMagResist;
	char _pFireResist;
	char _pLghtResist;

	long  _pGold;

	BOOL _pInfraFlag;

	long _pVar1;		// scratch var 1
	long _pVar2;		// scratch var 2
	long _pVar3;		// scratch var 3
	long _pVar4;		// scratch var 4
	long _pVar5;		// scratch var 5
	long _pVar6;		// scratch var 6
	long _pVar7;		// scratch var 7
	long _pVar8;		// scratch var 8

	BYTE _pLvlVisited[NUMLEVELS];		// Have I been on this level?
	BYTE _pSLvlVisited[NUMLEVELS];		// Have I been on this set piece level?

	int _pGFXLoad;		// Flag for which graphics are loaded

	BYTE *_pNAnim[8];	// Neutral anims
	int	_pNFrames;		// Number of neutral frames
	long _pNWidth;		// Width of neutral frames
	BYTE *_pWAnim[8];	// Walk anims
	int _pWFrames;		// Number of walk frames
	long _pWWidth;		// Width of walk frames
	BYTE *_pAAnim[8];	// Attack anims
	int _pAFrames;		// Number of attack frames
	long _pAWidth;		// Width of attack frames
	int _pAFNum;		// Which frame to check for attack on
	BYTE *_pLAnim[8];	// Lightning Spell anims
	BYTE *_pFAnim[8];	// Fire Spell anims
	BYTE *_pTAnim[8];	// Misc Spell anims
	int _pSFrames;		// Number of spell frames
	long _pSWidth;		// Width of spell frames
	int _pSFNum;		// Which frame to cast spell on
	BYTE *_pHAnim[8];	// Got Hit anims
	int _pHFrames;		// Number of got hit frames
	long _pHWidth;		// Width of got hit frames
	BYTE *_pDAnim[8];	// Death anims
	int _pDFrames;		// Number of death frames
	long _pDWidth;		// Width of death frames
	BYTE *_pBAnim[8];	// Block anims
	int _pBFrames;		// Number of block frames
	long _pBWidth;		// Width of block frames

	ItemStruct InvBody[NUM_INVLOC];
	ItemStruct InvList[MAXINV];
	int _pNumInv;
	char InvGrid[MAXINV];
	ItemStruct SpdList[MAXSPD];
	ItemStruct HoldItem;	// Item in transit

	int _pIMinDam;			// Min damage from all items
	int _pIMaxDam;			// Max damage from all items
	int _pIAC;				// AC bonus from all items
	int _pIBonusDam;		// Added to damage after calced (%)
	int _pIBonusToHit;		// Added to hit percent
	int _pIBonusAC;			// Added to AC
	int _pIBonusDamMod;		// Added to damage after calced (number)
	__int64 _pISpells;		// item activated spells
	long _pIFlags;			// tot item flags from all items
	int _pIGetHit;			// When I gen hit, added to or subtracted from damage
	char _pISplLvlAdd;		// What to add to each spell level
	char _pISplCost;		// % modifier to spell cost
	int _pISplDur;			// % modifier to spell duration
	int _pIEnAc;
	int _pIFMinDam;			// Fire hit min damage
	int _pIFMaxDam;			// Fire hit min damage
	int _pILMinDam;			// Lightning hit min damage
	int _pILMaxDam;			// Lightning hit min damage

/*	BOOL _WarpActive;		// If I am on the town level from a portal, this turns true.
	int _WarpLevel;			// Level Town portal was cast on
	int _WarpLvlType;		// Level type where warp occurred
	BOOL _WarpSet;			// Was it a set level?
	int _WarpX;				// Level X pos where warp occurred
	int _WarpY;				// Level Y pos where warp occurred */

	int _pOilType;			// Temp hold for oil creation


	// these fields are to be used if more variables need to be added
	// to the player structure so that the size won't change
	BYTE pTownWarps;		// Town warp flags for single player (drb 11/22)
	BYTE pDungMsgs;			// Dungeon phrases that the player says
	BYTE pLvlLoad;
	BYTE pHellfireMsgs;		// Message slots for nest messages
	BYTE bReserved5;
	BYTE bReserved6;
	BYTE bReserved7;
	BYTE bReserved8;

	WORD _pReflectCount;	// Number of reflected hits left.
	WORD wReserved2;
	WORD wReserved3;
	WORD wReserved4;
	WORD wReserved5;
	WORD wReserved6;
	WORD wReserved7;
	WORD wReserved8;

	DWORD pDiabloKillLevel;	// player killed Diablo at what level
	DWORD _gnDifficulty;
	DWORD _pIFlags2;
	DWORD dwReserved4;
	DWORD dwReserved5;
	DWORD dwReserved6;
	DWORD dwReserved7;
	DWORD dwReserved8;

	// Anything below this will not be saved or sent during a sync
	#define SAVE_PLAYER_SIZE offsetof(PlayerStruct,_pNData)
	BYTE *_pNData;		// Neutral anim memory
	BYTE *_pWData;		// Walk anim memory
	BYTE *_pAData;		// Attack anim memory
	BYTE *_pLData;		// Spell anim memory
	BYTE *_pFData;		// Spell anim memory
	BYTE *_pTData;		// Spell anim memory
	BYTE *_pHData;		// Got Hit anim memory
	BYTE *_pDData;		// Death anim memory
	BYTE *_pBData;		// Block anim memory
} PlayerStruct;

// for compatiblity with existing code
#define HeadItem	InvBody[INVLOC_HEAD]
#define BodyItem	InvBody[INVLOC_BODY]
#define Ring1Item	InvBody[INVLOC_RING1]
#define Ring2Item	InvBody[INVLOC_RING2]
#define NeckItem	InvBody[INVLOC_NECK]
#define Hand1Item	InvBody[INVLOC_HAND1]
#define Hand2Item	InvBody[INVLOC_HAND2]


extern DWORD glSeedTbl[NUMLEVELS];
extern int gnLevelTypeTbl[NUMLEVELS];
extern char *ClassStrTbl[NUM_CLASSES];

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/
// pjw.patch1.start
// extern PlayerStruct plr[MAX_PLRS];
extern PlayerStruct * plr;
// pjw.patch1.end

extern int myplr;
extern int pholdx, pholdy;
extern int MaxStats[NUM_CLASSES][4];

extern int deathdelay;
extern BOOL deathflag;
extern BOOL gbValidSaveFile;
extern BOOL gbSaveFileExists;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitPlrGFXMem(int);
void LoadPlrGFX(int,DWORD);
void InitPlayerGFX(int);
void FreePlayerGFX(int);
void SetPlrAnims(int);

void InitPlayer(int, BOOL);
void InitMultiView();
void ProcessPlayers();
int PlrGetDirXY(int, int, int);

void StartSpell(int, int, int, int);
void StartPlrKill(int, BOOL);
void SyncPlrKill(int, BOOL);
void StartPlrHit(int, int, BOOL);
void StartPlrBlock(int, int);
void StartNewLvl(int, int, int);
void StartWarpLvl(int, int);
void RestartTownLvl(int);

void CreatePlayer(int, char);

void CreateMyPacket();
void ProcessPackets();

void AddPlrExperience(int, int, long);
void AddPlrMonstExper(int, long, char);

void SyncInitPlr(int);

void ClrPlrPath(int);
BOOL PosOkPlayer(int, int, int);
void MakePlrPath(int, int, int, BOOL);
void CheckPlrSpell();

void PlrClrTrans(int, int);
void PlrDoTrans(int, int);

void ModifyPlrStr(int, int);
void ModifyPlrMag(int, int);
void ModifyPlrDex(int, int);
void ModifyPlrVit(int, int);

void SetPlayerHitPoints(int pnum, int newhp);
void SetPlrStr(int p, int v);
void SetPlrMag(int p, int v);
void SetPlrDex(int p, int v);
void SetPlrVit(int p, int v);

void DropHalfPlayersGold(int pnum);
extern void StripTopGold(int pnum);

void InitDungMsgs(int pnum);
void PlayDungMsgs();

void CheckStats(int p);

void FixPlrWalkTags(int pnum);
void FixPlayerLocation(int pnum,int bDir);
void SetPlayerOld(int pnum);

int GetMaxStr(int /* class */);
int GetMaxMag(int /* class */);
int GetMaxDex(int /* class */);
