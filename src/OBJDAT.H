/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/OBJDAT.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXOBJFILES			56

#define OBJ_L1LIGHT			0
#define OBJ_L1DOORL			1
#define OBJ_L1DOORR			2
#define OBJ_SKFIRE			3
#define OBJ_LEVER			4
#define OBJ_CHEST1			5
#define OBJ_CHEST2			6
#define OBJ_CHEST3			7
#define OBJ_CANDLE1			8
#define OBJ_CANDLE2			9
#define OBJ_CANDLEO			10
#define OBJ_BANNERL			11
#define OBJ_BANNERM			12
#define OBJ_BANNERR			13
#define OBJ_SKPILE			14
#define OBJ_SKSTICK1		15
#define OBJ_SKSTICK2		16
#define OBJ_SKSTICK3		17
#define OBJ_SKSTICK4		18
#define OBJ_SKSTICK5		19
#define OBJ_CRUX1			20
#define OBJ_CRUX2			21
#define OBJ_CRUX3			22
#define OBJ_STAND			23
#define OBJ_ANGEL			24
#define OBJ_BOOK2L			25
#define OBJ_BCROSS			26
#define OBJ_NUDEW2R			27
#define OBJ_SWITCHSKL		28
#define OBJ_TNUDEM1			29
#define OBJ_TNUDEM2			30
#define OBJ_TNUDEM3			31
#define OBJ_TNUDEM4			32
#define OBJ_TNUDEW1			33
#define OBJ_TNUDEW2			34
#define OBJ_TNUDEW3			35
#define OBJ_TORTURE1		36
#define OBJ_TORTURE2		37
#define OBJ_TORTURE3		38
#define OBJ_TORTURE4		39
#define OBJ_TORTURE5		40
#define OBJ_BOOK2R			41
#define OBJ_L2DOORL			42
#define OBJ_L2DOORR			43
#define OBJ_TORCHL			44
#define OBJ_TORCHR			45
#define OBJ_TORCHL2			46
#define OBJ_TORCHR2			47
#define OBJ_SARC			48
#define OBJ_FLAMEHOLE		49
#define OBJ_FLAMELVR		50
#define OBJ_WATER			51
#define OBJ_BOOKLVR			52
#define OBJ_TRAPL			53
#define OBJ_TRAPR			54
#define OBJ_BOOKSHELF		55
#define OBJ_WEAPRACK		56	
#define OBJ_BARREL			57
#define OBJ_BARRELEX		58
#define OBJ_SHRINEL			59
#define OBJ_SHRINER			60
#define OBJ_SKELBOOK		61
#define OBJ_BOOKCASEL		62
#define OBJ_BOOKCASER		63
#define OBJ_BOOKSTAND		64
#define OBJ_BOOKCANDLE		65
#define OBJ_BLOODFTN		66
#define OBJ_DECAP			67
#define OBJ_TCHEST1			68
#define OBJ_TCHEST2			69
#define OBJ_TCHEST3			70
#define OBJ_BLINDBOOK		71
#define OBJ_BLOODBOOK		72
#define OBJ_PEDISTAL		73
#define OBJ_L3DOORL			74 
#define OBJ_L3DOORR			75 
#define OBJ_PURIFYINGFTN	76
#define OBJ_ARMORSTAND		77
#define OBJ_ARMORSTANDN		78
#define OBJ_GOATSHRINE		79
#define OBJ_CAULDRON		80
#define OBJ_MURKYFTN		81
#define OBJ_TEARFTN			82
#define	OBJ_ALTBOY			83
#define OBJ_MCIRCLE1		84
#define OBJ_MCIRCLE2		85
#define OBJ_STORYBOOK		86
#define OBJ_STORYCANDLE		87
#define OBJ_STEELTOME		88
#define OBJ_WARARMOR		89
#define OBJ_WARWEAP			90
#define OBJ_TBCROSS			91
#define OBJ_WEAPONRACK		92
#define OBJ_WEAPONRACKN		93
#define OBJ_MUSHPATCH		94
#define OBJ_LAZSTAND		95
#define	OBJ_SLAINHERO		96
#define OBJ_SIGNCHEST		97

// These are not used yet, so their numbers are not relevent
#define OBJ_BLADEL		0
#define OBJ_BLADER		0
#define OBJ_BLOCK		0
#define OBJ_BOOK1L		0
#define OBJ_BOOK1R		0
#define OBJ_CANBRA		0
#define OBJ_CHAINEL		0
#define OBJ_CHAINER		0
#define OBJ_CHAING1L	0
#define OBJ_CHAING1R	0
#define OBJ_CHAING2L	0
#define OBJ_CHAING2R	0
#define OBJ_CHAINLL		0
#define OBJ_CHAINLR		0
#define OBJ_FTNROUND	0
#define OBJ_FTNL		0
#define OBJ_FTNR		0
#define OBJ_GATEL		0
#define OBJ_GATER		0
#define OBJ_SPKTRAP		0
#define OBJ_STEAMTRAP	0
#define OBJ_SWITCHML	0
#define OBJ_SWITCHMR	0
#define OBJ_SWITCHRL	0
#define OBJ_SWITCHRR	0
#define OBJ_SWITCHSKR	0
#define OBJ_WATERJUG	0
#define OBJ_TORTURE6R	0
#define OBJ_NUDEW1L		0
#define OBJ_NUDEW1R		0
#define OBJ_NUDEW2L		0
#define OBJ_NUDEW8		0
#define OBJ_NUDEMAN1L	0
#define OBJ_NUDEMAN1R	0
#define OBJ_NUDEMAN3	0
#define OBJ_VAPOR		0
#define	OBJ_DEADSKL		0
#define OBJ_INVPLATE	0
#define OBJ_WOODPLATE	0
#define OBJ_MTLPLATE	0
#define OBJ_STNPLATE	0
#define	OBJ_ANVIL		0
#define	OBJ_FORGE		0
#define OBJ_BIGROCK		0
#define	OBJ_ROCK1		0
#define OBJ_ROCK2		0
#define	OBJ_ROCK3		0
#define	OBJ_ROCK4		0
#define	OBJ_STALAG1		0
#define	OBJ_STALAG2		0
#define	OBJ_STALAG3		0
#define	OBJ_STALAG4		0
#define	OBJ_STALAG5		0
#define	OBJ_ALTGIRL		0
#define	OBJ_TORCH		0
#define OBJ_VILEPORT	0

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern int ObjTypeConv[];
extern ObjDataStruct AllObjects[];
extern ObjDataStruct QuestObjects[];
extern char *ObjMasterFList[];
extern char *HObjMasterFList[];
extern char *CObjMasterFList[];
