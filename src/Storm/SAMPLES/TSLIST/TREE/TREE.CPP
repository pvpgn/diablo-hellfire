/****************************************************************************
*
*  TREE.CPP
*
*  This sample demonstrates using linked lists to implement a tree structure.
*
***/

#include <windows.h>
#include <stdio.h>
#include <storm.h>

NODEDECL(TEST) {
    LPCSTR     string;
    LIST(TEST) children;

    ~TEST () {
      children.Clear();
    }

} *TESTPTR;

static LIST(TEST) s_list;

//===========================================================================
static void DumpListRecursive (LISTPTR(TEST) list, int depth) {
  ITERATELISTPTR(TEST,list,curr) {
    printf("%*s%s\n",depth,"",curr->string);
    DumpListRecursive(&curr->children,depth+1);
  }
}

//===========================================================================
int __cdecl main () {

  // CREATE A TREE STRUCTURE FOR TESTING PURPOSES
  TESTPTR one   = s_list.NewNode();
  TESTPTR two   = one->children.NewNode();
  TESTPTR three = one->children.NewNode();
  TESTPTR four  = one->children.NewNode();
  TESTPTR five  = three->children.NewNode();
  one->string   = "one";
  two->string   = "two";
  three->string = "three";
  four->string  = "four";
  five->string  = "five";

  // DUMP THE TREE TO THE SCREEN
  printf("Original Tree Structure\n");
  DumpListRecursive(&s_list,0);

  // DEMONSTRATE DELETING A NODE
  delete three;

  // DUMP THE MODIFIED TREE TO THE SCREEN
  printf("\nAfter Deleting Node Three\n");
  DumpListRecursive(&s_list,0);

  // DELETE ALL REMAINING NODES
  s_list.Clear();

  return 0;
}
