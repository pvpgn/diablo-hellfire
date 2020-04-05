/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Monstint.h
**
**		Definitions internal to Monster modules
**		See Monster.h for definitions necessary outside monster modules
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MONSTINT.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

// Monster Modes
#define MM_STAND	0
#define MM_WALK		1
#define MM_WALK2	2
#define MM_WALK3	3
#define MM_ATTACK	4
#define MM_GOTHIT	5
#define MM_DEATH	6
#define MM_SATTACK	7		// Special Attack
#define MM_FADEIN	8
#define MM_FADEOUT	9
#define MM_RATTACK	10		// Ranged weapon attack
#define MM_SPSTAND	11		// Special Neutral
#define MM_RSATTACK	12		// Ranged Special attack
#define MM_DELAY	13		// Stand for a while
#define	MM_MISSILE	14		// Do nothing while monster is a missile
#define	MM_STONE	15		// Monster is Stone Cursed
#define MM_HEAL		16		// Gargoyle still while healing
#define MM_TALK		17		// Monster is in talk mode

// Monster Goals
#define MG_ATTACK		1
#define MG_RUN_AWAY		2
#define MG_EAT			3
#define MG_WALK_AROUND1	4
#define MG_ATTACK2		5
#define MG_TALK			6
#define MG_WAITTOTALK	7

// Monster Anims
#define MA_STAND	0
#define MA_WALK		1
#define MA_ATTACK	2
#define MA_GOTHIT	3
#define MA_DEATH	4
#define MA_SPECIAL	5

// Monster Direction
#define	M_DIRU	4
#define	M_DIRUR	5
#define	M_DIRR	6
#define	M_DIRDR	7
#define	M_DIRD	0
#define	M_DIRDL	1

#define	M_DIRL	2
#define	M_DIRUL	3

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void M_FallenFear(int x, int y);
BOOL SolidLoc(int x, int y);
