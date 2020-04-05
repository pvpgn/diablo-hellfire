/****************************************************************************
*
*  CHAT.CPP
*
*  This is a simple chat program which uses Storm's networking.
*  It demonstrates enumerating and selecting a network service provider,
*  enumerating and selecting a game, asynchronous messaging, and event
*  handlers.
*
***/

#include <windows.h>
#include <storm.h>
#include <conio.h>
#include <stdio.h>

#define  TITLE       "Chat"
#define  PROGRAMID   'Chat'
#define  VERSIONID   1
#define  MAXPLAYERS  16

typedef struct _GAME {
    int        number;
    DWORD      id;
    char       name[SNET_MAXNAMELENGTH];
    _GAME     *next;
} GAME, *GAMEPTR;

typedef struct _PROVIDER {
    int        number;
    DWORD      id;
    _PROVIDER *next;
} PROVIDER, *PROVIDERPTR;

GAMEPTR     gamehead                       = NULL;
int         games                          = 0;
char        playername[SNET_MAXNAMELENGTH] = "";
char        playerdesc[SNET_MAXDESCLENGTH] = "";
PROVIDERPTR providerhead                   = NULL;
int         providers                      = 0;

//===========================================================================
void Chat () {
  printf("Press escape to exit.\n\n");
  for (;;) {
    if (_kbhit()) {
      char ch = _getch();
      if (ch == 27)
        return;
      SNetSendMessage(SNET_BROADCASTPLAYERID,&ch,sizeof(char));
    }
    LPVOID data      = NULL;
    DWORD  databytes = 0;
    if (SNetReceiveMessage(NULL,&data,&databytes))
      if (data && (databytes == sizeof(char)))
        if (*(char *)data == 13)
          printf("\n");
        else
          printf("%c",*(char *)data);
    Sleep(1);
  }
}

//===========================================================================
BOOL CALLBACK EnumGamesCallback (DWORD  id,
                                 LPCSTR name,
                                 LPCSTR description) {

  // IF THIS GAME IS ALREADY IN THE LIST, IGNORE IT
  GAMEPTR curr = gamehead;
  while (curr)
    if (curr->id == id)
      return 1;
    else
      curr = curr->next;

  // ADD THE GAME TO THE LIST
  GAME game;
  game.number = ++games;
  game.id     = id;
  strncpy(game.name,name,SNET_MAXNAMELENGTH);
  LISTADDEND(&gamehead,&game);

  return 1;
}

//===========================================================================
BOOL CALLBACK EnumProvidersCallback (DWORD       id,
                                     LPCSTR      description,
                                     LPCSTR      requirements,
                                     SNETCAPSPTR caps) {
  PROVIDER provider;
  provider.number = ++providers;
  provider.id     = id;
  LISTADD(&providerhead,&provider);
  printf("[%u] %s\n",providers,description);
  return 1;
}

//===========================================================================
void GetPlayerName () {
  char  username[MAX_PATH]     = "";
  char  computername[MAX_PATH] = "";
  DWORD usernamelength         = MAX_PATH;
  DWORD computernamelength     = MAX_PATH;
  GetUserName(username,&usernamelength);
  GetComputerName(computername,&computernamelength);
  strncpy(playername,username,SNET_MAXNAMELENGTH);
  if (usernamelength+computernamelength+14 < SNET_MAXDESCLENGTH)
    sprintf(playerdesc,"%s on computer %s",username,computername);
}

//===========================================================================
BOOL InitializeProvider (DWORD providerid) {
  SNETPROGRAMDATA programdata;
  ZeroMemory(&programdata,sizeof(SNETPROGRAMDATA));
  programdata.size        = sizeof(SNETPROGRAMDATA);
  programdata.programname = TITLE;
  programdata.programid   = PROGRAMID;
  programdata.versionid   = VERSIONID;
  programdata.maxplayers  = MAXPLAYERS;
  SNETPLAYERDATA playerdata;
  ZeroMemory(&playerdata,sizeof(SNETPLAYERDATA));
  playerdata.size              = sizeof(SNETPLAYERDATA);
  playerdata.playername        = playername;
  playerdata.playerdescription = playerdesc;
  return SNetInitializeProvider(providerid,
                                &programdata,
                                &playerdata,
                                NULL,
                                NULL);
}

//===========================================================================
void CALLBACK OnPlayerJoin (SNETEVENTPTR event) {
  char name[SNET_MAXNAMELENGTH] = "";
  if (SNetGetPlayerName(event->playerid,name,SNET_MAXNAMELENGTH))
    printf("\n%s has entered the room.\n\n",name);
}

//===========================================================================
void CALLBACK OnPlayerLeave (SNETEVENTPTR) {
  printf("\nSomeone has left the room.\n\n");
}

//===========================================================================
int Select (int lowest, int highest) {
  int selection;
  do {
    printf("Choice: ");
    scanf("%u",&selection);
  } while ((selection < lowest) || (selection > highest));
  printf("\n");
  return selection;
}

//===========================================================================
BOOL SelectGame () {

  // BUILD A LIST OF GAMES
  DWORD hintnextcall = 0;
  SNetEnumGames(0,0,EnumGamesCallback,&hintnextcall);

  // SLEEP FOR THE HINTED AMOUNT OF TIME TO MAKE SURE THAT THE PROVIDER'S
  // LIST OF GAMES IS COMPLETE
  Sleep(hintnextcall);

  // UPDATE THE LIST OF GAMES
  SNetEnumGames(0,0,EnumGamesCallback,NULL);

  // DISPLAY THE LIST OF GAMES, ALONG WITH A CHOICE TO CREATE A GAME
  printf("[0] Create Game\n");
  {
    GAMEPTR curr = gamehead;
    while (curr) {
      printf("[%u] %s\n",curr->id,curr->name);
      curr = curr->next;
    }
  }

  // GET THE SELECTION
  int selection = Select(0,games);

  // IF THE USER SELECTED THE CREATE GAME OPTION, THEN CREATE A NEW GAME
  // USING THE PLAYER NAME AND DESCRIPTION AS THE GAME NAME AND DESCRIPTION
  BOOL  success;
  DWORD playerid = 0;
  if (!selection)
    success = SNetCreateGame(playername,
                             NULL,
                             playerdesc,
                             0,
                             NULL,
                             0,
                             MAXPLAYERS,
                             playername,
                             playerdesc,
                             &playerid);

  // OTHERWISE, JOIN THE GAME THAT THE USER SELECTED
  else {
    GAMEPTR curr = gamehead;
    while (curr && (curr->number != selection))
      curr = curr->next;
    if (curr)
      success = SNetJoinGame(curr->id,
                             NULL,
                             NULL,
                             playername,
                             playerdesc,
                             &playerid);
  }

  // FREE THE LIST OF GAMES
  LISTCLEAR(&gamehead);

  return success;
}

//===========================================================================
DWORD SelectProvider () {

  // BUILD AND DISPLAY THE LIST OF PROVIDERS
  if (!SNetEnumProviders(NULL,EnumProvidersCallback))
    return 0;

  // SELECT A PROVIDER FROM THE LIST
  int selection = Select(0,providers);

  // FIND THE PROVIDER'S ID AND FREE THE LIST OF PROVIDERS
  DWORD id = 0;
  while (providerhead) {
    if (providerhead->number == selection)
      id = providerhead->id;
    LISTFREE(&providerhead,providerhead);
  }

  return id;
}

//===========================================================================
int __cdecl main () {
  GetPlayerName();
  if (!InitializeProvider(SelectProvider())) {
    printf("ERROR: Unable to initialize network provider\n");
    return 1;
  }
  if (!SelectGame()) {
    printf("ERROR: Unable to join game\n");
    return 1;
  }
  SNetRegisterEventHandler(SNET_EVENT_PLAYERJOIN ,OnPlayerJoin);
  SNetRegisterEventHandler(SNET_EVENT_PLAYERLEAVE,OnPlayerLeave);
  Chat();
  SNetLeaveGame(0);
  SNetDestroy();
  return 0;
}
