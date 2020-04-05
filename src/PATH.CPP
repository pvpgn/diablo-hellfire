/*-----------------------------------------------------------------------**
** 								Diablo
**
**							  Path file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/PATH.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 A* Path Search Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"gendung.h"
#include	"items.h"
#include	"player.h"
#include	"path.h"

PATHNODE PNodePool[MAXPNODES];
PATHNODE *OPEN;
PATHNODE *CLOSED;

int tempath[MAXPATHLEN];

PATHNODE pathnodes[MAXPNODES];		// node cache
int numpnodes;						// index to next pathnode

PATHNODE *Stack[MAXPNODES];			// stack of nodes to process
int numstack;						// index to next stack item

static char DirCmd[] = 
	{ PCMD_WALKU, PCMD_WALKUR, PCMD_WALKR, PCMD_WALKUL, 0,
	  PCMD_WALKDR, PCMD_WALKL, PCMD_WALKDL, PCMD_WALKD
	};

int PATHDIST(int x1, int y1, int x2, int y2);
PATHNODE *GetPathNode();
PATHNODE *ReturnBestNode(void);
BOOL GenerateSuccessors(CHECKFUNC1 PosOk, int PosOkArg, PATHNODE *BestNode,int dx,int dy);
BOOL GenerateSucc(PATHNODE *BestNode,int x, int y, int dx, int dy);
PATHNODE *CheckOPEN(int x, int y);
PATHNODE *CheckCLOSED(int x, int y);
void Insert(PATHNODE *Successor);
void PropagateDown(PATHNODE *Old);

void Push(PATHNODE *Node);
PATHNODE *Pop(void);


/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int FindPath(CHECKFUNC1 PosOk, int PosOkArg, int sx,int sy,int dx,int dy, char path[])
{
    PATHNODE *Node, *BestNode;
	int i,len;

	numpnodes = 0;					// clear node chache
    OPEN=GetPathNode();
    CLOSED=GetPathNode();
	numstack = 0;					// clear stack

    Node=GetPathNode();
    Node->g=0;
    Node->h=PATHDIST(sx,sy,dx,dy);
    Node->f=Node->g+Node->h;
    Node->x=sx;
    Node->y=sy;

    OPEN->NextNode=Node;        /* make Open List point to first node */
    for (;;)
    {
		BestNode=(PATHNODE *)ReturnBestNode();
		if(!BestNode)
			return 0;

		if (BestNode->x == dx && BestNode->y == dy)  /* if we've found the end, break and finish */
		   break;
		if(!GenerateSuccessors(PosOk, PosOkArg, BestNode,dx,dy))
			return 0;
    }

	// generate list of moves
	Node = BestNode;

	len = 0;

	while(Node->Parent && len < MAXPATHLEN)
	{
		tempath[len++] = DirCmd[(Node->x - Node->Parent->x + 1) + (Node->y - Node->Parent->y + 1)*3];
		Node = Node->Parent;
	}
	if(len == MAXPATHLEN)
		return 0;

	for(i = 0; i < len; i++)
		path[i] = tempath[len - i - 1];

	return i;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

int PATHDIST(int x1, int y1, int x2, int y2)
{
	int dx,dy;
	int mind,maxd;

	dx = abs(x1-x2);
	dy = abs(y1-y2);
	mind = min(dx,dy);
	maxd = max(dx,dy);

	// Assign dist of 3 for diagonal, 2 for straight
	// Therefore, 3*(mind) + 2*(maxd-mind)
	// = mind + 2*maxd
	return mind + maxd<<1;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
int PathVal(PATHNODE *Node, int x, int y)
{
	if(Node->x == x || Node->y == y)
		return 2;
	return 3;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

PATHNODE *ReturnBestNode(void)
{
    PATHNODE *tmp;

    if (OPEN->NextNode == NULL)
	{
		return NULL;
	}

/* Pick Node with lowest f, in this case it's the first node in list
   because we sort the OPEN list wrt lowest f. Call it BESTNODE. */

    tmp=OPEN->NextNode;   // point to first node on OPEN
    OPEN->NextNode=tmp->NextNode;    // Make OPEN point to nextnode or NULL.

/* Next take BESTNODE (or temp in this case) and put it on CLOSED */

    tmp->NextNode=CLOSED->NextNode;
    CLOSED->NextNode=tmp;

    return(tmp);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL PathDirOk(PATHNODE *Node, int x, int y)
{
	BOOL ok = TRUE;

	switch(DirCmd[(x - Node->x + 1) + (y - Node->y + 1)*3])
	{
	case PCMD_WALKU:
		ok = !(nSolidTable[dPiece[x][y+1]] || nSolidTable[dPiece[x+1][y]]);
		break;
	case PCMD_WALKR:
		ok = !(nSolidTable[dPiece[x][y+1]] || nSolidTable[dPiece[x-1][y]]);
		break;
	case PCMD_WALKD:
		ok = !(nSolidTable[dPiece[x][y-1]] || nSolidTable[dPiece[x-1][y]]);
		break;
	case PCMD_WALKL:
		ok = !(nSolidTable[dPiece[x+1][y]] || nSolidTable[dPiece[x][y-1]]);
		break;
	}

	return ok;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL GenerateSuccessors(CHECKFUNC1 PosOk, int PosOkArg, PATHNODE *BestNode,int dx,int dy)
{
	int x,y;
	int i;
	BOOL posok;
	static const char nextx[] = { -1, -1, 1, 1, -1, 0, 1, 0 };
	static const char nexty[] = { -1, 1, -1, 1, 0, -1, 0, 1 };

	for(i = 0; i < 8; i++)
	{
		x = BestNode->x + nextx[i];
		y = BestNode->y + nexty[i];

		posok = PosOk(PosOkArg, x, y);
		if((posok && PathDirOk(BestNode, x, y))
			|| (!posok && x==dx && y==dy))	// allow user to click on walls
			if(!GenerateSucc(BestNode,x,y,dx,dy))
				return FALSE;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

BOOL GenerateSucc(PATHNODE *BestNode,int x, int y, int dx, int dy)
{
    int g,c=0;
    PATHNODE *Old,*Successor;

    g=BestNode->g+PathVal(BestNode,x,y);	    /* g(Successor)=g(BestNode)+cost of getting from BestNode to Successor */

    if ((Old=CheckOPEN(x, y)) != NULL) /* if equal to NULL then not in OPEN list, else it returns the Node in Old */
    {
		for(c=0;c<8;c++)
		  if(BestNode->Child[c] == NULL) /* Add Old to the list of BestNode's Children (or Successors). */
		   break;
		BestNode->Child[c]=Old;

		if (g < Old->g && PathDirOk(BestNode, x, y))  /* if our new g value is < Old's then reset Old's parent to point to BestNode */
		{
			Old->Parent=BestNode;
			Old->g=g;
			Old->f=g+Old->h;
		}
    }
    else if ((Old=CheckCLOSED(x, y)) != NULL) /* if equal to NULL then not in OPEN list, else it returns the Node in Old */
    {
		for(c=0;c<8;c++)
			if (BestNode->Child[c] == NULL) /* Add Old to the list of BestNode's Children (or Successors). */
			  break;
		BestNode->Child[c]=Old;

		if (g < Old->g && PathDirOk(BestNode, x, y))  /* if our new g value is < Old's then reset Old's parent to point to BestNode */
		{
			Old->Parent=BestNode;
			Old->g=g;
			Old->f=g+Old->h;
			PropagateDown(Old);       /* Since we changed the g value of Old, we need
						 to propagate this new value downwards, i.e.
						 do a Depth-First traversal of the tree! */
		}
    }
    else
    {
		if(!(Successor=GetPathNode()))
			return FALSE;
		else
		{
			Successor->Parent=BestNode;
			Successor->g=g;
			Successor->h=PATHDIST(x,y,dx,dy);
			Successor->f=g+Successor->h;
			Successor->x=x;
			Successor->y=y;
			Insert(Successor);     /* Insert Successor on OPEN list wrt f */
			for(c=0;c<8;c++)
			if (BestNode->Child[c] == NULL) /* Add Successor to the list of BestNode's Children (or Successors). */
			  break;
			BestNode->Child[c]=Successor;
		}
    }
	return TRUE;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

PATHNODE *CheckOPEN(int x, int y)
{
    PATHNODE *tmp;

    tmp=OPEN->NextNode;
    while (tmp != NULL)
    {
		if (tmp->x == x && tmp->y == y)
		  return (tmp);
		else
		  tmp=tmp->NextNode;
    }
  return (NULL);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

PATHNODE *CheckCLOSED(int x, int y)
{
    PATHNODE *tmp;

    tmp=CLOSED->NextNode;

    while (tmp != NULL)
    {
		if (tmp->x == x && tmp->y == y)
		  return (tmp);
		else
		  tmp=tmp->NextNode;
    }
  return (NULL);
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void Insert(PATHNODE *Successor)
{
    PATHNODE *tmp1,*tmp2;
    int f;

    if (OPEN->NextNode == NULL)
	{
		OPEN->NextNode=Successor;
		return;
	}

       /* insert into OPEN successor wrt f */

    f=Successor->f;
    tmp1=OPEN;
    tmp2=OPEN->NextNode;

    while ((tmp2 != NULL) && (tmp2->f < f))
    {
      tmp1=tmp2;
      tmp2=tmp2->NextNode;
    }
	Successor->NextNode=tmp2;
	tmp1->NextNode=Successor;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void PropagateDown(PATHNODE *Old)
{
    int c;
    PATHNODE *Child, *Father;

	Push(Old);
    while (numstack)
    {
		Father=Pop();
		for(c=0;c<8;c++)
		{
			if ((Child=Father->Child[c])==NULL)       /* we may stop the propagation 2 ways: either */
				break;
			if (Father->g+PathVal(Father, Child->x, Child->y) < Child->g && PathDirOk(Father, Child->x, Child->y)) /* there are no children, or that the g value of */
			{                           /* the child is equal or better than the cost we're propagating */
				Child->Parent=Father;
				Child->g=Father->g+PathVal(Father, Child->x, Child->y);
				Child->f=Child->g+Child->h;
				Push(Child);
			}
		}
    }
}

/**************************************************************************/
/*                            STACK FUNCTIONS                             */
/**************************************************************************/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

void Push(PATHNODE *Node)
{
	Stack[numstack++] = Node;
}

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/

PATHNODE *Pop()
{
	return Stack[--numstack];
}

PATHNODE *GetPathNode()
{
	PATHNODE *Node;

	if(numpnodes == MAXPNODES)
		return NULL;
	else
	{
		Node = &pathnodes[numpnodes++];

		memset((void *)Node, 0, sizeof(PATHNODE));
		return Node;
	}
}
