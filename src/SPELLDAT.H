/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/SPELLDAT.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define ST_FIRE		0
#define ST_LIGHT	1
#define ST_MISC		2

#define MA_MAX		255

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	BYTE sName;				// #defines name of spell for ease of use
	BYTE sManaCost;			// base cost in mana
	BYTE sType;				// spell type (fire, light, misc)
	char *sNameText;		// text name of spell
	char *sSkillText;		// text name of spell if can be a skill
	int  sBookLvl;			// lvl book can be found
	int  sStaffLvl;			// lvl staff can be found
	BOOL sTargeted;			// Targeted spell?
	BOOL sTownSpell;		// Avail in town?
	int  sMinInt;			// min intelligence to use
	BYTE sSFX;				// sfx to use
	BYTE sMissiles[3];		// missiles to launch per casting
	BYTE sManaAdj;			// mana adj per spell lvl
	BYTE sMinMana;			// min mana for use
	int  sStaffMin;			// min staff charges
	int  sStaffMax;			// max staff charges
	int  sBookCost;			// book cost
	int  sStaffCost;		// staff cost
} SpellData;

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/
extern SpellData spelldata[];

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
