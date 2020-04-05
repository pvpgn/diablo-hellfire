/****************************************************************************
*
*  RPG.CPP
*
*  This is a simple game which demonstrates the use of synchonous networking.
*  It is basically an Ultima IV style tiled graphics engine, with Warcraft
*  style mouse controls.
*
*  At the beginning of the game, each player sends out two null orders.
*  During the game, players send out orders as they are received from the
*  user (a sample order would be "move to this location"), and fabricate
*  null orders as necessary to make sure there are always at least two in
*  transit.
*
*  Each turn, each player waits until he has received the next set of orders
*  from all players, and then executes all orders, starting at the lowest
*  player number.  Since orders are processed at the same time and in the
*  same order on every computer in the network, the game stays in sync.
*
***/

#include <windows.h>
#include <storm.h>
#include "bnetart.h"

#define  PROGRAMID        'RPG1'
#define  VERSIONID        1
#define  MAXPLAYERS       8
#define  TITLE            "RPG Example"
#define  ARTDIR           "art\\"
#define  NETARTDIR        "netart\\"

#define  ORDER_NULL       0
#define  ORDER_START      1
#define  ORDER_MOVE       2
#define  ORDER_QUIT       3

#define  TILE_DEEPSEA     0
#define  TILE_SEA         1
#define  TILE_SHALLOWSEA  2
#define  TILE_SWAMP       3
#define  TILE_GRASS       4
#define  TILE_BRUSH       5
#define  TILE_FOREST      6
#define  TILE_HILLS       7
#define  TILE_MOUNTAINS   8
#define  TILE_DUNGEON     9
#define  TILE_TOWN        10
#define  TILE_CASTLE      11
#define  TILE_VILLAGE     12

#define  SCREENCX         640
#define  SCREENCY         480
#define  SCREENTILESX     20
#define  SCREENTILESY     15
#define  SCROLLCX         672
#define  SCROLLCY         512
#define  SCROLLTILESX     21
#define  SCROLLTILESY     16
#define  TILES            16
#define  TILECX           32
#define  TILECY           32
#define  WORLDMAPCX       256
#define  WORLDMAPCY       256

LPBYTE       cursor       = NULL;
POINT        cursorpos    = {0,0};
DWORD        localplayer  = 0;
POINT        orderpos[MAXPLAYERS];
PALETTEENTRY pe[256];
LPBYTE       player       = NULL;
POINT        playerpos[MAXPLAYERS];
DWORD        players      = 0;
LPBYTE       scrollbuffer = NULL;
POINT        scrollpos;
POINT        scrollvel    = {0,0};
BOOL         started      = 0;
LPBYTE       tileset      = NULL;
LPBYTE       worldmap     = NULL;

BOOL CALLBACK CreateCallback (SNETCREATEDATAPTR   createdata,
                              SNETPROGRAMDATAPTR  programdata,
                              SNETPLAYERDATAPTR   playerdata,
                              SNETUIDATAPTR       interfacedata,
                              SNETVERSIONDATAPTR  versiondata,
                              DWORD              *playerid);
BOOL LoadGraphicsData ();
void MovePlayer (DWORD player);
void CALLBACK OnButtonDown (LPPARAMS params);
void CALLBACK OnClose (LPPARAMS params);
void CALLBACK OnEraseBkgnd (LPPARAMS params);
void CALLBACK OnPaint (LPPARAMS params);
void CALLBACK OnVkEscape (LPPARAMS params);
void CALLBACK OnVkSpace (LPPARAMS params);
BOOL ProcessOrders ();
BOOL UpdateScrollVelocity ();

//===========================================================================
BOOL CALLBACK ArtCallback (DWORD           providerid,
                           DWORD           artid,
                           LPPALETTEENTRY  pe,
                           LPBYTE          buffer,
                           DWORD           buffersize,
                           int            *width,
                           int            *height,
                           int            *bitdepth) {

	char filename[MAX_PATH] = "";

	if (providerid == 'BNET') {
		// Check for battlenet specific art
		switch (artid) {
		
			case SNET_ART_BACKGROUND:		strcpy(filename,NETARTDIR "chat_bkg.pcx");  break;
			case SNET_ART_BATTLE_LISTBOX:	strcpy(filename, NETARTDIR "bnlist.pcx");  break;
			
			case SNET_ART_BATTLE_BTNS:		strcpy(filename, NETARTDIR "bnbuttns.pcx"); break;

			case SNET_ART_BATTLE_HELP_BKG:	strcpy(filename, NETARTDIR "lpopup.pcx"); break;
			case SNET_ART_JOINBACKGROUND:	strcpy(filename, NETARTDIR "bnjoinbg.pcx"); break;

			case SNET_ART_BATTLE_REDLAG:	strcpy(filename, NETARTDIR "redlag.pcx");  break;
			case SNET_ART_BATTLE_YELLOWLAG:	strcpy(filename, NETARTDIR "yellolag.pcx");  break;
			case SNET_ART_BATTLE_GREENLAG:	strcpy(filename, NETARTDIR "greenlag.pcx");  break;
		}

	}
	else if ((providerid == 'IPXN') || (providerid == 'TEST')) {
		switch (artid) {
			case SNET_ART_BACKGROUND:		strcpy(filename, NETARTDIR "ipx_bkg.pcx"); break;
		}
	}
	else if (providerid == 0) {
		switch (artid) {
			case SNET_ART_BACKGROUND:		strcpy(filename, NETARTDIR "connect.pcx"); break;
		}
	}

	if (!filename[0]) {
		// Check for generic artwork
		switch (artid) {
			case SNET_ART_JOINBACKGROUND:	// fall through
			case SNET_ART_BACKGROUND:		strcpy(filename,NETARTDIR "menu.pcx");  break;
			case SNET_ART_BUTTONTEXTURE:	strcpy(filename,NETARTDIR "but_lrg.pcx");  break;
			case SNET_ART_POPUPBACKGROUND:	strcpy(filename,NETARTDIR "lpopup.pcx");  break;
			case SNET_ART_HELPBACKGROUND:	strcpy(filename,NETARTDIR "lpopup.pcx"); break;

			case SNET_ART_BUTTON_XSML:		strcpy(filename,NETARTDIR "but_xsm.pcx"); break;
			case SNET_ART_BUTTON_SML:		strcpy(filename,NETARTDIR "but_sml.pcx"); break;
			case SNET_ART_BUTTON_MED:		strcpy(filename,NETARTDIR "but_med.pcx"); break;
			case SNET_ART_BUTTON_LRG:		strcpy(filename,NETARTDIR "but_lrg.pcx"); break;
		}
	}

	if (filename[0])
	  return SBmpLoadImage(filename,pe,buffer,buffersize,width,height,bitdepth);
  else
    return 0;

}

//===========================================================================
void ChooseStartingLocations () {
  for (DWORD loop = 0; loop < MAXPLAYERS; ++loop) {
    BOOL found = 0;
    while (!found) {
      int x = rand() % WORLDMAPCX;
      int y = rand() % WORLDMAPCY;
      if (*(worldmap+y*WORLDMAPCX+x) == TILE_GRASS) {
        found = 1;
        for (DWORD loop2 = 1; loop2 < loop; ++loop2)
          if ((playerpos[loop2].x == x) && (playerpos[loop2].y == y))
            found = 0;
        if (found) {
          orderpos[loop].x  = x;
          orderpos[loop].y  = y;
          playerpos[loop].x = x;
          playerpos[loop].y = y;
        }
      }
    }
  }
}

//===========================================================================
BOOL Connect () {

  // BUILD A PROGRAM DATA RECORD
  SNETPROGRAMDATA programdata;
  ZeroMemory(&programdata,sizeof(SNETPROGRAMDATA));
  programdata.size        = sizeof(SNETPROGRAMDATA);
  programdata.programname = TITLE;
  programdata.programid   = PROGRAMID;
  programdata.versionid   = VERSIONID;
  programdata.maxplayers  = MAXPLAYERS;

  // GENERATE A PLAYER NAME
  char  computername[MAX_COMPUTERNAME_LENGTH+1] = "";
  DWORD size = MAX_COMPUTERNAME_LENGTH+1;
  GetComputerName(computername,&size);

  // BUILD A PLAYER DATA RECORD
  SNETPLAYERDATA playerdata;
  ZeroMemory(&playerdata,sizeof(SNETPLAYERDATA));
  playerdata.size       = sizeof(SNETPLAYERDATA);
  playerdata.playername = computername;

  // BUILD AN INTERFACE DATA RECORD
  SNETUIDATA interfacedata;
  ZeroMemory(&interfacedata,sizeof(SNETUIDATA));
  interfacedata.size           = sizeof(SNETUIDATA);
  interfacedata.parentwindow   = SDrawGetFrameWindow();
  interfacedata.artcallback    = ArtCallback;
  interfacedata.createcallback = CreateCallback;

  // SELECT A NETWORK PROVIDER
  while (SNetSelectProvider(NULL,
                            &programdata,
                            &playerdata,
                            &interfacedata,
                            NULL,
                            NULL)) {

    // SELECT A GAME
    if (SNetSelectGame(0,
                       &programdata,
                       &playerdata,
                       &interfacedata,
                       NULL,
                       &localplayer))
      return 1;

  }
  return 0;
}

//===========================================================================
BOOL CALLBACK ConnectIdleProc (DWORD) {
  DWORD currtime = GetTickCount();

  // TWICE A SECOND, UPDATE THE LIST OF PLAYERS
  {
    static DWORD lasttime = 0;
    if (currtime-lasttime > 1000)
      lasttime = currtime-500;
    while (currtime-lasttime >= 500) {

      // DRAW THE LIST
      LPBYTE videobuffer;
      int    videopitch;
      if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
        SGdiSetPitch(videopitch);
        SNetGetNumPlayers(NULL,&players,NULL);
        ++players;
        for (DWORD loop = 0; loop < players; ++loop) {
          char buffer[SNET_MAXNAMELENGTH+2] = "0 ";
          buffer[0] += (char)loop;
          if (SNetGetPlayerName(loop,buffer+2,SNET_MAXNAMELENGTH))
            SGdiTextOut(videobuffer,4,loop*20,PALETTEINDEX(255-loop),buffer);
        }
        SGdiTextOut(videobuffer,440,20,PALETTEINDEX(255),"PRESS SPACE TO START");
        SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
      }

      lasttime += 500;
    }
  }

  // FOUR TIMES A SECOND, PROCESS ORDERS
  {
    static DWORD lasttime = 0;
    if (currtime-lasttime > 1000)
      lasttime = currtime-250;
    while (currtime-lasttime >= 250) {
      if (ProcessOrders())
        lasttime += 250;
      else
        lasttime = GetTickCount();
      currtime = GetTickCount();
    }
  }

  return 1;
}

//===========================================================================
BOOL CALLBACK CreateCallback (SNETCREATEDATAPTR   createdata,
                              SNETPROGRAMDATAPTR  programdata,
                              SNETPLAYERDATAPTR   playerdata,
                              SNETUIDATAPTR       interfacedata,
                              SNETVERSIONDATAPTR  versiondata,
                              DWORD              *playerid) {
  return SNetCreateGame(playerdata->playername,
                        NULL,
                        NULL,
                        0,
                        NULL,
                        0,
                        createdata->maxplayers,
                        playerdata->playername,
                        NULL,
                        playerid);
}

//===========================================================================
BOOL CreateScrollBuffer () {
  scrollbuffer = (LPBYTE)ALLOC(SCROLLTILESX*TILECX*SCROLLTILESY*TILECY);
  return (scrollbuffer != NULL);
}

//===========================================================================
void Destroy () {
  if (cursor) {
    FREE(cursor);
    cursor = NULL;
  }
  if (player) {
    FREE(player);
    player = NULL;
  }
  if (scrollbuffer) {
    FREE(scrollbuffer);
    scrollbuffer = NULL;
  }
  if (tileset) {
    FREE(tileset);
    tileset = NULL;
  }
  if (worldmap) {
    FREE(worldmap);
    worldmap = NULL;
  }
}

//===========================================================================
void DrawScreen () {
  LPBYTE videobuffer;
  int    pitch;
  if (SDrawLockSurface(SDRAW_SURFACE_BACK,NULL,&videobuffer,&pitch)) {
    int startx = scrollpos.x % SCROLLCX;
    int starty = scrollpos.y % SCROLLCY;

    // BLT FROM THE SCROLL BUFFER ONTO THE BACK SURFACE
    SBltROP3(videobuffer,
             scrollbuffer+starty*SCROLLCX+startx,
             min(SCREENCX,SCROLLCX-startx),
             min(SCREENCY,SCROLLCY-starty),
             pitch,
             SCROLLCX,
             0,
             SRCCOPY);
    if (SCROLLCX-startx < SCREENCX)
      SBltROP3(videobuffer+SCROLLCX-startx,
               scrollbuffer+starty*SCROLLCX,
               SCREENCX+startx-SCROLLCX,
               min(SCREENCY,SCROLLCY-starty),
               pitch,
               SCROLLCX,
               0,
               SRCCOPY);
    if (SCROLLCY-starty < SCREENCY)
      SBltROP3(videobuffer+(SCROLLCY-starty)*pitch,
               scrollbuffer+startx,
               min(SCREENCX,SCROLLCX-startx),
               SCREENCY+starty-SCROLLCY,
               pitch,
               SCROLLCX,
               0,
               SRCCOPY);
    if ((SCROLLCX-startx < SCREENCX) &&
        (SCROLLCY-starty < SCREENCY))
      SBltROP3(videobuffer+(SCROLLCY-starty)*pitch+SCROLLCX-startx,
               scrollbuffer,
               SCREENCX+startx-SCROLLCX,
               SCREENCY+starty-SCROLLCY,
               pitch,
               SCROLLCX,
               0,
               SRCCOPY);

    // DRAW THE MOUSE CURSOR ONTO THE BACK SURFACE
    if (started) {
      GetCursorPos(&cursorpos);
      SBltROP3(videobuffer+cursorpos.y*pitch+cursorpos.x,
               cursor,
               min(32,SCREENCX-cursorpos.x),
               min(32,SCREENCY-cursorpos.y),
               pitch,
               32,
               0,
               SRCPAINT);
    }

    SDrawUnlockSurface(SDRAW_SURFACE_BACK,videobuffer);
    SDrawFlipPage();
  }
}

//===========================================================================
void FillScrollBuffer (LPRECT rect) {
  int startxtile = scrollpos.x/TILECX;
  int startytile = scrollpos.y/TILECY;
  int startxpos  = (scrollpos.x % SCROLLCX)/TILECX;
  int startypos  = (scrollpos.y % SCROLLCY)/TILECY;
  for (int y = rect->top; y <= rect->bottom; ++y) {
    int mapy = startytile+y+((y >= startypos) ? 0 : SCROLLTILESY)-startypos;
    for (int x = rect->left; x <= rect->right; ++x) {
      int mapx = startxtile+x+((x >= startxpos) ? 0 : SCROLLTILESX)-startxpos;
      int tile = min(TILES-1,*(worldmap+mapy*WORLDMAPCX+mapx));
      SBltROP3(scrollbuffer+y*TILECY*SCROLLCX+x*TILECX,
               tileset+tile*TILECX,
               TILECX,
               TILECY,
               SCROLLCX,
               TILES*TILECX,
               0,
               SRCCOPY);
    }
  }
  for (DWORD loop = 0; loop < players; ++loop) {
    if ((playerpos[loop].x >= startxtile) &&
        (playerpos[loop].y >= startytile) &&
        (playerpos[loop].x <  startxtile+SCROLLTILESX) &&
        (playerpos[loop].y <  startytile+SCROLLTILESY)) {
      int posx = (startxpos+(playerpos[loop].x-startxtile)) % SCROLLTILESX;
      int posy = (startypos+(playerpos[loop].y-startytile)) % SCROLLTILESY;
      if ((posx >= rect->left) && (posx <= rect->right) &&
          (posy >= rect->top) && (posy <= rect->bottom)) {
        DWORD pattern = ((255-loop) << 24) | ((255-loop) << 16) | ((255-loop) << 8) | (255-loop);
        // pattern 11110000
        //  source 11001100
        //    dest 10101010
        //         --------
        //     rop 11100010 = 0xE2
        SBltROP3(scrollbuffer+posy*TILECY*SCROLLCX+posx*TILECX,
                 player,
                 TILECX,
                 TILECY,
                 SCROLLCX,
                 TILECX,
                 pattern,
                 0xE20746);
      }
    }
  }
}

//===========================================================================
BOOL CALLBACK GameIdleProc (DWORD) {
  DWORD currtime = GetTickCount();

  // FOUR TIMES A SECOND, MOVE THE PLAYERS
  {
    static DWORD lasttime = GetTickCount();
    if (currtime-lasttime > 1000)
      lasttime = currtime-250;
    while (currtime-lasttime >= 250) {
      for (DWORD loop = 0; loop < players; ++loop)
        MovePlayer(loop);
      RECT rect = {0,0,SCROLLTILESX-1,SCROLLTILESY-1};
      FillScrollBuffer(&rect);
      lasttime += 250;
    }
  }

  // TWICE A SECOND, PROCESS ORDERS
  {
    static DWORD lasttime = GetTickCount();
    if (currtime-lasttime > 2000)
      lasttime = currtime-500;
    while (currtime-lasttime >= 500) {
      if (ProcessOrders())
        lasttime += 500;
      else
        lasttime = GetTickCount();
      currtime = GetTickCount();
    }
  }

  // PROCESS SCROLLING
  if (UpdateScrollVelocity()) {

    // UPDATE THE SCROLL POSITION
    POINT oldscrollpos = scrollpos;
    scrollpos.x = max(0,min(WORLDMAPCX*TILECX-SCROLLCX,scrollpos.x+scrollvel.x));
    scrollpos.y = max(0,min(WORLDMAPCY*TILECY-SCROLLCY,scrollpos.y+scrollvel.y));

    // UPDATE THE SCROLL BUFFER
    if (scrollpos.x/TILECX != oldscrollpos.x/TILECX) {
      int updatex = (scrollvel.x > 0) ? (oldscrollpos.x % SCROLLCX)/TILECX
                                      : (scrollpos.x % SCROLLCX)/TILECX;
      RECT rect = {updatex,0,updatex,SCROLLTILESY-1};
      FillScrollBuffer(&rect);
    }
    if (scrollpos.y/TILECY != oldscrollpos.y/TILECY) {
      int updatey = (scrollvel.y > 0) ? (oldscrollpos.y % SCROLLCY)/TILECY
                                      : (scrollpos.y % SCROLLCY)/TILECY;
      RECT rect = {0,updatey,SCROLLTILESX-1,updatey};
      FillScrollBuffer(&rect);
    }

  }

  // DRAW THE SCREEN
  DrawScreen();

  return 1;
}

//===========================================================================
BOOL CALLBACK IdleProc (DWORD count) {
  if (started)
    return GameIdleProc(count);
  else
    return ConnectIdleProc(count);
}

//===========================================================================
void IssueOrder (LPDWORD paramarray, DWORD paramcount) {
  SNetSendTurn(paramarray,
               paramcount*sizeof(DWORD));
}

//===========================================================================
BOOL Initialize (HINSTANCE instance) {
  if (!SDrawAutoInitialize(instance,
                           TITLE "_CLASS",
                           TITLE,
                           NULL,
                           SDRAW_SERVICE_PAGEFLIP)) {
    SDrawMessageBox("Unable to initialize DirectDraw.",TITLE,0);
    return 0;
  }
  if (!LoadGraphicsData()) {
    SDrawMessageBox("Unable to load graphics data.",TITLE,0);
    return 0;
  }
  if (!CreateScrollBuffer()) {
    SDrawMessageBox("Out of memory.",TITLE,0);
    return 0;
  }
  SMsgRegisterMessage(NULL,WM_ERASEBKGND,OnEraseBkgnd);
  if (!Connect())
    return 0;
  ChooseStartingLocations();
  scrollpos.x = max(0,min(WORLDMAPCX*TILECX-SCROLLCX,playerpos[localplayer].x*TILECX-SCROLLCX/2));
  scrollpos.y = max(0,min(WORLDMAPCY*TILECY-SCROLLCY,playerpos[localplayer].y*TILECY-SCROLLCY/2));
  SDrawUpdatePalette(0,256,&pe[0]);
  SMsgRegisterMessage(NULL,WM_CLOSE      ,OnClose);
  SMsgRegisterMessage(NULL,WM_LBUTTONDOWN,OnButtonDown);
  SMsgRegisterMessage(NULL,WM_PAINT      ,OnPaint);
  SMsgRegisterMessage(NULL,WM_RBUTTONDOWN,OnButtonDown);
  SMsgRegisterKeyDown(NULL,VK_ESCAPE     ,OnVkEscape);
  SMsgRegisterKeyDown(NULL,VK_SPACE      ,OnVkSpace);
  return 1;
}

//===========================================================================
BOOL LoadGraphicsData () {

  // LOAD IN THE CURSOR
  cursor = (LPBYTE)ALLOC(32*32);
  if (!cursor)
    return 0;
  if (!SBmpLoadImage(ARTDIR "cursor.pcx",NULL,cursor,32*32))
    return 0;

  // LOAD IN THE PLAYER IMAGE
  player = (LPBYTE)ALLOC(32*32);
  if (!player)
    return 0;
  if (!SBmpLoadImage(ARTDIR "player.pcx",NULL,player,32*32))
    return 0;

  // LOAD IN THE WORLD MAP
  worldmap = (LPBYTE)ALLOC(WORLDMAPCX*WORLDMAPCY);
  if (!worldmap)
    return 0;
  if (!SBmpLoadImage(ARTDIR "map.pcx",NULL,worldmap,WORLDMAPCX*WORLDMAPCY))
    return 0;

  // LOAD IN THE GRAPHICS TILESET
  tileset = (LPBYTE)ALLOC(TILES*TILECX*TILECY);
  if (!tileset)
    return 0;
  if (!SBmpLoadImage(ARTDIR "tileset.pcx",&pe[0],tileset,TILES*TILECX*TILECY))
    return 0;

  // CREATE A FONT
  HSGDIFONT font;
  {
    HFONT winfont = CreateFont(-17,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,
                               OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                               VARIABLE_PITCH | FF_ROMAN,TEXT("Times New Roman"));
    if (!SGdiImportFont(winfont,&font))
      return 0;
    DeleteObject(winfont);
  }
  if (!SGdiSelectObject(font))
    return 0;

  return 1;
}

//===========================================================================
void MovePlayer (DWORD player) {
  int xdir = orderpos[player].x-playerpos[player].x;
  xdir = (xdir > 0) ? 1 : xdir ? -1 : 0;
  int ydir = orderpos[player].y-playerpos[player].y;
  ydir = (ydir > 0) ? 1 : ydir ? -1 : 0;
  if (xdir) {
    BYTE tile = *(worldmap+playerpos[player].y*WORLDMAPCX+playerpos[player].x+xdir);
    if ((tile >= TILE_GRASS) && (tile != TILE_MOUNTAINS))
      playerpos[player].x += xdir;
  }
  if (ydir) {
    BYTE tile = *(worldmap+(playerpos[player].y+ydir)*WORLDMAPCX+playerpos[player].x);
    if ((tile >= TILE_GRASS) && (tile != TILE_MOUNTAINS))
      playerpos[player].y += ydir;
  }
}

//===========================================================================
void CALLBACK OnButtonDown (LPPARAMS params) {
  if (started) {
    DWORD order[3] = {ORDER_MOVE,
                      (scrollpos.x+LOWORD(params->lparam))/TILECX,
                      (scrollpos.y+HIWORD(params->lparam))/TILECY};
    IssueOrder(&order[0],3);
  }
}

//===========================================================================
void CALLBACK OnClose (LPPARAMS) {
  Destroy();
}

//===========================================================================
void CALLBACK OnEraseBkgnd (LPPARAMS params) {
  params->useresult = 1;
  params->result    = 1;
}

//===========================================================================
void CALLBACK OnPaint (LPPARAMS params) {
  if (scrollbuffer && tileset) {
    RECT rect = {0,0,SCROLLTILESX-1,SCROLLTILESY-1};
    FillScrollBuffer(&rect);
    DrawScreen();
  }
}

//===========================================================================
void CALLBACK OnVkEscape (LPPARAMS) {
  DWORD order = ORDER_QUIT;
  IssueOrder(&order,1);
}

//===========================================================================
void CALLBACK OnVkSpace (LPPARAMS) {
  if (!started) {
    DWORD order = ORDER_START;
    IssueOrder(&order,1);
  }
}

//===========================================================================
BOOL ProcessOrders () {

  // MAKE SURE THERE ARE ALWAYS AT LEAST TWO ORDERS IN TRANSIT
  {
    DWORD turns;
    if (SNetGetTurnsInTransit(&turns)) {
      while (turns < 2) {
        DWORD order = ORDER_NULL;
        IssueOrder(&order,1);
        ++turns;
      }
    }
  }

  // GATHER ORDERS FROM ALL PLAYERS
  LPDWORD data[MAXPLAYERS];
  DWORD   databytes[MAXPLAYERS];
  DWORD   playerstatus[MAXPLAYERS];
  BOOL    insync = 1;
  {
    int timeout = 20;
    while ((!SNetReceiveTurns(0,players,(LPVOID *)&data[0],&databytes[0],&playerstatus[0])) &&
           --timeout) {
      insync = 0;
      Sleep(50);
    }
    if (!timeout) {
      SDrawMessageBox("network error",TITLE,0);
      SDrawPostClose();
    }
  }

  // PROCESS EACH PLAYER'S ORDERS
  for (DWORD player = 0; player < players; ++player)
    if (data[player] && databytes[player])
      switch (*data[player]) {

        case ORDER_START:
          started = 1;
          break;

        case ORDER_MOVE:
          orderpos[player].x = *(data[player]+1);
          orderpos[player].y = *(data[player]+2);
          break;

        case ORDER_QUIT:
          SDrawPostClose();
          break;

      }

  return insync;
}

//===========================================================================
BOOL UpdateScrollVelocity () {

  // DETERMINE THE SCROLL DIRECTION
  POINT pos;
  GetCursorPos(&pos);
  int x    = (pos.x == 639) ? 1 : pos.x ? 0 : -1;
  int y    = (pos.y == 479) ? 1 : pos.y ? 0 : -1;
  int velx = (scrollvel.x > 0) ? 1 : scrollvel.x ? -1 : 0;
  int vely = (scrollvel.y > 0) ? 1 : scrollvel.y ? -1 : 0;

  // UPDATE THE X VELOCITY
  if (x) {
    if ((velx != x) || (abs(scrollvel.x) < 24))
      scrollvel.x += (abs(scrollvel.x) > 12) ? 2*x : x;
  }
  else if (velx)
    scrollvel.x -= velx;

  // UPDATE THE Y VELOCITY
  if (y) {
    if ((vely != y) || (abs(scrollvel.y) < 16))
      scrollvel.y += (abs(scrollvel.y) > 8) ? 2*y : y;
  }
  else if (vely)
    scrollvel.y -= vely;

  return (scrollvel.x || scrollvel.y);
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {
  if (!Initialize(instance)) {
    Destroy();
    return 1;
  }
  return SMsgDoMessageLoop(IdleProc);
}
