/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/AUTOMAP.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/
#define AUTOMAPX	40
#define AUTOMAPY	40

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/
extern BOOL automapflag;
extern BYTE automapview[AUTOMAPX][AUTOMAPY];
extern int automapscale;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitAutomapOnce();
void InitAutomap();
void DrawAutomap();

void SetAutomapView(int, int);
void SyncAutomap();

void StartAutomap();
void AutomapUp();
void AutomapDown();
void AutomapLeft();
void AutomapRight();
void AutomapZoomIn();
void AutomapZoomOut();
