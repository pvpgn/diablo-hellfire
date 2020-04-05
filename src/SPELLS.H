/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/SPELLS.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define PRE_BETA		IS_VERSION(BETA)
#define PRE_NONE		TRUE
#define PRE_FIREBOLT	TRUE
#define PRE_HEAL		TRUE
#define PRE_LIGHTNING	TRUE
#define PRE_FLASH		TRUE
#define PRE_IDENTIFY	TRUE
#define PRE_WALL		TRUE
#define PRE_TOWN		TRUE
#define PRE_STONE		TRUE
#define PRE_INFRA		TRUE
#define PRE_PHASE		TRUE
#define PRE_MANASHLD	TRUE
#define PRE_FIREBALL	TRUE
#define PRE_GUARDIAN	FALSE
#define PRE_CHAIN		TRUE
#define PRE_WAVE		TRUE
#define PRE_DOOM		FALSE
#define PRE_BLOODR		FALSE
#define PRE_NOVA		TRUE
#define PRE_INVIS		FALSE
#define PRE_FLAME		TRUE
#define PRE_GOLEM		TRUE
#define PRE_BLOODB		TRUE
#define PRE_TELE		TRUE
#define PRE_APOCA		FALSE
#define PRE_ETHER		FALSE
#define PRE_REPAIR		TRUE
#define PRE_RECHARGE	TRUE
#define PRE_DISARM		TRUE
#define PRE_ELEMENT		TRUE
#define PRE_CBOLT		TRUE
#define PRE_HBOLT		TRUE
#define PRE_RESURRECT	TRUE
#define PRE_TELEKINESIS	TRUE
#define PRE_HEALOTHER	TRUE
#define PRE_BSTAR		FALSE
#define PRE_BONESPIRIT	FALSE

#define SPL_NONE		0
#define SPL_FIREBOLT	1
#define SPL_HEAL		2
#define SPL_LIGHTNING	3
#define SPL_FLASH		4
#define SPL_IDENTIFY	5
#define SPL_WALL		6
#define SPL_TOWN		7
#define SPL_STONE		8
#define SPL_INFRA		9
#define SPL_PHASE		10
#define SPL_MANASHLD	11
#define SPL_FIREBALL	12
#define SPL_GUARDIAN	13
#define SPL_CHAIN		14
#define SPL_WAVE		15
#define SPL_DOOM		16
#define SPL_BLOODR		17
#define SPL_NOVA		18
#define SPL_INVIS		19
#define SPL_FLAME		20
#define SPL_GOLEM		21
//#define SPL_BLOODB		22
#define SPL_RAGE		22
#define SPL_TELE		23
#define SPL_APOCA		24
#define SPL_ETHER		25
#define SPL_REPAIR		26
#define SPL_RECHARGE	27
#define SPL_DISARM		28
#define SPL_ELEMENT		29
#define SPL_CBOLT		30
#define SPL_HBOLT		31
#define SPL_RESURRECT	32
#define SPL_TELEKINESIS	33
#define SPL_HEALOTHER	34
#define SPL_BSTAR		35
#define SPL_BONESPIRIT	36

// added 7/31 by donald
#define SPL_MANA        37   // staff versions of mana potions
#define SPL_FMANA       38
#define SPL_RANDOM      39
// Added 9/5/97 by Gary
#define SPL_LTWALL		40
#define SPL_IMMOLATION	41
#define SPL_TELESTAIRS	42
#define SPL_REFLECT		43
#define SPL_BERSERK		44
#define SPL_RINGOFFIRE	45
#define SPL_SHOWMAGITEMS	46
#define SPL_RUNEOFFIRE	47
#define SPL_RUNEOFLIGHT	48
#define SPL_RUNEOFNOVA	49
#define SPL_RUNEOFIMMOLATION	50
#define SPL_RUNEOFSTONE	51


#if defined(HELLFIRE2)
#define SPL_RINGOFLIGHT	46
#define SPL_AURA		48
#define SPL_SPIRALFIREBALL	49
#endif // HELLFIRE2

#define SPL_LAST		(SPL_RUNEOFSTONE + 1)

#define MAXSPELLS		SPL_LAST
#define SPELLCAP		15

#define SPT_ABILITY		0
#define SPT_MEMORIZED	1
#define SPT_SCROLL		2
#define SPT_ITEM		3
#define SPT_NONE		4

#define MI_PLR			0
#define MI_MONST		1

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void CastSpell(int, int, int, int, int, int, int, int);
void DoResurrect(int, int);
void DoHealOther(int, int);
void UseMana(int, int);
int GetManaAmount(int, int);
BOOL CheckSpell(int, int, char, BOOL);
