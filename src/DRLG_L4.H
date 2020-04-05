/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DRLG_L4.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define L4DIR_HORIZ		0
#define L4DIR_VERT		1

#define L4ROOM_MIN 		2
#define L4ROOM_MAX 		6

#define L4DUNX			20
#define L4DUNY			20

#define L4MIN_AREA		((L4DUNX*L4DUNY)/3) + ((L4DUNX*L4DUNY)/10)

#define L4DX			80
#define L4DY			80
#define L4_DIRT			12

#define NUMBBLOCKS		140

// PIC == PIECE
#define VWALL_PIC		1
#define HWALL_PIC		2
#define LRWALL_PIC		12
#define FLOOR_PIC		6
#define URWALL_PIC		16
#define ULWALL_PIC		9
#define LLWALL_PIC		15

#define DVWALL_PIC		18
#define DHWALL_PIC		19
#define DFLOOR_PIC		20
#define DULWALL_PIC		21

#define DURWALL_PIC		25
#define DURWALL2_PIC	28
#define DURWALL3_PIC	23

#define DLLWALL_PIC		27
#define DLLWALL2_PIC	26
#define DLLWALL3_PIC	22

#define DLRWALL_PIC		24
#define DIRT_PIC		30
//Arches
#define YARCHWALL_PIC	53
#define XARCHWALL_PIC	57

#define NUMSPATS	37
#define _1S			47
#define _2S			48
#define _3S			49
#define _4S			50
#define _5S			51
#define _6S			54
#define _7S			55
#define _8S			58
#define _9S			59
#define _10S		60


/*-----------------------------------------------------------------------**
** externs
**-----------------------------------------------------------------------*/

extern int diabquad1x, diabquad2x, diabquad3x, diabquad4x;
extern int diabquad1y, diabquad2y, diabquad3y, diabquad4y;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void CreateL4Dungeon(unsigned int, int);
//int DRLG_L4Spawn(int, int, int *);
