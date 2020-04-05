/****************************************************************************
*
*  LIST.CPP
*
*  This simple command line program can be used to check an S-code string for
*  errors, and to list pseudocode for the compiled string.
*
***/

#include <windows.h>
#include <storm.h>
#include <stdio.h>

int __cdecl main () {

  // GET THE S-CODE STRING
  printf("Enter S-code string: ");
  char scodestring[256];
  gets(scodestring);
  printf("\n");

  // DO A TEST COMPILE TO CHECK FOR ERRORS
  {
    HSCODESTREAM handle;
    const char  *firsterror = NULL;
    if (SCodeCompile(NULL,scodestring,&firsterror,1,0,&handle))
      SCodeDelete(handle);
    else if (firsterror) {
      printf("%s\n%*s^\n",scodestring,firsterror-scodestring,"");
      return 0;
    }
  }

  // COMPILE IT INTO PSEUDOCODE AND LIST THE PSEUDOCODE
  {
    char buffer[1024] = "";
    SCodeGetPseudocode(scodestring,buffer,1024);
    printf("%s\n",buffer);
  }

  return 0;
}
