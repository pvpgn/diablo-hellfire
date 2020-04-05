/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/ITEMS.H 3     2/06/97 6:08p Jessmac $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXITEMS		127

#define MAXUITEMS		128			// Max number of uniques

#define TEMPAVAIL		127

#define ITEM_RND		-1

#define INFINITE_DUR	255

#define GOLD_VT1	1000			// Gold gfx transition from 1 to several
#define GOLD_VT2	2500			// Gold gfx transition from several to many
//#define GOLD_VMAX	5000			// Max gold in an inv slot
extern int GOLD_VMAX;
extern const int GOLD_DOUBLE_VMAX;

// Used for cursors
// 1x1
typedef enum {
	ITEM_BLUEBTL	= 0,
	ITEM_SCROLL		,	// 1
	ITEM_SCROLL2	,	// 2
	ITEM_SCROLL3	,	// 3
	ITEM_1GOLD		,	// 4
	ITEM_3GOLD		,	// 5
	ITEM_5GOLD		,	// 6
	ITEM_GOLDRING	,	// 7
	ITEM_1JRING		,	// 8
	ITEM_WOODRING	,	// 9
	ITEM_BLUERING	,	// 10
	ITEM_3JRING		,	// 11
	ITEM_SLVRRING	,	// 12
	ITEM_MJRING		,	// 13
	ITEM_BRNRING	,	// 14
	ITEM_SPECTRAL	,	// 15
	ITEM_3COLORPOT	,	// 16
	ITEM_GOLDENELIX	,	// 17
	ITEM_EMPYBAND	,	// 18
	ITEM_EAR1		,	// 19
	ITEM_EAR2		,	// 20
	ITEM_EAR3		,	// 21
	ITEM_SPHERE		,	// 22
	ITEM_CUBE		,	// 23
	ITEM_PYRIMID	,	// 24
	ITEM_BLOODGEM	,	// 25
	ITEM_JSPHERE	,	// 26
	ITEM_JCUBE		,	// 27
	ITEM_JPYRIMID	,	// 28
	ITEM_VILE		,	// 29
	ITEM_BLKBTL		,	// 30
	ITEM_WHTEBTL	,	// 31
	ITEM_REDBTL		,	// 32
	ITEM_YELBTL		,	// 33
	ITEM_ORGBTL		,	// 34
	ITEM_BREDBTL	,	// 35
	ITEM_BLKBTL2	,	// 36
	ITEM_GOLDBTL	,	// 37
	ITEM_LTBLUEBTL	,	// 38
	ITEM_BLUEBTL2	,	// 39
	ITEM_BRAIN		,	// 40
	ITEM_CLAW		,	// 41
	ITEM_FANG		,	// 42
	ITEM_BREAD		,	// 43
	ITEM_AMULET		,	// 44
	ITEM_AMULET1	,	// 45
	ITEM_AMULET2	,	// 46
	ITEM_AMULET3	,	// 47
	ITEM_AMULET4	,	// 48
	ITEM_POUCH1		,	// 49
// 1x2
	ITEM_DAGGER1	,	// 50
	ITEM_DAGGER2	,	// 51
	ITEM_BIGBOTTLE	,	// 52
	ITEM_DAGGER3	,	// 53
	ITEM_DAGGER4	,	// 54
	ITEM_DAGGER5	,	// 55
// 1x3
	ITEM_BLADE		,	// 56
	ITEM_BASTSRD	,	// 57
	ITEM_FALCHION	,	// 58
	ITEM_MACE		,	// 59
	ITEM_LONGSRD	,	// 60
	ITEM_BROADSRD	,	// 61
	ITEM_SCIMITAR	,	// 62
	ITEM_MORNSTAR	,	// 63
	ITEM_SHORTSRD	,	// 64
	ITEM_CLAYMORE	,	// 65
	ITEM_CLUB		,	// 66
	ITEM_SABRE		,	// 67
	ITEM_KNTSWORD	,	// 68
	ITEM_CLUB1		,	// 69
	ITEM_CLUB2		,	// 70
	ITEM_CLUB3		,	// 71
	ITEM_SCIMITAR2	,	// 72
	ITEM_MAGSWORD	,	// 73
	ITEM_SKULSWORD	,	// 74
// 2x2
	ITEM_HELM		,	// 75
	ITEM_ROCK		,	// 76
	ITEM_CROWN		,	// 77
	ITEM_SKCROWN	,	// 78
	ITEM_MCROWN		,	// 79
	ITEM_JESTER		,	// 80
	ITEM_HARLEQ		,	// 81
	ITEM_FHELM		,	// 82
	ITEM_BUCKLER	,	// 83
	ITEM_FHELM2		,	// 84
	ITEM_GRTHELM	,	// 85
	ITEM_BOOK2		,	// 86
	ITEM_BOOK3		,	// 87
	ITEM_BOOK		,	// 88
	ITEM_MUSHROOM	,	// 89
	ITEM_SKLCAP		,	// 90
	ITEM_LCAP		,	// 91
	ITEM_FLESH		,	// 92
	ITEM_SKLCAP2	,	// 93
	ITEM_CLOTHES	,	// 94
	ITEM_CROWN2		,	// 95
	ITEM_MAP		,	// 96
	ITEM_BOOK4		,	// 97
	ITEM_FHELM3		,	// 98
	ITEM_SAMHELM	,	// 99
///#define ITEM_MUSHROOM	100
// 2x3					 
	ITEM_COMPSHLD	,	// 100
	ITEM_BTLAXE		,	// 101
	ITEM_LONGBOW	,	// 102
	ITEM_PARMOR		,	// 103
	ITEM_AXE		,	// 104
	ITEM_WSHIELD	,	// 105
	ITEM_CLEAVER	,	// 106
	ITEM_STDARMOR	,	// 107
	ITEM_COMPBOW	,	// 108
	ITEM_SHRTSTAFF	,	// 109
	ITEM_2HSWORD	,	// 110
	ITEM_CHARMOR	,	// 111
	ITEM_SMALLAXE	,	// 112
	ITEM_HVYSHIELD	,	// 113
	ITEM_SCLARMOR	,	// 114
	ITEM_SMLSHLD	,	// 115
	ITEM_SKULLSHLD	,	// 116
	ITEM_WOLFSHLD	,	// 117
	ITEM_SHORTBOW	,	// 118
	ITEM_STLLONGBOW	,	// 119
	ITEM_STLSHRTBOW	,	// 120
	ITEM_SMLWARHAM	,	// 121
	ITEM_MAUL		,	// 122
	ITEM_IRONSTAFF	,	// 123
	ITEM_STLSTAFF	,	// 124
	ITEM_LONGSTAFF	,	// 125
	ITEM_INNSIGN	,	// 126
	ITEM_HLARMOR	,	// 127
	ITEM_RAGS		,	// 128
	ITEM_QARMOR		,	// 129
	ITEM_BALLNCHN	,	// 130
	ITEM_FLAIL		,	// 131
	ITEM_TSHIELD	,	// 132
	ITEM_HNTRBOW	,	// 133
	ITEM_GRTSWORD	,	// 134
	ITEM_LARMOR		,	// 135
	ITEM_SPLTARMOR	,	// 136
	ITEM_ROBE		,	// 137
	ITEM_HVYROBE	,	// 138
	ITEM_RINGARMOR	,	// 139
	ITEM_ANVIL		,	// 140		//#define ITEM_OBOLISK	140
	ITEM_BROADAXE	,	// 141
	ITEM_LRGAXE		,	// 142
	ITEM_WICKAXE	,	// 143
	ITEM_HANDAXE	,	// 144
	ITEM_GREATAXE	,	// 145
	ITEM_IRONSHLD	,	// 146
	ITEM_KITESHLD	,	// 147
	ITEM_LRGSHLD	,	// 148
	ITEM_CLOAK		,	// 149
	ITEM_CAPE		,	// 150
	ITEM_PARMOR2	,	// 151
	ITEM_PARMOR3	,	// 152
	ITEM_BPLATE		,	// 153
	ITEM_RINGMAIL	,	// 154
	ITEM_BISHOPSTF	,	// 155
	ITEM_GEMGRTAXE	,	// 156
	ITEM_ARKARMOR	,	// 157
					
	ITEM_CROSBOW	,	// 158
	ITEM_NAJARMOR	,	// 159
	ITEM_GRIZZLY	,	// 160
	ITEM_GRANDPA	,	// 161
	ITEM_PROTECT	,	// 162
	ITEM_REAVER		,	// 163
	ITEM_WINDFOR	,	// 164
	ITEM_SWARBOW	,	// 165
	ITEM_COMPSTF	,	// 166
	ITEM_SBATLBOW	,	// 167
					
	ITEM_GOLD		,	// 168
	
// New 1x1
	ITEM_MERLINRING	= 168,	// 168	// intentional duplicate to gold.
	ITEM_MANARING	,	// 169
	ITEM_AMULWARD	,	// 170
	ITEM_NECMAGIC	,	// 171
	ITEM_NECHEALTH	,	// 172
	ITEM_KARIKSRING	,	// 173
	ITEM_RINGGROUND	,	// 174
	ITEM_AMULPROT	,	// 175
	ITEM_MERCRING	,	// 176
	ITEM_RINGTHUND	,	// 177
	ITEM_NECTRUTH	,	// 178
	ITEM_RINGGIANTS	,	// 179
	ITEM_AMULGOLD	,	// 180
	ITEM_RINGMYSTIC	,	// 181
	ITEM_RINGCOPPER	,	// 182
	ITEM_AMULACOLYT	,	// 183
	ITEM_RINGMAGMA	,	// 184
	ITEM_NECPURIFY	,	// 185
	ITEM_RINGGLADTR	,	// 186
	ITEM_RUNEBOMB	,	// 187
	ITEM_THEODORE	,
	ITEM_TORNPAPER1	,
	ITEM_TORNPAPER2	,
	ITEM_TORNPAPER3	,
	ITEM_WHOLEPAPER	,
	ITEM_FIRERUNE1  ,
	ITEM_FIRERUNE2  ,
	ITEM_LIGHTRUNE1 ,
	ITEM_LIGHTRUNE2 ,
	ITEM_STONERUNE  ,
					
// new 2x2
	ITEM_SUITGREY,
	ITEM_SUITBRWN,
	
// new 1x3
	ITEM_SWORDEDGE	,	// 188
	ITEM_SWORDGLAM	,	// 189
	ITEM_SWORDSERR	,	// 190

// new 2x3
	ITEM_ARMRDARK	,	// 191
	ITEM_ARMRBONECH	,	// 192
	ITEM_HAMRTHUND	,	// 193
	ITEM_SWRDCRYSTL	,	// 194
	ITEM_STAFJESTER	,	// 195
	ITEM_STAFMANA	,	// 196
	ITEM_BOWVULCAN	,	// 197
	ITEM_BOWSPEED	,	// 198
	ITEM_AXEANCIENT	,	// 199
	ITEM_CLUBCARNAG	,	// 200
	ITEM_MACEDARK	,	// 201
	ITEM_CLUBDECAY	,	// 202
	ITEM_AXEDECAY	,	// 203
	ITEM_SWRDDECAY	,	// 204
	ITEM_MACEDECAY	,	// 205
	ITEM_STAFDECAY	,	// 206
	ITEM_BOWDECAY	,	// 207
	ITEM_CLUBOUCH	,	// 208
	ITEM_SWRDDEVAST	,	// 209
	ITEM_AXEDEVAST	,	// 210
	ITEM_MORNDEVAST	,	// 211
	ITEM_MACEDEVAST,	// 212
	ITEM_ARMRDMNPLT,
	ITEM_ARMRCOW,
	
	ITEM_LAST_ID
} ITEM_IDS;


// Split later			  

// Used for plr gfx
#define IT_MISC		0
#define IT_SWORD	1
#define IT_AXE		2
#define IT_BOW		3
#define IT_MACE		4
#define IT_SHIELD	5
#define IT_ARMOR	6
#define IT_HELM		7
#define IT_MARMOR	8
#define IT_HARMOR	9
#define IT_STAFF	10
#define IT_GOLD		11
#define IT_RING		12
#define IT_AMULET	13
#define IT_FOOD		14

// Used for inv location
#define	IL_HAND		1
#define	IL_2HAND	2
#define	IL_BODY		3
#define	IL_HEAD		4
#define	IL_RING		5
#define IL_NECK		6
#define IL_INV		7
#define IL_SPD		8

// Item classification for treasure types
#define IC_WEAP		1
#define IC_ARMOR	2
#define IC_ITEM		3
#define IC_GOLD		4
#define IC_SPECIAL	5

// Set item indexes for first non-random items
enum _item_indexes {
	IDI_GOLD=0,				// Item Data Table indexes
// Init items
	IDI_WARRIOR,
	IDI_WARRSHLD,
	IDI_WARRCLUB,
	IDI_ROGUE,
	IDI_SORCEROR,
// Quest items
	IDI_FIRSTQUEST,
	IDI_CLEAVER=IDI_FIRSTQUEST,
	IDI_SKCROWN,	// Same as cleaver
	IDI_INFRARING,	// Same as cleaver
	IDI_ROCK,
	IDI_OPTAMULET,
	IDI_TRING,		// Same as cleaver
	IDI_BANNER,
	IDI_HARCREST,	// Same as cleaver
	IDI_STEELVEIL,	// Same as cleaver
	IDI_GLDNELIX,	// Golden Elixor
	IDI_ANVIL,		// Anvil of Dawn
	IDI_MUSHROOM,	// Black Mushroom
	IDI_BRAIN,		// Brain
	IDI_FUNGALTM,	// Fungal Tome
	IDI_SPECELIX,	// Spectral Elixir
	IDI_BLDSTONE,	// Blood Stones
	IDI_MAPOFDOOM,
	IDI_LASTQUEST=IDI_MAPOFDOOM,
// Ears
	IDI_EAR,
// Useful item
	IDI_HEAL,
	IDI_MANA,
	IDI_IDENTIFY,
	IDI_PORTAL,
// New items
	IDI_ARMOFVAL,	// Same as cleaver
	IDI_FULLHEAL,
	IDI_FULLMANA,
	IDI_GRISWOLD,
	IDI_ARMRCOW,
	IDI_LAZSTAFF,
	IDI_RESURRECT,
	IDI_OILACC,
	IDI_MONK,
	IDI_BARD,
	IDI_BARDDAGGER,
	IDI_RUNEBOMB,
	IDI_THEODORE,
	IDI_AURIC,
	IDI_NOTE1,
	IDI_NOTE2,
	IDI_NOTE3,
	IDI_FULLNOTE,
	IDI_SUITBRWN,
	IDI_SUITGREY,
	// Items for randomizing.
	// Helmets and caps
	IDI_CAP,
	IDI_SKULLCAP,
	IDI_HELM,
	IDI_FULLHELM,
	IDI_CROWN,
	IDI_GREATHEALM,
	// Body Armor
	IDI_CAPE,
	IDI_RAGS,
	IDI_CLOAK,
	IDI_ROBE,
	IDI_QUILTED_ARMOR,
	IDI_LEATHER_ARMOR,
	IDI_HARD_LEATHER_ARMOR,
	IDI_STUDDED_LEATHER_ARMOR,
	IDI_RING_MAIL,
	IDI_CHAIN_MAIL,
	IDI_SCALE_MAIL,
	IDI_BREAST_PLATE,
	IDI_SPLINT_MAIL,
	IDI_PLATE_MAIL,
	IDI_FIELD_PLATE,
	IDI_GOTHIC_PLATE,
	IDI_FULL_PLATE_MAIL,
	IDI_BUCKLER,
	IDI_SMALL_SHIELD,
	IDI_LARGE_SHIELD,
	IDI_KITE_SHIELD,
	IDI_TOWER_SHIELD,
	IDI_GOTHIC_SHIELD,
	IDI_POTION_OF_HEALING,
	IDI_POTION_OF_FULL_HEALING,
	IDI_POTION_OF_MANA,
	IDI_POTION_OF_FULL_MANA,
	// unused IDI_POTION_OF_EXPERIENCE,
	IDI_POTION_OF_REJUVENATION,
	IDI_POTION_OF_FULL_REJUVENATION,
	IDI_BLACKSMITH_OIL,
	IDI_OIL_OF_ACCURACY,
	IDI_OIL_OF_SHARPNESS,
	IDI_OIL,	// random attributes.
	IDI_ELIXIR_OF_STRENGTH,
	IDI_ELIXIR_OF_MAGIC,
	IDI_ELIXIR_OF_DEXTERITY,
	IDI_ELIXIR_OF_VITALITY,
	// unused IDI_SCROLL_OF_FIREBOLT,
	// unused IDI_SCROLL_OF_CHARGED_BOLT,
	// unused IDI_SCROLL_OF_HOLY_BOLT,
	IDI_SCROLL_OF_HEALING,
	IDI_SCROLL_OF_SEARCH,
	IDI_SCROLL_OF_LIGHTNING,
	IDI_SCROLL_OF_IDENTIFY,
	IDI_SCROLL_OF_RESURRECT,
	IDI_SCROLL_OF_FIREWALL,
	// unused IDI_SCROLL_OF_TELEKINESIS,
	IDI_SCROLL_OF_INFERNO,
	IDI_SCROLL_OF_TOWN_PORTAL,
	IDI_SCROLL_OF_FLASH,
	IDI_SCROLL_OF_INFRAVISION,
	IDI_SCROLL_OF_PHASING,
	IDI_SCROLL_OF_MANA_SHIELD,
	IDI_SCROLL_OF_FLAMEWAVE,
	IDI_SCROLL_OF_FIREBALL,
	IDI_SCROLL_OF_STONECURSE,
	IDI_SCROLL_OF_CHAIN_LIGHTNING,
	IDI_SCROLL_OF_GUARDIAN,
	IDI_UNUSED_SCROLL,
	IDI_SCROLL_OF_NOVA,
	IDI_SCROLL_OF_GOLEM,
	IDI_SCROLL_OF_BLOODBOIL,	// unused
	IDI_SCROLL_OF_TELEPORT,
	IDI_SCROLL_OF_APOCALYPSE,
	// unused IDI_SCROLL_OF_BONESPIRIT,
	// unused IDI_SCROLL_OF_BLOODSTAR,
	IDI_FIRST_BOOK,
	IDI_SECOND_BOOK,
	IDI_THIRD_BOOK,
	IDI_LAST_BOOK,
	IDI_DAGGER,
	IDI_SHORT_SWORD,
	IDI_FALCHION,
	IDI_SCIMITAR,
	IDI_CLAYMORE,
	IDI_BLADE,
	IDI_SABRE,
	IDI_LONG_SWORD,
	IDI_BROAD_SWORD,
	IDI_BASTARD_SWORD,
	IDI_TWO_HANDED_SWORD,
	IDI_GREAT_SWORD,
	IDI_SMALL_AXE,
	IDI_AXE,
	IDI_LARGE_AXE,
	IDI_BROAD_AXE,
	IDI_BATTLE_AXE,
	IDI_GREAT_AXE,
	IDI_MACE,
	IDI_MORNINGSTAR,
	IDI_WAR_HAMMER,
	IDI_SPIKED_CLUB,
	IDI_CLUB,
	IDI_FLAIL,
	IDI_MAUL,
	IDI_SHORT_BOW,
	IDI_HUNTERS_BOW,
	IDI_LONG_BOW,
	IDI_COMPOSITE_BOW,
	IDI_SHORT_WAR_BOW,
	IDI_LONG_WAR_BOW,
	IDI_SHORT_STAFF,
	IDI_LONG_STAFF,
	IDI_COMPOSITE_STAFF,
	IDI_QUARTER_STAFF,
	IDI_WAR_STAFF,
	IDI_FIRST_RING,
	IDI_SECOND_RING,
	IDI_LAST_RING,
	IDI_FIRST_AMULET,
	IDI_LAST_AMULET,
	IDI_RUNE_OF_FIRE,
	IDI_RUNE_OF_LIGHTNING,
	IDI_GREATER_RUNE_OF_FIRE,
	IDI_GREATER_RUNE_OF_LIGHTNING,
	IDI_RUNE_OF_STONE,
	
	// Insert any new items above this line.
	IDI_LAST_RANDOM_ITEM
	
};

#define IDI_BARBARIAN	IDI_SPIKED_CLUB
#define IDI_BARSHLD		IDI_WARRSHLD

#define IAF_INFRAVISION		0x00000001
#define IAF_SKING			0x00000002
#define IAF_RNDARROW		0x00000004
#define IAF_FIREARROW		0x00000008
#define IAF_FIREHIT			0x00000010
#define IAF_LIGHTHIT		0x00000020
#define IAF_CONSTRICT		0x00000040
#define IAF_NOMANA			0x00000080
#define IAF_NOHEAL			0x00000100
#define IAF_RABID			0x00000200  // not in game
#define IAF_HALFTRAP		0x00000400  // not in game -called TRAPDAM
#define IAF_KNOCKBACK		0x00000800
#define IAF_MNOHEAL			0x00001000
#define IAF_BAT10			0x00002000
#define IAF_BAT20			0x00004000
#define IAF_ALLBAT			(IAF_BAT10 | IAF_BAT20)
#define IAF_LEECH10			0x00008000
#define IAF_LEECH20			0x00010000
#define IAF_ALLLEECH		(IAF_LEECH10 | IAF_LEECH20)
#define IAF_ATANIM1			0x00020000
#define IAF_ATANIM2			0x00040000
#define IAF_ATANIM3			0x00080000
#define IAF_ATANIM4			0x00100000
#define IAF_ALLATANIM		(IAF_ATANIM1 | IAF_ATANIM2 | IAF_ATANIM3 | IAF_ATANIM4 )
#define IAF_HTANIM1			0x00200000
#define IAF_HTANIM2			0x00400000
#define IAF_HTANIM3			0x00800000
#define IAF_ALLHTANIM		(IAF_HTANIM1 | IAF_HTANIM2 | IAF_HTANIM3)
#define IAF_BLANIM			0x01000000
#define IAF_LARROW			0x02000000
#define IAF_THORN			0x04000000			
#define IAF_LMANA           0x08000000
#define IAF_TRAPDAM			0x10000000
#define IAF_OMEHAND			0x20000000
#define IAF_DAMDEMON		0x40000000
#define IAF_ZERORES	 		0x80000000

#define IAF2_DEVASTATION	0x00000001
#define IAF2_DECAY			0x00000002
#define IAF2_PERIL			0x00000004
#define IAF2_JESTER			0x00000008
#define IAF2_CLONE			0x00000010
#define IAF2_DEMONAC		0x00000020
#define IAF2_UNDEADAC		0x00000040

#define ISEL_NONE		0		// Items start out unselectable
#define ISEL_FLR		1		// Most items
#define ISEL_TOP		2		// Items on objects usually
#define ISEL_ALL		3		// Large (2 square) items

#define IMAGIC_NONE		0
#define IMAGIC_MAGIC	1
#define IMAGIC_UNIQUE	2

// Item re-creation information
// Creation bits are as follows:
// bit# desc
// 1-6	Level
// 7	Item Goodonly (T/F)
// 8,9	Unique percentage (0 = 0%, 1 = 15%, 2 = 1%)
// 8&9	Useful item only
// 10	Unique item
// 11	Spawned by Blacksmith
// 12	Spawned by Blacksmith premium
// 13	Spawned by Pegboy
// 14	Spawned by Witch
// 15	Spawned by Healer
#define ICI_USEFUL		0x0180
#define ICI_UPER1		0x0100
#define ICI_UPER15		0x0080
#define ICI_ONLYGOOD	0x0040
#define ICI_UNIQUE		0x0200
#define ICI_SMITH		0x0400
#define ICI_PREMIUM		0x0800
#define ICI_BOY			0x1000
#define ICI_WITCH		0x2000
#define ICI_HEALER		0x4000
#define ICI_PREGEN		0x8000

#define ICI_LVLMASK		0x003f
#define ICI_TOWNMASK	0x7c00
#define ICI_PREGENMASK	0x7fff

// Unique item index list
#define UID_CLEAVER		0		// Butcher's cleaver
#define UID_SKCROWN		1		// Skeleton King's crown
#define UID_INFRARING	2		// Infravision ring
#define UID_OPTAMULET	3		// Optic Amulet
#define UID_TRING		4		// Ring of truth
#define UID_HARCREST	5		// Harlequin Crest
#define UID_STEELVEIL	6		// Veil of Steel
#define UID_ARMOFVAL	7		// Armor of Valor
#define UID_GRISWOLD	8		// Griswold's Edge
#define UID_ARMRCOW		9		// Cow Armor
#define UID_LGTFORGE	9

// item no random spawn, normal random spawn, or double chance random spawn
#define IRND_NO			0
#define IRND_NORMAL		1
#define IRND_DOUBLE		2

#define RESIST_MAX		75

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/


typedef struct {
	char *PLName;		// Name of power
	int PLPower;		// Power type
	int PLParam1;		// Misc param 1
	int PLParam2;		// Misc param 2
	char PLMinLvl;		// Min dungeon level of power appearing
	long PLIType;		// Item type (armor/shield/weapon/staff/bow/ring)
	byte PLGOE;			// Good/Evil/Either
	BOOL PLDouble;		// Double chance of spawning (more common magic)
	BOOL PLOk;			// Item good or bad
	int PLMinVal;		// Item min value modifier
	int PLMaxVal;		// Item max value modifier
	int PLMultVal;		// Item value multiplier
} PLStruct;

typedef struct {
	char *UIName;		// Unique item name
	char UIItemId;		// Item id for base stats
	char UIMinLvl;		// Min level can be found at
	char UINumPL;		// Number of power list items
	int UIValue;		// Items value
	char UIPower1;		// Power 1 and 2 params
	int UIParam1;
	int UIParam2;
	char UIPower2;		// Power 2 and 2 params
	int UIParam3;
	int UIParam4;
	char UIPower3;		// Power 3 and 2 params
	int UIParam5;
	int UIParam6;
	char UIPower4;		// Power 4 and 2 params
	int UIParam7;
	int UIParam8;
	char UIPower5;		// Power 5 and 2 params
	int UIParam9;
	int UIParam10;
	char UIPower6;		// Power 6 and 2 params
	int UIParam11;
	int UIParam12;
} UItemStruct;

typedef struct {
	BOOL iRnd;			// Random item or special
	char iClass;		// Item classification
	char iLoc;			// Item Body Location
	int iCurs;			// Item cursor gfx
	char itype;			// Item type
	char iItemId;		// Item id#
	char *iName;		// name
	char *iSName;		// short name
	char iMinMLvl;		// Min monster level to drop it
	int iDurability;	// Durability of the item
	int	iMinDam;		// Min damage
	int iMaxDam;		// Max damage
	int iMinAC;			// Min Armor Class
	int iMaxAC;			// Max Armor Class
	char iMinStr;		// Min Strength stat to use item
	char iMinMag;		// Min Magic stat to use item
	char iMinDex;		// Min Dexterity stat to use item
	long iFlags;		// Item ability flags
	int iMiscId;		// Misc item uses id
	long iSpell;		// item spell
	BOOL iUsable;		// Usable item?
	int iValue;			// item min value
	int iMaxValue;		// item max value
} ItemDataStruct;

// PATCH1.JMM
typedef struct {
	int nSeed;
	WORD wCI;
	int nIndex;
	DWORD dwTimestamp;
} ItemGetRecordStruct;
// ENDPATCH1.JMM

typedef struct {
	int _iSeed;			// item seed to generate itself
	WORD _iCreateInfo;	// item re-creation info
	int _itype;			// item type
	int _ix;			// item map x
	int _iy;			// item map y
	BOOL _iAnimFlag;	// Does this item animate?
	BYTE *_iAnimData;	// Data pointer to anim tables
	int	_iAnimLen;		// number of anim frames
	int	_iAnimFrame;	// current anim frame
	long _iAnimWidth;	// Width of anim
	long _iAnimWidth2;	// (Width - 64) >> 1 of anim
// PATCH1.JMM
	// FLAG IS NO LONGER USED
	//BOOL _iDelFlag;		// Delete this item
	BOOL _iInvalid;
// ENDPATCH1.JMM
	
	char _iSelFlag;		// Select top, floor, or all
	BOOL _iPostDraw;	// Draw after objects or before?

	BOOL _iIdentified;		// Has item been identified?
	char _iMagical;			// (No/Reg/Unique) Does the item have magical attributes?
	char _iName[64];		// item name
	char _iIName[64];		// identified name
	char _iLoc;				// item body location
	char _iClass;			// item classification
	int _iCurs;				// item cursor type
	int _ivalue;			// item value
	int _iIvalue;			// item identified value
	int _iMinDam;			// item min damage
	int _iMaxDam;			// item max damage
	int _iAC;				// item armor class
	long _iFlags;			// item ability flags
	int _iMiscId;			// Misc item uses id
	int _iSpell;			// item spell

	int _iCharges;			// random number of charges
	int _iMaxCharges;		// Max Charges of a staff

	int _iDurability;		// How much strength until it breaks
	int _iMaxDur;			// Max Durability

	int _iPLDam;			// Power List damage multiplier
	int _iPLToHit;			// Power List to hit increase
	int _iPLAC;				// Power List AC increase
	int _iPLStr;			// Power List Strength increase
	int _iPLMag;			// Power List Magic increase
	int _iPLDex;			// Power List Dexterity increase
	int _iPLVit;			// Power List Vitality increase
	int _iPLFR;				// Power List Fire resistance
	int _iPLLR;				// Power List Lightning resistance
	int _iPLMR;				// Power List Misc Magic resistance
	long _iPLMana;			// Power List Mana
	long _iPLHP;			// Power List Hit Points
	int _iPLDamMod;			// Power List damage modifier (num, not %)
	int _iPLGetHit;			// Power List Get hit modifier (+/-)
	int _iPLLight;			// Power List light radius
	char _iSplLvlAdd;		// What to add to each spell level

	char _iRequest;			// If item has be requested to be picked up (drb 12/9)
	int _iUid;				// If unique item, index into unique table (drb 12/8)
	
	int  _iFMinDam;			// Fire hit min damage
	int  _iFMaxDam;			// Fire hit max damage
	int  _iLMinDam;			// Lightning hit min damage
	int  _iLMaxDam;			// Lightning hit max damage
	
	int  _iPLEnAc;			// Enemy armor class reduced by this amount

	char _iPrePower;		// Power List index for prefix
	char _iSufPower;		// Power List index for suffix

	int _iVAdd1;			// value add #1
	int _iVMult1;			// value multiplier #1
	int _iVAdd2;			// value add #2
	int _iVMult2;			// value multiplier #2

	char _iMinStr;			// Min Strength stat to use item
	byte _iMinMag;			// Min Magic stat to use item
	char _iMinDex;			// Min Dexterity stat to use item
	BOOL _iStatFlag;		// Draw with red filter or not
	int IDidx;				// AllItemsData index
	char _oldlight;			// Old prelight val
	long _iFlags2;			// item ability flags
} ItemStruct;

#define SAVE_ITEM_SIZE sizeof(ItemStruct)



/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern ItemStruct	item[MAXITEMS+1];
extern long numitems;

extern int itemactive[MAXITEMS];
extern int itemavail[MAXITEMS];

// PATCH1.JMM
extern ItemGetRecordStruct itemgets[MAXITEMS];
extern int gnNumGetRecords;
// ENDPATCH1.JMM

extern BOOL UniqueItemFlag[MAXUITEMS];
extern BOOL uitemflag;

extern int ItemInvSnds[];
extern BYTE ItemCAnimTbl[];
#if CHEATS
extern BOOL davecheat;
extern int tstQMsgSpd;
extern int tstQMsgIndex;
extern BOOL tstQMsgFlag;
extern BOOL tstQMsgIndexFlag;
#endif

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitItems();
void ProcessItems();
void InitItemGFX();
void FreeItemGFX();
void DaveGold();
void DaveNewPremium();
void DaveCleanUp();
void DaveSpells();
void DaveSpells2();
void DaveQuestText();

BOOL ItemSpaceOk(int, int);

void SpawnItem(int, int, int, BOOL);				// Called by monsters
void SpawnUnique(int, int, int);					// Called by monsters
void RespawnItem(int ii, BOOL FlipFlag);			// Called by plr placing object back

void CreateItem(int, int, int);						// Spawn a specific item at x, y
void CreateRndItem(int, int, BOOL, BOOL, BOOL);		// Spawn any item around x,y (item level >= (currlevel*2))
void CreateRndUseful(int, int, int, BOOL);			// Spawn either health, mana, or identify
void CreateTypeItem(int, int, BOOL, int, int, BOOL, BOOL);		// Spawn a specific type of item
void CreateSpellBook(int x, int y, int ispell, BOOL sendmsg, BOOL delta); //Spawn a specific spell book
void CreateMagicArmor(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta); //Spawn specific magical armor
void CreateAmulet(int x, int y, int level, BOOL sendmsg, BOOL delta); //Spawn an amulet
void CreateMagicWeapon(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta); //Spawn specific magical weapon

void RecreateItem(int, int, WORD, int, int);
void RecreateEar(int, WORD, int, BOOL, int, int, int, int, int, int);

void SyncItemAnim(int);

void GetItemStr(int);

void CalcPlrItemVals(int,BOOL);
void CalcPlrScrolls(int);
void CalcPlrStaff(int);
void CalcPlrItemMin(int);
void CalcPlrInv(int,BOOL);

void CreatePlrItems(int);

void SpawnRock();

void CheckIdentify(int, int);
void DoRepair(int, int);
void DoRecharge(int, int);
void DoOil(int, int);

void PrintItemPower(char,const ItemStruct * x);
void PrintItemDetails(const ItemStruct * x);
void PrintItemDur(const ItemStruct * x);

void UseItem(int, int, int);

void SpawnSmith(int);
void SpawnPremium(int);
void SpawnWitch(int);
void SpawnBoy(int);
void SpawnHealer(int);
void SpawnStoreGold();
void SpawnQuestItem(int itemid, int x,int y, int randarea, int selflag);

void DrawUniqueInfo();

void GetItemAttrs(int i, int idata, int lvl);

int ItemNoFlippy();
void GetSuperItemLoc(int x, int y, int &xx, int &yy);

// PATCH1.JMM
BOOL CheckGetRecord( int nSeed, WORD wCI, int nIndex );
void AddGetRecord( int nSeed, WORD wCI, int nIndex );
void RemoveGetRecord( int nSeed, WORD wCI, int nIndex );
// ENDPATCH1.JMM

void SetPlrHandItem(ItemStruct *h, int idata);
void GetPlrHandSeed(ItemStruct *h);

typedef struct {
	int x;
	int y;
	BOOL Initted;
	ItemStruct item;
} CornerStoneType;

extern CornerStoneType CornerStone;

extern void CornerstoneRestore(int x, int y);
extern void CornerstoneSave();
