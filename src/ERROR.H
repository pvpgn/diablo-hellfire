/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/ERROR.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MSG_NONE		0
#define MSG_AMAPTWN		1
#define MSG_MULTIBTN	2
#define MSG_SOUND		3
#define MSG_SHAREWARE	4
#define MSG_SAVESIZE	5
#define MSG_NOPAUSE		6
#define MSG_HDRIVE		7

#define MSG_SYNC		8		// Multi sync
#define MSG_MULTIPAUSE	9		// No multiplayer pause
#define MSG_LOADGAME	10
#define MSG_SAVEGAME	11

#define SHRINE_1		12		// Mysterious
#define SHRINE_2		13		// Hidden
#define SHRINE_3		14		// Gloomy
#define SHRINE_4		15		// Weird
#define SHRINE_5		16		// Magical
#define SHRINE_6		17		// Stone
#define SHRINE_7		18		// Religious
#define SHRINE_8		19		// Enchanted
#define SHRINE_9		20		// Thaumaturgic
#define SHRINE_10		21		// Fascinating
#define SHRINE_11		22		// Cryptic
#define SHRINE_12		23		// Supernatural
#define SHRINE_13		24		// Eldritch
#define SHRINE_14		25		// Eerie
#define SHRINE_15		26		// Divine
#define SHRINE_16		27		// Holy
#define SHRINE_17		28		// Sacred
#define SHRINE_18		29		// Spiritual
#define SHRINE_19		30		// Spooky
#define SHRINE_19B		31		// Spooky for multiplayer
#define SHRINE_20		32		// Abandoned
#define SHRINE_21		33		// Creepy
#define SHRINE_22		34		// Quiet
#define SHRINE_23		35		// Secluded
#define SHRINE_24		36		// Ornate
#define SHRINE_25		37		// Glimmering
#define SHRINE_26		38		// Tainted
#define SHRINE_26B		39		// Tainted for mulitplayer
#define SHRINE_27		44		// Oily
#define SHRINE_28		45		// Glowing
#define SHRINE_29		46		// Mendicants
#define SHRINE_30		47		// Edisons
#define SHRINE_31		48		// Town
#define SHRINE_32		49		// Energy
#define SHRINE_33A		50		// Time Morning
#define SHRINE_33B		51		// Time Afternoon
#define SHRINE_33C		52		// Time Evening
#define SHRINE_33D		53		// Time Night
#define SHRINE_34		54		// Murphy's

#define MSG_TRIG1		40		// Shortcut to catacombs
#define MSG_TRIG2		41		// Shortcut to caves
#define MSG_TRIG3		42		// Shortcut to hell

#define MSG_INBONE		43		// Book with spell in Bone Chamber

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern char msgflag;
extern char msgdelay;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitDiabloMsg(char);
void ClrDiabloMsg();
void DrawDiabloMsg();

