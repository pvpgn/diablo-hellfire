/****************************************************************************
*
*  JOIN.CPP
*
*  This program demonstrates the SNetUi functions used for selecting a
*  network provider and joining a game.
*
***/

#include <windows.h>
#include <storm.h>

#define  TITLE       "SNetUi Sample Program"
#define  PROGRAMID   'Join'
#define  VERSIONID   1
#define  MAXPLAYERS  8

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
  if (providerid == 'BTLN')
    switch (artid) {
      case SNET_ART_BACKGROUND:    strcpy(filename,"bnbkg.pcx"   );  break;
      case SNET_ART_BUTTONTEXTURE: strcpy(filename,"bnbutton.pcx");  break;
    }
  else
    switch (artid) {
      case SNET_ART_BACKGROUND:    strcpy(filename,"bkg.pcx"     );  break;
      case SNET_ART_BUTTONTEXTURE: strcpy(filename,"button.pcx"  );  break;
    }
  if (filename[0])
    return SBmpLoadImage(filename,pe,buffer,buffersize,width,height,bitdepth);
  else
    return 0;
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR cmdline, int) {

  // INITIALIZE DIRECTDRAW
  if (!SDrawAutoInitialize(instance,
                           TITLE,
                           TITLE))
    return 1;

  // BUILD A PROGRAM DATA STRUCTURE
  SNETPROGRAMDATA programdata;
  ZeroMemory(&programdata,sizeof(SNETPROGRAMDATA));
  programdata.size        = sizeof(SNETPROGRAMDATA);
  programdata.programname = TITLE;
  programdata.programid   = PROGRAMID;
  programdata.versionid   = VERSIONID;
  programdata.maxplayers  = MAXPLAYERS;

  // BUILD A PLAYER DATA STRUCTURE
  SNETPLAYERDATA playerdata;
  ZeroMemory(&playerdata,sizeof(SNETPLAYERDATA));
  playerdata.size              = sizeof(SNETPLAYERDATA);
  playerdata.playername        = "PlayerName";
  playerdata.playerdescription = "PlayerDesc";

  // BUILD AN INTERFACE DATA STRUCTURE
  SNETUIDATA interfacedata;
  ZeroMemory(&interfacedata,sizeof(SNETUIDATA));
  interfacedata.size         = sizeof(SNETUIDATA);
  interfacedata.parentwindow = SDrawGetFrameWindow();
  interfacedata.artcallback  = ArtCallback;

  // SELECT A NETWORK PROVIDER
  DWORD providerid;
  if (!SNetSelectProvider(NULL,
                          &programdata,
                          &playerdata,
                          &interfacedata,
                          NULL,
                          &providerid))
    return 1;

  // SELECT A GAME
  DWORD playerid;
  if (!SNetSelectGame(0,
                      &programdata,
                      &playerdata,
                      &interfacedata,
                      NULL,
                      &playerid))
    return 1;

  // SHUTDOWN
  StormDestroy();

  return 0;
}
