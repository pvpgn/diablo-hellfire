/*-----------------------------------------------------------------------**
** 								Diablo
**
**						   Spells Data file
**
** 				(C)1996 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/SPELLDAT.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include    "effects.h"
#include    "spelldat.h"
#include    "spells.h"
#include    "missiles.h"

/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
SpellData spelldata[SPL_LAST] =
{
	{ SPL_NONE,								// #defines name of spell for ease of use
	0,										// base cost in mana
	NULL,									// spell type (fire, light, misc)
	NULL,									// text name of spell
	NULL,									// text name of spell if it can be a skill
	0,										// lvl book can be found
	0,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	0,										// min intelligence to use
	NULL,									// sfx to use
	{ NULL, NULL, NULL },					// missiles to launch per casting
	0,										// mana adj per spell lvl
	0,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	0,										// book cost
	0 },									// scroll cost

	{ SPL_FIREBOLT,							// #defines name of spell for ease of use
	6,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Firebolt",								// text name of spell
	"Firebolt",								// text name of spell if it can be a skill
	1,										// lvl book can be found
	1,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	15,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_FIREBOLT, NULL, NULL },			// missiles to launch per casting
	1,										// mana adj per spell lvl
	3,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	1000,									// book cost
	50 },									// scroll cost

	{ SPL_HEAL,								// #defines name of spell for ease of use
	5,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Healing",								// text name of spell
	NULL,									// text name of spell if it can be a skill
	1,										// lvl book can be found
	1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	TRUE,									// Avail in town?
	17,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_HEAL, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	1,										// min mana for use
	20,										// min number of staff charges
	40,										// max number of staff charges
	1000,										// book cost
	50 },									// scroll cost

	{ SPL_LIGHTNING,						// #defines name of spell for ease of use
	10,										// base cost in mana
	ST_LIGHT,								// spell type (fire, light, misc)
	"Lightning",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	4,										// lvl book can be found
	3,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	20,										// min intelligence to use
	IS_CAST4,								// sfx to use
	{ MIT_LIGHTCTRL, NULL, NULL },			// missiles to launch per casting
	1,										// mana adj per spell lvl
	6,										// min mana for use
	20,										// min number of staff charges
	60,										// max number of staff charges
	3000,										// book cost
	150 },									// scroll cost

	{ SPL_FLASH,							// #defines name of spell for ease of use
	30,										// base cost in mana
	ST_LIGHT,								// spell type (fire, light, misc)
	"Flash",								// text name of spell
	NULL,									// text name of spell if it can be a skill
	5,										// lvl book can be found
	4,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	33,										// min intelligence to use
	IS_CAST4,								// sfx to use
	{ MIT_FLASH, MIT_FLASH2, NULL },		// missiles to launch per casting
	2,										// mana adj per spell lvl
	16,										// min mana for use
	20,										// min number of staff charges
	40,										// max number of staff charges
	7500,									// book cost
	500 },									// scroll cost

	{ SPL_IDENTIFY,							// #defines name of spell for ease of use
	13,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Identify",								// text name of spell
	"Identify",								// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	TRUE,									// Avail in town?
	23,										// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_IDENTIFY, NULL, NULL },			// missiles to launch per casting
	2,										// mana adj per spell lvl
	1,										// min mana for use
	8,										// min number of staff charges
	12,										// max number of staff charges
	0,										// book cost
	100 },									// scroll cost

	{ SPL_WALL,								// #defines name of spell for ease of use
	28,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Fire Wall",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	3,										// lvl book can be found
	2,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	27,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_FIREWALLC, NULL, NULL },			// missiles to launch per casting
	2,										// mana adj per spell lvl
	16,										// min mana for use
	8,										// min number of staff charges
	16,										// max number of staff charges
	6000,									// book cost
	400 },									// scroll cost

	{ SPL_TOWN,								// #defines name of spell for ease of use
	35,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Town Portal",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	3,										// lvl book can be found
	3,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	20,										// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_TOWN, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	18,										// min mana for use
	8,										// min number of staff charges
	12,										// max number of staff charges
	3000,										// book cost
	200 },									// scroll cost

	{ SPL_STONE,							// #defines name of spell for ease of use
	60,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Stone Curse",							// text name of spell
	NULL,									// text name of spell if it can be a skill
#if IS_VERSION(RETAIL)
	6,										// lvl book can be found
	5,										// lvl staff can be found
#else
	-1,-1,
#endif
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	51,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_STONE, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	40,										// min mana for use
	8,										// min number of staff charges
	16,										// max number of staff charges
	12000,										// book cost
	800 },									// scroll cost

	{ SPL_INFRA,							// #defines name of spell for ease of use
	40,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Infravision",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	36,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_INFRA, NULL, NULL },				// missiles to launch per casting
	5,										// mana adj per spell lvl
	20,										// min mana for use
	0,										// min number of staff charges
	0,										// max number of staff charges
	0,										// book cost
	600 },									// scroll cost

	{ SPL_PHASE,							// #defines name of spell for ease of use
	12,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Phasing",								// text name of spell
	NULL,									// text name of spell if it can be a skill
	7,										// lvl book can be found
	6,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	39,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_PHASE, NULL, NULL },				// missiles to launch per casting
	2,										// mana adj per spell lvl
	4,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	3500,									// book cost
	200 },									// scroll cost

	{ SPL_MANASHLD,							// #defines name of spell for ease of use
	33,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Mana Shield",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	6,										// lvl book can be found
	5,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	25,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_MANASHIELD, NULL, NULL },			// missiles to launch per casting
	0,										// mana adj per spell lvl
	33,										// min mana for use
	4,										// min number of staff charges
	10,										// max number of staff charges
	16000,										// book cost
	1200 },									// scroll cost

	{ SPL_FIREBALL,							// #defines name of spell for ease of use
	16,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Fireball",								// text name of spell
	NULL,									// text name of spell if it can be a skill
	8,										// lvl book can be found
	7,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	48,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_FIREBALL, NULL, NULL },			// missiles to launch per casting
	1,										// mana adj per spell lvl
	10,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	8000,										// book cost
	300 },									// scroll cost

	{ SPL_GUARDIAN,							// #defines name of spell for ease of use
	50,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Guardian",								// text name of spell
	NULL,									// text name of spell if it can be a skill
#if IS_VERSION(RETAIL)
	9,										// lvl book can be found
	8,										// lvl staff can be found
#else
	-1,-1,
#endif
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	61,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_GUARDIAN, NULL, NULL },			// missiles to launch per casting
	2,										// mana adj per spell lvl
	30,										// min mana for use
	16,										// min number of staff charges
	32,										// max number of staff charges
	14000,									// book cost
	950 },									// scroll cost

	{ SPL_CHAIN,							// #defines name of spell for ease of use
	30,										// base cost in mana
	ST_LIGHT,								// spell type (fire, light, misc)
	"Chain Lightning",						// text name of spell
	NULL,									// text name of spell if it can be a skill
	8,										// lvl book can be found
	7,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	54,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_CHAIN, NULL, NULL },				// missiles to launch per casting
	1,										// mana adj per spell lvl
	18,										// min mana for use
	20,										// min number of staff charges
	60,										// max number of staff charges
	11000,									// book cost
	750 },									// scroll cost

	{ SPL_WAVE,								// #defines name of spell for ease of use
	35,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Flame Wave",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	9,										// lvl book can be found
	8,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	54,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_WAVE, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	20,										// min mana for use
	20,										// min number of staff charges
	40,										// max number of staff charges
	10000,										// book cost
	650 },									// scroll cost

	{ SPL_DOOM,								// #defines name of spell for ease of use
	0,										// base cost in mana
	ST_LIGHT,								// spell type (fire, light, misc)
	"Doom Serpents",						// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	0,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ NULL, NULL, NULL },					// missiles to launch per casting
	0,										// mana adj per spell lvl
	0,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	0,										// book cost
	0 },									// scroll cost

	{ SPL_BLOODR,							// #defines name of spell for ease of use
	0,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Blood Ritual",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	0,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ NULL, NULL, NULL },					// missiles to launch per casting
	0,										// mana adj per spell lvl
	0,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	0,										// book cost
	0 },									// scroll cost

	{ SPL_NOVA,								// #defines name of spell for ease of use
	60,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Nova",									// text name of spell
	NULL,									// text name of spell if it can be a skill
	14,										// lvl book can be found
	10,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	87,										// min intelligence to use
	IS_CAST4,								// sfx to use
	{ MIT_NOVA, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	35,										// min mana for use
	16,										// min number of staff charges
	32,										// max number of staff charges
	21000,									// book cost
	1300 },									// scroll cost

	{ SPL_INVIS,							// #defines name of spell for ease of use
	0,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Invisibility",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	0,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ NULL, NULL, NULL },					// missiles to launch per casting
	0,										// mana adj per spell lvl
	0,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	0,										// book cost
	0 },									// scroll cost

	{ SPL_FLAME,							// #defines name of spell for ease of use
	11,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Inferno",								// text name of spell
	NULL,									// text name of spell if it can be a skill
	3,										// lvl book can be found
	2,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	20,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_FLAMEC, NULL, NULL },				// missiles to launch per casting
	1,	 									// mana adj per spell lvl
	6,										// min mana for use
	20,										// min number of staff charges
	40,										// max number of staff charges
	2000,										// book cost
	100 },									// scroll cost

	{ SPL_GOLEM,							// #defines name of spell for ease of use
	100,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Golem",								// text name of spell
	NULL,									// text name of spell if it can be a skill
#if IS_VERSION(RETAIL)
	11,										// lvl book can be found
	9,										// lvl staff can be found
#else
	-1,-1,
#endif
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	81,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_GOLEM, NULL, NULL },				// missiles to launch per casting
	6,										// mana adj per spell lvl
	60,										// min mana for use
	16,										// min number of staff charges
	32,										// max number of staff charges
	18000,										// book cost
	1100 },									// scroll cost

	#if 0
	{ SPL_BLOODB,							// #defines name of spell for ease of use
	0,										// base cost in mana
	ST_LIGHT,								// spell type (fire, light, misc)
	"Blood Boil",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	0,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ NULL, NULL, NULL },					// missiles to launch per casting
	0,										// mana adj per spell lvl
	0,										// min mana for use
	0,										// min number of staff charges
	0,										// max number of staff charges
	0,										// book cost
	0 },									// scroll cost
	#else
	{ SPL_RAGE,								// #defines name of spell for ease of use
	15,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Rage",									// text name of spell
	"Rage",									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	0,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_RAGE, NULL, NULL },				// missiles to launch per casting
	1,										// mana adj per spell lvl
	1,										// min mana for use
	0,										// min number of staff charges
	0,										// max number of staff charges
	0,										// book cost
	0 },									// scroll cost
	#endif

	{ SPL_TELE,								// #defines name of spell for ease of use
	35,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Teleport",								// text name of spell
	NULL,									// text name of spell if it can be a skill
	14,										// lvl book can be found
	12,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	105,									// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_TELE, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	15,										// min mana for use
	16,										// min number of staff charges
	32,										// max number of staff charges
	20000,									// book cost
	1250 },									// scroll cost

	{ SPL_APOCA,							// #defines name of spell for ease of use
	150,									// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Apocalypse",							// text name of spell
	NULL,									// text name of spell if it can be a skill
#if IS_VERSION(RETAIL)
	19,										// lvl book can be found
	15,										// lvl staff can be found
#else
	-1,-1,
#endif
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	149,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_APOCA, NULL, NULL },				// missiles to launch per casting
	6,										// mana adj per spell lvl
	90,										// min mana for use
	8,										// min number of staff charges
	12,										// max number of staff charges
	30000,										// book cost
	2000 },									// scroll cost

	{ SPL_ETHER,							// #defines name of spell for ease of use
	100,									// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Etherealize",							// text name of spell
	NULL,									// text name of spell if it can be a skill
#if IS_VERSION(RETAIL)
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
#else
	-1,-1,
#endif
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	93,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_ETHER, NULL, NULL },				// missiles to launch per casting
	0,										// mana adj per spell lvl
	100,									// min mana for use
	2,										// min number of staff charges
	6,										// max number of staff charges
	26000,										// book cost
	1600 },									// scroll cost

	{ SPL_REPAIR,							// #defines name of spell for ease of use
	0,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Item Repair",							// text name of spell
	"Item Repair",							// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	TRUE,									// Avail in town?
	-1,										// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_REPAIR, NULL, NULL },				// missiles to launch per casting
	0,										// mana adj per spell lvl
	0,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	0,										// book cost
	0 },									// scroll cost

	{ SPL_RECHARGE,							// #defines name of spell for ease of use
	0,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Staff Recharge",						// text name of spell
	"Staff Recharge",						// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	TRUE,									// Avail in town?
	-1,										// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_RECHARGE, NULL, NULL },			// missiles to launch per casting
	0,										// mana adj per spell lvl
	0,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	0,										// book cost
	0 },									// scroll cost

	{ SPL_DISARM,							// #defines name of spell for ease of use
	0,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Trap Disarm",							// text name of spell
	"Trap Disarm",							// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	-1,										// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_DISARM, NULL, NULL },				// missiles to launch per casting
	0,	 									// mana adj per spell lvl
	0,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	0,										// book cost
	0 },									// scroll cost

	{ SPL_ELEMENT,							// #defines name of spell for ease of use
	35,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Elemental",							// text name of spell
	NULL,									// text name of spell if it can be a skill
#if IS_VERSION(RETAIL)
	 8,										// lvl book can be found
	 6,										// lvl staff can be found
#else
	-1,-1,
#endif
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	68,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_ELEMENT, NULL, NULL },			// missiles to launch per casting
	2,	 									// mana adj per spell lvl
	20,										// min mana for use
	20,										// min number of staff charges
	60,										// max number of staff charges
	10500,										// book cost
	700 },									// scroll cost

	{ SPL_CBOLT,							// #defines name of spell for ease of use
	6,										// base cost in mana
	ST_LIGHT,								// spell type (fire, light, misc)
	"Charged Bolt",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	1,										// lvl book can be found
	1,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	25,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_CBOLT, NULL, NULL },				// missiles to launch per casting
	1,	 									// mana adj per spell lvl
	6,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	1000,										// book cost
	50 },									// scroll cost

	{ SPL_HBOLT,							// #defines name of spell for ease of use
	7,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Holy Bolt",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	1,										// lvl book can be found
	1,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	20,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_HBOLT, NULL, NULL },				// missiles to launch per casting
	1,	 									// mana adj per spell lvl
	3,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	1000,										// book cost
	50 },									// scroll cost

	{ SPL_RESURRECT,						// #defines name of spell for ease of use
	20,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Resurrect",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	5,										// lvl staff can be found
	FALSE,									// Targeted spell?
	TRUE,									// Avail in town?
	30,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_RESURRECT, NULL, NULL },			// missiles to launch per casting
	0,	 									// mana adj per spell lvl
	20,										// min mana for use
	4,										// min number of staff charges
	10,										// max number of staff charges
	4000,									// book cost
	250 },									// scroll cost

	{ SPL_TELEKINESIS,						// #defines name of spell for ease of use
	15,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Telekinesis",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	2,										// lvl book can be found
	2,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	33,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_TELEKINESIS, NULL, NULL },		// missiles to launch per casting
	2,	 									// mana adj per spell lvl
	8,										// min mana for use
	20,										// min number of staff charges
	40,										// max number of staff charges
	2500,										// book cost
	200 },									// scroll cost

	{ SPL_HEALOTHER,						// #defines name of spell for ease of use
	5,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Heal Other",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	1,										// lvl book can be found
	1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	TRUE,									// Avail in town?
	17,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_HEALOTHER, NULL, NULL },			// missiles to launch per casting
	3,	 									// mana adj per spell lvl
	1,										// min mana for use
	20,										// min number of staff charges
	40,										// max number of staff charges
	1000,										// book cost
	50 },									// scroll cost

	{ SPL_BSTAR,							// #defines name of spell for ease of use
	25,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Blood Star",							// text name of spell
	NULL,									// text name of spell if it can be a skill
#if IS_VERSION(RETAIL)
	14,										// lvl book can be found
	13,										// lvl staff can be found
#else
	-1,-1,
#endif
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	70,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_FLARE, NULL, NULL },				// missiles to launch per casting
	2,	 									// mana adj per spell lvl
	14,										// min mana for use
	20,										// min number of staff charges
	60,										// max number of staff charges
	27500,										// book cost
	1800 },									// scroll cost

	{ SPL_BONESPIRIT,						// #defines name of spell for ease of use
	24,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Bone Spirit",							// text name of spell
	NULL,									// text name of spell if it can be a skill
#if IS_VERSION(RETAIL)
	9,										// lvl book can be found
	7,										// lvl staff can be found
#else
	-1,-1,
#endif
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	34,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_BONESPIRIT, NULL, NULL },			// missiles to launch per casting
	1, 										// mana adj per spell lvl
	12,										// min mana for use
	20,										// min number of staff charges
	60,										// max number of staff charges
	11500,										// book cost
	800 },										// scroll cost
	
	{ SPL_MANA,								// #defines name of spell for ease of use
	255,									// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Mana",									// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	5,										// lvl staff can be found
	FALSE,									// Targeted spell?
	TRUE,									// Avail in town?
	17,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_MANA, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	1,										// min mana for use
	12,										// min number of staff charges
	24,										// max number of staff charges
	1000,										// book cost
	50 },									// scroll cost
	
	{ SPL_FMANA,							// #defines name of spell for ease of use
	255,									// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"the Magi",								// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	20,										// lvl staff can be found
	FALSE,									// Targeted spell?
	TRUE,									// Avail in town?
	45,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_FMANA, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	1,										// min mana for use
	15,										// min number of staff charges
	30,										// max number of staff charges
	100000,									// book cost
	200 },									// scroll cost
	
	{ SPL_RANDOM,							// #defines name of spell for ease of use
	255,									// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"the Jester",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	4,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	30,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_RANDOM, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	1,										// min mana for use
	15,										// min number of staff charges
	30,										// max number of staff charges
	100000,									// book cost
	200 },									// scroll cost
	
	{ SPL_LTWALL,							// #defines name of spell for ease of use
	28,										// base cost in mana
	ST_LIGHT,								// spell type (fire, light, misc)
	"Lightning Wall",				  		// text name of spell
	NULL,									// text name of spell if it can be a skill
	3,										// lvl book can be found
	2,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	27,										// min intelligence to use
	IS_CAST4,								// sfx to use
	{ MIT_LIGHTWALLC, NULL, NULL },			// missiles to launch per casting
	2,										// mana adj per spell lvl
	16,										// min mana for use
	8,										// min number of staff charges
	16,										// max number of staff charges
	6000,									// book cost
	400 },									// scroll cost
	
	{ SPL_IMMOLATION,						// #defines name of spell for ease of use
	60,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Immolation",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	14,										// lvl book can be found
	10,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	87,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_IMMOLATION, NULL, NULL },				// missiles to launch per casting
	3,										// mana adj per spell lvl
	35,										// min mana for use
	16,										// min number of staff charges
	32,										// max number of staff charges
	21000,									// book cost
	1300 },									// scroll cost

	{ SPL_TELESTAIRS,						// #defines name of spell for ease of use
	35,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Warp",									// text name of spell
	NULL,									// text name of spell if it can be a skill
	3,										// lvl book can be found
	3,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	25,										// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_TELESTAIRS, NULL, NULL },			// missiles to launch per casting
	3,										// mana adj per spell lvl
	18,										// min mana for use
	8,										// min number of staff charges
	12,										// max number of staff charges
	3000,									// book cost
	200 },									// scroll cost

	{ SPL_REFLECT,							// #defines name of spell for ease of use
	35,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Reflect",								// text name of spell
	NULL,									// text name of spell if it can be a skill
	3,										// lvl book can be found
	3,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	25,										// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_REFLECT, NULL, NULL },			// missiles to launch per casting
	3,										// mana adj per spell lvl
	15,										// min mana for use
	8,										// min number of staff charges
	12,										// max number of staff charges
	3000,									// book cost
	200 },									// scroll cost

	{ SPL_BERSERK,							// #defines name of spell for ease of use
	35,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Berserk",								// text name of spell
	NULL,									// text name of spell if it can be a skill
	3,										// lvl book can be found
	3,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	35,										// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_BERSERK, NULL, NULL },			// missiles to launch per casting
	3,										// mana adj per spell lvl
	15,										// min mana for use
	8,										// min number of staff charges
	12,										// max number of staff charges
	3000,									// book cost
	200 },									// scroll cost

	{ SPL_RINGOFFIRE,								// #defines name of spell for ease of use
	28,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Ring of Fire",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	5,										// lvl book can be found
	5,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	27,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_FLAMEBOX, NULL, NULL },			// missiles to launch per casting
	2,										// mana adj per spell lvl
	16,										// min mana for use
	8,										// min number of staff charges
	16,										// max number of staff charges
	6000,									// book cost
	400 },									// scroll cost
	
	{ SPL_SHOWMAGITEMS,						// #defines name of spell for ease of use
	15,										// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Search",								// text name of spell
	"Search",								// text name of spell if it can be a skill
	1,										// lvl book can be found
	3,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	25,										// min intelligence to use
	IS_CAST6,								// sfx to use
	{ MIT_SHOWMAGITEMS, NULL, NULL },		// missiles to launch per casting
	1,										// mana adj per spell lvl
	1,										// min mana for use
	8,										// min number of staff charges
	12,										// max number of staff charges
	3000,									// book cost
	200 },									// scroll cost

	{ SPL_RUNEOFFIRE,						// #defines name of spell for ease of use
	255,									// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Rune of Fire",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	48,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_RUNEOFFIRE, NULL, NULL },			// missiles to launch per casting
	1,										// mana adj per spell lvl
	10,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	8000,									// book cost
	300 },									// scroll cost
	
	{ SPL_RUNEOFLIGHT,						// #defines name of spell for ease of use
	255,									// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Rune of Light",						// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	48,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_RUNEOFLIGHT, NULL, NULL },   		// missiles to launch per casting
	1,										// mana adj per spell lvl
	10,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	8000,									// book cost
	300 },									// scroll cost

	{ SPL_RUNEOFNOVA,						// #defines name of spell for ease of use
	255,									// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Rune of Nova",							// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	48,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_RUNEOFNOVA, NULL, NULL },   		// missiles to launch per casting
	1,										// mana adj per spell lvl
	10,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	8000,									// book cost
	300 },									// scroll cost

	{ SPL_RUNEOFIMMOLATION,					// #defines name of spell for ease of use
	255,									// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Rune of Immolation",						// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	48,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_RUNEOFIMMOLATION, NULL, NULL },   		// missiles to launch per casting
	1,										// mana adj per spell lvl
	10,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	8000,									// book cost
	300 },									// scroll cost

	{ SPL_RUNEOFSTONE,						// #defines name of spell for ease of use
	255,									// base cost in mana
	ST_MISC,								// spell type (fire, light, misc)
	"Rune of Stone",						// text name of spell
	NULL,									// text name of spell if it can be a skill
	-1,										// lvl book can be found
	-1,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	48,										// min intelligence to use
	IS_CAST8,								// sfx to use
	{ MIT_RUNEOFSTONE, NULL, NULL },   		// missiles to launch per casting
	1,										// mana adj per spell lvl
	10,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	8000,									// book cost
	300 },									// scroll cost

#if defined (HELLFIRE2)
	{ SPL_RINGOFLIGHT,	  					// #defines name of spell for ease of use
	28,										// base cost in mana
	ST_LIGHT,								// spell type (fire, light, misc)
	"Ring of Light",						// text name of spell
	NULL,									// text name of spell if it can be a skill
	6,										// lvl book can be found
	6,										// lvl staff can be found
	TRUE,									// Targeted spell?
	FALSE,									// Avail in town?
	23,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_LIGHTBOX, NULL, NULL },			// missiles to launch per casting
	2,										// mana adj per spell lvl
	16,										// min mana for use
	8,										// min number of staff charges
	16,										// max number of staff charges
	6000,									// book cost
	400 },									// scroll cost
	
	{ SPL_AURA,								// #defines name of spell for ease of use
	30,										// base cost in mana
	ST_LIGHT,								// spell type (fire, light, misc)
	"Aura",									// text name of spell
	NULL,									// text name of spell if it can be a skill
	1,										// lvl book can be found
	1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	33,										// min intelligence to use
	IS_CAST4,								// sfx to use
	{ MIT_AURA, MIT_AURA2, NULL },			// missiles to launch per casting
	2,										// mana adj per spell lvl
	16,										// min mana for use
	20,										// min number of staff charges
	40,										// max number of staff charges
	7500,									// book cost
	500 },									// scroll cost

	{ SPL_SPIRALFIREBALL,					// #defines name of spell for ease of use
	6,										// base cost in mana
	ST_FIRE,								// spell type (fire, light, misc)
	"Spiral Firebolt",						// text name of spell
	NULL,									// text name of spell if it can be a skill
	1,										// lvl book can be found
	1,										// lvl staff can be found
	FALSE,									// Targeted spell?
	FALSE,									// Avail in town?
	15,										// min intelligence to use
	IS_CAST2,								// sfx to use
	{ MIT_SPIRALFIREBALL, NULL, NULL },			// missiles to launch per casting
	1,										// mana adj per spell lvl
	3,										// min mana for use
	40,										// min number of staff charges
	80,										// max number of staff charges
	1000,									// book cost
	50 },									// scroll cost
#endif	// HELLFIRE2
};
