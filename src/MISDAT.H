/*-----------------------------------------------------------------------**
** 								Diablo
**
**						 Constants and Variables
**
** 				(C)1996 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MISDAT.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/
typedef void (*MIADDPRC)(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
typedef void (*MIPROC)(int i);

#define MIS_WEAP		0
#define MIS_SPL			1
#define MIS_NONE		2

// Missile Magic Type
#define MIMT_NONE		0
#define MIMT_FIRE		1
#define MIMT_LGHT		2
#define MIMT_MISC		3
#define MIMT_ACID		4

// Missile File Flags
#define	MFF_MONSTONLY	1		// Only loaded for particular monsters
#define MFF_STATIC		2		// Do not animate -- one frame per direction
#define MFF_MULTI		4		// All directions of anim are in one file

#define MF_NONE			255
#define MF_STARTLOAD	0
#define MF_ARROW		0
#define MF_FIREBOLT		1
#define MF_GUARDIAN		2
#define MF_LIGHTNING	3
#define MF_FIRE			4
#define MF_EXP1			5
#define MF_TOWN			6
#define MF_FLASH1		7
#define MF_FLASH2		8
#define MF_MANASHLD		9
#define MF_BLOOD		10
#define MF_BONE			11
#define MF_METAL		12
#define MF_FARROW		13
#define MF_DOOM			14
#define MF_GOLEM		15
#define MF_SPURT		16
#define MF_BOOM			17
#define MF_STONE		18
#define MF_BIGEXP		19
#define MF_FLAMES		20
#define MF_THINLIGHT	21
#define MF_FLARE		22
#define MF_FLAREXP		23
#define MF_MAGBALL		24
#define MF_KRULL		25
#define MF_CBOLT		26
#define MF_HBOLT		27
#define MF_HEXPL		28
#define MF_LARROW		29
#define MF_FAEXP		30
#define	MF_ACID			31
#define MF_ACIDSPLAT	32
#define MF_ACIDPUD		33
#define MF_ETHER		34
#define MF_FIRERUN		35
#define MF_RESURRECT	36
#define MF_BONESPIRIT	37
#define MF_RPORTAL		38
#define MF_FIREPLAR		39
#define MF_BFLARE		40
#define MF_BFLAREXP		41
#define MF_CFLARE		42
#define MF_CFLAREXP		43
#define MF_DFLARE		44
#define MF_DFLAREXP		45
#define MF_HORKSPAWN	46
#define MF_REFLECTSHLD	47
#define MF_ORANGEFLARE	48
#define MF_BLUEFLARE	49
#define MF_REDFLARE		50
#define MF_YELLOWFLARE	51
#define MF_RUNEHOTSPOT	52
#define MF_YELLOWEXPLOSION	53
#define MF_BLUEEXPLOSION 54
#define MF_REDEXPLOSION	55
#define MF_BLUE2FLARE	56
#define MF_ORANGEEXPLOSION	57
#define MF_BLUE2EXPLOSION	58


/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/
typedef struct {
	BYTE mName;				// #defines name of missile for ease of use
	MIADDPRC mAddProc;		// procedure for creating one of these missiles
	MIPROC mProc;			// procedure to run each frame
	BOOL mDraw;				// missile draw or not flag
	BYTE mType;				// missile type (weapon, spell, none)
	BYTE mResist;			// missile resistance type (fire, light, misc, none)
	BYTE mFileNum;			// file# of missile gfx table
	int  mlSFX;				// missile launch sound
	int  miSFX;				// missile impact sound
} MissileData;

typedef struct {
	BYTE mAnimName;			// #defines name of missile file for ease of use
	BYTE mAnimFAmt;			// amount of files
	char *mAnimPath;		// path for cels
	BOOL mFlags;			// e.g. MFF_MONSTOLY
	BYTE *mAnimData[16];	// data pointer to anim tables
	BYTE mAnimDelay[16];	// anim delay amount
	BYTE mAnimLen[16];		// number of anim frames
	long mAnimWidth[16];	// anim width
	long mAnimWidth2[16];	// anim width2
} MisFileData;

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/
extern MissileData missiledata[];
extern MisFileData misfiledata[];

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
