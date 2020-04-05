/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DRLG_L2.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define DIR_NONE  0
#define DIR_NORTH 1
#define DIR_EAST  2
#define DIR_SOUTH 3
#define DIR_WEST  4

#define AREA_MIN  2

#define MAX_DOORS  7
#define MAX_ROOMS  80

#define ROOM_MAX  10
#define ROOM_MIN  6

#define VWALL_PIECE			1
#define HWALL_PIECE			2
#define FLOOR_PIECE			3
#define VDOOR_PIECE			4
#define HDOOR_PIECE			5
#define LRWALL_PIECE		6
#define URWALL_PIECE		7
#define ULWALL_PIECE		8
#define LLWALL_PIECE		9

#define DVWALL_PIECE		10
#define DHWALL_PIECE		11
#define DFLOOR_PIECE		12
#define DULWALL_PIECE		13
#define DURWALL_PIECE		14
#define DLLWALL_PIECE		15
#define DLRWALL_PIECE		16

#define THEME_PIECE			252

#define NO_CHAR				' '
#define WALL_CHAR			'#'
#define FLOOR_CHAR			'.'
#define HALL_CHAR			','
#define DOOR_CHAR			'D'
#define LRWALL_CHAR			'A'
#define URWALL_CHAR			'B'
#define ULWALL_CHAR			'C'
#define LLWALL_CHAR			'E'

#define L2_NUMBLOCKS	161
#define L2_NUMSPATS		2

#define NOTOK			255
#define OK				254

#define CNO				0
#define CWALL			1
#define CFLOOR			2
#define CDOOR			3
#define CEMPTY			4
#define CDoF			5
#define CDoW			6
#define CEoF			7
#define CDoWoF			8

#define SNUM1 2
#define SNUM2 2

#define SETP_BIT	0x80	// Non changeable set piece bit


/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct NODE {
	int nHallx1;
	int nHally1;
	int nHallx2;
	int nHally2;
	int nHalldir;
	struct NODE * pNext;
} HALLNODE;

typedef struct {
	int nRoomx1;
	int nRoomy1;
	int nRoomx2;
	int nRoomy2;
	int nRoomDest;
} ROOMNODE;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void CreateL2Dungeon(unsigned int, int);
void LoadL2Dungeon(char [], int, int);
void LoadPreL2Dungeon(char [], int, int);

