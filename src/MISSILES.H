/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MISSILES.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXMISSILES			125

#define MIT_ARROW			0
#define MIT_FIREBOLT		1
#define MIT_GUARDIAN		2
#define MIT_PHASE			3
#define MIT_LIGHTBALL		4
#define MIT_FIREWALL		5
#define MIT_FIREBALL		6
#define MIT_LIGHTCTRL		7
#define MIT_LIGHTNING		8
#define MIT_MISEXP			9
#define MIT_TOWN			10
#define MIT_FLASH			11
#define MIT_FLASH2			12
#define MIT_MANASHIELD		13
#define MIT_FIREMOVE		14
#define MIT_CHAIN			15
#define MIT_CHAINBALL		16
#define MIT_BLOOD			17
#define MIT_BONE			18
#define MIT_METAL			19
#define MIT_RHINO			20
#define MIT_MAGMABALL		21
#define MIT_THINLIGHTCTRL	22
#define MIT_THINLIGHT		23
#define MIT_FLARE			24
#define	MIT_FLAREXP			25
#define	MIT_TELE			26
#define MIT_FARROW			27
#define MIT_DOOM			28
#define MIT_FIREONLY		29
#define MIT_STONE			30
#define MIT_BLOODR			31
#define MIT_INVIS			32
#define MIT_GOLEM			33
#define MIT_ETHER			34
#define MIT_SPURT			35
#define MIT_BOOM			36
#define MIT_HEAL			37
#define MIT_FIREWALLC		38
#define MIT_INFRA			39
#define MIT_IDENTIFY		40
#define MIT_WAVE			41
#define MIT_NOVA			42
//#define MIT_BLDBOIL			43
#define MIT_RAGE			43
#define MIT_APOCA			44
#define MIT_REPAIR			45
#define MIT_RECHARGE		46
#define MIT_DISARM			47
#define MIT_FLAME			48
#define MIT_FLAMEC			49
#define MIT_FIREMAN			50
#define MIT_KRULL			51
#define MIT_CBOLT			52
#define MIT_HBOLT			53
#define MIT_RESURRECT		54
#define MIT_TELEKINESIS		55
#define MIT_LARROW			56
#define MIT_ACID			57
#define MIT_ACIDSPLAT		58
#define MIT_ACIDPUD			59
#define MIT_HEALOTHER		60
#define MIT_ELEMENT			61
#define MIT_RESURRECTBEAM	62
#define MIT_BONESPIRIT		63
#define MIT_WEAPEXP			64
#define MIT_RPORTAL			65
#define MIT_FIREPLAR		66
#define MIT_DIABAPOCA		67
#define MIT_MANA			68
#define MIT_FMANA			69
#define MIT_LIGHTWALL		70
#define MIT_LIGHTWALLC		71
#define MIT_IMMOLATION		72
#define MIT_SPECARROW		73
#define MIT_FBARROW			74
#define MIT_LTARROW			75
#define MIT_CBARROW			76
#define MIT_HBARROW			77
#define MIT_TELESTAIRS		78
#define MIT_REFLECT			79
#define MIT_BERSERK			80
#define MIT_FLAMEBOX		81
#define MIT_DISENCHANT		82
#define MIT_MANAREMOVE		83
#define MIT_LIGHTBOX		84
#define MIT_SHOWMAGITEMS	85
#define MIT_AURA			86
#define MIT_AURA2			87
#define MIT_SPIRALFIREBALL	88
#define MIT_RUNEOFFIRE		89
#define MIT_RUNEOFLIGHT		90
#define MIT_RUNEOFNOVA		91
#define MIT_RUNEOFIMMOLATION	92
#define MIT_RUNEOFSTONE		93
#define MIT_BIGEXPLOSION	94
#define MIT_HORKSPAWN		95
#define MIT_RANDOM			96
#define MIT_OPENNEST		97
#define MIT_ORANGEFLARE		98
#define MIT_BLUEFLARE 		99
#define MIT_REDFLARE		100
#define MIT_YELLOWFLARE		101
#define MIT_BLUE2FLARE		102
#define MIT_YELLOWEXPLOSION	103
#define MIT_REDEXPLOSION	104
#define MIT_BLUEEXPLOSION	105
#define MIT_BLUE2EXPLOSION	106
#define MIT_ORANGEEXPLOSION	107

#define NUMBER_OF_MISSILE_TYPES (1 + MIT_ORANGEEXPLOSION)

#define MI_ENEMYMONST		0
#define MI_ENEMYPLR			1
#define MI_ENEMYBOTH		2

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	int		_mitype;		// missile type
	int		_mix;			// missile map x
	int		_miy;			// missile map y
	long	_mixoff;		// offset x from left of map tile
	long	_miyoff;		// offset y from bottom of map tile
	long	_mixvel;		// current x rate
	long	_miyvel;		// current y rate
	int		_misx;			// missile map start x
	int		_misy;			// missile map start y
	long	_mitxoff;		// missile total offset from start x
	long	_mityoff;		// missile total offset from start y
	int		_mimfnum;		// current facing direction
	int		_mispllvl;		// level of missile
	BOOL	_miDelFlag;		// delete flag
	BYTE	_miAnimType;	// data pointer to anim tables
	BOOL	_miAnimFlags;	// various animation related flags
	BYTE	*_miAnimData;	// Data pointer to anim tables
	int		_miAnimDelay;	// anim delay amount
	int		_miAnimLen;		// number of anim frames
	long	_miAnimWidth;	// anim width
	long	_miAnimWidth2;	// anim width2
	int		_miAnimCnt;		// current anim delay value
	int		_miAnimAdd;		// anim number to add to next frame (-1 backwards)
	int		_miAnimFrame;	// current anim frame
	BOOL	_miDrawFlag;	// draw missile at all
	BOOL	_miLightFlag;	// draw with light sourcing?
	BOOL	_miPreFlag;		// draw missile behind plr,monsters,objects
	BOOL	_miUniqTrans;	// draw missile with special palette translation
	int		_mirange;		// max range of missile
	int		_misource;		// which monster/plr shot me
	int		_micaster;		// monst/plr/damages both.
	int		_midam;			// missile damage
	BOOL	_miHitFlag;		// did it hit a monster or plr?
	int		_midist;		// how long have I been traveling
	int		_mlid;			// light id
	int		_mirnd;			// random seed
	long	_miVar1;		// scratch var 1
	long	_miVar2;		// scratch var 2
	long	_miVar3;		// scratch var 3
	long	_miVar4;		// scratch var 4
	long	_miVar5;		// scratch var 5
	long	_miVar6;		// scratch var 6
	long	_miVar7;		// scratch var 7
	long	_miVar8;		// scratch var 8
} MissileStruct;
#define SAVE_MISSILE_SIZE	sizeof(MissileStruct)


/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern int nummissiles;
extern int missileactive[MAXMISSILES];
extern int missileavail[MAXMISSILES];
extern BOOL MissilePreFlag;
extern MissileStruct missile[MAXMISSILES];
extern int XDirAdd[8];
extern int YDirAdd[8];

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitMissileGFX();
void FreeMissileGFX();
void ILoadMissileGFX(BYTE);
void IFreeMissileGFX();
void InitMissiles();

void ProcessMissiles();
int  AddMissile(int, int, int, int, int, int, char, int, int, int);

//void RndBlood (int, int, int, int, int);

BOOL MonsterTrapHit(int, int, int, int, int, byte);
BOOL PlayerMHit(int, int, int, int, int, int, byte, BOOL, bool *);
int GetSpellLevel(int, int);
void GetDamageAmt(int, int *, int *);

void SyncMissAnim();

void ClearMissileSpot(int mi);
