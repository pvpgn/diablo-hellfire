/*-----------------------------------------------------------------------**
** 								Diablo
**
**						  Monsters Data file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MONSTDAT.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"sound.h"
#include	"cursor.h"
#include	"monster.h"
#include	"monstdat.h"
#include	"minitext.h"
#include	"textdat.h"
#include	"items.h"


#if RLE_DRAW
#define CEL_EXT ".CL2"
#else
#define CEL_EXT ".CEL"
#endif


/*-----------------------------------------------------------------------**
** Image width, Image size in K bytes
** Image files
** Special animation
** Sound files
** Special sound
** Trans file?, Transformation file name
** Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
** Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
** Name
** Min, Max dungeon level
** Monster level ranking (1 = easy, 30 = hardest)
** Min, Max hitpoints
** Intelligence (0 = easiest, 3 = hardest of type)
** Hit% #1
** Hit check frame #1
** Min, Max damage #1
** Hit% #2
** Hit check frame #2
** Min, Max damage #2
** Armor Class
** Magic resist
** Monster Class
** Selection Flags
** Experience points
**-----------------------------------------------------------------------*/

MonsterData monsterdata[] =
{
// MT_NZOMBIE	
	{ 128, 799,									// Image width, Image size
	"Monsters\\Zombie\\Zombie%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\Zombie\\Zombie%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 11, 24, 12, 6, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  4,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Zombie",		// Name
	1, 3,		// Min, Max dungeon level
	1,			// Monster level ranking (1 = easy, 30 = hardest)
	4, 7,		// Min, Max hitpoints
	AI_ZOMBIE,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	10,			// Hit% #1
	8,			// Hit check frame #1
	2, 5,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	5,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	54},		// Experience points

// MT_BZOMBIE	
	{ 128, 799,									// Image width, Image size
	"Monsters\\Zombie\\Zombie%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Zombie\\Zombie%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Zombie\\Bluered.TRN",	// Trans file?, Transformation file name
	{ 11, 24, 12, 6, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  4,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Ghoul",		// Name
	2, 4,		// Min, Max dungeon level
	2,			// Monster level ranking (1 = easy, 30 = hardest)
	7, 11,		// Min, Max hitpoints
	AI_ZOMBIE,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	10,			// Hit% #1
	8,			// Hit check frame #1
	3, 10,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	10,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	58 },		// Experience points

// MT_GZOMBIE	
	{ 128, 799,									// Image width, Image size
	"Monsters\\Zombie\\Zombie%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\Zombie\\Zombie%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Zombie\\Grey.TRN",	// Trans file?, Transformation file name
	{ 11, 24, 12, 6, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  4,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Rotting Carcass",		// Name
	2, 6,		// Min, Max dungeon level
	4,			// Monster level ranking (1 = easy, 30 = hardest)
	15, 25,		// Min, Max hitpoints
	AI_ZOMBIE,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	25,			// Hit% #1
	8,			// Hit check frame #1
	5, 15,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	15,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II+M_RF,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	136 },		// Experience points

// MT_YZOMBIE	
	{ 128, 799,									// Image width, Image size
	"Monsters\\Zombie\\Zombie%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\Zombie\\Zombie%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Zombie\\Yellow.TRN",	// Trans file?, Transformation file name
	{ 11, 24, 12, 6, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  4,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Black Death",		// Name
	4, 8,		// Min, Max dungeon level
	6,			// Monster level ranking (1 = easy, 30 = hardest)
	25, 40,		// Min, Max hitpoints
	AI_ZOMBIE,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	30,			// Hit% #1
	8,			// Hit check frame #1
	6, 22,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	20,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II+M_RL,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	240 },		// Experience points

// MT_RFALLSP	
	{ 128, 543,									// Image width, Image size
	"Monsters\\FalSpear\\Phall%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\FalSpear\\Phall%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\FalSpear\\FallenT.TRN",	// Trans file?, Transformation file name
	{ 11, 11, 13, 11, 18, 13 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Fallen One",		// Name
	1, 3,		// Min, Max dungeon level
	1,			// Monster level ranking (1 = easy, 30 = hardest)
	1, 4,		// Min, Max hitpoints
	AI_FALLEN,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	15,			// Hit% #1
	7,			// Hit check frame #1
	1, 3,		// Min, Max damage #1
	0,			// Hit% #2
	5,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_NONE,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	46 },		// Experience points

// MT_DFALLSP	
	{ 128, 543,									// Image width, Image size
	"Monsters\\FalSpear\\Phall%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\FalSpear\\Phall%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\FalSpear\\Dark.TRN",	// Trans file?, Transformation file name
	{ 11, 11, 13, 11, 18, 13 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Carver",		// Name
	2, 5,		// Min, Max dungeon level
	3,			// Monster level ranking (1 = easy, 30 = hardest)
	4, 8,		// Min, Max hitpoints
	AI_FALLEN,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	20,			// Hit% #1
	7,			// Hit check frame #1
	2, 5,		// Min, Max damage #1
	0,			// Hit% #2
	5,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	5,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_NONE,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	80 },		// Experience points

// MT_YFALLSP	
	{ 128, 543,									// Image width, Image size
	"Monsters\\FalSpear\\Phall%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\FalSpear\\Phall%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 11, 11, 13, 11, 18, 13 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Devil Kin",		// Name
	3, 7,		// Min, Max dungeon level
	5,			// Monster level ranking (1 = easy, 30 = hardest)
	12, 24,		// Min, Max hitpoints
	AI_FALLEN,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	25,			// Hit% #1
	7,			// Hit check frame #1
	3, 7,		// Min, Max damage #1
	0,			// Hit% #2
	5,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	10,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	155 },		// Experience points

// MT_BFALLSP	
	{ 128, 543,									// Image width, Image size
	"Monsters\\FalSpear\\Phall%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\FalSpear\\Phall%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\FalSpear\\Blue.TRN",	// Trans file?, Transformation file name
	{ 11, 11, 13, 11, 18, 13 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Dark One",		// Name
	5, 9,		// Min, Max dungeon level
	7,			// Monster level ranking (1 = easy, 30 = hardest)
	20, 36,		// Min, Max hitpoints
	AI_FALLEN,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	30,			// Hit% #1
	7,			// Hit check frame #1
	4, 8,		// Min, Max damage #1
	0,			// Hit% #2
	5,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	15,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_II,		// Magic resist
	M_II+M_RL,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	255 },		// Experience points

// MT_WSKELAX	
	{ 128, 553,									// Image width, Image size
	"Monsters\\SkelAxe\\SklAx%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelAxe\\SklAx%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\SkelAxe\\White.TRN",		// Trans file?, Transformation file name
	{ 12, 8, 13, 6, 17, 16 },	// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 5,  0,  0, 0,  0, 0 },	// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Skeleton",		// Name
	1, 3,		// Min, Max dungeon level
	1,			// Monster level ranking (1 = easy, 30 = hardest)
	2, 4,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	0,			// Intelligence
	20,			// Hit% #1
	8,			// Hit check frame #1
	1, 4,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	64 },		// Experience points

// MT_TSKELAX	
	{ 128, 553,									// Image width, Image size
	"Monsters\\SkelAxe\\SklAx%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelAxe\\SklAx%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\SkelAxe\\Skelt.TRN",		// Trans file?, Transformation file name
	{ 12, 8, 13, 6, 17, 16 },	// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4,  0,  0, 0,  0, 0 },	// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Corpse Axe",		// Name
	2, 5,		// Min, Max dungeon level
	2,			// Monster level ranking (1 = easy, 30 = hardest)
	4, 7,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	25,			// Hit% #1
	8,			// Hit check frame #1
	3, 5,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	68 },		// Experience points

// MT_RSKELAX	
	{ 128, 553,									// Image width, Image size
	"Monsters\\SkelAxe\\SklAx%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelAxe\\SklAx%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12, 8, 13, 6, 17, 16 },	// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2,  0,  0, 0,  0, 0 },	// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Burning Dead",		// Name
	2, 6,		// Min, Max dungeon level
	4,			// Monster level ranking (1 = easy, 30 = hardest)
	8, 12,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	30,			// Hit% #1
	8,			// Hit check frame #1
	3, 7,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	5,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RF+M_IM+M_II,	// Magic resist
	M_IF+M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	154 },		// Experience points

// MT_XSKELAX	
	{ 128, 553,									// Image width, Image size
	"Monsters\\SkelAxe\\SklAx%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelAxe\\SklAx%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\SkelAxe\\Black.TRN",		// Trans file?, Transformation file name
	{ 12, 8, 13, 6, 17, 16 },	// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3,  0,  0, 0,  0, 0 },	// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Horror",	// Name
	4, 8,		// Min, Max dungeon level
	6,			// Monster level ranking (1 = easy, 30 = hardest)
	12, 20,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	35,			// Hit% #1
	8,			// Hit check frame #1
	4, 9,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	15,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RL+M_IM+M_II,	// Magic resist
	M_RL+M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	264 },		// Experience points

// MT_RFALLSD	
	{ 128, 623,									// Image width, Image size
	"Monsters\\FalSword\\Fall%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\FalSword\\Fall%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\FalSword\\FallenT.TRN",	// Trans file?, Transformation file name
	{ 12, 12, 13, 11, 14, 15 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Fallen One",		// Name
	1, 3,		// Min, Max dungeon level
	1,			// Monster level ranking (1 = easy, 30 = hardest)
	2, 5,		// Min, Max hitpoints
	AI_FALLEN,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	15,			// Hit% #1
	8,			// Hit check frame #1
	1, 4,		// Min, Max damage #1
	0,			// Hit% #2
	5,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	10, 		// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_NONE,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	52 },		// Experience points

// MT_DFALLSD	
	{ 128, 623,									// Image width, Image size
	"Monsters\\FalSword\\Fall%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\FalSword\\Fall%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\FalSword\\Dark.TRN",	// Trans file?, Transformation file name
	{ 12, 12, 13, 11, 14, 15 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Carver",		// Name
	2, 5,		// Min, Max dungeon level
	3,			// Monster level ranking (1 = easy, 30 = hardest)
	5, 9,		// Min, Max hitpoints
	AI_FALLEN,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	20,			// Hit% #1
	8,			// Hit check frame #1
	2, 7,		// Min, Max damage #1
	0,			// Hit% #2
	5,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	15,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_NONE,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	90 },		// Experience points

// MT_YFALLSD	
	{ 128, 623,									// Image width, Image size
	"Monsters\\FalSword\\Fall%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\FalSword\\Fall%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12, 12, 13, 11, 14, 15 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Devil Kin",		// Name
	3, 7,		// Min, Max dungeon level
	5,			// Monster level ranking (1 = easy, 30 = hardest)
	16, 24,		// Min, Max hitpoints
	AI_FALLEN,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	25,			// Hit% #1
	8,			// Hit check frame #1
	4, 10,		// Min, Max damage #1
	0,			// Hit% #2
	5,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	20,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	180 },		// Experience points

// MT_BFALLSD	
	{ 128, 623,									// Image width, Image size
	"Monsters\\FalSword\\Fall%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\FalSword\\Fall%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\FalSword\\Blue.TRN",	// Trans file?, Transformation file name
	{ 12, 12, 13, 11, 14, 15 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Dark One",		// Name
	5, 9,		// Min, Max dungeon level
	7,			// Monster level ranking (1 = easy, 30 = hardest)
	24, 36,		// Min, Max hitpoints
	AI_FALLEN,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	30,			// Hit% #1
	8,			// Hit check frame #1
	4, 12,		// Min, Max damage #1
	0,			// Hit% #2
	5,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	25,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_II,		// Magic resist
	M_II+M_RL,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	280 },		// Experience points

// MT_NSCAV	
	{ 128, 410,									// Image width, Image size
	"Monsters\\Scav\\Scav%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Scav\\Scav%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12,  8, 12, 6, 20, 11 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Scavenger",		// Name
	1, 4,		// Min, Max dungeon level
	2,			// Monster level ranking (1 = easy, 30 = hardest)
	3, 6,		// Min, Max hitpoints
	AI_SCAV,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	20,			// Hit% #1
	7,			// Hit check frame #1
	1, 5,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	10,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	80 },		// Experience points

// MT_BSCAV	
	{ 128, 410,									// Image width, Image size
	"Monsters\\Scav\\Scav%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Scav\\Scav%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Scav\\ScavBr.TRN",	// Trans file?, Transformation file name
	{ 12,  8, 12, 6, 20, 11 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Plague Eater",		// Name
	3, 6,		// Min, Max dungeon level
	4,			// Monster level ranking (1 = easy, 30 = hardest)
	12, 24,		// Min, Max hitpoints
	AI_SCAV,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	30,			// Hit% #1
	7,			// Hit check frame #1
	1, 8,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	20,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RL,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	188 },		// Experience points

// MT_WSCAV	
	{ 128, 410,									// Image width, Image size
	"Monsters\\Scav\\Scav%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Scav\\Scav%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Scav\\ScavBe.TRN",	// Trans file?, Transformation file name
	{ 12,  8, 12, 6, 20, 11 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Shadow Beast",		// Name
	4, 8,		// Min, Max dungeon level
	6,			// Monster level ranking (1 = easy, 30 = hardest)
	24, 36,		// Min, Max hitpoints
	AI_SCAV,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	35,			// Hit% #1
	7,			// Hit check frame #1
	3, 12,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	25,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_II,		// Magic resist
	M_II+M_RF,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	375 },		// Experience points

// MT_YSCAV	
	{ 128, 410,									// Image width, Image size
	"Monsters\\Scav\\Scav%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Scav\\Scav%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Scav\\ScavW.TRN",	// Trans file?, Transformation file name
	{ 12,  8, 12, 6, 20, 11 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Bone Gasher",		// Name
	6, 10,		// Min, Max dungeon level
	8,			// Monster level ranking (1 = easy, 30 = hardest)
	28, 40,		// Min, Max hitpoints
	AI_SCAV,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	35,			// Hit% #1
	7,			// Hit check frame #1
	5, 15,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RM+M_II,	// Magic resist
	M_II+M_RL,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	552 },		// Experience points

// MT_WSKELBW	
	{ 128, 567,									// Image width, Image size
	"Monsters\\SkelBow\\SklBw%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelBow\\SklBw%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\SkelBow\\White.TRN",		// Trans file?, Transformation file name
	{ 9, 8, 16, 5, 16, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4, 0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Skeleton",		// Name
	2, 5,		// Min, Max dungeon level
	3,			// Monster level ranking (1 = easy, 30 = hardest)
	2, 4,		// Min, Max hitpoints
	AI_SKELBOW,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	15,			// Hit% #1
	12,			// Hit check frame #1
	1, 2,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	110 },		// Experience points

// MT_TSKELBW	
	{ 128, 567,									// Image width, Image size
	"Monsters\\SkelBow\\SklBw%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelBow\\SklBw%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\SkelBow\\Skelt.TRN",		// Trans file?, Transformation file name
	{ 9, 8, 16, 5, 16, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4, 0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Corpse Bow",		// Name
	3, 7,		// Min, Max dungeon level
	5,			// Monster level ranking (1 = easy, 30 = hardest)
	8, 16,		// Min, Max hitpoints
	AI_SKELBOW,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	25,			// Hit% #1
	12,			// Hit check frame #1
	1, 4,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	210 },		// Experience points

// MT_RSKELBW	
	{ 128, 567,									// Image width, Image size
	"Monsters\\SkelBow\\SklBw%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelBow\\SklBw%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 9, 8, 16, 5, 16, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2, 0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Burning Dead",		// Name
	5, 9,		// Min, Max dungeon level
	7,			// Monster level ranking (1 = easy, 30 = hardest)
	10, 24,		// Min, Max hitpoints
	AI_SKELBOW,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	30,			// Hit% #1
	12,			// Hit check frame #1
	1, 6,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	5,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RF+M_IM+M_II,	// Magic resist
	M_IF+M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	364 },		// Experience points

// MT_XSKELBW	
	{ 128, 567,									// Image width, Image size
	"Monsters\\SkelBow\\SklBw%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelBow\\SklBw%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\SkelBow\\Black.TRN",		// Trans file?, Transformation file name
	{ 9, 8, 16, 5, 16, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Horror",	// Name
	7, 11,		// Min, Max dungeon level
	9,			// Monster level ranking (1 = easy, 30 = hardest)
	15, 45,		// Min, Max hitpoints
	AI_SKELBOW,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	35,			// Hit% #1
	12,			// Hit check frame #1
	2, 9,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	15,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RL+M_IM+M_II,	// Magic resist
	M_RL+M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	594 },		// Experience points

// MT_WSKELSD	
	{ 128, 575,									// Image width, Image size
	"Monsters\\SkelSd\\SklSr%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelSd\\SklSr%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\SkelSd\\White.TRN",		// Trans file?, Transformation file name
	{ 13, 8, 12, 7, 15, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Skeleton Captain",		// Name
	1, 4,		// Min, Max dungeon level
	2,			// Monster level ranking (1 = easy, 30 = hardest)
	3, 6,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	20,			// Hit% #1
	8,			// Hit check frame #1
	2, 7,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	10,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	90 },		// Experience points

// MT_TSKELSD	
	{ 128, 575,									// Image width, Image size
	"Monsters\\SkelSd\\SklSr%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelSd\\SklSr%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\SkelSd\\Skelt.TRN",		// Trans file?, Transformation file name
	{ 13, 8, 12, 7, 15, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Corpse Captain",		// Name
	2, 6,		// Min, Max dungeon level
	4,			// Monster level ranking (1 = easy, 30 = hardest)
	12, 20,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	30,			// Hit% #1
	8,			// Hit check frame #1
	3, 9,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	5,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	200 },		// Experience points

// MT_RSKELSD	
	{ 128, 575,									// Image width, Image size
	"Monsters\\SkelSd\\SklSr%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelSd\\SklSr%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 13, 8, 12, 7, 15, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Burning Dead Captain",		// Name
	4, 8,		// Min, Max dungeon level
	6,			// Monster level ranking (1 = easy, 30 = hardest)
	16, 30,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	35,			// Hit% #1
	8,			// Hit check frame #1
	4, 10,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	15,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RF+M_IM+M_II,	// Magic resist
	M_IF+M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	393 },		// Experience points

// MT_XSKELSD	
	{ 128, 575,									// Image width, Image size
	"Monsters\\SkelSd\\SklSr%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\SkelSd\\SklSr%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\SkelSd\\Black.TRN",		// Trans file?, Transformation file name
	{ 13, 8, 12, 7, 15, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Horror Captain",		// Name
	6, 10,		// Min, Max dungeon level
	8,			// Monster level ranking (1 = easy, 30 = hardest)
	35, 50,		// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	40,			// Hit% #1
	8,			// Hit check frame #1
	5, 14,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RL+M_IM+M_II,	// Magic resist
	M_RL+M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	604 },		// Experience points

// MT_INVILORD	
	{ 128, 800/*2000*/,							// Image width, Image size
	"Monsters\\TSneak\\TSneak%c" CEL_EXT,		// Image files
	FALSE,									// Special animation
	"Monsters\\TSneak\\Sneakl%c%i.WAV",			// Sound files
	FALSE,									// Special sound
	FALSE, NULL,							// Trans file?, Transformation file name
	{ 13, 13, 15, 11, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Invisible Lord",		// Name
	36, 39,		// Min, Max dungeon level
	14,			// Monster level ranking (1 = easy, 30 = hardest)
	278, 278,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	65,			// Hit% #1
	8,			// Hit check frame #1
	16, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_RF+M_RL+M_II,		// Magic resist
	M_RM+M_RF+M_RL+M_II,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	2000 },		// Experience points

// MT_SNEAK	
	{ 128, 992,									// Image width, Image size
	"Monsters\\Sneak\\Sneak%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Sneak\\Sneak%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 16, 8, 12, 8, 24, 15 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hidden",		// Name
	3, 8,		// Min, Max dungeon level
	5,			// Monster level ranking (1 = easy, 30 = hardest)
	8, 24,		// Min, Max hitpoints
	AI_SNEAK,
	MFLAG_INVISIBLE,		// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	35,			// Hit% #1
	8,			// Hit check frame #1
	3, 6,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	25,			// Armor Class
	MC_DEMON,	// Monster Class
	M_NONE,		// Magic resist
	M_II,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	278 },		// Experience points

// MT_STALKER	
	{ 128, 992,									// Image width, Image size
	"Monsters\\Sneak\\Sneak%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Sneak\\Sneak%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Sneak\\Sneakv2.TRN",		// Trans file?, Transformation file name
	{ 16, 8, 12, 8, 24, 15 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Stalker",		// Name
	8, 12,		// Min, Max dungeon level
	9,			// Monster level ranking (1 = easy, 30 = hardest)
	30, 45,		// Min, Max hitpoints
	AI_SNEAK,
	MFLAG_INVISIBLE|MFLAG_PATH,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	40,			// Hit% #1
	8,			// Hit check frame #1
	8, 16,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_DEMON,	// Monster Class
	M_NONE,		// Magic resist
	M_II,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	630 },		// Experience points

// MT_UNSEEN	
	{ 128, 992,									// Image width, Image size
	"Monsters\\Sneak\\Sneak%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Sneak\\Sneak%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Sneak\\Sneakv3.TRN",		// Trans file?, Transformation file name
	{ 16, 8, 12, 8, 24, 15 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Unseen",		// Name
	10, 14,		// Min, Max dungeon level
	11,			// Monster level ranking (1 = easy, 30 = hardest)
	35, 50,		// Min, Max hitpoints
	AI_SNEAK,
	MFLAG_INVISIBLE|MFLAG_PATH,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	45,			// Hit% #1
	8,			// Hit check frame #1
	12, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	935 },		// Experience points

// MT_ILLWEAV	
	{ 128, 992,									// Image width, Image size
	"Monsters\\Sneak\\Sneak%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Sneak\\Sneak%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Sneak\\Sneakv1.TRN",		// Trans file?, Transformation file name
	{ 16, 8, 12, 8, 24, 15 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Illusion Weaver",		// Name
	14, 18,		// Min, Max dungeon level
	13,			// Monster level ranking (1 = easy, 30 = hardest)
	40, 60,		// Min, Max hitpoints
	AI_SNEAK,
	MFLAG_INVISIBLE|MFLAG_PATH,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	8,			// Hit check frame #1
	16, 24,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_RF,	// Magic resist
	M_IM+M_RF+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1500 },		// Experience points

// MT_LRDSAYTR	
	{ 160, 800 /*2000*/,							// Image width, Image size
	"Monsters\\GoatLord\\GoatL%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
//	"Monsters\\GoatLord\\Goatl%c%i.WAV",			// Sound files
	"Monsters\\newsfx\\Satyr%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 13, 13, 14, 9, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Satyr Lord",		// Name
	40, 43,		// Min, Max dungeon level
	28,			// Monster level ranking (1 = easy, 30 = hardest)
	160, 200,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	90,			// Hit% #1
	8,			// Hit check frame #1
	20, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RL+M_RF,	// Magic resist
	M_RM+M_IL+M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	2800 },		// Experience points

// MT_NGOATMC	
	{ 128, 1030,								// Image width, Image size
	"Monsters\\GoatMace\\Goat%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\GoatMace\\Goat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12,  8, 12, 6, 20, 12 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  1,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Flesh Clan",		// Name
	6, 10,		// Min, Max dungeon level
	8,			// Monster level ranking (1 = easy, 30 = hardest)
	30, 45,		// Min, Max hitpoints
	AI_GOATMC,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	50,			// Hit% #1
	8,			// Hit check frame #1
	4, 10,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	40,			// Armor Class
	MC_DEMON,	// Monster Class
	M_NONE,		// Magic resist
	M_NONE,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	460 },		// Experience points

// MT_BGOATMC	
	{ 128, 1030,								// Image width, Image size
	"Monsters\\GoatMace\\Goat%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\GoatMace\\Goat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\GoatMace\\Beige.TRN",	// Trans file?, Transformation file name
	{ 12,  8, 12, 6, 20, 12 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  1,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Stone Clan",		// Name
	8, 12,		// Min, Max dungeon level
	10,			// Monster level ranking (1 = easy, 30 = hardest)
	40, 55,		// Min, Max hitpoints
	AI_GOATMC,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	8,			// Hit check frame #1
	6, 12,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	40,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	685 },		// Experience points

// MT_RGOATMC	
	{ 128, 1030,								// Image width, Image size
	"Monsters\\GoatMace\\Goat%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\GoatMace\\Goat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\GoatMace\\Red.TRN",	// Trans file?, Transformation file name
	{ 12,  8, 12, 6, 20, 12 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  1,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Fire Clan",		// Name
	10, 14,		// Min, Max dungeon level
	12,			// Monster level ranking (1 = easy, 30 = hardest)
	50, 65,		// Min, Max hitpoints
	AI_GOATMC,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	70,			// Hit% #1
	8,			// Hit check frame #1
	8, 16,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	45,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF,		// Magic resist
	M_IF,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	906 },		// Experience points

// MT_GGOATMC	
	{ 128, 1030,								// Image width, Image size
	"Monsters\\GoatMace\\Goat%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\GoatMace\\Goat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\GoatMace\\Gray.TRN",	// Trans file?, Transformation file name
	{ 12,  8, 12, 6, 20, 12 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  1,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Night Clan",		// Name
	12, 16,		// Min, Max dungeon level
	14,			// Monster level ranking (1 = easy, 30 = hardest)
	55, 70,		// Min, Max hitpoints
	AI_GOATMC,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	8,			// Hit check frame #1
	10, 20,		// Min, Max damage #1
	15,			// Hit% #2
	0,			// Hit check frame #2
	30, 30,		// Min, Max damage #2
	50,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1190 },		// Experience points

// MT_FIEND	
	{ 96, 364,								// Image width, Image size
	"Monsters\\Bat\\Bat%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\Bat\\Bat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Bat\\red.trn",		// Trans file?, Transformation file name
	{ 9, 13, 10,  9, 13, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Fiend",		// Name
	2, 5,		// Min, Max dungeon level
	3,			// Monster level ranking (1 = easy, 30 = hardest)
	3, 6,		// Min, Max hitpoints
	AI_BAT,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	35,			// Hit% #1
	5,			// Hit check frame #1
	1, 6,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_NONE,		// Magic resist2
	T_NONE,		// Treasure
	MSEL_FLY,	// Selection Type
	102 },		// Experience points
	
// MT_BLINK	
	{ 96, 364,								// Image width, Image size
	"Monsters\\Bat\\Bat%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\Bat\\Bat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 9, 13, 10,  9, 13, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Blink",		// Name
	5, 9,		// Min, Max dungeon level
	7,			// Monster level ranking (1 = easy, 30 = hardest)
	12, 28,		// Min, Max hitpoints
	AI_BAT,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	45,			// Hit% #1
	5,			// Hit check frame #1
	1, 8,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	15,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_NONE,		// Magic resist2
	T_NONE,		// Treasure
	MSEL_FLY,	// Selection Type
	340 },		// Experience points
	
// MT_GLOOM	
	{ 96, 364,								// Image width, Image size
	"Monsters\\Bat\\Bat%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\Bat\\Bat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Bat\\grey.trn",		// Trans file?, Transformation file name
	{ 9, 13, 10,  9, 13, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Gloom",		// Name
	7, 11,		// Min, Max dungeon level
	9,			// Monster level ranking (1 = easy, 30 = hardest)
	28, 36,		// Min, Max hitpoints
	AI_BAT,
	MFLAG_PATH,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	70,			// Hit% #1
	5,			// Hit check frame #1
	4, 12,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	35,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RM,		// Magic resist
	M_RM+M_II,	// Magic resist2
	T_NONE,		// Treasure
	MSEL_FLY,	// Selection Type
	509 },		// Experience points
	
// MT_FAMILIAR	
	{ 96, 364,								// Image width, Image size
	"Monsters\\Bat\\Bat%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\Bat\\Bat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Bat\\orange.trn",		// Trans file?, Transformation file name
	{ 9, 13, 10,  9, 13, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Familiar",		// Name
	11, 15,		// Min, Max dungeon level
	13,			// Monster level ranking (1 = easy, 30 = hardest)
	20, 35,		// Min, Max hitpoints
	AI_BAT,
	MFLAG_PATH,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	50,			// Hit% #1
	5,			// Hit check frame #1
	4, 16,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	35,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_IL,	// Magic resist
	M_RM+M_IL+M_II,	// Magic resist2
	T_NONE,		// Treasure
	MSEL_FLY,	// Selection Type
	448 },		// Experience points

// MT_NGOATBW	
	{ 128, 1040,								// Image width, Image size
	"Monsters\\GoatBow\\GoatB%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\GoatBow\\GoatB%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12,  8, 16, 6, 20,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Flesh Clan",		// Name
	6, 10,		// Min, Max dungeon level
	8,			// Monster level ranking (1 = easy, 30 = hardest)
	20, 35,		// Min, Max hitpoints
	AI_GOATBOW,
	MFLAG_CHECKDOORS,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	35,			// Hit% #1
	13,			// Hit check frame #1
	1, 7,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	35,			// Armor Class
	MC_DEMON,	// Monster Class
	M_NONE,		// Magic resist
	M_NONE,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	448 },		// Experience points

// MT_BGOATBW	
	{ 128, 1040,								// Image width, Image size
	"Monsters\\GoatBow\\GoatB%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\GoatBow\\GoatB%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\GoatBow\\Beige.TRN",		// Trans file?, Transformation file name
	{ 12,  8, 16, 6, 20,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Stone Clan",		// Name
	8, 12,		// Min, Max dungeon level
	10,			// Monster level ranking (1 = easy, 30 = hardest)
	30, 40,		// Min, Max hitpoints
	AI_GOATBOW,
	MFLAG_CHECKDOORS,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	40,			// Hit% #1
	13,			// Hit check frame #1
	2, 9,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	35,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	645 },		// Experience points

// MT_RGOATBW	
	{ 128, 1040,								// Image width, Image size
	"Monsters\\GoatBow\\GoatB%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\GoatBow\\GoatB%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\GoatBow\\Red.TRN",		// Trans file?, Transformation file name
	{ 12,  8, 16, 6, 20,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Fire Clan",		// Name
	10, 14,		// Min, Max dungeon level
	12,			// Monster level ranking (1 = easy, 30 = hardest)
	40, 50,		// Min, Max hitpoints
	AI_GOATBOW,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	45,			// Hit% #1
	13,			// Hit check frame #1
	3, 11,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	35,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF,		// Magic resist
	M_IF,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	822 },		// Experience points

// MT_GGOATBW	
	{ 128, 1040,								// Image width, Image size
	"Monsters\\GoatBow\\GoatB%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\GoatBow\\GoatB%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\GoatBow\\Gray.TRN",		// Trans file?, Transformation file name
	{ 12,  8, 16, 6, 20,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Night Clan",		// Name
	12, 16,		// Min, Max dungeon level
	14,			// Monster level ranking (1 = easy, 30 = hardest)
	50, 65,		// Min, Max hitpoints
	AI_GOATBOW,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	50,			// Hit% #1
	13,			// Hit check frame #1
	4, 13,		// Min, Max damage #1
	15,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	40,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1092 },		// Experience points

// MT_NACID	
	{ 128, 716,									// Image width, Image size
	"Monsters\\Acid\\Acid%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 13, 8, 12, 8, 16, 12 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Acid Beast",		// Name
	10, 14,		// Min, Max dungeon level
	11,			// Monster level ranking (1 = easy, 30 = hardest)
	40, 66,		// Min, Max hitpoints
	AI_ACID,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	40,			// Hit% #1
	8,			// Hit check frame #1
	4, 12,		// Min, Max damage #1
	25,			// Hit% #2
	8,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_IA,		// Magic resist
	M_IA+M_IM,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	846 },		// Experience points

// MT_RACID	
	{ 128, 716,									// Image width, Image size
	"Monsters\\Acid\\Acid%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Acid\\AcidBlk.TRN",	// Trans file?, Transformation file name
	{ 13, 8, 12, 8, 16, 12 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Poison Spitter",		// Name
	14, 18,		// Min, Max dungeon level
	15,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 85,		// Min, Max hitpoints
	AI_ACID,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	45,			// Hit% #1
	8,			// Hit check frame #1
	4, 16,		// Min, Max damage #1
	25,			// Hit% #2
	8,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_IA,		// Magic resist
	M_IA+M_IM,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1248 },		// Experience points

// MT_BACID	
	{ 128, 716,									// Image width, Image size
	"Monsters\\Acid\\Acid%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Acid\\AcidB.TRN",	// Trans file?, Transformation file name
	{ 13, 8, 12, 8, 16, 12 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Pit Beast",		// Name
	18, 22,		// Min, Max dungeon level
	21,			// Monster level ranking (1 = easy, 30 = hardest)
	80, 110,	// Min, Max hitpoints
	AI_ACID,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	55,			// Hit% #1
	8,			// Hit check frame #1
	8, 18,		// Min, Max damage #1
	35,			// Hit% #2
	8,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	35,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_IA|M_RM,	// Magic resist
	M_IA|M_IM|M_RL,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	2060 },		// Experience points

// MT_XACID	
	{ 128, 716,									// Image width, Image size
	"Monsters\\Acid\\Acid%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Acid\\AcidR.TRN",	// Trans file?, Transformation file name
	{ 13, 8, 12, 8, 16, 12 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Lava Maw",		// Name
	22, 27,		// Min, Max dungeon level
	25,			// Monster level ranking (1 = easy, 30 = hardest)
	100, 150,	// Min, Max hitpoints
	AI_ACID,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	65,			// Hit% #1
	8,			// Hit check frame #1
	10, 20,		// Min, Max damage #1
	40,			// Hit% #2
	8,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	35,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_IA|M_RM|M_IF,	// Magic resist
	M_IA|M_IM|M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	2940 },		// Experience points

// MT_SKING	
	{ 160, 1010,								// Image width, Image size
	"Monsters\\SKing\\SKing%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\SKing\\SKing%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\SkelAxe\\White.TRN",		// Trans file?, Transformation file name
	{  8, 6, 16, 6, 16, 6 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2, 0,  0, 0,  0, 2 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Skeleton King",		// Name
	6, 6,		// Min, Max dungeon level
	9,			// Monster level ranking (1 = easy, 30 = hardest)
	140, 140,	// Min, Max hitpoints
	AI_SKELKING,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	8,			// Hit check frame #1
	6, 16,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RF+M_RL+M_IM+M_II,	// Magic resist
	M_IF+M_IL+M_IM+M_II,	// Magic resist2
	T_U+UID_SKCROWN,	// Treasure
	MSEL_BIG,	// Selection Type
	570 },		// Experience points

// MT_CLEAVER	
	{ 128, 980,									// Image width, Image size
	"Monsters\\FatC\\FatC%c" CEL_EXT,				// Image files
	FALSE,										// Special animation
	"Monsters\\FatC\\FatC%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 10, 8, 12, 6, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  1, 0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"The Butcher",		// Name
	0, 0,		// Min, Max dungeon level
	1,			// Monster level ranking (1 = easy, 30 = hardest)
	320, 320, 	// Min, Max hitpoints
	AI_CLEAVER,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	50,			// Hit% #1
	8,			// Hit check frame #1
	6, 12,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	50,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_RL,	// Magic resist
	M_IF+M_IL+M_RM,	// Magic resist2
	T_U+UID_CLEAVER,	// Treasure
	MSEL_REG,	// Selection Type
	710 },		// Experience points

// MT_FAT		
	{ 128, 1130,								// Image width, Image size
	"Monsters\\Fat\\Fat%c" CEL_EXT,				// Image files
	TRUE,										// Special animation
	"Monsters\\Fat\\Fat%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 8, 10, 15, 6, 16, 10 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Overlord",		// Name
	8, 12,		// Min, Max dungeon level
	10,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 80,		// Min, Max hitpoints
	AI_FAT,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	55,			// Hit% #1
	8,			// Hit check frame #1
	6, 12,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	55,			// Armor Class
	MC_DEMON,	// Monster Class
	M_NONE,		// Magic resist
	M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	635 },		// Experience points

// MT_MUDMAN	
	{ 128, 1130,								// Image width, Image size
	"Monsters\\Fat\\Fat%c" CEL_EXT,				// Image files
	TRUE,										// Special animation
	"Monsters\\Fat\\Fat%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Fat\\Blue.TRN",			// Trans file?, Transformation file name
	{ 8, 10, 15, 6, 16, 10 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Mud Man",		// Name
	13, 17,		// Min, Max dungeon level
	14,			// Monster level ranking (1 = easy, 30 = hardest)
	100, 125,	// Min, Max hitpoints
	AI_FAT,
	MFLAG_PATH,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	8,			// Hit check frame #1
	8, 16,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_DEMON,	// Monster Class
	M_NONE,		// Magic resist
	M_IL,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1165 },		// Experience points

// MT_TOAD		
	{ 128, 1130,								// Image width, Image size
	"Monsters\\Fat\\Fat%c" CEL_EXT,				// Image files
	TRUE,										// Special animation
	"Monsters\\Fat\\Fat%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Fat\\FatB.TRN",			// Trans file?, Transformation file name
	{ 8, 10, 15, 6, 16, 10 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Toad Demon",		// Name
	15, 19,		// Min, Max dungeon level
	16,			// Monster level ranking (1 = easy, 30 = hardest)
	135, 160,	// Min, Max hitpoints
	AI_FAT,
	MFLAG_PATH,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	70,			// Hit% #1
	8,			// Hit check frame #1
	8, 16,		// Min, Max damage #1
	40,			// Hit% #2
	0,			// Hit check frame #2
	8, 20,		// Min, Max damage #2
	65,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IM,		// Magic resist
	M_IM+M_RL,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1380 },		// Experience points

// MT_FLAYED	
	{ 128, 1130,								// Image width, Image size
	"Monsters\\Fat\\Fat%c" CEL_EXT,				// Image files
	TRUE,										// Special animation
	"Monsters\\Fat\\Fat%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Fat\\FatF.TRN",			// Trans file?, Transformation file name
	{ 8, 10, 15, 6, 16, 10 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 4,  0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Flayed One",		// Name
	19, 23,		// Min, Max dungeon level
	20,			// Monster level ranking (1 = easy, 30 = hardest)
	160, 200,	// Min, Max hitpoints
	AI_FAT,
	MFLAG_PATH,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	85,			// Hit% #1
	8,			// Hit check frame #1
	10, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_IF,	// Magic resist
	M_IM+M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	2058 },		// Experience points

// MT_WYRM		
	{ 160, 2420,								// Image width, Image size
	"Monsters\\Worm\\Worm%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Fat\\Fat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 13, 13, 13, 11, 19, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Wyrm",		// Name
	9, 13,		// Min, Max dungeon level
	11,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 90,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	40,			// Hit% #1
	8,			// Hit check frame #1
	4, 10,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	25,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RM,		// Magic resist
	M_RM,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	660 },		// Experience points

// MT_CAVSLUG		
	{ 160, 2420,								// Image width, Image size
	"Monsters\\Worm\\Worm%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Fat\\Fat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 13, 13, 13, 11, 19, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Cave Slug",		// Name
	11, 15,		// Min, Max dungeon level
	13,			// Monster level ranking (1 = easy, 30 = hardest)
	75, 110,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	50,			// Hit% #1
	8,			// Hit check frame #1
	6, 13,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RM,		// Magic resist
	M_RM,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	994 },		// Experience points

// MT_DVLWYRM		
	{ 160, 2420,								// Image width, Image size
	"Monsters\\Worm\\Worm%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Fat\\Fat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 13, 13, 13, 11, 19, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Devil Wyrm",		// Name
	13, 17,		// Min, Max dungeon level
	15,			// Monster level ranking (1 = easy, 30 = hardest)
	100, 140,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	55,			// Hit% #1
	8,			// Hit check frame #1
	8, 16,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RM+M_RF,	// Magic resist
	M_RM+M_RF,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1320 },		// Experience points

// MT_DEVOUR		
	{ 160, 2420,								// Image width, Image size
	"Monsters\\Worm\\Worm%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Fat\\Fat%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 13, 13, 13, 11, 19, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Devourer",		// Name
	15, 19,		// Min, Max dungeon level
	17,			// Monster level ranking (1 = easy, 30 = hardest)
	125, 200,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	8,			// Hit check frame #1
	10, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	35,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RM+M_RF+M_II,		// Magic resist
	M_RM+M_RF+M_II,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1827 },		// Experience points

// MT_NMAGMA	
	{ 128, 1680,								// Image width, Image size
	"Monsters\\Magma\\Magma%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Magma\\Magma%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 8, 10, 14, 7, 18, 18 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2,  0,  0, 0,  1,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Magma Demon",		// Name
	14, 17,		// Min, Max dungeon level
	13,			// Monster level ranking (1 = easy, 30 = hardest)
	50, 70,		// Min, Max hitpoints
	AI_MAGMA,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	45,			// Hit% #1
	4,			// Hit check frame #1
	2, 10,		// Min, Max damage #1
	50,			// Hit% #2
	13,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	45,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_IM,	// Magic resist
	M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1076 },		// Experience points

// MT_YMAGMA	
	{ 128, 1680,								// Image width, Image size
	"Monsters\\Magma\\Magma%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Magma\\Magma%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Magma\\Yellow.TRN",		// Trans file?, Transformation file name
	{ 8, 10, 14, 7, 18, 18 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2,  0,  0, 0,  1,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Blood Stone",		// Name
	15, 19,		// Min, Max dungeon level
	14,			// Monster level ranking (1 = easy, 30 = hardest)
	55, 75,		// Min, Max hitpoints
	AI_MAGMA,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	50,			// Hit% #1
	4,			// Hit check frame #1
	2, 12,		// Min, Max damage #1
	50,			// Hit% #2
	14,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	45,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IF+M_IM,	// Magic resist
	M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1309 },		// Experience points

// MT_BMAGMA	
	{ 128, 1680,								// Image width, Image size
	"Monsters\\Magma\\Magma%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Magma\\Magma%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Magma\\Blue.TRN",		// Trans file?, Transformation file name
	{ 8, 10, 14, 7, 18, 18 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2,  0,  0, 0,  1,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hell Stone",		// Name
	16, 20,		// Min, Max dungeon level
	16,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 80,		// Min, Max hitpoints
	AI_MAGMA,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	4,			// Hit check frame #1
	2, 20,		// Min, Max damage #1
	60,			// Hit% #2
	14,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	50,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IF+M_IM,	// Magic resist
	M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1680 },		// Experience points

// MT_WMAGMA	
	{ 128, 1680,								// Image width, Image size
	"Monsters\\Magma\\Magma%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Magma\\Magma%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Magma\\Wierd.TRN",		// Trans file?, Transformation file name
	{ 8, 10, 14, 7, 18, 18 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 2,  0,  0, 0,  1,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Lava Lord",		// Name
	17, 21,		// Min, Max dungeon level
	18,			// Monster level ranking (1 = easy, 30 = hardest)
	70, 85,		// Min, Max hitpoints
	AI_MAGMA,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	75,			// Hit% #1
	4,			// Hit check frame #1
	4, 24,		// Min, Max damage #1
	60,			// Hit% #2
	14,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IF+M_IM,	// Magic resist
	M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2124 },		// Experience points

// MT_HORNED	
	{ 160, 1630,								// Image width, Image size
	"Monsters\\Rhino\\Rhino%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Rhino\\Rhino%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{  8,  8, 14, 6, 16, 6 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Horned Demon",		// Name
	12, 16,		// Min, Max dungeon level
	13,			// Monster level ranking (1 = easy, 30 = hardest)
	40, 80,		// Min, Max hitpoints
	AI_RHINO,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	7,			// Hit check frame #1
	2, 16,		// Min, Max damage #1
	100,		// Hit% #2
	0,			// Hit check frame #2
	5, 32,		// Min, Max damage #2
	40,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1172 },		// Experience points

// MT_MUDRUN	
	{ 160, 1630,								// Image width, Image size
	"Monsters\\Rhino\\Rhino%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Rhino\\Rhino%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Rhino\\Orange.TRN",		// Trans file?, Transformation file name
	{  8,  8, 14, 6, 16, 6 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Mud Runner",		// Name
	14, 18,		// Min, Max dungeon level
	15,			// Monster level ranking (1 = easy, 30 = hardest)
	50, 90,		// Min, Max hitpoints
	AI_RHINO,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	70,			// Hit% #1
	7,			// Hit check frame #1
	6, 18,		// Min, Max damage #1
	100,		// Hit% #2
	0,			// Hit check frame #2
	12,36,		// Min, Max damage #2
	45,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1404 },		// Experience points

// MT_FROSTC	
	{ 160, 1630,								// Image width, Image size
	"Monsters\\Rhino\\Rhino%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Rhino\\Rhino%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Rhino\\Blue.TRN",		// Trans file?, Transformation file name
	{  8,  8, 14, 6, 16, 6 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Frost Charger",		// Name
	16, 20,		// Min, Max dungeon level
	17,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 100,	// Min, Max hitpoints
	AI_RHINO,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	7,			// Hit check frame #1
	8, 20,		// Min, Max damage #1
	100,		// Hit% #2
	0,			// Hit check frame #2
	20, 40,		// Min, Max damage #2
	50,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RL+M_IM,	// Magic resist
	M_RL+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1720 },		// Experience points

// MT_OBLORD	
	{ 160, 1630,								// Image width, Image size
	"Monsters\\Rhino\\Rhino%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Rhino\\Rhino%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Rhino\\RhinoB.TRN",		// Trans file?, Transformation file name
	{  8,  8, 14, 6, 16, 6 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Obsidian Lord",		// Name
	18, 22,		// Min, Max dungeon level
	19,			// Monster level ranking (1 = easy, 30 = hardest)
	70, 110,	// Min, Max hitpoints
	AI_RHINO,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	90,			// Hit% #1
	7,			// Hit check frame #1
	10, 22,		// Min, Max damage #1
	100,		// Hit% #2
	0,			// Hit check frame #2
	20, 50,		// Min, Max damage #2
	55,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RL+M_IM,	// Magic resist
	M_IL+M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1809 },		// Experience points

// MT_BONEDMN	
	{ 128, 1740,								// Image width, Image size
	"Monsters\\Demskel\\Demskl%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Thin\\Thin%c%i.WAV",			// Sound files
	TRUE,										// Special sound
	FALSE, "Monsters\\Thin\\Thinv3.TRN",			// Trans file?, Transformation file name
	{ 10, 8, 20, 6, 24, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"oldboned",		// Name
	46, 47,		// Min, Max dungeon level
	12,			// Monster level ranking (1 = easy, 30 = hardest)
	70, 70,		// Min, Max hitpoints
	AI_STORM,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	8,			// Hit check frame #1
	6, 14,		// Min, Max damage #1
	12,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	50,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IM+M_II,	// Magic resist
	M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1344 },		// Experience points

// MT_REDDTH	
	{ 160, 1740,								// Image width, Image size
	"Monsters\\Thin\\Thin%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Thin\\Thin%c%i.WAV",			// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Thin\\Thinv3.TRN",			// Trans file?, Transformation file name
	{ 8, 8, 18, 4, 17, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Red Death",		// Name
	14, 18,		// Min, Max dungeon level
	16,			// Monster level ranking (1 = easy, 30 = hardest)
	96, 96,		// Min, Max hitpoints
	AI_STORM,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	75,			// Hit% #1
	5,			// Hit check frame #1
	10, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IM+M_IF,	// Magic resist
	M_IM+M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2168 },		// Experience points

// MT_LTCHDMN	
	{ 160, 1740,								// Image width, Image size
	"Monsters\\Thin\\Thin%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Thin\\Thin%c%i.WAV",			// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Thin\\Thinv3.TRN",			// Trans file?, Transformation file name
	{ 8, 8, 18, 4, 17, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Litch Demon",		// Name
	16, 20,		// Min, Max dungeon level
	18,			// Monster level ranking (1 = easy, 30 = hardest)
	110, 110,	// Min, Max hitpoints
	AI_STORM,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	5,			// Hit check frame #1
	10, 24,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	45,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IM+M_IL+M_II,	// Magic resist
	M_IM+M_IL+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2736 },		// Experience points

// MT_UDEDBLRG	
	{ 160, 1740,								// Image width, Image size
	"Monsters\\Thin\\Thin%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Thin\\Thin%c%i.WAV",			// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Thin\\Thinv3.TRN",			// Trans file?, Transformation file name
	{ 8, 8, 18, 4, 17, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Undead Balrog",		// Name
	20, 24,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	130, 130,	// Min, Max hitpoints
	AI_STORM,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	85,			// Hit% #1
	5,			// Hit check frame #1
	12, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	65,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_RL+M_IM+M_II,	// Magic resist
	M_RF+M_RL+M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3575 },		// Experience points

// MT_INCIN	
	{ 128, 1460,									// Image width, Image size
	"Monsters\\Fireman\\FireM%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 14, 19, 20,  8, 14, 23 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Incinerator",		// Name
	40, 43,		// Min, Max dungeon level
	16,			// Monster level ranking (1 = easy, 30 = hardest)
	30, 45,		// Min, Max hitpoints
	AI_FIREMAN,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	75,			// Hit% #1
	8,			// Hit check frame #1
	8, 16,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	25,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IF+M_IM,	// Magic resist
	M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1888 },		// Experience points

// MT_FLAMLRD	
	{ 128, 1460,									// Image width, Image size
	"Monsters\\Fireman\\FireM%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 14, 19, 20,  8, 14, 23 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Flame Lord",		// Name
	42, 45,		// Min, Max dungeon level
	18,			// Monster level ranking (1 = easy, 30 = hardest)
	40, 55,		// Min, Max hitpoints
	AI_FIREMAN,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	75,			// Hit% #1
	8,			// Hit check frame #1
	10, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	25,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IF+M_IM,	// Magic resist
	M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	2250 },		// Experience points

// MT_DOOMFIRE	
	{ 128, 1460,									// Image width, Image size
	"Monsters\\Fireman\\FireM%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 14, 19, 20,  8, 14, 23 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Doom Fire",		// Name
	44, 47,		// Min, Max dungeon level
	20,			// Monster level ranking (1 = easy, 30 = hardest)
	50, 65,		// Min, Max hitpoints
	AI_FIREMAN,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	8,			// Hit check frame #1
	12, 24,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_IF+M_IM,		// Magic resist
	M_RL+M_IF+M_IM,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	2740 },		// Experience points

// MT_HELLBURN	
	{ 128, 1460,									// Image width, Image size
	"Monsters\\Fireman\\FireM%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 14, 19, 20,  8, 14, 23 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hell Burner",		// Name
	46, 47,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 80,		// Min, Max hitpoints
	AI_FIREMAN,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	85,			// Hit% #1
	8,			// Hit check frame #1
	15, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_IF+M_IM,		// Magic resist
	M_RL+M_IF+M_IM,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	3355 },		// Experience points

// MT_STORM	
	{ 160, 1740,								// Image width, Image size
	"Monsters\\Thin\\Thin%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Thin\\Thin%c%i.WAV",			// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Thin\\Thinv3.TRN",			// Trans file?, Transformation file name
	{ 8, 8, 18, 4, 17, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Red Storm",		// Name
	17, 21,		// Min, Max dungeon level
	18,			// Monster level ranking (1 = easy, 30 = hardest)
	55, 110,	// Min, Max hitpoints
	AI_STORM,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	5,			// Hit check frame #1
	8, 18,		// Min, Max damage #1
	75,			// Hit% #2
	8,			// Hit check frame #2
	4, 16,		// Min, Max damage #2
	30,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_IM,	// Magic resist
	M_IM+M_IL,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2160 },		// Experience points

// MT_RSTORM	
	{ 160, 1740,								// Image width, Image size
	"Monsters\\Thin\\Thin%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Thin\\Thin%c%i.WAV",			// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 8, 8, 18, 4, 17, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Storm Rider",		// Name
	19, 23,		// Min, Max dungeon level
	20,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 120,	// Min, Max hitpoints
	AI_STORM,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	5,			// Hit check frame #1
	8, 18,		// Min, Max damage #1
	80,			// Hit% #2
	8,			// Hit check frame #2
	4, 16,		// Min, Max damage #2
	30,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_IL,	// Magic resist
	M_IM+M_IL,	// Magic resist2
	0,	// Treasure
	MSEL_BIG,	// Selection Type
	2391 },		// Experience points

// MT_STORML	
	{ 160, 1740,								// Image width, Image size
	"Monsters\\Thin\\Thin%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Thin\\Thin%c%i.WAV",			// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Thin\\Thinv2.TRN",			// Trans file?, Transformation file name
	{ 8, 8, 18, 4, 17, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Storm Lord",		// Name
	21, 25,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	75, 135,	// Min, Max hitpoints
	AI_STORM,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	85,			// Hit% #1
	5,			// Hit check frame #1
	12, 24,		// Min, Max damage #1
	75,			// Hit% #2
	8,			// Hit check frame #2
	4, 16,		// Min, Max damage #2
	35,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_IL,	// Magic resist
	M_IM+M_IL,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2775 },		// Experience points

// MT_MAEL		
	{ 160, 1740,								// Image width, Image size
	"Monsters\\Thin\\Thin%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Thin\\Thin%c%i.WAV",			// Sound files
	TRUE,										// Special sound
	TRUE, "Monsters\\Thin\\Thinv1.TRN",			// Trans file?, Transformation file name
	{ 8, 8, 18, 4, 17, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Maelstorm",		// Name
	23, 27,		// Min, Max dungeon level
	24,			// Monster level ranking (1 = easy, 30 = hardest)
	90, 150,	// Min, Max hitpoints
	AI_STORM,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	90,			// Hit% #1
	5,			// Hit check frame #1
	12, 28,		// Min, Max damage #1
	75,			// Hit% #2
	8,			// Hit check frame #2
	4, 16,		// Min, Max damage #2
	40,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_II+M_IL,	// Magic resist
	M_IM+M_II+M_IL,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3177 },		// Experience points

	// MT_BIGFALL	
	{ 128, 800 /*1650*/,					// Image width, Image size
	"Monsters\\BigFall\\Fallg%c" CEL_EXT,	// Image files
	TRUE,									// Special animation
//	"Monsters\\BigFall\\Bfal%c%i.WAV",		// Sound files
	"Monsters\\newsfx\\KBrute%c%i.WAV",		// Sound files
	FALSE,							// Special sound
	FALSE, NULL,					// Trans file?, Transformation file name
	{ 10, 8, 11, 8, 17, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  2,  2 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Devil Kin Brute",				// Name
	40, 43,		// Min, Max dungeon level
	27,			// Monster level ranking (1 = easy, 30 = hardest)
	120, 160,		// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	100,		// Hit% #1
	6,			// Hit check frame #1
	18, 24,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RF+M_RL,	// Magic resist
	M_RF+M_RL+M_RM,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	2400 },		// Experience points

	// MT_WINGED	
	{ 160, 1650,								// Image width, Image size
	"Monsters\\Gargoyle\\Gargo%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\Gargoyle\\Gargo%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 14, 14, 14, 10, 18, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0,  2 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Winged-Demon",		// Name
	8, 12,		// Min, Max dungeon level
	9,			// Monster level ranking (1 = easy, 30 = hardest)
	45, 60,		// Min, Max hitpoints
	AI_GARG,
	MFLAG_CHECKDOORS,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	50,			// Hit% #1
	7,			// Hit check frame #1
	10, 16,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	45,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_II+M_IM,		// Magic resist
	M_IF+M_II+M_IM,		// Magic resist2
	0,			// Treasure
	MSEL_FLY,	// Selection Type
	662 },		// Experience points

// MT_GARGOYLE	
	{ 160, 1650,								// Image width, Image size
	"Monsters\\Gargoyle\\Gargo%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\Gargoyle\\Gargo%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Gargoyle\\GarE.TRN",	// Trans file?, Transformation file name
	{ 14, 14, 14, 10, 18, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0,  2 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Gargoyle",		// Name
	12, 16,		// Min, Max dungeon level
	13,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 90,		// Min, Max hitpoints
	AI_GARG,
	MFLAG_CHECKDOORS,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	65,			// Hit% #1
	7,			// Hit check frame #1
	10, 16,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	45,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_II+M_IM,	// Magic resist
	M_IL+M_II+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_FLY,	// Selection Type
	1205 },		// Experience points

// MT_BLOODCLW	
	{ 160, 1650,								// Image width, Image size
	"Monsters\\Gargoyle\\Gargo%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\Gargoyle\\Gargo%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Gargoyle\\GargBr.TRN",	// Trans file?, Transformation file name
	{ 14, 14, 14, 10, 18, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Blood Claw",		// Name
	16, 20,		// Min, Max dungeon level
	19,			// Monster level ranking (1 = easy, 30 = hardest)
	75, 125,	// Min, Max hitpoints
	AI_GARG,
	MFLAG_CHECKDOORS,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	7,			// Hit check frame #1
	14, 22,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	50,			// Armor Class
	MC_DEMON,	// Monster Class
	M_II+M_IM+M_IF,	// Magic resist
	M_IF+M_RL+M_II+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_FLY,	// Selection Type
	1873 },		// Experience points

// MT_DEATHW	
	{ 160, 1650,								// Image width, Image size
	"Monsters\\Gargoyle\\Gargo%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\Gargoyle\\Gargo%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Gargoyle\\GargB.TRN",	// Trans file?, Transformation file name
	{ 14, 14, 14, 10, 18, 14 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Death Wing",		// Name
	18, 22,		// Min, Max dungeon level
	23,			// Monster level ranking (1 = easy, 30 = hardest)
	90, 150,	// Min, Max hitpoints
	AI_GARG,
	MFLAG_CHECKDOORS,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	95,			// Hit% #1
	7,			// Hit check frame #1
	16, 28,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_DEMON,	// Monster Class
	M_II+M_IM+M_IL,	// Magic resist
	M_RF+M_IL+M_II+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_FLY,	// Selection Type
	2278 },		// Experience points

// MT_MEGA		
	{ 160, 2220,							// Image width, Image size
	"Monsters\\Mega\\Mega%c" CEL_EXT,			// Image files
	TRUE,									// Special animation
	"Monsters\\Mega\\Mega%c%i.WAV",			// Sound files
	TRUE,									// Special sound
	FALSE, NULL,							// Trans file?, Transformation file name
	{ 6, 7, 14, 1, 24, 5 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  2, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Slayer",		// Name
	19, 23,		// Min, Max dungeon level
	20,			// Monster level ranking (1 = easy, 30 = hardest)
	120, 140,	// Min, Max hitpoints
	AI_MEGA,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	100,		// Hit% #1
	8,			// Hit check frame #1
	12, 20,		// Min, Max damage #1
	0,			// Hit% #2
	3,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_IF,	// Magic resist
	M_RM+M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2300 },		// Experience points

// MT_GUARD    
	{ 160, 2220,							// Image width, Image size
	"Monsters\\Mega\\Mega%c" CEL_EXT,			// Image files
	TRUE,									// Special animation
	"Monsters\\Mega\\Mega%c%i.WAV",			// Sound files
	TRUE,									// Special sound
	TRUE, "Monsters\\Mega\\Guard.TRN",		// Trans file?, Transformation file name
	{ 6, 7, 14, 1, 24, 5 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  2, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Guardian",		// Name
	21, 25,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	140, 160,	// Min, Max hitpoints
	AI_MEGA,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	110,		// Hit% #1
	8,			// Hit check frame #1
	14, 22,		// Min, Max damage #1
	0,			// Hit% #2
	3,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	65,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_IF,	// Magic resist
	M_RM+M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2714 },		// Experience points

// MT_VTEXLRD  
	{ 160, 2220,							// Image width, Image size
	"Monsters\\Mega\\Mega%c" CEL_EXT,			// Image files
	TRUE,									// Special animation
	"Monsters\\Mega\\Mega%c%i.WAV",			// Sound files
	TRUE,									// Special sound
	TRUE, "Monsters\\Mega\\Vtexl.TRN",		// Trans file?, Transformation file name
	{ 6, 7, 14, 1, 24, 5 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  2, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Vortex Lord",		// Name
	23, 26,		// Min, Max dungeon level
	24,			// Monster level ranking (1 = easy, 30 = hardest)
	160, 180,	// Min, Max hitpoints
	AI_MEGA,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	120,		// Hit% #1
	8,			// Hit check frame #1
	18, 24,		// Min, Max damage #1
	0,			// Hit% #2
	3,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_IF+M_II,		// Magic resist
	M_RM+M_RL+M_IF+M_II,		// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3252 },		// Experience points

// MT_BALROG   
	{ 160, 2220,							// Image width, Image size
	"Monsters\\Mega\\Mega%c" CEL_EXT,			// Image files
	TRUE,									// Special animation
	"Monsters\\Mega\\Mega%c%i.WAV",			// Sound files
	TRUE,									// Special sound
	TRUE, "Monsters\\Mega\\Balr.TRN",		// Trans file?, Transformation file name
	{ 6, 7, 14, 1, 24, 5 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  2, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Balrog",		// Name
	25, 29,		// Min, Max dungeon level
	26,			// Monster level ranking (1 = easy, 30 = hardest)
	180, 200,	// Min, Max hitpoints
	AI_MEGA,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	130,		// Hit% #1
	8,			// Hit check frame #1
	22, 30,		// Min, Max damage #1
	0,			// Hit% #2
	3,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	75,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_IF+M_II,		// Magic resist
	M_RM+M_RL+M_IF+M_II,		// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3643 },		// Experience points

// MT_NSNAKE	
	{ 160, 1270,							// Image width, Image size
	"Monsters\\Snake\\Snake%c" CEL_EXT,			// Image files
	FALSE,									// Special animation
	"Monsters\\Snake\\Snake%c%i.WAV",		// Sound files
	FALSE,									// Special sound
	FALSE, NULL,							// Trans file?, Transformation file name
	{ 12, 11, 13, 5, 18, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  1, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Cave Viper",					// Name
	20, 24,		// Min, Max dungeon level
	21,			// Monster level ranking (1 = easy, 30 = hardest)
	100, 150,	// Min, Max hitpoints
	AI_SNAKE,
	MFLAG_PATH,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	90,			// Hit% #1
	8,			// Hit check frame #1
	8, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_DEMON,	// Monster Class
	M_IM,		// Magic resist
	M_IM,		// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2725 },		// Experience points

// MT_RSNAKE	
	{ 160, 1270,							// Image width, Image size
	"Monsters\\Snake\\Snake%c" CEL_EXT,			// Image files
	FALSE,									// Special animation
	"Monsters\\Snake\\Snake%c%i.WAV",		// Sound files
	FALSE,									// Special sound
	TRUE, "Monsters\\Snake\\SnakR.TRN",		// Trans file?, Transformation file name
	{ 12, 11, 13, 5, 18, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  1, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Fire Drake",		// Name
	22, 26,		// Min, Max dungeon level
	23,			// Monster level ranking (1 = easy, 30 = hardest)
	120, 170,	// Min, Max hitpoints
	AI_SNAKE,
	MFLAG_PATH,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	105,		// Hit% #1
	8,			// Hit check frame #1
	12, 24,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	65,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_IM,	// Magic resist
	M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3139 },		// Experience points

// MT_BSNAKE	
	{ 160, 1270,							// Image width, Image size
	"Monsters\\Snake\\Snake%c" CEL_EXT,			// Image files
	FALSE,									// Special animation
	"Monsters\\Snake\\Snake%c%i.WAV",		// Sound files
	FALSE,									// Special sound
	TRUE, "Monsters\\Snake\\Snakg.TRN",		// Trans file?, Transformation file name
	{ 12, 11, 13, 5, 18, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  1, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Gold Viper",		// Name
	24, 27,		// Min, Max dungeon level
	25,			// Monster level ranking (1 = easy, 30 = hardest)
	140, 180,	// Min, Max hitpoints
	AI_SNAKE,
	MFLAG_PATH,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	120,		// Hit% #1
	8,			// Hit check frame #1
	15, 26,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_IM,	// Magic resist
	M_RL+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3540 },		// Experience points

// MT_GSNAKE	
	{ 160, 1270,							// Image width, Image size
	"Monsters\\Snake\\Snake%c" CEL_EXT,			// Image files
	FALSE,									// Special animation
	"Monsters\\Snake\\Snake%c%i.WAV",		// Sound files
	FALSE,									// Special sound
	TRUE, "Monsters\\Snake\\Snakb.TRN",		// Trans file?, Transformation file name
	{ 12, 11, 13, 5, 18, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  1, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Azure Drake",		// Name
	28, 30,		// Min, Max dungeon level
	27,			// Monster level ranking (1 = easy, 30 = hardest)
	160, 200,	// Min, Max hitpoints
	AI_SNAKE,
	MFLAG_PATH,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	130,		// Hit% #1
	8,			// Hit check frame #1
	18, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	75,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_RL,	// Magic resist
	M_IL+M_RF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3791 },		// Experience points

// MT_NBLACK	
	{ 160, 2120,								// Image width, Image size
	"Monsters\\Black\\Black%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Black\\Black%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{  8,  8, 16, 4, 24, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Black Knight",		// Name
	23, 27,		// Min, Max dungeon level
	24,			// Monster level ranking (1 = easy, 30 = hardest)
	150, 150,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	110,		// Hit% #1
	8,			// Hit check frame #1
	15, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	75,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_RM+M_II,	// Magic resist
	M_RM+M_IL+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3360 },		// Experience points

// MT_RTBLACK	
	{ 160, 2120,								// Image width, Image size
	"Monsters\\Black\\Black%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Black\\Black%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Black\\BlkKntRT.TRN",	// Trans file?, Transformation file name
	{  8,  8, 16, 4, 24, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Doom Guard",		// Name
	25, 29,		// Min, Max dungeon level
	26,			// Monster level ranking (1 = easy, 30 = hardest)
	165, 165,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	130,		// Hit% #1
	8,			// Hit check frame #1
	18, 25,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	75,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_RM+M_II,	// Magic resist
	M_RM+M_IF+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3650 },		// Experience points

// MT_BTBLACK	
	{ 160, 2120,								// Image width, Image size
	"Monsters\\Black\\Black%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Black\\Black%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Black\\BlkKntBT.TRN",	// Trans file?, Transformation file name
	{  8,  8, 16, 4, 24, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Steel Lord",		// Name
	27, 30,		// Min, Max dungeon level
	28,			// Monster level ranking (1 = easy, 30 = hardest)
	180, 180,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	120,		// Hit% #1
	8,			// Hit check frame #1
	20, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	80,			// Armor Class
	MC_DEMON,		// Monster Class
	M_RL+M_RM+M_IF+M_II,	// Magic resist
	M_RL+M_IM+M_IF+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	4252 },		// Experience points

// MT_RBLACK	
	{ 160, 2120,								// Image width, Image size
	"Monsters\\Black\\Black%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Black\\Black%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Black\\BlkKntBe.TRN",	// Trans file?, Transformation file name
	{  8,  8, 16, 4, 24, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Blood Knight",		// Name
	24, 26,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	200, 200,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	130,		// Hit% #1
	8,			// Hit check frame #1
	25, 35,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	85,		// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_IL+M_II+M_IM,	// Magic resist
	M_RF+M_IL+M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	5130 },		// Experience points

// MT_UNRAV	
	{ 96, 484,									// Image width, Image size
	"Monsters\\Unrav\\Unrav%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\Shred%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 10, 10, 12,  5, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"The Shredded",		// Name
	32, 35,		// Min, Max dungeon level
	23,			// Monster level ranking (1 = easy, 30 = hardest)
	70, 90,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	75,			// Hit% #1
	7,			// Hit check frame #1
	4, 12,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	65,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RF+M_RL+M_II,		// Magic resist
	M_RF+M_RL+M_II,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	900 },		// Experience points

// MT_HOLOWONE	
	{ 96, 484,									// Image width, Image size
	"Monsters\\Unrav\\Unrav%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 10, 10, 12,  5, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hollow One",		// Name
	34, 37,		// Min, Max dungeon level
	27,			// Monster level ranking (1 = easy, 30 = hardest)
	135, 240,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	75,			// Hit% #1
	7,			// Hit check frame #1
	12, 24,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	75,			// Armor Class
	MC_UNDEAD,		// Monster Class
	M_RL+M_IM+M_IF+M_II,		// Magic resist
	M_RL+M_IM+M_IF+M_II,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	4374 },		// Experience points

// MT_PAINMSTR	
	{ 96, 484,									// Image width, Image size
	"Monsters\\Unrav\\Unrav%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 10, 10, 12,  5, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Pain Master",		// Name
	36, 39,		// Min, Max dungeon level
	29,			// Monster level ranking (1 = easy, 30 = hardest)
	110, 200,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	7,			// Hit check frame #1
	16, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	80,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RL+M_IM+M_IF+M_II,		// Magic resist
	M_RL+M_IM+M_IF+M_II,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	5147 },		// Experience points

// MT_REALWEAV	
	{ 96, 484,									// Image width, Image size
	"Monsters\\Unrav\\Unrav%c" CEL_EXT,			// Image files
	FALSE,										// Special animation
	"Monsters\\Acid\\Acid%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 10, 10, 12,  5, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Reality Weaver",		// Name
	38, 39,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	135, 240,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	85,			// Hit% #1
	7,			// Hit check frame #1
	20, 35,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	85,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RM+M_IL+M_IF+M_II,		// Magic resist
	M_RM+M_IL+M_IF+M_II,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	5925 },		// Experience points

// MT_SUCCUBUS	
	{ 128, 980,									// Image width, Image size
	"Monsters\\Succ\\Scbs%c" CEL_EXT,				// Image files
	FALSE,										// Special animation
	"Monsters\\Succ\\Scbs%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 14, 8, 16, 7, 24, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0, 0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Succubus",		// Name
	22, 26,		// Min, Max dungeon level
	24,			// Monster level ranking (1 = easy, 30 = hardest)
	120, 150,	// Min, Max hitpoints
	AI_SUCC,
	MFLAG_CHECKDOORS,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	100,		// Hit% #1
	10,			// Hit check frame #1
	1, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM,		// Magic resist
	M_IM+M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	3696 },		// Experience points

// MT_SNOWWICH 
	{ 128, 980,									// Image width, Image size
	"Monsters\\Succ\\Scbs%c" CEL_EXT,				// Image files
	FALSE,										// Special animation
	"Monsters\\Succ\\Scbs%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Succ\\Succb.TRN",			// Trans file?, Transformation file name
	{ 14, 8, 16, 7, 24, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0, 0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Snow Witch",		// Name
	25, 28,		// Min, Max dungeon level
	26,			// Monster level ranking (1 = easy, 30 = hardest)
	135, 175,	// Min, Max hitpoints
	AI_SUCC,
	MFLAG_CHECKDOORS,			// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	110,		// Hit% #1
	10,			// Hit check frame #1
	1, 24,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	65,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_II,	// Magic resist
	M_IM+M_II+M_RL,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	4084 },		// Experience points

// MT_HLSPWN   
	{ 128, 980,									// Image width, Image size
	"Monsters\\Succ\\Scbs%c" CEL_EXT,				// Image files
	FALSE,										// Special animation
	"Monsters\\Succ\\Scbs%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Succ\\Succrw.TRN",			// Trans file?, Transformation file name
	{ 14, 8, 16, 7, 24, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0, 0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hell Spawn",		// Name
	27, 30,		// Min, Max dungeon level
	28,			// Monster level ranking (1 = easy, 30 = hardest)
	150, 200,	// Min, Max hitpoints
	AI_SUCC,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	115,		// Hit% #1
	10,			// Hit check frame #1
	1, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	75,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RM+M_IL,	// Magic resist
	M_IM+M_IF+M_RL,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	4480 },		// Experience points

// MT_SOLBRNR  
	{ 128, 980,									// Image width, Image size
	"Monsters\\Succ\\Scbs%c" CEL_EXT,				// Image files
	FALSE,										// Special animation
	"Monsters\\Succ\\Scbs%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Succ\\Succbw.TRN",			// Trans file?, Transformation file name
	{ 14, 8, 16, 7, 24, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0, 0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Soul Burner",		// Name
	28, 30,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	140, 225,		// Min, Max hitpoints
	AI_SUCC,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	120,		// Hit% #1
	10,			// Hit check frame #1
	1, 35,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	85,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_RL+M_IF,		// Magic resist
	M_IM+M_IL+M_IF,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	4644 },		// Experience points

// MT_COUNSLR		
	{ 128, 2000,								// Image width, Image size
	"Monsters\\Mage\\Mage%c" CEL_EXT,				// Image files
	TRUE,										// Special animation
	"Monsters\\Mage\\Mage%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12, 1, 20,  8, 28, 20 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Counselor",		// Name
	24, 26,		// Min, Max dungeon level
	25,			// Monster level ranking (1 = easy, 30 = hardest)
	70, 70,		// Min, Max hitpoints
	AI_COUNSLR,
	MFLAG_CHECKDOORS,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	90,			// Hit% #1
	8,			// Hit check frame #1
	8, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_DEMON,		// Monster Class
	M_RM+M_RL+M_RF,		// Magic resist
	M_RM+M_RL+M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	4070 },		// Experience points

// MT_MAGISTR		
	{ 128, 2000,								// Image width, Image size
	"Monsters\\Mage\\Mage%c" CEL_EXT,				// Image files
	TRUE,										// Special animation
	"Monsters\\Mage\\Mage%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Mage\\Cnselg.TRN",			// Trans file?, Transformation file name
	{ 12, 1, 20,  8, 28, 20 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Magistrate",		// Name
	26, 28,		// Min, Max dungeon level
	27,			// Monster level ranking (1 = easy, 30 = hardest)
	85, 85,		// Min, Max hitpoints
	AI_COUNSLR,
	MFLAG_CHECKDOORS,	// ai related flags
	1,			// Intelligence (0 = easiest, 3 = hardest of type)
	100,		// Hit% #1
	8,			// Hit check frame #1
	10, 24,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_RL+M_II+M_IF,// Magic resist
	M_IM+M_RL+M_II+M_IF,// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	4478 },		// Experience points

// MT_CABALIST		
	{ 128, 2000,								// Image width, Image size
	"Monsters\\Mage\\Mage%c" CEL_EXT,				// Image files
	TRUE,										// Special animation
	"Monsters\\Mage\\Mage%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Mage\\Cnselgd.TRN",			// Trans file?, Transformation file name
	{ 12, 1, 20,  8, 28, 20 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Cabalist",		// Name
	28, 30,		// Min, Max dungeon level
	29,			// Monster level ranking (1 = easy, 30 = hardest)
	120, 120,		// Min, Max hitpoints
	AI_COUNSLR,
	MFLAG_CHECKDOORS,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	110,		// Hit% #1
	8,			// Hit check frame #1
	14, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_RF+M_II+M_IL,	// Magic resist
	M_IM+M_RF+M_II+M_IL,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	4929 },		// Experience points

// MT_ADVOCATE		
	{ 128, 2000,								// Image width, Image size
	"Monsters\\Mage\\Mage%c" CEL_EXT,				// Image files
	TRUE,										// Special animation
	"Monsters\\Mage\\Mage%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	TRUE, "Monsters\\Mage\\Cnselbk.TRN",			// Trans file?, Transformation file name
	{ 12, 1, 20,  8, 28, 20 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Advocate",		// Name
	30, 30,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	145, 145,		// Min, Max hitpoints
	AI_COUNSLR,
	MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	120,		// Hit% #1
	8,			// Hit check frame #1
	15, 25,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	0,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_IL+M_II+M_IM,	// Magic resist
	M_IM+M_IL+M_IF+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	4968 },		// Experience points

// MT_GOLEM		
	{ 96, 386,									// Image width, Image size
	"Monsters\\Golem\\Golem%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\Golem\\Golm%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{  0, 16, 12,  0, 12, 20 },					// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0,  0 },					// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Golem",	// Name
	0, 0,		// Min, Max dungeon level
	12,			// Monster level ranking (1 = easy, 30 = hardest)
	1, 1,		// Min, Max hitpoints (calc in SPAWNGOLUM)
	AI_GOLUM,	// AI type
	MFLAG_CHECKDOORS,	// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	0,			// Hit% #1 (calc in SPAWNGOLUM)
	7,			// Hit check frame #1
	1, 1,		// Min, Max damage #1 (calc in SPAWNGOLUM)
	//0, 0,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	1,			// Armor Class (calc in SPAWNGOLUM)
	MC_DEMON,	// Monster Class
	0,			// Magic resist
	0,			// Magic resist2
	0,			// Treasure
	MSEL_NONE,	// Selection Type
	0 },		// Experience points

// MT_DIABLO	
	{ 160, 2000,							// Image width, Image size
	"Monsters\\Diablo\\Diablo%c" CEL_EXT,		// Image files
	TRUE,									// Special animation
	"Monsters\\Diablo\\Diablo%c%i.WAV",			// Sound files
	TRUE,									// Special sound
	FALSE, NULL,							// Trans file?, Transformation file name
	{ 16,  6, 16, 2, 16, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"The Dark Lord",		// Name
	50, 50,		// Min, Max dungeon level
	45,			// Monster level ranking (1 = easy, 30 = hardest)
	3333, 3333,	// Min, Max hitpoints
	AI_DIABLO,
	MFLAG_KNOCKBACK|MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	220,		// Hit% #1
	4,			// Hit check frame #1
	30, 60,		// Min, Max damage #1
	0,			// Hit% #2
	11,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	90,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_RL+M_IM+M_II,	// Magic resist
	M_RF+M_RL+M_IM+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	31666 },		// Experience points

// MT_DARKMAGE		
	{ 128, 1060,								// Image width, Image size
	"Monsters\\DarkMage\\Dmage%c" CEL_EXT,				// Image files
	TRUE,										// Special animation
	"Monsters\\DarkMage\\Dmag%c%i.WAV",				// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{  6,  1, 21,  6, 23, 18 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"The Arch-Litch Malignus",		// Name
	40, 41 /*24, 24*/,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	160, 160,		// Min, Max hitpoints
	AI_COUNSLR,
	MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	120,		// Hit% #1
	8,			// Hit check frame #1
	20, 40,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM+M_RL+M_RF+M_II,	// Magic resist
	M_IM+M_IL+M_IF+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	4968 },		// Experience points

//---------------------//
//    New Monsters     //
//---------------------//
	
// Festering Nest
	
	{ 188, 800 /*1250*/,								// Image width, Image size
	"Monsters\\Fork\\Fork%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\HBoar%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 10, 10, 15, 6, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hellboar",		// Name
	32, 35,		// Min, Max dungeon level
	23,			// Monster level ranking (1 = easy, 30 = hardest)
	80, 100,	// Min, Max hitpoints
	AI_SKELSD,	// eventually AI_RHINO?
	MFLAG_PATH|MFLAG_KNOCKBACK,	// ai related flags
	2,			// Intelligence (0 = easiest, 3 = hardest of type)
	70,			// Hit% #1
	7,			// Hit check frame #1
	16, 24,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_DEMON,	// Monster Class
	M_NONE,		// Magic resist
	M_RL+M_RF,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	750},		// Experience points
	
	{ 64, 305,							// Image width, Image size
	"Monsters\\Scorp\\Scorp%c" CEL_EXT,		// Image files
	FALSE,								// Special animation
	"Monsters\\newsfx\\Stingr%c%i.WAV",		// Sound files
	FALSE,								// Special sound
	FALSE, NULL,						// Trans file?, Transformation file name
	{ 10, 10, 12, 6, 15, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Stinger",	// Name
	32, 35,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	30, 40,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	85,			// Hit% #1
	8,			// Hit check frame #1
	1, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	50,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RL,		// Magic resist2
	0,			// Treasure
	MSEL_FLR,	// Selection Type
	500},		// Experience points
	
	{ 156, 800 /*1156*/,								// Image width, Image size
	"Monsters\\Eye\\Eye%c" CEL_EXT,				// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\psyco%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12, 13, 13, 7, 21, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Psychorb",		// Name
	32, 35,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	20, 30,		// Min, Max hitpoints
	AI_PSYCHORB,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	8,			// Hit check frame #1
	10, 10,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	40,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_FLY,	// Selection Type
	450},		// Experience points
	
	{ 148, 800/*1000*/,								// Image width, Image size
	"Monsters\\Spider\\Spider%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\SLord%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12, 10, 15, 6, 20, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Arachnon",		// Name
	32, 35,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 80,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	50,			// Hit% #1
	8,			// Hit check frame #1
	5, 15,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	50,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RL,		// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	500},		// Experience points
	
	{ 128, 800/*2000*/,							// Image width, Image size
	"Monsters\\TSneak\\TSneak%c" CEL_EXT,		// Image files
	FALSE,									// Special animation
	"Monsters\\newsfx\\FTwin%c%i.WAV",			// Sound files
	FALSE,									// Special sound
	FALSE, NULL,							// Trans file?, Transformation file name
	{ 13, 13, 15, 11, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0,  0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Felltwin",	// Name
	32, 35,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	50, 70,		// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	70,			// Hit% #1
	8,			// Hit check frame #1
	10, 18,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	50,			// Armor Class
	MC_DEMON,	// Monster Class
	M_II,		// Magic resist
	M_RF+M_RL+M_II,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	600},		// Experience points
	
	{ 164, 520,								// Image width, Image size
	"Monsters\\Spawn\\Spawn%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\HSpawn%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 15, 12, 14, 11, 14, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hork Spawn",		// Name
	34, 37,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	30, 30,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	8,			// Hit check frame #1
	10, 25,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	25,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RM,		// Magic resist
	M_RM,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	250},		// Experience points
	
	{ 86, 305,							// Image width, Image size
	"Monsters\\WScorp\\WScorp%c" CEL_EXT,		// Image files
	FALSE,								// Special animation
	"Monsters\\newsfx\\Stingr%c%i.WAV",		// Sound files
	FALSE,								// Special sound
	FALSE, NULL,						// Trans file?, Transformation file name
	{ 10, 10, 12, 6, 15, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Venomtail",	// Name
	36, 39,		// Min, Max dungeon level
	24,			// Monster level ranking (1 = easy, 30 = hardest)
	40, 50,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	85,		// Hit% #1
	8,			// Hit check frame #1
	1, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RL,		// Magic resist
	M_IL,		// Magic resist2
	0,			// Treasure
	MSEL_FLR,	// Selection Type
	1000},		// Experience points
	
	{ 140, 800 /*1200*/,								// Image width, Image size
	"Monsters\\Eye2\\Eye2%c" CEL_EXT,				// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\Psyco%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12, 13, 13, 7, 21, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Necromorb",// Name
	36, 39,		// Min, Max dungeon level
	24,			// Monster level ranking (1 = easy, 30 = hardest)
	30, 40,		// Min, Max hitpoints
	AI_NECROMORB,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	8,			// Hit check frame #1
	20, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	50,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RF,		// Magic resist
	M_RL+M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_FLY,	// Selection Type
	1100},		// Experience points

	{ 148, 800/*1100*/,								// Image width, Image size
	"Monsters\\bSpidr\\bSpidr%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\newsfx\\SLord%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12, 10, 15, 6, 20, 10 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Spider Lord",		// Name
	36, 39,		// Min, Max dungeon level
	24,			// Monster level ranking (1 = easy, 30 = hardest)
	80, 100,	// Min, Max hitpoints
	AI_ACID,
	MFLAG_PATH,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	8,			// Hit check frame #1
	8, 20,		// Min, Max damage #1
	75,			// Hit% #2
	8,			// Hit check frame #2
	10, 10,		// Min, Max damage #2
	60,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_RL,		// Magic resist
	M_RF+M_IL,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1250},		// Experience points

	{ 176, 800 /* 1700 */,				  	// Image width, Image size
	"Monsters\\Clasp\\Clasp%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\Lworm%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 10, 12, 15, 6, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Lashworm",	// Name
	36, 39,		// Min, Max dungeon level
	20,			// Monster level ranking (1 = easy, 30 = hardest)
	30, 30,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	90,			// Hit% #1
	8,			// Hit check frame #1
	12, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	50,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RF,		// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	600},		// Experience points

	{ 192, 800 /*1080*/,					// Image width, Image size
	"Monsters\\AntWorm\\Worm%c" CEL_EXT,		// Image files
	FALSE,								// Special animation
	"Monsters\\newsfx\\TchAnt%c%i.WAV",		// Sound files
	FALSE,								// Special sound
	FALSE, NULL,						// Trans file?, Transformation file name
	{ 14, 12, 12, 6, 20, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Torchant",	// Name
	36, 39,		// Min, Max dungeon level
	22,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 80,		// Min, Max hitpoints
	AI_HELLBAT,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	75,			// Hit% #1
	8,			// Hit check frame #1
	20, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_IF,		// Magic resist
	M_RL+M_RM+M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	1250},		// Experience points

	{ 138, 800/*2000*/,						// Image width, Image size
	"Monsters\\HorkD\\HorkD%c" CEL_EXT,		// Image files
	TRUE,									// Special animation
	"Monsters\\newsfx\\HDemon%c%i.WAV",		// Sound files
	TRUE,									// Special sound
	FALSE, NULL,							// Trans file?, Transformation file name
	{ 15,  8, 16, 6, 16, 9 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 2 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hork Demon",		// Name
	36, 37,		// Min, Max dungeon level
	27,			// Monster level ranking (1 = easy, 30 = hardest)
	120, 160,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	60,			// Hit% #1
	8,			// Hit check frame #1
	20, 35,		// Min, Max damage #1
	80,			// Hit% #2
	8,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	80,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL,		// Magic resist
	M_RM+M_IL,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2000},		// Experience points
	
	{ 198, 800 /* 2000 */,				  	// Image width, Image size
	"Monsters\\Hellbug\\Hellbg%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\newsfx\\Defile%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{  8,  8, 14, 6, 14, 12 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hell Bug",	// Name
	38, 39,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	240, 240,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_PATH,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	110,		// Hit% #1
	8,			// Hit check frame #1
	20, 30,		// Min, Max damage #1
	90,			// Hit% #2
	8,			// Hit check frame #2
	50, 60,		// Min, Max damage #2
	80,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RF+M_RM+M_IL,	// Magic resist
	M_RM+M_IF+M_IL,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	5000},		// Experience points


// Demon Crypt

	{ 124, 800,								// Image width, Image size
	"Monsters\\Gravdg\\Gravdg%c" CEL_EXT,	// Image files
	TRUE,									// Special animation
	"Monsters\\newsfx\\GDiggr%c%i.WAV",		// Sound files
	TRUE,									// Special sound
	FALSE, NULL,							// Trans file?, Transformation file name
	{ 24, 24, 12, 6, 16, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Gravedigger",		// Name
	40, 41,		// Min, Max dungeon level
	26,			// Monster level ranking (1 = easy, 30 = hardest)
	120, 240,	// Min, Max hitpoints
	AI_SCAV,
	MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	80,			// Hit% #1
	6,			// Hit check frame #1
	2, 12,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	20,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IL+M_II,	// Magic resist
	M_RF+M_RM+M_IL+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	2000},		// Experience points

	{ 104, 550,							// Image width, Image size
	"Monsters\\Rat\\Rat%c" CEL_EXT,		// Image files
	FALSE,								// Special animation
	"Monsters\\newsfx\\TmbRat%c%i.WAV",	// Sound files
	FALSE,								// Special sound
	FALSE, NULL,						// Trans file?, Transformation file name
	{ 11, 8, 12, 6, 20, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2, 0,  0, 0,  0, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Tomb Rat",	// Name
	40, 43,		// Min, Max dungeon level
	24,			// Monster level ranking (1 = easy, 30 = hardest)
	80, 120,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	120,		// Hit% #1
	8,			// Hit check frame #1
	12, 25,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	30,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_NONE,		// Magic resist
	M_RF+M_RL,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	1800},		// Experience points
	
	{ 96, 550,							// Image width, Image size
	"Monsters\\Hellbat\\Helbat%c" CEL_EXT,		// Image files
	FALSE,								// Special animation
	"Monsters\\newsfx\\HelBat%c%i.WAV",		// Sound files
	FALSE,								// Special sound
	FALSE, NULL,						// Trans file?, Transformation file name
	{ 18, 16, 14, 6, 18, 11},			// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0,  0},			// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Firebat",	// Name
	40, 43,		// Min, Max dungeon level
	24,			// Monster level ranking (1 = easy, 30 = hardest)
	60, 80,		// Min, Max hitpoints
	AI_FIREBAT,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	100,		// Hit% #1
	8,			// Hit check frame #1
	15, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_ANIMAL,	// Monster Class
	M_IF,		// Magic resist
	M_RL+M_RM+M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	2400},		// Experience points

	{ 128, 1740,								// Image width, Image size
	"Monsters\\Demskel\\Demskl%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\newsfx\\SWing%c%i.WAV",			// Sound files
	FALSE,										// Special sound
	FALSE, "Monsters\\Thin\\Thinv3.TRN",			// Trans file?, Transformation file name
	{ 10, 8, 20, 6, 24, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Skullwing",		// Name
	40, 43,		// Min, Max dungeon level
	27,			// Monster level ranking (1 = easy, 30 = hardest)
	70, 70,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	75,			// Hit% #1
	7,			// Hit check frame #1
	15, 20,		// Min, Max damage #1
	75,			// Hit% #2
	9,			// Hit check frame #2
	15, 20,		// Min, Max damage #2
	80,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RF+M_RL+M_II,	// Magic resist
	M_RF+M_RL+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3000 },		// Experience points

	{ 96, 800 /*1109*/,								// Image width, Image size
	"Monsters\\Lich\\Lich%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\Lich%c%i.WAV",				// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12, 10, 10, 7, 21, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  2, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Lich",		// Name
	40, 43,		// Min, Max dungeon level
	25,			// Monster level ranking (1 = easy, 30 = hardest)
	80, 100,	// Min, Max hitpoints
	AI_LICH,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	100,			// Hit% #1
	8,			// Hit check frame #1
	15, 20,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	60,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RL+M_II,	// Magic resist
	M_RF+M_RM+M_IL+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	3000},		// Experience points

	{ 154, 800 /* 1100 */,							// Image width, Image size
	"Monsters\\Bubba\\Bubba%c" CEL_EXT,	// Image files
	FALSE,								// Special animation
	"Monsters\\newsfx\\Crypt%c%i.WAV",		// Sound files
	TRUE,								// Special sound
	FALSE, NULL,						// Trans file?, Transformation file name
	{  8, 18, 12, 8, 21, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  3, 0,  0, 0,  0, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Crypt Demon",	// Name
	42, 45,		// Min, Max dungeon level
	28,			// Monster level ranking (1 = easy, 30 = hardest)
	200, 240,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	100,		// Hit% #1
	8,			// Hit check frame #1
	20, 40,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	85,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_RF+M_IM,	// Magic resist
	M_RL+M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	3200},		// Experience points

	{ 96, 550,							// Image width, Image size
	"Monsters\\Hellbat2\\bhelbt%c" CEL_EXT,		// Image files
	TRUE,								// Special animation
	"Monsters\\newsfx\\HelBat%c%i.WAV",		// Sound files
	FALSE,								// Special sound
	FALSE, NULL,						// Trans file?, Transformation file name
	{ 18, 16, 14, 6, 18, 11 },			// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  0,  0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Hellbat",	// Name
	44, 47,		// Min, Max dungeon level
	29,			// Monster level ranking (1 = easy, 30 = hardest)
	100, 140,	// Min, Max hitpoints
	AI_HELLBAT,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	110,		// Hit% #1
	8,			// Hit check frame #1
	30, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	80,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_RM+M_IF,	// Magic resist
	M_IL+M_RM+M_IF,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	3600},		// Experience points

	{ 128, 1740,								// Image width, Image size
	"Monsters\\Demskel\\Demskl%c" CEL_EXT,			// Image files
	TRUE,										// Special animation
	"Monsters\\newsfx\\SWing%c%i.WAV",			// Sound files
	TRUE,										// Special sound
	FALSE, "Monsters\\Thin\\Thinv3.TRN",			// Trans file?, Transformation file name
	{ 10, 8, 20, 6, 24, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{ 3, 0,  0, 0,  0,  0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Bone Demon",		// Name
	44, 47,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	240, 280,		// Min, Max hitpoints
	AI_BONED,
	0,			// ai related flags
	0,			// Intelligence (0 = easiest, 3 = hardest of type)
	100, 		// Hit% #1
	8,			// Hit check frame #1
	40, 50,		// Min, Max damage #1
	160,		// Hit% #2
	12,			// Hit check frame #2
	50, 50,		// Min, Max damage #2
	50,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_IF+M_IL+M_II,	// Magic resist
	M_IF+M_IL+M_II,	// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	5000 },		// Experience points

	{ 136, 800 /*1109*/,								// Image width, Image size
	"Monsters\\Lich2\\Lich2%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\Lich%c%i.WAV",				// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 12, 10, 10, 7, 21, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  2, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Arch Lich",		// Name
	44, 47,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	180, 200,	// Min, Max hitpoints
	AI_ARCHLICH,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	120,			// Hit% #1
	8,			// Hit check frame #1
	30, 30,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	75,			// Armor Class
	MC_UNDEAD,	// Monster Class
	M_RF+M_RM+M_IL+M_II,// Magic resist
	M_IF+M_IM+M_IL+M_II,// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	4000},		// Experience points

	{ 180, 800 /*1000*/,								// Image width, Image size
	"Monsters\\Byclps\\Byclps%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\Biclop%c%i.WAV",		// Sound files
	FALSE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 10, 11, 16, 6, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2,  0,  0, 0,  2, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Biclops",		// Name
	44, 47,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	200, 240,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_KNOCKBACK|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	90,			// Hit% #1
	8,			// Hit check frame #1
	40, 50,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	80,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL,		// Magic resist
	M_RL+M_RF,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	4000},		// Experience points

	{ 164, 800 /* 1700 */,				  	// Image width, Image size
	"Monsters\\Flesh\\Flesh%c" CEL_EXT,		// Image files
	FALSE,										// Special animation
	"Monsters\\newsfx\\FleshT%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{ 15, 24, 15, 6, 16, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Flesh Thing",		// Name
	44, 47,		// Min, Max dungeon level
	28,			// Monster level ranking (1 = easy, 30 = hardest)
	300, 400,		// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	150,			// Hit% #1
	8,			// Hit check frame #1
	12, 18,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	70,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_RF+M_RM,	// Magic resist
	M_RL+M_RF+M_RM,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	4000},		// Experience points

	{ 180, 800 /* 1600 */,							// Image width, Image size
	"Monsters\\Reaper\\Reap%c" CEL_EXT,	// Image files
	FALSE,								// Special animation
	"Monsters\\newsfx\\Reaper%c%i.WAV",		// Sound files
	FALSE,								// Special sound
	FALSE, NULL,						// Trans file?, Transformation file name
	{ 12, 10, 14, 6, 16, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  2, 0,  0, 0,  0, 0 },			// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Reaper",	// Name
	44, 47,		// Min, Max dungeon level
	30,			// Monster level ranking (1 = easy, 30 = hardest)
	260, 300,	// Min, Max hitpoints
	AI_SKELSD,
	0,			// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	120,		// Hit% #1
	8,			// Hit check frame #1
	30, 35,		// Min, Max damage #1
	0,			// Hit% #2
	0,			// Hit check frame #2
	0, 0,		// Min, Max damage #2
	90,			// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_IF+M_IM,	// Magic resist
	M_IL+M_IF+M_IM,	// Magic resist2
	0,			// Treasure
	MSEL_REG,	// Selection Type
	6000},		// Experience points

	{ 226, 1200/* 2100 */,								// Image width, Image size
	"Monsters\\Nkr\\Nkr%c" CEL_EXT,		// Image files
	TRUE,										// Special animation
	"Monsters\\newsfx\\Nakrul%c%i.WAV",		// Sound files
	TRUE,										// Special sound
	FALSE, NULL,								// Trans file?, Transformation file name
	{  2,  6, 16, 3, 16, 16 },		// Neutral, Walk, Attack, Get Hit, Death, Special (# frames)
	{  0,  0,  0, 0,  0, 0 },		// Neutral, Walk, Attack, Get Hit, Death, Special (anim delays)
	"Na-Krul",		// Name
	60, 60,		// Min, Max dungeon level
	40,			// Monster level ranking (1 = easy, 30 = hardest)
	1332, 1332,	// Min, Max hitpoints
	AI_SKELSD,
	MFLAG_KNOCKBACK|MFLAG_PATH|MFLAG_CHECKDOORS,	// ai related flags
	3,			// Intelligence (0 = easiest, 3 = hardest of type)
	150,		// Hit% #1
	7,			// Hit check frame #1
	40, 50,	// Min, Max damage #1
	150,		// Hit% #2
	10,			// Hit check frame #2
	40, 50,		// Min, Max damage #2
	125,		// Armor Class
	MC_DEMON,	// Monster Class
	M_RL+M_IF+M_IM+M_II,// Magic resist
	M_IL+M_IF+M_IM+M_II,// Magic resist2
	0,			// Treasure
	MSEL_BIG,	// Selection Type
	13333},		// Experience points

};

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

int MonstConvTbl[] = {

		MT_NZOMBIE,	
		MT_BZOMBIE,	
		MT_GZOMBIE,	
		MT_YZOMBIE,	

		MT_RFALLSP,	
		MT_DFALLSP,	
		MT_YFALLSP,	
		MT_BFALLSP,	

		MT_WSKELAX,
		MT_TSKELAX,	
		MT_RSKELAX,	
		MT_XSKELAX,	

		MT_RFALLSD,	
		MT_DFALLSD,	
		MT_YFALLSD,	
		MT_BFALLSD,	

		MT_NSCAV,	
		MT_BSCAV,	
		MT_WSCAV,	
		MT_YSCAV,	

		MT_WSKELBW,	
		MT_TSKELBW,	
		MT_RSKELBW,	
		MT_XSKELBW,	

		MT_WSKELSD,	
		MT_TSKELSD,	
		MT_RSKELSD,	
		MT_XSKELSD,	

		//MT_INVILORD,

		MT_SNEAK,
		MT_STALKER,	
		MT_UNSEEN,	
		MT_ILLWEAV,	

		//MT_LRDSAYTR,	

		MT_NGOATMC,
		MT_BGOATMC,	
		MT_RGOATMC,	
		MT_GGOATMC,	

		MT_FIEND,	
		MT_GLOOM,	
		MT_BLINK,	
		MT_FAMILIAR,

		MT_NGOATBW,	
		MT_BGOATBW,	
		MT_RGOATBW,	
		MT_GGOATBW,	
		
		MT_NACID,	
		MT_RACID,	
		MT_BACID,	
		MT_XACID,	

		MT_SKING,	
		MT_FAT,
		MT_MUDMAN,	
		MT_TOAD,	
		MT_FLAYED,	

		MT_WYRM,	
		MT_CAVSLUG, 
		MT_DEVOUR,  
		MT_DVLWYRM,

		MT_NMAGMA,	
		MT_YMAGMA,	
		MT_BMAGMA,	
		MT_WMAGMA,	
		MT_HORNED,	
		MT_MUDRUN,	
		MT_FROSTC,	
		MT_OBLORD,	
		MT_BONEDMN, 
		MT_REDDTH, 
		MT_LTCHDMN, 
		MT_UDEDBLRG,

		0,
		0,
		0,
		0,

		MT_INCIN,	
		MT_FLAMLRD,  
		MT_DOOMFIRE,
		MT_HELLBURN,

		0,
		0,
		0,
		0,
		
		MT_RSTORM,	
		MT_STORM,	
		MT_STORML,	
		MT_MAEL,	

		MT_WINGED,	
		MT_GARGOYLE,
		MT_BLOODCLW,
		MT_DEATHW,	
		MT_MEGA,	
		MT_GUARD,   
		MT_VTEXLRD, 
		MT_BALROG, 
		MT_NSNAKE,	
		MT_RSNAKE,	

		MT_GSNAKE,	
		MT_BSNAKE,	

		MT_NBLACK,	
		MT_RTBLACK,	
		MT_BTBLACK,	
		MT_RBLACK,	
		MT_UNRAV,	
		MT_HOLOWONE,
		MT_PAINMSTR,
		MT_REALWEAV,
		MT_SUCCUBUS,
		MT_SNOWWICH, 
		MT_HLSPWN,   
		MT_SOLBRNR,  
		MT_COUNSLR,	
		MT_MAGISTR,  
		MT_CABALIST, 
		MT_ADVOCATE, 

		0,
		MT_DIABLO,
		0,
		MT_GOLEM,	
		0, //BlackWound
		0, //SinWar
		0,
		0, //Lazarus
		0, //Warlord
		0, //FleshDoom
		0, //Snotspil
		0, //Temptress
		0,  //Lachdanan
		MT_BIGFALL,	
		MT_DARKMAGE,


		MT_FORK,
		MT_SCORP,
		MT_EYE,
		MT_SPIDER,
		MT_FELLTWIN,
		MT_SPAWN,
		MT_SCORP,
		MT_EYE,
		MT_SPIDER,
		MT_LASH,
		MT_ANT,
		MT_HORKD,
		MT_BUG,
		
		MT_GRAVDG,
		MT_RAT,
		MT_HELLBAT,
		MT_BONED,
		MT_LICH,
		MT_BUBBA,
		MT_HELLBAT,
		MT_BONED,
		MT_LICH,
		MT_BYCLPS,
		MT_FLESH,
		MT_REAPER,
		MT_NKR,
};

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

int MonstAvailTbl[] = {
		MAT_SW,		// MT_NZOMBIE
		MAT_SW,		// MT_BZOMBIE
		MAT_SW,		// MT_GZOMBIE
		MAT_SW,		// MT_YZOMBIE

		MAT_SW,		// MT_RFALLSP
		MAT_SW,		// MT_DFALLSP
		MAT_SW,		// MT_YFALLSP
		MAT_SW,		// MT_BFALLSP

		MAT_SW,		// MT_WSKELAX
		MAT_SW,		// MT_TSKELAX
		MAT_SW,		// MT_RSKELAX
		MAT_SW,		// MT_XSKELAX

		MAT_SW,		// MT_RFALLSD
		MAT_SW,		// MT_DFALLSD
		MAT_SW,		// MT_YFALLSD
		MAT_SW,		// MT_BFALLSD

		MAT_SW,		// MT_NSCAV
		MAT_SW,		// MT_BSCAV
		MAT_SW,		// MT_WSCAV
		MAT_SW,		// MT_YSCAV

		MAT_SW,		// MT_WSKELBW
		MAT_SW,		// MT_TSKELBW
		MAT_SW,		// MT_RSKELBW
		MAT_SW,		// MT_XSKELBW

		MAT_SW,		// MT_WSKELSD
		MAT_SW,		// MT_TSKELSD
		MAT_SW,		// MT_RSKELSD
		MAT_SW,		// MT_XSKELSD

		MAT_NO,		// MT_INVILORD		8/15

		MAT_YES,	// MT_SNEAK
		MAT_YES,	// MT_STALKER
		MAT_YES,	// MT_UNSEEN
		MAT_YES,	// MT_ILLWEAV

		MAT_YES,	// MT_LRDSAYTR		8/15

		MAT_YES,	// MT_NGOATMC
		MAT_YES,	// MT_BGOATMC
		MAT_YES,	// MT_RGOATMC
		MAT_YES,	// MT_GGOATMC

		MAT_SW,		// MT_FIEND				
		MAT_SW,		// MT_BLINK
		MAT_SW,		// MT_GLOOM
		MAT_SW,		// MT_FAMILIAR

		MAT_YES,	// MT_NGOATBW
		MAT_YES,	// MT_BGOATBW
		MAT_YES,	// MT_RGOATBW
		MAT_YES,	// MT_GGOATBW

		MAT_YES,	// MT_NACID
		MAT_YES,	// MT_RACID
		MAT_YES,	// MT_BACID
		MAT_YES,	// MT_XACID

		MAT_NO,		// MT_SKING

		MAT_NO,		// MT_CLEAVER
		
		MAT_YES,	// MT_FAT
		MAT_YES,	// MT_MUDMAN
		MAT_YES,	// MT_TOAD
		MAT_YES,	// MT_FLAYED

		MAT_NO,		// MT_WYRM
		MAT_NO,		// MT_CAVSLUG
		MAT_NO,		// MT_DVLWYRM
		MAT_NO,		// MT_DEVOUR

		MAT_YES,	// MT_NMAGMA
		MAT_YES,	// MT_YMAGMA
		MAT_YES,	// MT_BMAGMA
		MAT_YES,	// MT_WMAGMA

		MAT_YES,	// MT_HORNED
		MAT_YES,	// MT_MUDRUN
		MAT_YES,	// MT_FROSTC
		MAT_YES,	// MT_OBLORD

		MAT_NO,		// MT_BONEDMN		8/15
		MAT_NO,		// MT_REDDTH
		MAT_NO,		// MT_LTCHDMN
		MAT_NO,		// MT_UDEDBLRG

		MAT_NO,	// MT_INCIN			8/15
		MAT_NO,	// MT_FLAMLRD		8/15
		MAT_NO,	// MT_DOOMFIRE		8/15
		MAT_NO,	// MT_HELLBURN		8/15

		MAT_YES,	// MT_STORM
		MAT_YES,	// MT_RSTORM
		MAT_YES,	// MT_STORML
		MAT_YES,	// MT_MAEL

		MAT_YES,	// MT_BIGFALL		8/15

		MAT_YES,	// MT_WINGED
		MAT_YES,	// MT_GARGOYLE
		MAT_YES,	// MT_BLOODCLW
		MAT_YES,	// MT_DEATHW

		MAT_YES,	// MT_MEGA
		MAT_YES,	// MT_GUARD
		MAT_YES,	// MT_VTEXLRD
		MAT_YES,	// MT_BALROG

		MAT_YES,	// MT_NSNAKE
		MAT_YES,	// MT_RSNAKE
		MAT_YES,	// MT_BSNAKE
		MAT_YES,	// MT_GSNAKE

		MAT_YES,	// MT_NBLACK
		MAT_YES,	// MT_RTBLACK
		MAT_YES,	// MT_BTBLACK
		MAT_YES,	// MT_RBLACK

		MAT_YES,	// MT_UNRAV
		MAT_NO,		// MT_HOLOWONE
		MAT_NO,		// MT_PAINMSTR
		MAT_NO,		// MT_REALWEAV

		MAT_YES,	// MT_SUCCUBUS
		MAT_YES,	// MT_SNOWICH
		MAT_YES,	// MT_HLSPWN
		MAT_YES,	// MT_SOLBURNR

		MAT_YES,	// MT_COUNSLR
		MAT_YES,	// MT_MAGISTR
		MAT_YES,	// MT_CABALIST
		MAT_YES,	// MT_ADVOCATE

		MAT_NO,		// MT_GOLEM

		MAT_NO,		// MT_DIABLO
		
		MAT_NO,	// MT_DARKMAGE		8/15
		
		MAT_YES,	// MT_FORK
		MAT_YES,	// MT_SCORP
		MAT_YES,	// MT_EYE
		MAT_YES,	// MT_SPIDER
		MAT_YES,	// MT_INVILORD
		MAT_YES,	// MT_SPAWN
		MAT_YES,	// MT_SCORP2
		MAT_YES,	// MT_EYE2
		MAT_YES,	// MT_SPIDER2
		MAT_YES,	// MT_LASH
		MAT_YES,	// MT_ANT
		MAT_NO,		// MT_HORKD
		MAT_NO,		// MT_BUG
		
		MAT_YES,	// MT_GRAVDG
		MAT_YES,	// MT_RAT
		MAT_YES,	// MT_HELLBAT
		MAT_YES,	// MT_BONED
		MAT_YES,	// MT_LICH
		MAT_YES,	// MT_BUBBA
		MAT_YES,	// MT_HELLBAT2
		MAT_YES,	// MT_BONED2
		MAT_YES,	// MT_LICH2
		MAT_YES,	// MT_BYCLPS
		MAT_YES,	// MT_FLESH
		MAT_YES,	// MT_REAPER
		MAT_NO,		// MT_NKR
};

UniqMonstStruct UniqMonst[] =
{
#if IS_VERSION(RETAIL)
	// The first few are quest monsters and must go in this order for indexing
	{
	MT_NGOATMC,				// i.e. MT_SKELSD
	"Gharbad the Weak",		// Monster Name
	"BSDB",					// Translation filename
	4,						// level on which it appears
	120,					// Hit Points
	AI_GARBUD,				// AI type
	3,						// AI level
	8,						// min damage
	16,						// max damage
	M_IL+M_II,				// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	TXT_GARB1				// Talking monster message number
	},
	
	{
	MT_SKING,				// i.e. MT_SKELSD
	"Skeleton King",		// Monster Name
	"GENRL",				// Translation filename
	0,						// level on which it appears
	240,					// Hit Points
	AI_SKELKING,			// AI type
	3,						// AI level
	6,						// min damage
	16,						// max damage
	M_RF+M_RL+M_IM+M_II,	// Magic resistance
	UN_PACK,				// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_COUNSLR,				// i.e. MT_SKELSD				***	Has to be the first level 8 monster loaded ***
	"Zhar the Mad",			// Monster Name
	"GENERAL",				// Translation filename
	8,						// level on which it appears
	360,					// Hit Points
	AI_ZHAR,				// AI type
	3,						// AI level
	16,						// min damage
	40,						// max damage
	M_RF+M_RL+M_IM,			// Magic resistance
	0,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	TXT_ZHAR1				// Talking monster message number
	},

	{
	MT_BFALLSP,				// i.e. MT_SKELSD
	"Snotspill",			// Monster Name
	"BNG",					// Translation filename
	4,						// level on which it appears
	220,					// Hit Points
	AI_SNOTSPIL,			// AI type
	3,						// AI level
	10,						// min damage
	18,						// max damage
	M_RL,					// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	TXT_BOL1				// Talking monster message number
	},

	{
	MT_ADVOCATE,			// i.e. MT_SKELSD
	"Arch-Bishop Lazarus",	// Monster Name
	"GENERAL",				// Translation filename
	0,						// level on which it appears
	600,					// Hit Points
	AI_LAZURUS,				// AI type
	3,						// AI level
	30,						// min damage
	50,						// max damage
	M_RF+M_RL+M_IM+M_II,	// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	TXT_VB1					// Talking monster message number
	},

	{
	MT_HLSPWN,				// i.e. MT_SKELSD
	"Red Vex",				// Monster Name
	"REDV",					// Translation filename
	0,						// level on which it appears
	400,					// Hit Points
	AI_LAZHELP,				// AI type
	3,						// AI level
	30,						// min damage
	50,						// max damage
	M_RF+M_IM+M_II,			// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	TXT_VB1					// Talking monster message number
	},

	{
	MT_HLSPWN,				// i.e. MT_SKELSD
	"BlackJade",			// Monster Name
	"BLKJD",					// Translation filename
	0,						// level on which it appears
	400,					// Hit Points
	AI_LAZHELP,				// AI type
	3,						// AI level
	30,						// min damage
	50,						// max damage
	M_RL+M_IM+M_II,			// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	TXT_VB1					// Talking monster message number
	},

	{
	MT_RBLACK,				// i.e. MT_SKELSD
	"Lachdanan",			// Monster Name
	"BHKA",					// Translation filename
	14,						// level on which it appears
	500,					// Hit Points
	AI_LACHDANAN,			// AI type
	3,						// AI level
	0,						// min damage
	0,						// max damage
	0,						// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	TXT_VEIL1				// Talking monster message number
	},

	{
	MT_BTBLACK,				// i.e. MT_SKELSD
	"Warlord of Blood",		// Monster Name
	"GENERAL",				// Translation filename
	13,						// level on which it appears
	850,					// Hit Points
	AI_WARLORD,				// AI type
	3,						// AI level
	35,						// min damage
	50,						// max damage
	M_IF+M_IL+M_IM+M_II,	// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	TXT_WARLRD1				// Talking monster message number
	},

	{
	MT_CLEAVER,				// i.e. MT_SKELSD
	"The Butcher",			// Monster Name
	"GENRL",				// Translation filename
	0,						// level on which it appears
	220,					// Hit Points
	AI_CLEAVER,				// AI type
	3,						// AI level
	6,						// min damage
	12,						// max damage
	M_RF+M_RL+M_II,			// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_HORKD,				// i.e. MT_SKELSD
	"Hork Demon",			// Monster Name
	"GENRL",				// Translation filename
	19,						// level on which it appears
	300,					// Hit Points
	AI_HORKDEMON,			// AI type
	3,						// AI level
	20,						// min damage
	35,						// max damage
	M_RL,					// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_BUG,					// i.e. MT_SKELSD
	"The Defiler",			// Monster Name
	"GENRL",				// Translation filename
	20,						// level on which it appears
	480,					// Hit Points
	AI_SKELSD,				// AI type
	3,						// AI level
	30,						// min damage
	40,						// max damage
	M_RF+M_RM+M_IL,			// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_NKR,					// i.e. MT_SKELSD
	"Na-Krul",			// Monster Name
	"GENRL",				// Translation filename
	0,						// level on which it appears
	1332,					// Hit Points
	AI_SKELSD,				// AI type
	3,						// AI level
	40,						// min damage
	50,						// max damage
	M_IL+M_IF+M_IM+M_II,	// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	// From here down are random unique monsters
	{
	MT_TSKELAX,				// i.e. MT_SKELSD
	"Bonehead Keenaxe",		// Monster Name
	"BHKA",					// Translation filename
	2,						// level on which it appears
	91,						// Hit Points
	AI_SKELSD,				// AI type
	2,						// AI level
	4,						// min damage
	10,						// max damage
	M_II+M_IM,				// Magic resistance
	UN_L|UN_H,				// Unique attributes, i.e., pack leader
	100,					// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_RFALLSD,				// i.e. MT_SKELSD
	"Bladeskin the Slasher",// Monster Name
	"BSTS",					// Translation filename
	2,						// level on which it appears
	51,						// Hit Points
	AI_FALLEN,				// AI type
	0,						// AI level
	6,						// min damage
	18,						// max damage
	M_RF,					// Magic resistance
	UN_L|UN_A,				// Unique attributes, i.e., pack leader
	45,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_NZOMBIE,				// i.e. MT_SKELSD
	"Soulpus",				// Monster Name
	"GENERAL",				// Translation filename
	2,						// level on which it appears
	133,					// Hit Points
	AI_ZOMBIE,				// AI type
	0,						// AI level
	4,						// min damage
	8,						// max damage
	M_RF+M_RL,				// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_RFALLSP,				// i.e. MT_SKELSD
	"Pukerat the Unclean",	// Monster Name
	"PTU",					// Translation filename
	2,						// level on which it appears
	77,						// Hit Points
	AI_FALLEN,				// AI type
	3,						// AI level
	1,						// min damage
	5,						// max damage
	M_RF,					// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_WSKELAX,				// i.e. MT_SKELSD
	"Boneripper",			// Monster Name
	"BR",					// Translation filename
	2,						// level on which it appears
	54,						// Hit Points
	AI_BAT,					// AI type
	0,						// AI level
	6,						// min damage
	15,						// max damage
	M_IF+M_II+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_NZOMBIE,				// i.e. MT_SKELSD
	"Rotfeast the Hungry",	// Monster Name
	"ETH",					// Translation filename
	2,						// level on which it appears
	85,						// Hit Points
	AI_SKELSD,				// AI type
	3,						// AI level
	4,						// min damage
	12,						// max damage
	M_II+M_IM,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_DFALLSD,				// i.e. MT_SKELSD
	"Gutshank the Quick",	// Monster Name
	"GTQ",					// Translation filename
	3,						// level on which it appears
	66,						// Hit Points
	AI_BAT,					// AI type
	2,						// AI level
	6,						// min damage
	16,						// max damage
	M_RF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_TSKELSD,				// i.e. MT_SKELSD
	"Brokenhead Bangshield",	// Monster Name
	"BHBS",					// Translation filename
	3,						// level on which it appears
	108,					// Hit Points
	AI_SKELSD,				// AI type
	3,						// AI level
	12,						// min damage
	20,						// max damage
	M_RL+M_II+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_YFALLSP,				// i.e. MT_SKELSD
	"Bongo",				// Monster Name
	"BNG",					// Translation filename
	3,						// level on which it appears
	178,					// Hit Points
	AI_FALLEN,				// AI type
	3,						// AI level
	9,						// min damage
	21,						// max damage
	M_NONE,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_BZOMBIE,				// i.e. MT_SKELSD
	"Rotcarnage",			// Monster Name
	"RCRN",					// Translation filename
	3,						// level on which it appears
	102,					// Hit Points
	AI_ZOMBIE,				// AI type
	3,						// AI level
	9,						// min damage
	24,						// max damage
	M_RL+M_II+M_IM,			// Magic resistance
	UN_L|UN_A,				// Unique attributes, i.e., pack leader
	45,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_NSCAV,				// i.e. MT_SKELSD
	"Shadowbite",			// Monster Name
	"SHBT",					// Translation filename
	2,						// level on which it appears
	60,						// Hit Points
	AI_SKELSD,				// AI type
	3,						// AI level
	3,						// min damage
	20,						// max damage
	M_IF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_WSKELBW,				// i.e. MT_SKELSD
	"Deadeye",				// Monster Name
	"DE",					// Translation filename
	2,						// level on which it appears
	49,						// Hit Points
	AI_GOATBOW,				// AI type
	0,						// AI level
	6,						// min damage
	9,						// max damage
	M_RF+M_II+M_IM,			// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_RSKELAX,				// i.e. MT_SKELSD
	"Madeye the Dead",		// Monster Name
	"MTD",					// Translation filename
	4,						// level on which it appears
	75,						// Hit Points
	AI_BAT,					// AI type
	0,						// AI level
	9,						// min damage
	21,						// max damage
	M_IF+M_IM,				// Magic resistance
	UN_L|UN_A,				// Unique attributes, i.e., pack leader
	30,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_BSCAV,				// i.e. MT_SKELSD
	"El Chupacabras",		// Monster Name
	"GENERAL",				// Translation filename
	3,						// level on which it appears
	120,					// Hit Points
	AI_GOATMC,				// AI type
	0,						// AI level
	10,						// min damage
	18,						// max damage
	M_RF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	30,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_TSKELBW,				// i.e. MT_SKELSD
	"Skullfire",			// Monster Name
	"SKFR",					// Translation filename
	3,						// level on which it appears
	125,					// Hit Points
	AI_GOATBOW,				// AI type
	1,						// AI level
	6,						// min damage
	10,						// max damage
	M_IF,					// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	100,					// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_SNEAK,				// i.e. MT_SKELSD
	"Warpskull",			// Monster Name
	"TSPO",					// Translation filename
	3,						// level on which it appears
	117,					// Hit Points
	AI_SNEAK,				// AI type
	2,						// AI level
	6,						// min damage
	18,						// max damage
	M_RF+M_RL,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_GZOMBIE,				// i.e. MT_SKELSD
	"Goretongue",			// Monster Name
	"PMR",					// Translation filename
	3,						// level on which it appears
	156,					// Hit Points
	AI_SKELSD,				// AI type
	1,						// AI level
	15,						// min damage
	30,						// max damage
	M_II+M_IM,				// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_WSCAV,				// i.e. MT_SKELSD
	"Pulsecrawler",			// Monster Name
	"BHKA",					// Translation filename
	4,						// level on which it appears
	150,					// Hit Points
	AI_SCAV,				// AI type
	0,						// AI level
	16,						// min damage
	20,						// max damage
	M_RL+M_IF,				// Magic resistance
	UN_L|UN_A,				// Unique attributes, i.e., pack leader
	45,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_BLINK,				// i.e. MT_SKELSD
	"Moonbender",			// Monster Name
	"GENERAL",				// Translation filename
	4,						// level on which it appears
	135,					// Hit Points
	AI_BAT,					// AI type
	0,						// AI level
	9,						// min damage
	27,						// max damage
	M_IF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_BLINK,				// i.e. MT_SKELSD
	"Wrathraven",			// Monster Name
	"GENERAL",				// Translation filename
	5,						// level on which it appears
	135,					// Hit Points
	AI_BAT,					// AI type
	2,						// AI level
	9,						// min damage
	22,						// max damage
	M_IF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_YSCAV,				// i.e. MT_SKELSD
	"Spineeater",			// Monster Name
	"GENERAL",				// Translation filename
	4,						// level on which it appears
	180,					// Hit Points
	AI_SCAV,				// AI type
	1,						// AI level
	18,						// min damage
	25,						// max damage
	M_IL+M_II,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_RSKELBW,				// i.e. MT_SKELSD
	"Blackash the Burning",	// Monster Name
	"BASHTB",				// Translation filename
	4,						// level on which it appears
	120,					// Hit Points
	AI_GOATBOW,				// AI type
	0,						// AI level
	6,						// min damage
	16,						// max damage
	M_IF+M_IM,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_BFALLSD,				// i.e. MT_SKELSD
	"Shadowcrow",			// Monster Name
	"GENERAL",					// Translation filename
	5,						// level on which it appears
	270,					// Hit Points
	AI_SNEAK,				// AI type
	2,						// AI level
	12,						// min damage
	25,						// max damage
	M_NONE,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_LRDSAYTR,			// i.e. MT_SKELSD
	"Blightstone the Weak",	// Monster Name
	"BHKA",					// Translation filename
	4,						// level on which it appears
	360,					// Hit Points
	AI_SKELSD,				// AI type
	0,						// AI level
	4,						// min damage
	12,						// max damage
	M_RL+M_IM,				// Magic resistance
	UN_L|UN_H,				// Unique attributes, i.e., pack leader
	70,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_FAT,					// i.e. MT_SKELSD
	"Bilefroth the Pit Master",		// Monster Name
	"BFTP",					// Translation filename
	6,						// level on which it appears
	210,					// Hit Points
	AI_BAT,					// AI type
	1,						// AI level
	16,						// min damage
	23,						// max damage
	M_RL+M_IM+M_IF,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_NGOATBW,				// i.e. MT_SKELSD
	"Bloodskin Darkbow",	// Monster Name
	"BSDB",					// Translation filename
	5,						// level on which it appears
	207,					// Hit Points
	AI_GOATBOW,				// AI type
	0,						// AI level
	3,						// min damage
	16,						// max damage
	M_RF+M_RL,				// Magic resistance
	UN_L|UN_A,				// Unique attributes, i.e., pack leader
	55,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_GLOOM,				// i.e. MT_SKELSD
	"Foulwing",				// Monster Name
	"DB",					// Translation filename
	5,						// level on which it appears
	246,					// Hit Points
	AI_RHINO,				// AI type
	3,						// AI level
	12,						// min damage
	28,						// max damage
	M_RF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_XSKELSD,				// i.e. MT_SKELSD
	"Shadowdrinker",		// Monster Name
	"SHDR",					// Translation filename
	5,						// level on which it appears
	300,					// Hit Points
	AI_SNEAK,				// AI type
	1,						// AI level
	18,						// min damage
	26,						// max damage
	M_RF+M_RL+M_II+M_IM,	// Magic resistance
	UN_A,					// Unique attributes, i.e., pack leader
	45,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_UNSEEN,				// i.e. MT_SKELSD
	"Hazeshifter",			// Monster Name
	"BHKA",					// Translation filename
	5,						// level on which it appears
	285,					// Hit Points
	AI_SNEAK,				// AI type
	3,						// AI level
	18,						// min damage
	30,						// max damage
	M_IL+M_II,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_NACID,				// i.e. MT_SKELSD
	"Deathspit",			// Monster Name
	"BFDS",					// Translation filename
	6,						// level on which it appears
	303,					// Hit Points
	AI_ACIDUNIQ,			// AI type
	0,						// AI level
	12,						// min damage
	32,						// max damage
	M_RL+M_RF,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_RGOATMC,				// i.e. MT_SKELSD
	"Bloodgutter",			// Monster Name
	"BGBL",					// Translation filename
	6,						// level on which it appears
	315,					// Hit Points
	AI_BAT,					// AI type
	1,						// AI level
	24,						// min damage
	34,						// max damage
	M_IF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_BGOATMC,				// i.e. MT_SKELSD
	"Deathshade Fleshmaul",	// Monster Name
	"DSFM",					// Translation filename
	6,						// level on which it appears
	276,					// Hit Points
	AI_RHINO,				// AI type
	0,						// AI level
	12,						// min damage
	24,						// max damage
	M_RF+M_IM,				// Magic resistance
	UN_A,					// Unique attributes, i.e., pack leader
	65,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_WYRM,				// i.e. MT_SKELSD
	"Warmaggot the Mad",	// Monster Name
	"GENERAL",					// Translation filename
	6,						// level on which it appears
	246,					// Hit Points
	AI_BAT,					// AI type
	3,						// AI level
	15,						// min damage
	30,						// max damage
	M_RL,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_STORM,				// i.e. MT_SKELSD
	"Glasskull the Jagged",	// Monster Name
	"BHKA",					// Translation filename
	7,						// level on which it appears
	354,					// Hit Points
	AI_STORM,				// AI type
	0,						// AI level
	18,						// min damage
	30,						// max damage
	M_IM+M_II+M_IF,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_RGOATBW,				// i.e. MT_SKELSD
	"Blightfire",			// Monster Name
	"BLF",					// Translation filename
	7,						// level on which it appears
	321,					// Hit Points
	AI_SUCC,				// AI type
	2,						// AI level
	13,						// min damage
	21,						// max damage
	M_IF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_GARGOYLE,			// i.e. MT_SKELSD
	"Nightwing the Cold",	// Monster Name
	"GENERAL",					// Translation filename
	7,						// level on which it appears
	342,					// Hit Points
	AI_BAT,					// AI type
	1,						// AI level
	18,						// min damage
	26,						// max damage
	M_RL+M_IM+M_II,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_GGOATBW,				// i.e. MT_SKELSD
	"Gorestone",			// Monster Name
	"GENERAL",				// Translation filename
	7,						// level on which it appears
	303,					// Hit Points
	AI_GOATBOW,				// AI type
	1,						// AI level
	15,						// min damage
	28,						// max damage
	M_RL+M_II,				// Magic resistance
	UN_L|UN_H,				// Unique attributes, i.e., pack leader
	70,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_BMAGMA,				// i.e. MT_SKELSD
	"Bronzefist Firestone",	// Monster Name
	"GENERAL",					// Translation filename
	8,						// level on which it appears
	360,					// Hit Points
	AI_MAGMA,				// AI type
	0,						// AI level
	30,						// min damage
	36,						// max damage
	M_RF+M_IM,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_INCIN,				// i.e. MT_SKELSD
	"Wrathfire the Doomed",	// Monster Name
	"WFTD",					// Translation filename
	8,						// level on which it appears
	270,					// Hit Points
	AI_SKELSD,				// AI type
	2,						// AI level
	20,						// min damage
	30,						// max damage
	M_RF+M_RL+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_NMAGMA,				// i.e. MT_SKELSD
	"Firewound the Grim",	// Monster Name
	"BHKA",					// Translation filename
	8,						// level on which it appears
	303,					// Hit Points
	AI_MAGMA,				// AI type
	0,						// AI level
	18,						// min damage
	22,						// max damage
	M_RF+M_IM,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_MUDMAN,				// i.e. MT_SKELSD
	"Baron Sludge",			// Monster Name
	"BSM",					// Translation filename
	8,						// level on which it appears
	315,					// Hit Points
	AI_SNEAK,				// AI type
	3,						// AI level
	25,						// min damage
	34,						// max damage
	M_RF+M_RL+M_IM+M_II,	// Magic resistance
	UN_L|UN_A,					// Unique attributes, i.e., pack leader
	75,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_GGOATMC,				// i.e. MT_SKELSD
	"Blighthorn Steelmace",	// Monster Name
	"BHSM",					// Translation filename
	7,						// level on which it appears
	250,					// Hit Points
	AI_RHINO,				// AI type
	0,						// AI level
	20,						// min damage
	28,						// max damage
	M_RL,					// Magic resistance
	UN_L|UN_A,				// Unique attributes, i.e., pack leader
	45,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_RACID,				// i.e. MT_SKELSD
	"Chaoshowler",			// Monster Name
	"GENERAL",					// Translation filename
	8,						// level on which it appears
	240,					// Hit Points
	AI_ACIDUNIQ,			// AI type
	0,						// AI level
	12,						// min damage
	20,						// max damage
	0,						// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_REDDTH,				// i.e. MT_SKELSD
	"Doomgrin the Rotting",	// Monster Name
	"GENERAL",				// Translation filename
	8,						// level on which it appears
	405,					// Hit Points
	AI_STORM,				// AI type
	3,						// AI level
	25,						// min damage
	50,						// max damage
	M_RL+M_RF+M_IM+M_II,	// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_FLAMLRD,				// i.e. MT_SKELSD
	"Madburner",			// Monster Name
	"GENERAL",				// Translation filename
	9,						// level on which it appears
	270,					// Hit Points
	AI_STORM,				// AI type
	0,						// AI level
	20,						// min damage
	40,						// max damage
	M_IF+M_IL+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_LTCHDMN,				// i.e. MT_SKELSD
	"Bonesaw the Litch",	// Monster Name
	"GENERAL",				// Translation filename
	9,						// level on which it appears
	495,					// Hit Points
	AI_STORM,				// AI type
	2,						// AI level
	30,						// min damage
	55,						// max damage
	M_RF+M_RL+M_II+M_IM,	// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_MUDRUN,				// i.e. MT_SKELSD
	"Breakspine",			// Monster Name
	"GENERAL",					// Translation filename
	9,						// level on which it appears
	351,					// Hit Points
	AI_RHINO,				// AI type
	0,						// AI level
	25,						// min damage
	34,						// max damage
	M_RF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_REDDTH,				// i.e. MT_SKELSD
	"Devilskull Sharpbone",	// Monster Name
	"GENERAL",				// Translation filename
	9,						// level on which it appears
	444,					// Hit Points
	AI_STORM,				// AI type
	1,						// AI level
	25,						// min damage
	40,						// max damage
	M_IF,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_STORM,				// i.e. MT_SKELSD
	"Brokenstorm",			// Monster Name
	"GENERAL",					// Translation filename
	9,						// level on which it appears
	411,					// Hit Points
	AI_STORM,				// AI type
	2,						// AI level
	25,						// min damage
	36,						// max damage
	M_IL,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_RSTORM,				// i.e. MT_SKELSD
	"Stormbane",			// Monster Name
	"GENERAL",					// Translation filename
	9,						// level on which it appears
	555,					// Hit Points
	AI_STORM,				// AI type
	3,						// AI level
	30,						// min damage
	30,						// max damage
	M_IL,					// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_TOAD,				// i.e. MT_SKELSD
	"Oozedrool",			// Monster Name
	"GENERAL",					// Translation filename
	9,						// level on which it appears
	483,					// Hit Points
	AI_FAT,					// AI type
	3,						// AI level
	25,						// min damage
	30,						// max damage
	M_RL,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_BLOODCLW,			// i.e. MT_SKELSD
	"Goldblight of the Flame",	// Monster Name
	"GENERAL",					// Translation filename
	10,						// level on which it appears
	405,					// Hit Points
	AI_GARG,				// AI type
	0,						// AI level
	15,						// min damage
	35,						// max damage
	M_IF+M_IM,				// Magic resistance
	UN_L|UN_A,				// Unique attributes, i.e., pack leader
	80,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_OBLORD,				// i.e. MT_SKELSD
	"Blackstorm",			// Monster Name
	"GENERAL",					// Translation filename
	10,						// level on which it appears
	525,					// Hit Points
	AI_RHINO,				// AI type
	3,						// AI level
	20,						// min damage
	40,						// max damage
	M_IL+M_IM,				// Magic resistance
	UN_L|UN_A,				// Unique attributes, i.e., pack leader
	90,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_RACID,				// i.e. MT_SKELSD
	"Plaguewrath",			// Monster Name
	"GENERAL",					// Translation filename
	10,						// level on which it appears
	450,					// Hit Points
	AI_ACIDUNIQ,			// AI type
	2,						// AI level
	20,						// min damage
	30,						// max damage
	M_RF+M_IM+M_II,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_RSTORM,				// i.e. MT_SKELSD
	"The Flayer",			// Monster Name
	"GENERAL",					// Translation filename
	10,						// level on which it appears
	501,					// Hit Points
	AI_STORM,				// AI type
	1,						// AI level
	20,						// min damage
	35,						// max damage
	M_RF+M_RM+M_IL+M_II,	// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_FROSTC,				// i.e. MT_SKELSD
	"Bluehorn",				// Monster Name
	"GENERAL",					// Translation filename
	11,						// level on which it appears
	477,					// Hit Points
	AI_RHINO,				// AI type
	1,						// AI level
	25,						// min damage
	30,						// max damage
	M_RF+M_IM,				// Magic resistance
	UN_L|UN_A,				// Unique attributes, i.e., pack leader
	90,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_HELLBURN,			// i.e. MT_SKELSD
	"Warpfire Hellspawn",	// Monster Name
	"GENERAL",				// Translation filename
	11,						// level on which it appears
	525,					// Hit Points
	AI_FIREMAN,				// AI type
	3,						// AI level
	10,						// min damage
	40,						// max damage
	M_RM+M_IF,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_NSNAKE,				// i.e. MT_SKELSD
	"Fangspeir",			// Monster Name
	"GENERAL",				// Translation filename
	11,						// level on which it appears
	444,					// Hit Points
	AI_SKELSD,				// AI type
	1,						// AI level
	15,						// min damage
	32,						// max damage
	M_IF+M_II,	// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_UDEDBLRG,				// i.e. MT_SKELSD
	"Festerskull",			// Monster Name
	"GENERAL",				// Translation filename
	11,						// level on which it appears
	600,					// Hit Points
	AI_STORM,				// AI type
	2,						// AI level
	15,						// min damage
	30,						// max damage
	M_IM+M_II,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_NBLACK,				// i.e. MT_SKELSD
	"Lionskull the Bent",	// Monster Name
	"GENERAL",					// Translation filename
	12,						// level on which it appears
	525,					// Hit Points
	AI_SKELSD,				// AI type
	2,						// AI level
	25,						// min damage
	25,						// max damage
	M_IM+M_IF+M_IL+M_II,	// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_COUNSLR,				// i.e. MT_SKELSD
	"Blacktongue",			// Monster Name
	"GENERAL",				// Translation filename
	12,						// level on which it appears
	360,					// Hit Points
	AI_COUNSLR,				// AI type
	3,						// AI level
	15,						// min damage
	30,						// max damage
	M_RF+M_II,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_DEATHW,				// i.e. MT_SKELSD
	"Viletouch",			// Monster Name
	"GENERAL",					// Translation filename
	12,						// level on which it appears
	525,					// Hit Points
	AI_GARG,				// AI type
	3,						// AI level
	20,						// min damage
	40,						// max damage
	M_IL+M_II,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_RSNAKE,				// i.e. MT_SKELSD
	"Viperflame",			// Monster Name
	"GENERAL",					// Translation filename
	12,						// level on which it appears
	570,					// Hit Points
	AI_SKELSD,				// AI type
	1,						// AI level
	25,						// min damage
	35,						// max damage
	M_RL+M_IF,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_BSNAKE,				// i.e. MT_SKELSD
	"Fangskin",				// Monster Name
	"BHKA",					// Translation filename
	14,						// level on which it appears
	681,					// Hit Points
	AI_SKELSD,				// AI type
	2,						// AI level
	15,						// min damage
	50,						// max damage
	M_RL+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_SUCCUBUS,			// i.e. MT_SKELSD
	"Witchfire the Unholy",	// Monster Name
	"GENERAL",					// Translation filename
	12,						// level on which it appears
	444,					// Hit Points
	AI_SUCC,				// AI type
	3,						// AI level
	10,						// min damage
	20,						// max damage
	M_RL+M_IF+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_BALROG,				// i.e. MT_SKELSD
	"Blackskull",			// Monster Name
	"BHKA",					// Translation filename
	13,						// level on which it appears
	750,					// Hit Points
	AI_SKELSD,				// AI type
	3,						// AI level
	25,						// min damage
	40,					// max damage
	M_RL+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_UNRAV,				// i.e. MT_SKELSD
	"Soulslash",			// Monster Name
	"GENERAL",					// Translation filename
	12,						// level on which it appears
	450,					// Hit Points
	AI_SKELSD,				// AI type
	0,						// AI level
	25,						// min damage
	25,						// max damage
	M_IM+M_II,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_VTEXLRD,				// i.e. MT_SKELSD
	"Windspawn",				// Monster Name
	"GENERAL",				// Translation filename
	12,						// level on which it appears
	711,					// Hit Points
	AI_SKELSD,				// AI type
	1,						// AI level
	35,						// min damage
	40,						// max damage
	M_IF+M_IM,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_GSNAKE,				// i.e. MT_SKELSD
	"Lord of the Pit",		// Monster Name
	"GENERAL",					// Translation filename
	13,						// level on which it appears
	762,					// Hit Points
	AI_SKELSD,				// AI type
	2,						// AI level
	25,						// min damage
	42,						// max damage
	M_RF+M_II,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_RTBLACK,				// i.e. MT_SKELSD
	"Rustweaver",			// Monster Name
	"GENERAL",					// Translation filename
	13,						// level on which it appears
	400,					// Hit Points
	AI_SKELSD,				// AI type
	3,						// AI level
	1,						// min damage
	60,						// max damage
	M_IF+M_IL+M_IM+M_II,	// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_HOLOWONE,			// i.e. MT_SKELSD
	"Howlingire the Shade",	// Monster Name
	"GENERAL",				// Translation filename
	13,						// level on which it appears
	450,					// Hit Points
	AI_SKELSD,					// AI type
	2,						// AI level
	40,						// min damage
	75,						// max damage
	M_RF+M_RL,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_MAEL,				// i.e. MT_SKELSD
	"Doomcloud",			// Monster Name
	"GENERAL",					// Translation filename
	13,						// level on which it appears
	612,					// Hit Points
	AI_STORM,				// AI type
	1,						// AI level
	1,						// min damage
	60,						// max damage
	M_RF+M_IL,				// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_PAINMSTR,					// i.e. MT_SKELSD
	"Bloodmoon Soulfire",	// Monster Name
	"GENERAL",					// Translation filename
	13,						// level on which it appears
	684,					// Hit Points
	AI_SKELSD,					// AI type
	1,						// AI level
	15,						// min damage
	40,						// max damage
	M_RF+M_RL+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_SNOWWICH,			// i.e. MT_SKELSD
	"Witchmoon",			// Monster Name
	"GENERAL",					// Translation filename
	13,						// level on which it appears
	310,					// Hit Points
	AI_SUCC,				// AI type
	3,						// AI level
	30,						// min damage
	40,						// max damage
	M_RL,					// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_VTEXLRD,				// i.e. MT_SKELSD
	"Gorefeast",			// Monster Name
	"GENERAL",					// Translation filename
	13,						// level on which it appears
	771,					// Hit Points
	AI_SKELSD,				// AI type
	3,						// AI level
	20,						// min damage
	55,						// max damage
	M_RF+M_II,			// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_RTBLACK,				// i.e. MT_SKELSD
	"Graywar the Slayer",	// Monster Name
	"GENERAL",					// Translation filename
	14,						// level on which it appears
	672,					// Hit Points
	AI_SKELSD,				// AI type
	1,						// AI level
	30,						// min damage
	50,						// max damage
	M_RL+M_II,				// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_MAGISTR,				// i.e. MT_SKELSD
	"Dreadjudge",			// Monster Name
	"GENERAL",				// Translation filename
	14,						// level on which it appears
	540,					// Hit Points
	AI_COUNSLR,				// AI type
	1,						// AI level
	30,						// min damage
	40,						// max damage
	M_RF+M_RL+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_HLSPWN,				// i.e. MT_SKELSD
	"Stareye the Witch",	// Monster Name
	"GENERAL",					// Translation filename
	14,						// level on which it appears
	726,					// Hit Points
	AI_SUCC,				// AI type
	2,						// AI level
	30,						// min damage
	50,						// max damage
	M_IF,					// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_BTBLACK,				// i.e. MT_SKELSD
	"Steelskull the Hunter",	// Monster Name
	"GENERAL",					// Translation filename
	14,						// level on which it appears
	831,					// Hit Points
	AI_SKELSD,				// AI type
	3,						// AI level
	40,						// min damage
	50,						// max damage
	M_RL+M_II,				// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_RBLACK,				// i.e. MT_SKELSD
	"Sir Gorash",			// Monster Name
	"GENERAL",					// Translation filename
	16,						// level on which it appears
	1050,					// Hit Points
	AI_SKELSD,				// AI type
	1,						// AI level
	20,						// min damage
	60,						// max damage
	M_II,					// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_CABALIST,					// i.e. MT_SKELSD
	"The Vizier",			// Monster Name
	"GENERAL",				// Translation filename
	15,						// level on which it appears
	850,					// Hit Points
	AI_COUNSLR,					// AI type
	2,						// AI level
	25,						// min damage
	40,						// max damage
	M_IF,				// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_REALWEAV,					// i.e. MT_SKELSD
	"Zamphir",				// Monster Name
	"GENERAL",					// Translation filename
	15,						// level on which it appears
	891,					// Hit Points
	AI_SKELSD,					// AI type
	2,						// AI level
	30,						// min damage
	50,						// max damage
	M_RF+M_RL+M_II+M_IM,	// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_HLSPWN,				// i.e. MT_SKELSD
	"Bloodlust",			// Monster Name
	"GENERAL",					// Translation filename
	15,						// level on which it appears
	825,					// Hit Points
	AI_SUCC,				// AI type
	1,						// AI level
	20,						// min damage
	55,						// max damage
	M_IL+M_IM+M_II,			// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_HLSPWN,				// i.e. MT_SKELSD
	"Webwidow",				// Monster Name
	"GENERAL",					// Translation filename
	16,						// level on which it appears
	774,					// Hit Points
	AI_SUCC,				// AI type
	1,						// AI level
	20,						// min damage
	50,						// max damage
	M_IF+M_IM+M_II,			// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
	
	{
	MT_SOLBRNR,				// i.e. MT_SKELSD
	"Fleshdancer",			// Monster Name
	"GENERAL",					// Translation filename
	16,						// level on which it appears
	999,					// Hit Points
	AI_SUCC,				// AI type
	3,						// AI level
	30,						// min damage
	50,						// max damage
	M_RF+M_II+M_IM,			// Magic resistance
	0,						// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_OBLORD,				// i.e. MT_SKELSD
	"Grimspike",			// Monster Name
	"GENERAL",				// Translation filename
	19,						// level on which it appears
	534,					// Hit Points
	AI_SNEAK,					// AI type
	1,						// AI level
	25,						// min damage
	40,						// max damage
	M_RF+M_II+M_IM,			// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},

	{
	MT_STORML,				// i.e. MT_SKELSD
	"Doomlock",				// Monster Name
	"GENERAL",				// Translation filename
	28,						// level on which it appears
	534,					// Hit Points
	AI_SNEAK,				// AI type
	1,						// AI level
	35,						// min damage
	55,						// max damage
	M_RF+M_RL+M_IM+M_II,	// Magic resistance
	UN_L,					// Unique attributes, i.e., pack leader
	0,						// parameter to unique attribute
	0,						// "
	0						// Talking monster message number
	},
#endif

	{ -1, NULL, NULL, 0, 0, 0, 0, 0, 0, 0 }		// Stopper

};

