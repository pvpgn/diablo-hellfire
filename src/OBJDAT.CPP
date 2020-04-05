/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Objects file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/OBJDAT.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"objects.h"
#include	"objdat.h"
#include	"quests.h"

/*-----------------------------------------------------------------------*/

int ObjTypeConv[] = {
		0,
		OBJ_LEVER,
		OBJ_CRUX1,
		OBJ_CRUX2,
		OBJ_CRUX3,
		OBJ_ANGEL,
		OBJ_BANNERL,
		OBJ_BANNERM,
		OBJ_BANNERR,
		OBJ_BLADEL,
		OBJ_BLADER,
		OBJ_BLOCK,
		OBJ_BOOK1L,
		OBJ_BOOK1R,
		OBJ_BOOK2L,
		OBJ_BOOK2R,
		OBJ_BCROSS,
		OBJ_CANBRA,
		OBJ_CANDLE1,
		OBJ_CANDLE2,
		OBJ_CANDLEO,
		OBJ_CAULDRON,
		OBJ_CHAINEL,
		OBJ_CHAINER,
		OBJ_CHAING1L,
		OBJ_CHAING1R,
		OBJ_CHAING2L,
		OBJ_CHAING2R,
		OBJ_CHAINLL,
		OBJ_CHAINLR,
		OBJ_FLAMEHOLE,
		OBJ_FTNROUND,
		OBJ_FTNL,
		OBJ_FTNR,
		OBJ_GATEL,
		OBJ_GATER,
		OBJ_MCIRCLE1,
		OBJ_MCIRCLE2,
		OBJ_SKFIRE,
		OBJ_SKPILE,
		OBJ_SKSTICK1,
		OBJ_SKSTICK2,
		OBJ_SKSTICK3,
		OBJ_SKSTICK4,
		OBJ_SKSTICK5,
		OBJ_SPKTRAP,
		OBJ_STEAMTRAP,
		OBJ_SWITCHML,
		OBJ_SWITCHMR,
		OBJ_SWITCHRL,
		OBJ_SWITCHRR,
		OBJ_SWITCHSKL,
		OBJ_SWITCHSKR,
		OBJ_TRAPL,
		OBJ_TRAPR,
		OBJ_TORTURE1,
		OBJ_TORTURE2,
		OBJ_TORTURE3,
		OBJ_TORTURE4,
		OBJ_TORTURE5,
		OBJ_WATERJUG,
		OBJ_TORTURE6R,
		OBJ_NUDEW1L,
		OBJ_NUDEW1R,
		OBJ_NUDEW2L,
		OBJ_NUDEW2R,
		OBJ_NUDEW8,
		OBJ_NUDEMAN1L,
		OBJ_NUDEMAN1R,
		OBJ_NUDEMAN3,
		OBJ_TNUDEM1,
		OBJ_TNUDEM2,
		OBJ_TNUDEM3,
		OBJ_TNUDEM4,
		OBJ_TNUDEW1,
		OBJ_TNUDEW2,
		OBJ_TNUDEW3,
		OBJ_CHEST1,
		OBJ_CHEST1,
		OBJ_CHEST1,
		OBJ_CHEST2,
		OBJ_CHEST2,
		OBJ_CHEST2,
		OBJ_CHEST3,
		OBJ_CHEST3,
		OBJ_CHEST3,
		OBJ_DEADSKL,
		OBJ_INVPLATE,
		OBJ_WOODPLATE,
		OBJ_MTLPLATE,
		OBJ_STNPLATE,
		OBJ_PEDISTAL,
		OBJ_ANVIL,
		OBJ_FORGE,
		OBJ_BIGROCK,
		OBJ_ROCK1,
		OBJ_ROCK2,
		OBJ_ROCK3,
		OBJ_ROCK4,
		OBJ_STALAG1,
		OBJ_STALAG2,
		OBJ_STALAG3,
		OBJ_STALAG4,
		OBJ_STALAG5,
		OBJ_ALTGIRL,
		OBJ_ALTBOY,
		OBJ_TORCH,
		OBJ_VILEPORT,
		OBJ_WARARMOR,		
		OBJ_WARWEAP,
		OBJ_TORCHR2,
		OBJ_TORCHL2,
		OBJ_MUSHPATCH,
};

/*-----------------------------------------------------------------------*/

// Random object, file list index, min level, max level, leveltype, theme number, quest number
// Animates, Anim Delay or Frame, Anim Length, Anim Width, Solid, Missile, Draw with Light, Breakable, Selectable, Trapable
ObjDataStruct AllObjects[] = {
		{ OBJMUST, 0, 1, 4, 1, -1, -1,		// OBJ_L1LIGHT
		TRUE, 1, 26, 64, TRUE, TRUE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 1, 1, 4, 1, -1, -1,		// OBJ_L1DOORL
		FALSE, 1, 0, 64, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, TRUE },

		{ OBJMUST, 1, 1, 4, 1, -1, -1,		// OBJ_L1DOORR
		FALSE, 2, 0, 64, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, TRUE },

		{ OBJTHEME, 7, 0, 0, 0, 3, -1,		// OBJ_SKFIRE
		TRUE, 2, 11, 96, TRUE, TRUE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 2, 1, 4, 1, -1, -1,		// OBJ_LEVER
		FALSE, 1, 1, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

		{ OBJMUST, 3, 1, 16, 0, -1, -1,		// OBJ_CHEST1
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

		{ OBJMUST, 4, 1, 16, 0, -1, -1,		// OBJ_CHEST2
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

		{ OBJMUST, 16, 1, 16, 0, -1, -1,	// OBJ_CHEST3
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

		{ OBJNO, 0, 0, 0, 0, -1, -1,		// OBJ_CANDLE1
		FALSE, 0, 0, 0, FALSE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJTHEME, 18, 0, 0, 0, 1, -1,		// OBJ_CANDLE2
		TRUE, 2, 4, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 0, 0, 0, 0, -1, -1,		// OBJ_CANDLEO
		FALSE, 0, 0, 0, FALSE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJTHEME, 5, 0, 0, 0, 3, -1,		// OBJ_BANNERL
		FALSE, 2, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJTHEME, 5, 0, 0, 0, 3, -1,		// OBJ_BANNERM
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJTHEME, 5, 0, 0, 0, 3, -1,		// OBJ_BANNERR
		FALSE, 3, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 6, 1, 4, 0, -1, -1,		// OBJ_SKPILE
		FALSE, 0, 1, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 0, 0, 0, 0, -1, -1,		// OBJ_SKSTICK1
		FALSE, 0, 0, 0, FALSE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 0, 0, 0, 0, -1, -1,		// OBJ_SKSTICK2
		FALSE, 0, 0, 0, FALSE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 0, 0, 0, 0, -1, -1,		// OBJ_SKSTICK3
		FALSE, 0, 0, 0, FALSE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 0, 0, 0, 0, -1, -1,		// OBJ_SKSTICK4
		FALSE, 0, 0, 0, FALSE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 0, 0, 0, 0, -1, -1,		// OBJ_SKSTICK5
		FALSE, 0, 0, 0, FALSE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 9, 0, 0, 0, -1, -1,		// OBJ_CRUX1
		FALSE, 1, 15, 96, TRUE, FALSE, TRUE, OBJ_BREAKABLE, OSEL_ALL, FALSE },

		{ OBJNO, 10, 0, 0, 0, -1, -1,		// OBJ_CRUX2
		FALSE, 1, 15, 96, TRUE, FALSE, TRUE, OBJ_BREAKABLE, OSEL_ALL, FALSE },

		{ OBJNO, 11, 0, 0, 0, -1, -1,		// OBJ_CRUX3
		FALSE, 1, 15, 96, TRUE, FALSE, TRUE, OBJ_BREAKABLE, OSEL_ALL, FALSE },

		{ OBJMUST, 14, 5, 5, 0, -1, -1,		// OBJ_STAND
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 15, 0, 0, 0, -1, -1,		// OBJ_ANGEL
		FALSE, 1, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 13, 0, 0, 0, -1, -1,		// OBJ_BOOK2L
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJNO, 17, 0, 0, 0, -1, -1,		// OBJ_BCROSS
		TRUE, 0, 10, 160, TRUE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 19, 0, 0, 0, -1, -1,		// OBJ_NUDEW2R
		TRUE, 3, 6, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 20, 16, 16, 0, -1, -1,		// OBJ_SWITCHSKL
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

		{ OBJMUST, 21, 13, 16, 0, -1, Q_BUTCHER,		// OBJ_TNUDEM1
		FALSE, 1, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 21, 13, 16, 0, 6, Q_BUTCHER,		// OBJ_TNUDEM2
		FALSE, 2, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 21, 13, 16, 0, 6, Q_BUTCHER,		// OBJ_TNUDEM3
		FALSE, 3, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 21, 13, 16, 0, 6, Q_BUTCHER,		// OBJ_TNUDEM4
		FALSE, 4, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 22, 13, 16, 0, 6, Q_BUTCHER,		// OBJ_TNUDEW1
		FALSE, 1, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 22, 13, 16, 0, 6, Q_BUTCHER,		// OBJ_TNUDEW2
		FALSE, 2, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 22, 13, 16, 0, 6, Q_BUTCHER,		// OBJ_TNUDEW3
		FALSE, 3, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 23, 13, 16, 0, -1, Q_BUTCHER,		// OBJ_TORTURE1
		FALSE, 1, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 23, 13, 16, 0, -1, Q_BUTCHER,		// OBJ_TORTURE2
		FALSE, 2, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 23, 13, 16, 0, -1, Q_BUTCHER,		// OBJ_TORTURE3
		FALSE, 3, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 23, 13, 16, 0, -1, Q_BUTCHER,		// OBJ_TORTURE4
		FALSE, 4, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 23, 13, 16, 0, -1, Q_BUTCHER,		// OBJ_TORTURE5
		FALSE, 5, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 13, 6, 6, 0, -1, -1,		// OBJ_BOOK2R
		FALSE, 4, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 24, 5, 8, 2, -1, -1,		// OBJ_L2DOORL
		FALSE, 1, 0, 64, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, TRUE },

		{ OBJMUST, 24, 5, 8, 2, -1, -1,		// OBJ_L2DOORR
		FALSE, 2, 0, 64, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, TRUE },

		{ OBJMUST, 25, 5, 8, 2, -1, -1,		// OBJ_TORCHL
		TRUE, 1, 9, 96, FALSE, TRUE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 26, 5, 8, 2, -1, -1,		// OBJ_TORCHR
		TRUE, 1, 9, 96, FALSE, TRUE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 33, 5, 8, 2, -1, -1,		// OBJ_TORCHL2
		TRUE, 1, 9, 96, FALSE, TRUE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 32, 5, 8, 2, -1, -1,		// OBJ_TORCHR2
		TRUE, 1, 9, 96, FALSE, TRUE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 27, 1, 4, 1, -1, -1,		// OBJ_SARC
		FALSE, 1, 5, 128, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, TRUE },

		{ OBJNO, 28, 1, 4, 1, -1, -1,		// OBJ_FLAMEHOLE
		FALSE, 1, 20, 96, FALSE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 2, 1, 4, 1, -1, -1,		// OBJ_FLAMELVR
		FALSE, 1, 2, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

		{ OBJNO, 31, 1, 4, 1, -1, -1,		// OBJ_WATER
		TRUE, 1, 10, 64, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 12, 3, 4, 1, -1, -1,		// OBJ_BOOKLVR
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 30, 1, 16, 0, -1, -1,	// OBJ_TRAPL
		FALSE, 1, 0, 64, FALSE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 30, 1, 16, 0, -1, -1,	// OBJ_TRAPR
		FALSE, 2, 0, 64, FALSE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 34, 0, 0, 0, -1, -1,		// OBJ_BOOKSHELF
		FALSE, 1, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 36, 0, 0, 0, -1, -1,		// OBJ_WEAPRACK
		FALSE, 1, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 37, 1, 16, 0, -1, -1,		// OBJ_BARREL
		FALSE, 1, 9, 96, TRUE, TRUE, TRUE, OBJ_BREAKABLE, OSEL_ALL, FALSE },

		{ OBJMUST, 38, 1, 16, 0, -1, -1,		// OBJ_BARRELEX
		FALSE, 1, 10, 96, TRUE, TRUE, TRUE, OBJ_BREAKABLE, OSEL_ALL, FALSE },

		{ OBJTHEME, 39, 0, 0, 0, 1, -1,		// OBJ_SHRINEL
		FALSE, 1, 11, 128, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 40, 0, 0, 0, 1, -1,		// OBJ_SHRINER
		FALSE, 1, 11, 128, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 13, 0, 0, 0, 3, -1,		// OBJ_SKELBOOK
		FALSE, 4, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 34, 0, 0, 0, 5, -1,		// OBJ_BOOKCASEL
		FALSE, 3, 0, 96, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 34, 0, 0, 0, 5, -1,		// OBJ_BOOKCASER
		FALSE, 4, 0, 96, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 13, 0, 0, 0, 5, -1,		// OBJ_BOOKSTAND
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 18, 0, 0, 0, 5, -1,		// OBJ_BOOKCANDLE
		TRUE, 2, 4, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJTHEME, 41, 0, 0, 0, 7, -1,		// OBJ_BLOODFTN
		TRUE, 2, 10, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 42, 13, 16, 0, 8, -1,		// OBJ_DECAP
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, FALSE },

		{ OBJMUST, 3, 1, 16, 0, -1, -1,		// OBJ_TCHEST1
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

		{ OBJMUST, 4, 1, 16, 0, -1, -1,		// OBJ_TCHEST2
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

		{ OBJMUST, 16, 1, 16, 0, -1, -1,	// OBJ_TCHEST3
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

		{ OBJMUST, 12, 7, 7, 2, -1, Q_BLIND,		// OBJ_BLINDBOOK
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 12, 5, 5, 2, -1, Q_BLOOD,		// OBJ_BLOODBOOK
		FALSE, 4, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 43, 5, 5, 2, -1, Q_BLOOD,		// OBJ_PEDISTAL
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 44, 9, 12, 3, -1, -1,		// OBJ_L3DOORL
		FALSE, 1, 0, 64, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, TRUE },

		{ OBJMUST, 44, 9, 12, 3, -1, -1,		// OBJ_L3DOORR
		FALSE, 2, 0, 64, FALSE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, TRUE },

		{ OBJTHEME, 45, 0, 0, 0, 9, -1,		// OBJ_PURIFYINGFTN
		TRUE, 2, 10, 128, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 46, 0, 0, 0, 10, -1,		// OBJ_ARMORSTAND
		FALSE, 1, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 46, 0, 0, 0, 10, -1,		// OBJ_ARMORSTANDN
		FALSE, 2, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJTHEME, 47, 0, 0, 0, 11, -1,		// OBJ_GOATSHRINE
		TRUE, 2, 10, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 48, 13, 16, 0, -1, -1,		// OBJ_CAULDRON
		FALSE, 1, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 49, 0, 0, 0, 13, -1,		// OBJ_MURKYFTN
		TRUE, 2, 10, 128, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJTHEME, 50, 0, 0, 0, 14, -1,		// OBJ_TEARFTN
		TRUE, 2, 4, 128, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 51, 0, 0, 1, -1, Q_BETRAYER,	// OBJ_ALTBOY
		FALSE, 1, 0, 128, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 52, 0, 0, 1, -1, Q_BETRAYER,	// OBJ_MCIRCLE1
		FALSE, 1, 0, 96, FALSE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 52, 0, 0, 1, -1, Q_BETRAYER,	// OBJ_MCIRCLE2
		FALSE, 1, 0, 96, FALSE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 53, 1, 24, 0, -1, -1,		// OBJ_STORYBOOK
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },	// change 24 back to 12 JKE

		{ OBJMUST, 18, 2, 12, 0, -1, 15,		// OBJ_STORYCANDLE
		TRUE, 2, 4, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJMUST, 12, 13, 13, 4, -1, Q_WARLORD, // OBJ_STEELTOME
		FALSE, 4, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 46, 13, 13, 0, -1, Q_WARLORD, // OBJ_WARARMOR
		FALSE, 1, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJNO, 36, 13, 13, 0, -1, Q_WARLORD,	// OBJ_WARWEAP
		FALSE, 1, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJNO, 17, 0, 0, 0, 15, -1,			// OBJ_TBCROSS
		TRUE, 0, 10, 160, TRUE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 36, 0, 0, 0, 16, -1,			// OBJ_WEAPONRACK
		FALSE, 1, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJNO, 36, 0, 0, 0, 16, -1,			// OBJ_WEAPONRACKN
		FALSE, 2, 0, 96, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_NONE, FALSE },

		{ OBJNO, 54, 0, 0, 0, -1, 1,			// OBJ_MUSHPATCH
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_ALL, TRUE },

		{ OBJNO, 55, 0, 0, 0, -1, 15,			// OBJ_LAZSTAND
		FALSE, 1, 0, 128, TRUE, FALSE, TRUE, OBJ_NOBREAK, OSEL_ALL, FALSE },

		{ OBJMUST, 42, 9, 9, 3, -1, -1,			// OBJ_SLAINHERO
		FALSE, 2, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, FALSE },

		{ OBJNO, 16, 0, 0, 0, -1, -1,			// OBJ_SIGNCHEST
		FALSE, 1, 0, 96, TRUE, TRUE, TRUE, OBJ_NOBREAK, OSEL_FLR, TRUE },

// Random object, file list index, min level, max level, leveltype, theme number, quest number
// Animates, Anim Delay or Frame, Anim Length, Anim Width, Solid, Missile, Draw with Light, Breakable, Selectable, Trapable

		// Stopper
		{ -1, 0, 0, 0, -1, -1, -1, FALSE, 0, 0, 0, FALSE, FALSE, FALSE, OBJ_NOBREAK, OSEL_NONE, FALSE }
};

/*-----------------------------------------------------------------------*/

char *ObjMasterFList[] = {
		"L1Braz",		// 0
		"L1Doors",		// 1
		"Lever",		// 2
		"Chest1",		// 3
		"Chest2",		// 4
		"Banner",		// 5
		"SkulPile",		// 6
		"SkulFire",		// 7
		"SkulStik",		// 8
		"CruxSk1",		// 9
		"CruxSk2",		// 10
		"CruxSk3",		// 11
		"Book1",		// 12
		"Book2",		// 13
		"Rockstan",		// 14
		"Angel",		// 15
		"Chest3",		// 16
		"Burncros",		// 17
		"Candle2",		// 18
		"Nude2",		// 19
		"Switch4",		// 20
		"TNudeM",		// 21
		"TNudeW",		// 22
		"TSoul",		// 23
		"L2Doors",		// 24
		"WTorch4",		// 25
		"WTorch3",		// 26
		"Sarc",			// 27
		"Flame1",		// 28
		"Prsrplt1",		// 29
		"Traphole",		// 30
		"MiniWatr",		// 31
		"WTorch2",		// 32
		"WTorch1",		// 33
		"BCase",		// 34
		"BShelf",		// 35
		"WeapStnd",		// 36
		"Barrel",		// 37
		"Barrelex",		// 38
		"LShrineG",		// 39
		"RShrineG",		// 40
		"Bloodfnt",		// 41
		"Decap",		// 42
		"Pedistl",		// 43
		"L3Doors",		// 44
		"PFountn",		// 45
		"Armstand",		// 46
		"Goatshrn",		// 47
		"Cauldren",		// 48
		"MFountn",		// 49
		"TFountn",		// 50
		"Altboy",		// 51
		"Mcirl",		// 52
		"Bkslbrnt",		// 53
		"Mushptch",		// 54
		"LzStand",		// 55
};

char *CObjMasterFList[] = {
		"L1Braz",		// 0
		"L5Door",		// 1
		"L5Lever",		// 2
		"Chest1",		// 3
		"Chest2",		// 4
		"Banner",		// 5
		"SkulPile",		// 6
		"SkulFire",		// 7
		"SkulStik",		// 8
		"CruxSk1",		// 9
		"CruxSk2",		// 10
		"CruxSk3",		// 11
		"Book1",		// 12
		"Book2",		// 13
		"Rockstan",		// 14
		"Angel",		// 15
		"Chest3",		// 16
		"Burncros",		// 17
		"L5Light",		// 18
		"Nude2",		// 19
		"Switch4",		// 20
		"TNudeM",		// 21
		"TNudeW",		// 22
		"TSoul",		// 23
		"L2Doors",		// 24
		"WTorch4",		// 25
		"WTorch3",		// 26
		"L5Sarco",		// 27
		"Flame1",		// 28
		"Prsrplt1",		// 29
		"Traphole",		// 30
		"MiniWatr",		// 31
		"WTorch2",		// 32
		"WTorch1",		// 33
		"BCase",		// 34
		"BShelf",		// 35
		"WeapStnd",		// 36
		"Urn",			// 37
		"Urnexpld",		// 38
		"LShrineG",		// 39
		"RShrineG",		// 40
		"Bloodfnt",		// 41
		"Decap",		// 42
		"Pedistl",		// 43
		"L3Doors",		// 44
		"PFountn",		// 45
		"Armstand",		// 46
		"Goatshrn",		// 47
		"Cauldren",		// 48
		"MFountn",		// 49
		"TFountn",		// 50
		"Altboy",		// 51
		"Mcirl",		// 52
		"L5Books",		// 53
		"Mushptch",		// 54
		"LzStand",		// 55
};

char *HObjMasterFList[] = {
		"L1Braz",		// 0
		"L1Doors",		// 1
		"Lever",		// 2
		"Chest1",		// 3
		"Chest2",		// 4
		"Banner",		// 5
		"SkulPile",		// 6
		"SkulFire",		// 7
		"SkulStik",		// 8
		"CruxSk1",		// 9
		"CruxSk2",		// 10
		"CruxSk3",		// 11
		"Book1",		// 12
		"Book2",		// 13
		"Rockstan",		// 14
		"Angel",		// 15
		"Chest3",		// 16
		"Burncros",		// 17
		"Candle2",		// 18
		"Nude2",		// 19
		"Switch4",		// 20
		"TNudeM",		// 21
		"TNudeW",		// 22
		"TSoul",		// 23
		"L2Doors",		// 24
		"WTorch4",		// 25
		"WTorch3",		// 26
		"Sarc",			// 27
		"Flame1",		// 28
		"Prsrplt1",		// 29
		"Traphole",		// 30
		"MiniWatr",		// 31
		"WTorch2",		// 32
		"WTorch1",		// 33
		"BCase",		// 34
		"BShelf",		// 35
		"WeapStnd",		// 36
		"L6Pod1",		// 37
		"L6Pod2",		// 38
		"LShrineG",		// 39
		"RShrineG",		// 40
		"Bloodfnt",		// 41
		"Decap",		// 42
		"Pedistl",		// 43
		"L3Doors",		// 44
		"PFountn",		// 45
		"Armstand",		// 46
		"Goatshrn",		// 47
		"Cauldren",		// 48
		"MFountn",		// 49
		"TFountn",		// 50
		"Altboy",		// 51
		"Mcirl",		// 52
		"Bkslbrnt",		// 53
		"Mushptch",		// 54
		"LzStand",		// 55
};


