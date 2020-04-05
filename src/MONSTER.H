/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MONSTER.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXMONSTERS		200

#define MONSTERTYPES	200

//#define MAX_LVLMTYPES	16	//JKE 7/29
#define MAX_LVLMTYPES	24	// new crypt level

#define MONSTDENSITY	30

#define MFLAG_INVISIBLE	0x00000001
#define MFLAG_BACKWARDS	0x00000002
#define MFLAG_STILL		0x00000004	// Used for Gargoyles while they are stone
#define MFLAG_NOHEAL	0x00000008	// Monster can no longer heal itself
#define MFLAG_MID		0x00000010	// _menemy element indexes monsters[] (default is plr[])
#define MFLAG_MKILLER	0x00000020	// monster is an attacker monster
#define MFLAG_DROP		0x00000040	// drop an item at special time
#define MFLAG_KNOCKBACK 0x00000080	// knock back enemy one square
#define MFLAG_PATH		0x00000100	// use path algorithm to navigate around obstacles
#define MFLAG_CHECKDOORS 0x00000200	// this monster has door-opening abilities
#define MFLAG_NOENEMY	0x00000400	// monster has no enemy (because there are none left)
#define MFLAG_BERSERK	0x00000800	// monster attacks everything.

// Monster Placement Flags
#define MPFLAG_SCATTER	1	// scatter these all over
#define MPFLAG_DONT		2	// don't place -- taken care of elsewhere
#define MPFLAG_UNIQ		4	// place once, as in a unique

#define MAX_ANIMTYPE	6

#define M_ID	0
#define P_ID	1

/*-----------------------------------------------------------------------*
** Macros
**-----------------------------------------------------------------------*/

#define DIST(x,y,d)	(abs(x) < d && abs(y) < d)
#define EquivMonst(m,t) ((m) >= (t) && (m) <= (t)+3)

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

// AnimStruct
//		One per animation, e.g. Fat demon walk
typedef struct {
	BYTE *CMem;
	BYTE *Cels[8];
	int Frames;
	int Rate;
} AnimStruct;

typedef struct {
	TSnd * effect[2];
} SndEffectStruct;

typedef struct {
	long mAnimWidth;	// width of monster
	long mImgSize;		// Size of graphics for monster
	char *filename;		// pattern used to denote this monster in file names
	BOOL has_special;	// has a special animation
	char *sndfile;		// Name of the sound file
	BOOL snd_special;	// has a special snd
	BOOL transflag;		// Does this monster need gfx translation
	char *TransFile;	// Name of the gfx translation file
	int Frames[MAX_ANIMTYPE];
	int Rate[MAX_ANIMTYPE];
	char *mName;		// name
	char mMinDLvl;		// Min dungeon level
	char mMaxDLvl;		// Max dungeon level
	char mLevel;		// monster ranking
	long mMinHP;		// base hitpoints
	long mMaxHP;		// random hitpoints
	BYTE mAi;			// AI type
	DWORD mFlags;		// ai related flags
	BYTE mInt;			// Intelligence (0-3)
	BYTE mHit;			// hit%
	BYTE mAFNum;		// Which frame to check for attack on
	BYTE mMinDamage;	// min damage
	BYTE mMaxDamage;	// max damage
	BYTE mHit2;			// hit%
	BYTE mAFNum2;		// Which frame to check for attack on
	BYTE mMinDamage2;	// min damage
	BYTE mMaxDamage2;	// max damage
	char mArmorClass;	// Armor class
	char mMonstClass;	// Monster class
	WORD mMagicRes;		// Magic resistance
	WORD mMagicRes2;	// Magic resistance (for nightmare & hell modes)
	WORD mTreasure;		// Treasure
	char mSelFlag;		// Which type of selection
	WORD mExp;			// base experience
} MonsterData;

// CMonster
//		One per monster class, e.g. Fat demon
typedef struct {
	int mtype;
	BYTE mPlaceFlags;	// monster placement flags
	AnimStruct Anims[6];
	SndEffectStruct Snds[4];
	long mAnimWidth;	// width of monster
	long mAnimWidth2;	// (width - 64) / 2 of monster for drawing
	long mMinHP;		// base hitpoints
	long mMaxHP;		// random hitpoints 
	BOOL has_special;	// has a special animation
	BYTE mAFNum;		// Which frame to check for attack on
	char mdeadval;		// Which val to put in flags when dead
	MonsterData *MData;
	byte *pTrans;		// Pointer to gfx translation table
} CMonster;

// MonsterStruct
//		One per monster in the dungeon, e.g., Joe the Fat demon
typedef struct {
 	int _mMTidx;		// Type index into Monsters[] array (*** NOT MT_whatever ***)
	int _mmode;			// monsters current mode
	BYTE _mgoal;		// higher-level mode, i.e., run away!
	int _mgoalvar1;		// goal scratch 1
	int _mgoalvar2;		// goal scratch 2
	int _mgoalvar3;		// goal scratch 3
	int _mgoalvar4;		// goal scratch 4
	BYTE _pathcount;	// 
	int _mx;			// monster map x
	int _my;			// monster map y
	int _mfutx;			// monster future map x
	int _mfuty;			// monster future map y
	int _moldx;			// monster starting walk map x
	int _moldy;			// monster starting walk map y
	long _mxoff;		// offset x from left of map tile
	long _myoff;		// offset y from bottom of map tile
	long _mxvel;		// current x rate
	long _myvel;		// current y rate
	int _mdir;			// current facing direction
	int _menemy;		// Which player is my enemy
	BYTE _menemyx;		// x location of enemy in dungeon
	BYTE _menemyy;		// y "
	BYTE *_mAnimData;	// Data pointer to anim tables
	int _mAnimDelay;	// anim delay amount
	int _mAnimCnt;		// current anim delay value
	int	_mAnimLen;		// number of anim frames
	int	_mAnimFrame;	// current anim frame
	int _meflag;		// draw extra tile to left for walk fix (flag)
	BOOL _mDelFlag;		// Delete/Kill monster
	long _mVar1;		// scratch var 1
	long _mVar2;		// scratch var 2
	long _mVar3;		// scratch var 3
	long _mVar4;		// scratch var 4
	long _mVar5;		// scratch var 5
	long _mVar6;		// scratch var 6
	long _mVar7;		// scratch var 7
	long _mVar8;		// scratch var 8
	long _mmaxhp;		// Fully healed monster hit points
	long _mhitpoints;	// Monster hit points
	BYTE _mAi;			// AI type
	BYTE _mint;			// Monster intelligence
	DWORD _mFlags;
	BYTE _msquelch;		// keeps monster active for a while after it's not visible
	int _mAFNum;		// Which frame to check for attack on
	int _lastx;			// Coordinates where player was last seen
	int _lasty;

	int _mRndSeed;		// Random seed for item generation
	int _mAISeed;		// Random seed for AI (multiplayer only)

	BOOL _Wandering;	// Is this a wandering monster

	BYTE _uniqtype;		// Type of unique (0==None)
	BYTE _uniqtrans;	// Pal translation index
	char _udeadval;		// If unique, it's dead val

	char mWhoHit;		// Which players have hit me so I can divvy up experience

	char mLevel;		// monster ranking
	WORD mExp;			// base experience
	BYTE mHit;			// hit%
	BYTE mMinDamage;	// min damage
	BYTE mMaxDamage;	// max damage
	BYTE mHit2;			// hit%
	BYTE mMinDamage2;	// min damage
	BYTE mMaxDamage2;	// max damage
	char mArmorClass;	// Armor class
	WORD mMagicRes;		// Magic resistance
	int mtalkmsg;		// talking monster message number

	BYTE leader;		// monster # of pack leader
	BYTE leaderflag;
	BYTE packsize;		// # of monsters in pack

	BYTE mlid;			// light id

	// Anything below this will not be saved or sent during a sync
	#define SAVE_MONSTER_SIZE	offsetof(MonsterStruct,mName)
	char *mName;
	CMonster *MType;
	MonsterData *MData;
} MonsterStruct;


typedef struct {
	int		mtype;		// i.e. MT_SKELSD
	char *	mName;
	char *	mTFile;		// Translation filename
	BYTE	mlevel;		// level on which it appears
	WORD	mmaxhp;
	BYTE	mAi;		// AI type
	BYTE	mint;		// AI level
	BYTE	mMinDamage;	// min damage
	BYTE	mMaxDamage;	// max damage
	WORD	mMagicRes;	// Magic resistance
	WORD	mUnqAttr;	// Unique attributes, i.e., pack leader
	BYTE	mUnqVar1;	// parameter to unique attribute
	BYTE	mUnqVar2;	// "
	int		mtalkmsg;	// talking monster message number
} UniqMonstStruct;


/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern MonsterStruct	monster[MAXMONSTERS];
extern long nummonsters;
extern int monstactive[MAXMONSTERS];

extern long monstkills[MONSTERTYPES];

extern int offset_x[];
extern int offset_y[];
extern int left[];
extern int right[];
extern int opposite[];

extern int nummtypes;
extern CMonster Monsters[MAX_LVLMTYPES];

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitLevelMonsters();
void GetLevelMTypes();
void InitMonsterGFX(int monst);
void InitMonsterSND(int monst);
void FreeMonsterGFX();
void InitMonsters();
void TalktoMonster(int i);
void SetMapMonsters(BYTE *, int, int);

void ProcessMonsters();

void M_StartStand(int i, int md);
void M_StartKill(int, int);
void M_SyncStartKill(int, int, int, int);
void M_StartHit(int, int, int);
void M_GetKnockback(int i);
void M_WalkDir(int i, int md);
void MAI_Golum(int i);

BOOL DirOK(int i, int mdir);

void SyncMonsterAnim(int);

void PrintMonstHistory(int);

void MissToMonst(int i, int x, int y);
BOOL PosOkMonst(int i, int x, int y);
BOOL PosOkMissile(int x, int y);

BOOL IsSkel(int mt);
BOOL SpawnSkeleton(int i, int x, int y);
int PreSpawnSkeleton();
void SpawnGolum(int, int, int, int);
void DeleteMonsterList();
void MonstStartKill(int, int, BOOL);

void PlaceQuestMonsters();

int AddMonster(int x, int y, int dir, int mtype, BOOL InMap);

BOOL IsGoat(int mt);

BOOL CanTalkToMonst(int m);

BOOL CheckMonsterHit(int m, BOOL &ret);
BOOL LineClear(int, int, int, int);
int encode_enemy(int m);
void decode_enemy(int m, int enemy);

void Hose_NaKrul();
void CloneMonster(int m);
