/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DRLG_L1.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXCHRS		17

#define D_NULL		0	// Unused/empty space
#define D_VWALL		1	// Vertical wall
#define D_HWALL		2	// Horizontal wall
#define D_LRC		3	// Lower right corner
#define D_ULC		4	// Upper left corner
#define D_AULC		5	// Archway upper left corner
#define D_URC		6	// Upper right corner
#define D_LLC		7	// Lower left corner
#define D_AURC		8	// Archway upper right corner
#define D_ALLC		9	// Archway lower left corner
#define D_TULC1		10	// Transition piece #1 upper left corner
#define D_AVW		11	// Archway veritcal wall
#define D_AHW		12	// Archway horizontal wall
#define D_FLOOR		13	// Floor
#define D_TULC2		14	// Transition piece #2 upper left corner
#define D_COL		15	// Column
#define D_BCAP		16	// Bottom cap
#define D_RCAP		17	// Right cap

#define D_DV		18	// Dirt with vertical edge
#define D_DH		19	// Dirt with horizontal edge
#define D_DLRC		20	// Dirt lower right corner
#define D_DULC		21	// Dirt upper left corner
#define D_DIRT		22	// Normal Dirt piece
#define D_DURC		23	// Dirt upper right corner
#define D_DLLC		24	// Dirt lower left corener

#define D_DRV		25	// Door on a vertical wall
#define D_DRH		26	// Door on a horizontal wall
#define D_DDULC		28	// Double door upper left corner
#define D_DRURC		30	// Door on upper right corner
#define D_DRLLC		31	// Door on lower left corner
#define D_DRVT1		40	// Vertical door on transition 1
#define D_DRVULC	41	// Vertical door on upper left corner
#define D_DRHT2		42	// Horizontal door on transition 2
#define D_DRHULC	43	// Horizontal door on upper left corner

#define AREAMIN		4
#define AREAMAX 	12

#define NODOOR		0x00	// No door flag
#define HDOOR		0x01	// Horizontal door
#define VDOOR		0x02	// Vertical door
#define DDOOR		0x03	// Double door (both dirs)
//#define SETP_BIT	0x80	// Non changeable set piece bit (defined in gendung.h)
#define SETP_MASK	0x7f
#define SETP_TEMP	0x40	// Temp non changeable set piece bit
#define SETP_TMASK	0xbf

#define NUMDPATS	9

#define NUMSPATS	37
#define _S1			139
#define _S2			140
#define _S3			141
#define _S4			142
#define _S5			143
#define _S6			144
#define _S7			145
#define _S8			146
#define _S9			147
#define _S10		148
#define _S11		149
#define _S12		150
#define _S13		151
#define _S14		152
#define _S15		153
#define _S16		154
#define _S17		155
#define _S18		156
#define _S19		157

#define NUMBLOCKS	206

#define NUMSETPIECES	4

#define NA_KRUL_LEVEL 24	// change to 24 for real game JKE
#define CORNERSTONE_LEVEL 21

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	int	qpat;
	int	d1;
	int	d2;
	int	d3;
	int	d4;
} DPatsStruct;

//JKE
typedef struct 
{
	int x,y;	//location of lower center door mini tile
	BOOL Open;	// is the place open?
	BOOL Books;	// were the books used to open?

	int LeverX, LeverY;
	BOOL Lever_Thrown;

	int MIndex;

} Na_Krul_Struct;

extern Na_Krul_Struct Na_Krul;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

//void CreateL1Dungeon(unsigned int, int);
void LoadL1Dungeon(char [], int, int);
void LoadPreL1Dungeon(char [], int, int);
//void InitL1DirtQuads();
//void DRLG_L1FloodTVal();

byte L5TileType(int t);
void CreateL5Dungeon(unsigned int, int);
