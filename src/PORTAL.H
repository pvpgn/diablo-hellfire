/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXPORTAL	4	// Must be the same as MAX_PLRS!

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	BOOL open;
	int	x;
	int	y;
	int level;
	int ltype;
	BOOL setlvl;
} PortalStruct;

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern PortalStruct portal[MAXPORTAL];

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitPortals();
void SyncPortals();
void AddInTownPortal(int i);

void ActivatePortal(int i, int x, int y, int lvl, int lvltype, BOOL sp);
void DeactivatePortal(int i);

void SetCurrentPortal(int p);

void GetPortalLvlPos();
void GetPortalLevel();

BOOL PortalOnLevel(int i);
void RemovePortalMissile(int i);
