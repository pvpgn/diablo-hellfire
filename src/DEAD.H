/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/DEAD.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXDEAD			31		// Always will be 31!

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	BYTE *_deadData[8];		// Data pointer to anim tables
	int	_deadFrame;			// current dead frame
	long _deadWidth;		// width of dead
	long _deadWidth2;		// (width - 64) / 2 of dead for drawing
	char _deadtrans;		// translations for unique monsters
} DeadStruct;

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern DeadStruct dead[MAXDEAD];
extern int spurtndx;
extern int stonendx;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitDead();
void AddDead(int, int, char, int);
void SyncUniqDead();
