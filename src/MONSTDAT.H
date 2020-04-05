/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MONSTDAT.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

// Monster Type
#define MT_NZOMBIE	0
#define MT_BZOMBIE	1
#define MT_GZOMBIE	2
#define MT_YZOMBIE	3

#define MT_RFALLSP	4
#define MT_DFALLSP	5
#define MT_YFALLSP	6
#define MT_BFALLSP	7

#define MT_WSKELAX	8
#define MT_TSKELAX	9
#define MT_RSKELAX	10
#define MT_XSKELAX	11

#define MT_RFALLSD	12
#define MT_DFALLSD	13
#define MT_YFALLSD	14
#define MT_BFALLSD	15

#define MT_NSCAV	16
#define MT_BSCAV	17
#define MT_WSCAV	18
#define MT_YSCAV	19

#define MT_WSKELBW	20
#define MT_TSKELBW	21
#define MT_RSKELBW	22
#define MT_XSKELBW	23

#define MT_WSKELSD	24
#define MT_TSKELSD	25
#define MT_RSKELSD	26
#define MT_XSKELSD	27

#define MT_INVILORD 28

#define MT_SNEAK	29
#define MT_STALKER	30
#define MT_UNSEEN	31
#define MT_ILLWEAV	32

#define MT_LRDSAYTR 33

#define MT_NGOATMC	34
#define MT_BGOATMC	35
#define MT_RGOATMC	36
#define MT_GGOATMC	37

#define MT_FIEND	38
#define MT_BLINK	39
#define MT_GLOOM	40
#define MT_FAMILIAR	41

#define MT_NGOATBW	42
#define MT_BGOATBW	43
#define MT_RGOATBW	44
#define MT_GGOATBW	45

#define MT_NACID	46
#define MT_RACID	47
#define MT_BACID	48
#define MT_XACID	49

#define MT_SKING	50

#define MT_CLEAVER	51

#define MT_FAT		52
#define MT_MUDMAN	53
#define MT_TOAD		54
#define MT_FLAYED	55

#define MT_WYRM		56
#define MT_CAVSLUG  57
#define MT_DVLWYRM  58
#define MT_DEVOUR   59

#define MT_NMAGMA	60
#define MT_YMAGMA	61
#define MT_BMAGMA	62
#define MT_WMAGMA	63

#define MT_HORNED	64
#define MT_MUDRUN	65
#define MT_FROSTC	66
#define MT_OBLORD	67

#define MT_BONEDMN  68
#define MT_REDDTH   69
#define MT_LTCHDMN  70
#define MT_UDEDBLRG 71

#define MT_INCIN	72
#define MT_FLAMLRD  73
#define MT_DOOMFIRE 74
#define MT_HELLBURN 75

#define MT_STORM	76
#define MT_RSTORM	77
#define MT_STORML	78
#define MT_MAEL		79

#define MT_BIGFALL	80

#define MT_WINGED	81
#define MT_GARGOYLE	82
#define MT_BLOODCLW	83
#define MT_DEATHW	84

#define	MT_MEGA		85
#define MT_GUARD    86
#define MT_VTEXLRD  87
#define MT_BALROG   88

#define MT_NSNAKE	89
#define MT_RSNAKE	90
#define MT_BSNAKE	91
#define MT_GSNAKE	92

#define MT_NBLACK	93
#define MT_RTBLACK	94
#define MT_BTBLACK	95
#define MT_RBLACK	96

#define MT_UNRAV	97
#define MT_HOLOWONE 98
#define MT_PAINMSTR 99
#define MT_REALWEAV 100

#define MT_SUCCUBUS	101
#define MT_SNOWWICH 102
#define MT_HLSPWN   103
#define MT_SOLBRNR  104

#define MT_COUNSLR	105
#define MT_MAGISTR  106
#define MT_CABALIST 107
#define MT_ADVOCATE 108

#define MT_GOLEM	109

#define MT_DIABLO	110
#define MT_DARKMAGE	111

#define MT_FORK		112
#define MT_SCORP	113
#define MT_EYE		114
#define MT_SPIDER	115
#define MT_FELLTWIN	116
#define MT_SPAWN	117
#define MT_SCORP2	118
#define MT_EYE2		119
#define MT_SPIDER2	120
#define MT_LASH		121
#define MT_ANT		122
#define MT_HORKD	123
#define MT_BUG		124

#define MT_GRAVDG	125
#define MT_RAT		126
#define MT_HELLBAT	127
#define MT_BONED	128
#define MT_LICH		129
#define MT_BUBBA	130
#define MT_HELLBAT2	131
#define MT_BONED2	132
#define MT_LICH2	133
#define MT_BYCLPS	134
#define MT_FLESH	135
#define MT_REAPER	136
#define MT_NKR		137

#define LASTMT		138

// Temp unique monster identification
#define MU_GARBUD		0
#define MU_SKELKING 	1
#define MU_ZHAR			2
#define MU_SNOTSPIL		3
#define MU_LAZARUS		4
#define MU_REDVEX		5
#define MU_BLKJADE		6
#define MU_LACHDA		7
#define MU_WARLORD		8
#define MU_CLEAVER		9
#define MU_HORKDEMON	10
#define MU_DEFILER		11
#define MU_NAKRUL		12

// Image data sizes
#define IMG_MAX		4000

// Unique Monster Attributes
#define UN_PACK		0x0001		// monster has pack surrounding him
#define UN_STICK	0x0002		// monster pack sticks near leader -- must be used w/UN_PACK
#define UN_H		0x0004		// modified hit points
#define UN_A		0x0008		// modified armor
#define UN_L		(UN_PACK|UN_STICK)

// Monster Classes
#define	MC_UNDEAD		0
#define MC_DEMON		1
#define MC_ANIMAL		2

// Monster resists
#define M_NONE	0x0000		// No resist
#define M_RM	0x0001		// resist magic
#define M_RF	0x0002		// resist fire
#define M_RL	0x0004		// resist lightning
#define M_IM	0x0008		// immune to magic
#define M_IF	0x0010		// immune to fire
#define M_IL	0x0020		// immune to lightning
#define M_II	0x0040		// immune to infravision
#define M_IA	0x0080		// immune to acid

#define T_U		0x8000		// Unique
#define T_NONE	0x4000
//#define Unused	0x2000
//#define Unused	0x1000
#define T_MASK	0x0fff

// Monster availability
#define MAT_NO		0		// Not available
#define MAT_SW		1		// Shareware + Normal
#define MAT_YES		2		// Normal only


#define	AI_ZOMBIE		0
#define	AI_FAT			1
#define	AI_SKELSD		2
#define	AI_SKELBOW		3
#define	AI_SCAV			4
#define	AI_RHINO		5
#define	AI_GOATMC		6
#define	AI_GOATBOW		7
#define	AI_FALLEN		8
#define	AI_MAGMA		9
#define	AI_SKELKING		10
#define	AI_BAT			11
#define	AI_GARG			12
#define	AI_CLEAVER		13
#define AI_SUCC			14
#define AI_SNEAK		15
#define AI_STORM		16
#define AI_FIREMAN		17
#define AI_GARBUD		18
#define AI_ACID			19
#define AI_ACIDUNIQ		20
#define AI_GOLUM		21
#define AI_ZHAR			22
#define AI_SNOTSPIL		23
#define AI_SNAKE		24
#define AI_COUNSLR		25
#define AI_MEGA			26
#define AI_DIABLO		27
#define AI_LAZURUS		28
#define AI_LAZHELP		29
#define AI_LACHDANAN	30
#define AI_WARLORD		31
#define AI_FIREBAT		32
#define AI_HELLBAT		33
#define AI_HORKDEMON	34
#define AI_LICH			35
#define AI_ARCHLICH		36
#define AI_PSYCHORB		37
#define AI_NECROMORB	38
#define AI_BONED		39
#define NUM_AI			(1 + AI_BONED)

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern MonsterData monsterdata[];
extern int MonstConvTbl[];
extern int MonstAvailTbl[];
extern UniqMonstStruct UniqMonst[];

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
