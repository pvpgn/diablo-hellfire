/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/ITEMDAT.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Powers List
**-----------------------------------------------------------------------*/

#define PL_TOHIT	0		// To hit 
#define PL_NTOHIT	1		// Negative to hit 
#define PL_TODAM	2		// Damage amount %
#define PL_NTODAM	3		// Negative damage amount %
#define PL_DAHT		4		// Damage and to hit %
#define PL_NDAHT	5		// Negative damage and to hit %
#define PL_AC		6		// Armor Class % increase
#define PL_NAC		7		// Negative % Armor Class

#define PL_RFIRE	8		// Resistance to fire %
#define PL_RLGHT	9		// Resistance to lightning %
#define PL_RMAG		10		// Resistance to misc magic %
#define PL_RALL		11		// Resistance to all

//#define PL_SCOST	12		// Spell cost (-) (removed by drb for multiplayer pickup flag)
//#define PL_SDUR		13		// Spell duration (removed by drb for unique crash)
#define PL_SLVL		14		// Spell levels

#define PL_CHRG		15		// Staff charges

#define PL_FHIT		16		// Fire hit
#define PL_LHIT		17		// Lightning hit
#define PL_CHAOS	18		//not in game

#define PL_STR		19		// Strength attribute
#define PL_NSTR		20		// Negative strength attribute
#define PL_MAG		21		// Magic attribute
#define PL_NMAG		22		// Negative magic attribute
#define PL_DEX		23		// Dexterity attribute
#define PL_NDEX		24		// Negative dexterity attribute
#define PL_VIT		25		// Vitality attribute
#define PL_NVIT		26		// Negative vitality attribute
#define PL_STATS	27		// All attributes
#define PL_NSTATS	28		// Negative all attributes

#define PL_GETHIT	29		// Add to every get hit
#define PL_NGETHIT	30		// Subtract from every get hit

#define PL_HP		31		// Hit points attribute
#define PL_NHP		32		// Negative hit points attribute
#define PL_MANA		33		// Mana attribute
#define PL_NMANA	34		// Negative mana attribute

#define PL_DUR		35		// Add to item's durability %
#define PL_NDUR		36		// Subtract from item's durability %
#define PL_IND		37		// Infinite durability

#define PL_LIGHT	38		// Add to player's light source
#define PL_NLIGHT	39		// Subtract from player's light source

#define PL_INVIS	40		//not in game  Player invisible from radius

#define PL_NUMARWS	41		// Shoots multiple arrows
#define PL_FARROW	42		// Fire arrows
#define PL_LARROW	43		// Lightning arrows

#define PL_GFX		44		// Change to unique graphic

#define PL_THORN	45		// When item deals damage user gets damaged too
#define PL_LMANA	46		// Player looses all mana/ can't regen
#define PL_NOHEAL	47		// User can't heal
#define PL_FEAR		48		// When monster is struck, runs in fear (50%-ML)
#define PL_RABID	49		//not in game
#define PL_HITADD	50		// Half damage is added to player's hp
#define PL_SEEINVIS	51		//not in game See invisible
#define PL_TRAPDAM	52		// Half trap damage
#define PL_BEAR		53		// Knock monster back a square (if poss)
#define PL_MNOHEAL	54		// Monster no longer heals
#define PL_BAT		55		// Damage done adds to mana
#define PL_LEECH	56		// Damage dones adds to life
#define PL_ENAC		57		// Reduces the enemies ac by this
#define PL_ATANIM	58		// Attack anim quicker
#define PL_HTANIM	59		// Hit anim quicker
#define PL_BLANIM	60		// Block anim quicker

#define PL_DAMADD	61		// Damage Hit point modifier
	
// From here down, from uniques

#define PL_RNDARW	62		// Random arrow speeds
#define PL_DAMAGE	63		// Changes weapon damage to (param1-param2)
#define PL_DURNUM	64		// Durability set to param1
#define PL_NSTRREQ	65		// No minimum strength requirement
#define PL_SPELL	66		// Add charges to your staff
#define PL_FALCON	67		// Skips frames 1-3 of swing anim
#define PL_ONEHAND	68		// Change item to one handed
#define PL_DAMDEMON	69		// damage vs. demon only
#define PL_ZERORES	70		// All resistance equal to zero
#define PL_HYPER	71		// Hyperspace spell (param1 charges)
#define PL_CONST	72		// Constricting
#define PL_SKING	73		// Skeleton king power (life stealing)
#define PL_INFRA	74		// Infravision
#define PL_ACTUALAC	75		// Actual Armor Class
#define PL_HARQUN	76		// +(Armor Class)HP
#define PL_HARQUN2	77		// +(Mana/10)armor
#define PL_HARQUN3	78		// +(30-charlevel) resist fire
#define PL_NACTULAC	79		// Negative actual armor class
#define PL_NRFIRE	80		// Resistance to fire % (negative)
#define PL_NRLGHT	81		// Resistance to lightning % (negative)
#define PL_NRMAG	82		// Resistance to misc magic % (negative)
#define PL_NRALL	83		// Resistance to all % (negative)
#define PL_DEVAST	84
#define PL_DECAY	85 
#define PL_PERIL	86
#define PL_RNDDAM	87
#define PL_FRAGILE	88
#define PL_DOPPEL	89
#define PL_DEMONAC	90
#define PL_UNDEADAC	91
#define PL_ACOLYTE	92
#define PL_GLADIATR	93
#define PL_X		94		// Unknown???? TEMP--------

/*-----------------------------------------------------------------------**
** Power List Bit flags
**-----------------------------------------------------------------------*/

#define PLF_ARMOR	0x100000
#define PLF_SHIELD	0x010000
#define PLF_WEAPON	0x001000
#define PLF_STAFF	0x000100
#define PLF_BOW		0x000010
#define PLF_RING	0x000001

/*-----------------------------------------------------------------------**
** Item Misc Id
**-----------------------------------------------------------------------*/

#define IMID_NONE		0	// No misc ability
#define IMID_FIRSTPOT		1
#define IMID_PHEAL			2	// Potion of full heal
#define IMID_PLHEAL			3	// Potion of Light heal
#define IMID_PSHEAL			4	// Potion of Serious heal
#define IMID_PDHEAL			5	// Potion of Deadly heal
#define IMID_PMANA			6	// Potion of Mana
#define IMID_PFMANA			7	// Potion of Full mana
#define IMID_PEXP			8	// Potion of Experience
#define IMID_PNEXP			9	// Potion of Negative Experience
#define IMID_ESTR			10	// Elixir of Strength
#define IMID_EMAG			11	// Elixir of Magic
#define IMID_EDEX			12	// Elixir of Dexterity
#define IMID_EVIT			13	// Elixir of Vitality
#define IMID_ENSTR			14	// Elixir of Negative Strength
#define IMID_ENMAG			15	// Elixir of Negative Magic
#define IMID_ENDEX			16	// Elixir of Negative Dexerity
#define IMID_ENVIT			17	// Elixir of Negative Vitaltiy
#define IMID_REJUV			18
#define IMID_FREJUV			19  
#define IMID_LASTPOT		20

#define IMID_SCROLL			21	// Scroll of spell
#define IMID_TSCROLL		22	// Scroll of targeted spell
#define IMID_STAFF			23	// Item with a spell

#define IMID_BOOK			24	// Book of spell
#define IMID_RING			25
#define IMID_AMULET			26

#define IMID_UNIQUE			27	// Unique item so no magic / magic already built in

#define IMID_MEAT			28	// Slab of meat

#define IMID_FIRSTOIL		29
#define IMID_OIL			30
#define IMID_OILACC			31
#define IMID_OILMAST		32	
#define IMID_OILSHRP		33
#define IMID_OILDEATH		34
#define IMID_OILSKILL		35
#define IMID_OILBLKSM		36
#define IMID_OILFORT		37
#define IMID_OILPERM		38
#define IMID_OILHARD		39
#define IMID_OILIMPER		40
#define IMID_LASTOIL		41

#define IMID_MAPOFDOOM		42	// The end quest map of doom item

#define IMID_EAR			43
#define IMID_SPECTRAL		44

#define IMID_BOMB			45

#define IMID_FIRSTRUNE		46
#define IMID_RUNEFIRE		47
#define IMID_RUNELIGHT		48
#define IMID_RUNENOVA		49
#define IMID_RUNEIMMOLATE	50
#define IMID_RUNESTONE		51
#define IMID_LASTRUNE		52

#define IMID_AURIC			53

#define IMID_FULLNOTE		54

/*-----------------------------------------------------------------------**
** Item Id's
**-----------------------------------------------------------------------*/

#define ITEMID_NONE			0

#define ITEMID_SHORTBOW		1
#define ITEMID_LONGBOW		2
#define ITEMID_BOW			3
#define ITEMID_COMPBOW		4
#define ITEMID_LWARBOW		5
#define ITEMID_LBATTLEBOW	6

#define ITEMID_DAGGER		7
#define ITEMID_FALCHION		8
#define ITEMID_CLAYMORE		9
#define ITEMID_BROADSWORD	10
#define ITEMID_SABRE		11
#define ITEMID_SCIMITAR		12
#define ITEMID_LONGSWORD	13
#define ITEMID_BASTSWORD	14
#define ITEMID_2HANDSWORD	15
#define ITEMID_GREATSWORD	16

#define ITEMID_CLEAVER		17
#define ITEMID_LARGEAXE		18
#define ITEMID_BROADAXE		19
#define ITEMID_SMALLAXE		20
#define ITEMID_BATTLEAXE	21
#define ITEMID_GREATAXE		22

#define ITEMID_MACE			23
#define ITEMID_MORNSTAR		24
#define ITEMID_CLUB			25
#define ITEMID_MAUL			26
#define ITEMID_WARHAMMER	27
#define ITEMID_FLAIL		28

#define ITEMID_LONGSTAFF	29
#define ITEMID_SHORTSTAFF	30
#define ITEMID_COMPSTAFF	31
#define ITEMID_QTRSTAFF		32
#define ITEMID_WARSTAFF		33

#define ITEMID_SKULLCAP		34
#define ITEMID_HELM			35
#define ITEMID_GREATHELM	36
#define ITEMID_CROWN		37

#define ITEMID_RAGS			39
#define ITEMID_STDLEATHER	40
#define ITEMID_CLOAK		41
#define ITEMID_ROBE			42
#define ITEMID_CHAINMAIL	43
#define ITEMID_LEATHER		44
#define ITEMID_BREASTPLATE	45
#define ITEMID_CAPE			46
#define ITEMID_PLATEMAIL	47
#define ITEMID_FULLPLATE	48

#define ITEMID_BUCKLER		49
#define ITEMID_SMALLSHLD	50
#define ITEMID_LARGESHLD	51
#define ITEMID_KITESHLD		52
#define ITEMID_TOWERSHLD	53

#define ITEMID_RING			54
#define ITEMID_BOOK			55
#define ITEMID_AMULET		56

#define ITEMID_SKCROWN		57
#define ITEMID_IRING		58
#define ITEMID_OPTAMULET	59
#define ITEMID_TRING		60
#define ITEMID_HALCREST		61
#define ITEMID_MAP			62
#define ITEMID_ELIXIR		63
#define ITEMID_ARMOFVAL		64
#define ITEMID_STEELVEIL	65
#define ITEMID_GRISWOLD		66
#define ITEMID_LGTFORGE		67
#define ITEMID_LAZSTAFF		68
#define ITEMID_ARMRCOW		69// dude!

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern ItemDataStruct AllItemsList[];
extern const PLStruct PL_Prefix[];
extern const PLStruct PL_Suffix[];
extern const UItemStruct UniqueItemList[];
