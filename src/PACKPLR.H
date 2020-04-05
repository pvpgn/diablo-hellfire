/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1996 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/PACKPLR.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Packed structures
**-----------------------------------------------------------------------*/

#pragma pack(push,1)
typedef struct {
	int iSeed;
	WORD iCreateInfo;
	WORD idx;
	BYTE bId;
	BYTE bDur;
	BYTE bMDur;
	BYTE bCh;
	BYTE bMCh;
	WORD wValue;
	DWORD dwBuff;
} PkItemStruct;

typedef struct {
	FILETIME archiveTime;
	BYTE destAction;
	BYTE destParam1;
	BYTE destParam2;
	BYTE plrlevel;
	BYTE px;
	BYTE py;
	BYTE targx;
	BYTE targy;

	// Player attributes
	char pName[PLR_NAME_LEN];
	BYTE pClass;

	BYTE pBaseStr;
	BYTE pBaseMag;
	BYTE pBaseDex;
	BYTE pBaseVit;
	BYTE pLevel;
	BYTE pStatPts;

	long pExperience;
	long pGold;

	long pHPBase;
	long pMaxHPBase;
	long pManaBase;
	long pMaxManaBase;

	// hack hack hack  --donald
	BYTE pSplLvl[SPL_BONESPIRIT+1];
	__int64 pMemSpells;

	PkItemStruct InvBody[NUM_INVLOC];
	PkItemStruct InvList[MAXINV];
	char InvGrid[MAXINV];
	BYTE _pNumInv;
	PkItemStruct SpdList[MAXSPD];

	// these fields are to be used if more variables need to be added
	// to the player structure so that the size won't change...
	BYTE bReserved1;
	BYTE bReserved2;
	BYTE bReserved3;
	BYTE bReserved4;
	BYTE bReserved5;
	BYTE bReserved6;
	BYTE bReserved7;
	BYTE bReserved8;

	WORD _pReflectCount;
	WORD wReserved2;
	WORD wReserved3;	//
	WORD wReserved4;	//
	WORD wReserved5;	//  wReserved3-8 are used for spell slots...
	WORD wReserved6;	//
	WORD wReserved7;	//
	WORD wReserved8;	//

	DWORD pDiabloKillLevel;	// player killed Diablo at what level
	DWORD _gnDifficulty;
	DWORD _pIFlags2;
	DWORD dwReserved4;
	DWORD dwReserved5;
	DWORD dwReserved6;
	DWORD dwReserved7;
	DWORD dwReserved8;

} PkPlayerStruct;
#pragma pack(pop)


/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/
void PackPlayer(PkPlayerStruct *p, int pnum);
void UnPackPlayer(const PkPlayerStruct *p, int pnum, BOOL killok);

void PackItem(PkItemStruct *id, const ItemStruct *is);
void UnPackItem(const PkItemStruct *is, ItemStruct *id);
