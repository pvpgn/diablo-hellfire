/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DRLG_L3.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define BLK_U	0
#define BLK_R	1
#define BLK_D	2
#define BLK_L	3

#define L3_DIRT	8

//#define MINFAREA	1536
#define MINFAREA	600

#define FR_FALSE	0
#define FR_TRUE	1

#define UR_ISLE		14
#define LL_ISLE		13
#define LR_ISLE		12
#define UL_WALL		11
#define TOP_WALL	10
#define LEFT_WALL	9
#define FILL		8
#define FLOOR		7
#define LR_WALL		6
#define UL_ISLE		5
#define RIGHT_WALL	4
#define UR_WALL		3
#define BOTTOM_WALL	2
#define LL_WALL		1
#define D3_NULL		0

#define L3_NUMBLOCKS 124

#define NORTH 0
#define SOUTH 1
#define EAST  2
#define WEST  3

#define	WOOD_HORIZWALL		134
#define	WOOD_VERTWALL		137
#define	WOOD_LRCORNER		138
#define WOOD_HORIZGATE		146
#define WOOD_VERTGATE		147
#define	WOOD_ULCORNER		150
#define WOOD_URCORNER		151
#define WOOD_LLCORNER		152

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void CreateL3Dungeon(unsigned int, int);
void LoadPreL3Dungeon(char [], int, int);
void LoadL3Dungeon(char [], int, int);
int DRLG_L3Spawn(int, int, int *);
BOOL SkipThemeRoom( int x, int y );
