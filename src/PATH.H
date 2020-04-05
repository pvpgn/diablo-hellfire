/*-----------------------------------------------------------------------**
** 								Diablo
**
**						   Path Header File
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/PATH.H 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Defines
**-----------------------------------------------------------------------*/

#define MAXPNODES	300

/*-----------------------------------------------------------------------**
** Structures
**-----------------------------------------------------------------------*/

typedef struct tagPATHNODE{
    char f,h;
    char g;
    int x,y;
    struct tagPATHNODE *Parent;
    struct tagPATHNODE *Child[8];       /* a node may have upto 8+(NULL) children. */
    struct tagPATHNODE *NextNode;       /* for filing purposes */
} PATHNODE;

/*-----------------------------------------------------------------------**
** Function Prototypes
**-----------------------------------------------------------------------*/

typedef BOOL (*CHECKFUNC1)(int arg1, int x, int y);

int FindPath(CHECKFUNC1 PosOk, int PosOkArg, int sx,int sy,int dx,int dy, char path[]);
