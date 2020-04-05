/****************************************************************************
*
*  MULTI.CPP
*
*  This sample demonstrates the use of multiple links, allowing one node
*  to be linked into more than one list.
*
***/

#include <windows.h>
#include <stdio.h>
#include <storm.h>

NODEDECLEX(TEST) {
    LPCSTR       string;
    LINKEX(TEST) normallink;
    LINKEX(TEST) sortedlink;
} *TESTPTR;

static LISTEX(TEST,normallink) s_list;
static LISTEX(TEST,sortedlink) s_sortedlist;

#define  TESTSTRINGS  5

static const LPCSTR s_teststring[TESTSTRINGS] =
    {"one","two","three","four","five"};

//===========================================================================
static void DumpList (LISTPTREX(TEST) list) {
  ITERATELISTPTR(TEST,list,curr)
    printf("%s\n",curr->string);
}

//===========================================================================
int __cdecl main () {

  // ALLOCATE AND INITIALIZE THE NODES
  for (int loop = 0; loop < TESTSTRINGS; ++loop) {

    // ALLOCATE A NEW NODE AND FILL IN ITS STRING
    TESTPTR newnode = s_list.NewNode();
    newnode->string = s_teststring[loop];

    // ADD IT TO THE SORTED LIST IN THE CORRECT ORDER
    TESTPTR checknode = s_sortedlist.Head();
    while (checknode && (_stricmp(checknode->string,newnode->string) < 0))
      checknode = s_sortedlist.Next(checknode);
    s_sortedlist.LinkNode(newnode,LIST_LINK_BEFORE,checknode);

  }

  // DUMP BOTH LISTS TO THE SCREEN
  printf("List Sorted By Add Order\n");
  DumpList(&s_list);
  printf("\nList Sorted Alphabetically\n");
  DumpList(&s_sortedlist);

  // DELETE ALL NODES
  // (UPON DELETION, EACH NODE IS AUTOMATICALLY UNLINKED FROM EVERY LIST
  //  IT'S LINKED INTO, SO THERE'S NO NEED TO SEPARATELY PROCESS THE
  //  SORTED LIST)
  s_list.Clear();

  return 0;
}
