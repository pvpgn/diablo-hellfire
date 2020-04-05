/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/LIGHTING.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define	LIGHTSIZE	27*256

#define MAXLIGHTS	32
#define MAXVISION	32
#define MAXTRANS	32

#define PLRLRAD		10		// Player light radius
#define PLRVRAD		10		// Player vision radius

#define HALF_T		1
#define HALF_B		2		
#define HALF_R		3	
#define HALF_L		4	
#define QTR_UR		5	
#define QTR_LR		6	
#define QTR_UL		7	
#define QTR_LL		8	
#define VERT_T		9	
#define VERT_B		10	
#define VERT_LINE	11	
#define HORT_L		12	
#define HORT_R		13	
#define HORT_LINE	14	
#define SELF		15	
#define WHOLE		16	
#define NEG_WHOLE	17

#define LIGHT_NORM	0		// Normal lighting
#define LIGHT_INFRA	1		// Infravision
#define LIGHT_STONE	2		// Stone curse
#define LIGHT_GREY	3		// Pause & death
#define LIGHT_U		4		// Unique monster transform 3-11

#define LFLAG_MINE	1		// whether or not my player is the source
							// (used in ProcessVision)

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	int	_lx;
	int	_ly;
	int _lradius;
	int _lid;
	BOOL _ldel;
	BOOL _lunflag;
	BOOL _lneg;
	int _lunx;
	int _luny;
	int _lunr;
	int _xoff;
	int _yoff;
	BOOL _lflags;
} LightListStruct;

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern int lightflag;
extern BYTE vCrawlTable[23][30];
extern BYTE RadiusAdj[23];
//extern int CrawlTable[2061];
extern char CrawlTable[2749];
extern char * pCrawlEntry[19];

extern LightListStruct		LightList[MAXLIGHTS];
extern BYTE lightactive[MAXLIGHTS];
extern int numlights;
extern BOOL dolighting;
extern "C" {
	extern char lightmax;
	extern BYTE *pLightTbl;
}
extern LightListStruct		VisionList[MAXVISION];
extern int numvision;
extern BOOL dovision;
extern int visionid;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void DoLighting (int, int, int, int);
void DoUnLight (int, int, int);

void InitLighting();
void InitLightMax();
int AddLight(int, int, int);
void AddUnLight(int);
void ChangeLightRadius(int, int);
void ChangeLightXY(int, int, int);
void ChangeLight(int, int, int, int);
void ChangeLightOff(int id, int x, int y);
void ProcessLightList();

void SavePreLighting();

void DoVision (int, int, int, BOOL, BOOL);
void DoUnVision (int, int, int);

void InitVision();
int AddVision(int, int, int, BOOL);
void AddUnVision(int);
void ChangeVisionRadius(int, int);
void ChangeVisionXY(int, int, int);
void ChangeVision(int, int, int, int);
void ProcessVisionList();

void ResetLight();
void ToggleLight();

void InitLightTable();
void MakeLightTable();
void FreeLightTable();

void BloodCycle();
