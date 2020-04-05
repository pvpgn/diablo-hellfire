/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/OBJECTS.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXOBJECTS		127

#define MAXLVLOBJS		40

#define OBJ_NOBREAK		0
#define OBJ_BREAKABLE	1
#define OBJ_BROKEN		-1

#define OSEL_NONE	0
#define OSEL_FLR	1
#define OSEL_TOP	2
#define OSEL_ALL	3

#define OBJRND		0
#define OBJMUST		1
#define OBJNO		2
#define OBJTHEME	3

#define OBJNOWARP	0
#define OBJWARP1	1
#define OBJWARP2	2
#define OBJWARP3	3
#define OBJDONEWARP	4

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct {
	char oload;			// Loading a must, rnd, or can't
	char ofindex;		// Filename index
	char ominlvl;		// Min level for object
	char omaxlvl;		// Max level for object
	char olvltype;		// Level type for set levels
	char otheme;		// Theme type
	char oquest;		// Quest related?
	BOOL oAnimFlag;		// Animates?
	int oAnimDelay;		// Anim Delay
	int oAnimLen;		// if Animates then Length of animation, else initial frame
	long oAnimWidth;	// Width of cels
	BOOL oSolidFlag;	// Can I stand on square with object?
	BOOL oMissFlag;		// Can I shoot missiles through this object?
	BOOL oLightFlag;	// Draw with lighting?
	char oBreak;		// Breakable
	char oSelFlag;		// Selectable object?
	BOOL oTrapFlag;		// Object trapable?
} ObjDataStruct;

typedef struct {
	int _otype;			// object type
	int _ox;			// object map x
	int _oy;			// object map y
	BOOL _oLight;		// Light source drawn?
	BOOL _oAnimFlag;	// Animate at all?
	BYTE *_oAnimData;	// Data pointer to anim tables
	int _oAnimDelay;	// anim delay amount
	int _oAnimCnt;		// current anim delay value
	int	_oAnimLen;		// number of anim frames
	int	_oAnimFrame;	// current anim frame
	long _oAnimWidth;	// width of object
	long _oAnimWidth2;	// (width - 64) / 2 of object for drawing
	BOOL _oDelFlag;		// Delete this object?
	char _oBreak;		// Breakable
	BOOL _oSolidFlag;	// Can I stand on square with object?
	BOOL _oMissFlag;	// Can I shoot missiles through this object?
	char _oSelFlag;		// Selectable object?
	BOOL _oPreFlag;		// Draw behind player?
	BOOL _oTrapFlag;	// Am I trapped?
	BOOL _oDoorFlag;	// Am I a door?
	int _olid;			// Light id
	int _oRndSeed;		// random seed
	long _oVar1;		// scratch var 1
	long _oVar2;		// scratch var 2
	long _oVar3;		// scratch var 3
	long _oVar4;		// scratch var 4
	long _oVar5;		// scratch var 5
	long _oVar6;		// scratch var 6
	long _oVar7;		// scratch var 7
	long _oVar8;		// scratch var 8
} ObjectStruct;

#define SAVE_OBJECT_SIZE sizeof(ObjectStruct)

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

extern ObjectStruct	object[MAXOBJECTS];
extern long numobjects;
extern int objectactive[MAXOBJECTS];
extern int objectavail[MAXOBJECTS];
extern BOOL InitObjFlag;

/*-----------------------------------------------------------------------**
** Prototypes
**-----------------------------------------------------------------------*/

void InitObjectGFX();
void FreeObjectGFX();
void InitObjects();
void SetMapObjects(BYTE *, int, int);
void ProcessObjects();
void AddObject(int, int, int);

void SyncObjectAnim(int);

void BreakObject(int, int);
void SyncBreakObj(int, int);

void MonstCheckDoors(int);

void OperateObject(int, int, BOOL);
void SyncOpObject(int, int, int);
void TryDisarm(int, int);

void ClrAllObjects ();

void GetObjectStr(int);

void ObjSetMicro(int, int, int);
void ObjSetMini(int, int, int);

void AddL1Objs(int, int, int, int);
void AddL2Objs(int, int, int, int);

void ObjL1Special(int, int, int, int);
void ObjL2Special(int, int, int, int);

void AddLeverObj(int, int, int, int, int, int, int, int);
void AddBookLever(int, int, int, int, int, int, int, int, int);

void SetObjMapRange(int, int, int, int, int, int);
void SetBookMsg(int, int);
void ObjChangeMap(int, int ,int, int);
void ObjChangeMapResync(int, int ,int, int);

int ItemMiscIdIdx(int /* imiscid */);

