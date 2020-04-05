/****************************************************************************
*
*  BASIC.CPP
*
*  This sample demonstrates basic linked list operation using Storm's
*  list manager.
*
***/

#include <windows.h>
#include <stdio.h>
#include <storm.h>

NODEDECL(TEST) {
    LPCSTR string;
} *TESTPTR;

static LIST(TEST) s_list;

#define  TESTSTRINGS  5

static const LPCSTR s_teststring[TESTSTRINGS] =
    {"one","two","three","four","five"};

//===========================================================================
int __cdecl main () {

  // ALLOCATE AND INITIALIZE THE NODES
  for (int loop = 0; loop < TESTSTRINGS; ++loop) {
    TESTPTR newnode = s_list.NewNode();
    newnode->string = s_teststring[loop];
  }

  // DUMP THE LIST CONTENTS TO THE SCREEN
  ITERATELIST(TEST,s_list,curr)
    printf("%s\n",curr->string);

  // DELETE ALL NODES
  s_list.Clear();

  return 0;
}
