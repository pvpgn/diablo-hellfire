/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/TRIGS.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

//#define MAXTRIGGERS	5
#define MAXTRIGGERS		7	// to add crypt and hive JKE

#define LVL_DOWN	0
#define LVL_UP		1
#define LVL_SET		2
#define LVL_RTN		3
#define LVL_NODIR	4
#define LVL_WARP	5
#define LVL_TWARPDN	6
#define LVL_TWARPUP	7

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	int	_tx;
	int	_ty;
	unsigned int _tmsg;
	int _tlvl;
} TriggerStruct;

/*-----------------------------------------------------------------------**
** Externs
**-----------------------------------------------------------------------*/

extern BOOL trigflag;
extern TriggerStruct trigs[MAXTRIGGERS];
extern int numtrigs;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void CheckTriggers();
void InitTownTriggers();
void InitL1Triggers();
void InitL2Triggers();
void InitL3Triggers();
void InitL4Triggers();
void InitSKingTriggers();
void InitSChambTriggers();
void InitPWaterTriggers();
void InitNoTriggers();
void InitVPTriggers();

void CheckTrigForce();
void Freeupstairs();