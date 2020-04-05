/****************************************************************************
*
*  PONG.CPP
*
*  This is a simple game which demonstrates the use of asynchronous
*  messaging.  It is sort of a cross between Pong and Breakout.  Players
*  bounce balls off of bricks in the center of the screen, and when they
*  have eliminated enough bricks, they try to bounce their ball into the
*  opponent's territory in the hopes he will miss it.
*
*  Pong is actually a good example of a game in which it is very difficult to
*  support an arbitrary latency network.  You cannot simply model the ball
*  locally and accept periodic updates from the remote player, because the
*  ball's trajectory will change significantly every time the remote player
*  hits it, depending on exactly where his paddle was at the moment of
*  contact.  Therefore, what this implementation does is take the ball
*  out of play (make it invisible and nonmoving) when it gets to a point
*  where it may or may not have collided with the opponent's paddle, and
*  then waits for a message from the opponent telling whether he hit or
*  missed, and what the new position and trajectory of the ball are.  It can
*  then model the ball locally until the next time it comes into possible
*  contact with the opponent's paddle.
*
*  Despite this, it is still possible for the game to become out of sync,
*  because there are two balls in play.  When one of them is taken out of
*  play pending a message from the opponent, the other one continues to
*  move.  On a high latency network, the end result is that, one the local
*  system, the ball that the local player just hit will tend to be slightly
*  farther along than the ball the remote player just hit, and on the remote
*  system, the inverse will be true.  So if the two balls hit the same
*  brick at the same time, each player will think that his own ball was
*  deflected and the opponent's wasn't.  If the balls are deflected straight
*  back to the players and the players both miss, then each player might
*  think that he missed but the opponent didn't.  This is actually no big
*  deal.  Players send out messages saying they missed the ball when they
*  do, and the recipient of the message will always take the player's word
*  for it that he did, in fact, miss the ball.  A worse situation happens
*  if the balls are not deflected straight back to the players, but instead,
*  each ball bounces off another brick and goes on to the opposing player.
*  This would be more serious, as now each player would take the ball out of
*  play while waiting for a message from the opposing player telling whether
*  he hit or missed the ball.  Since neither play would think that the ball
*  came to him, neither player would ever send out such a message.  Just in
*  case this situation ever happens, what Pong does is send out a messsage
*  whenever it thinks that the opponent needs to tell it whether he hit or
*  missed the ball.  When the opponent gets this message, if he finds that
*  he is waiting for the same message from the other player, then he knows
*  that a deadlock has occurred.  He breaks the deadlock by putting the ball
*  back into play with a new position and velocity, and sending that position
*  and velocity to the other player.
*
***/

#include <windows.h>
#include <storm.h>
#include <math.h>
#include <stdio.h>

#define  PROGRAMID        'Pong'
#define  VERSIONID        1
#define  MAXPLAYERS       2
#define  TITLE            "Pong"

#define  BRICKCOLS        7
#define  BRICKROWS        24

#define  SIN(a)           sintable[((a) & 255)]
#define  COS(a)           sintable[(((a)+64) & 255)]

#define  NETID_BALLPOS    1
#define  NETID_CHECKSYNC  2
#define  NETID_MISSED     3
#define  NETID_PADDLEPOS  4
#define  NETID_QUIT       5
#define  NETID_RESTART    6

typedef BYTE ANGLE;
typedef LONG FIXEDINT;

typedef struct _POINTF {
    FIXEDINT x;
    FIXEDINT y;
} POINTF;

HSARCHIVE archive       = (HSARCHIVE)0;
ANGLE     ballangle[2];
POINTF    ballpos[2];
SIZE      ballsize      = {5,5};
int       ballwait[2];
BYTE      brick[BRICKCOLS][BRICKROWS];
BOOL      computer      = 0;
int       lives[2];
int       missed[2];
POINT     paddle[2]     = {{15,240},{624,240}};
SIZE      paddlesize    = {5,40};
DWORD     player        = 0;
FIXEDINT  sintable[256];
FIXEDINT  speed[2];
BOOL      started       = 0;

BOOL CALLBACK CreateCallback (SNETCREATEDATAPTR   createdata,
                              SNETPROGRAMDATAPTR  programdata,
                              SNETPLAYERDATAPTR   playerdata,
                              SNETUIDATAPTR       interfacedata,
                              SNETVERSIONDATAPTR  versiondata,
                              DWORD              *playerid);
void CreateSinTable ();
FIXEDINT FixedMul (FIXEDINT a, FIXEDINT b);
BOOL ProcessNetworkMessages ();
void SendNetworkMessage (LPDWORD paramarray, DWORD paramcount);
void StartBall (int number, DWORD player);
void StartGame ();

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
  switch (artid) {
    case SNET_ART_BACKGROUND:    strcpy(filename,"bkg.pcx"   );  break;
    case SNET_ART_BUTTONTEXTURE: strcpy(filename,"button.pcx");  break;
  }
  if (filename[0])
    return SBmpLoadImage(filename,pe,buffer,buffersize,width,height,bitdepth);
  else
    return 0;
}

//===========================================================================
BOOL Bounce (ANGLE *angle, int xdir, int ydir) {
  BOOL bounced = 0;
  if (((xdir < 0) && ((ANGLE)(64+*angle) < 128)) ||
      ((xdir > 0) && ((ANGLE)(64+*angle) > 128))) {
    *angle  = 192-(64+*angle);
    bounced = 1;
  }
  if (((ydir < 0) && (*angle < 128)) ||
      ((ydir > 0) && (*angle > 128))) {
    *angle  = -*angle;
    bounced = 1;
  }
  return bounced;
}

//===========================================================================
void ComputerMovePaddle () {
  BOOL consider[2];
  int  ball;
  for (ball = 0; ball < 2; ++ball)
    consider[ball] = ((ballwait[ball] < 0) &&
                      ((ballpos[ball].x > 320*0x10000) == (int)player) &&
                      (((ANGLE)(ballangle[ball]+64) < 128) == (int)player));
  if (!(consider[0] ^ consider[1]))
    ball = ((ballpos[0].x > ballpos[1].x) == (int)player) ? 0 : 1;
  else if (consider[0])
    ball = 0;
  else 
    ball = 1;
  if (ball >= 0) {
    int y = ballpos[ball].y/0x10000;
    y -= (y-240)*paddlesize.cy/240;
    if (y < paddle[player].y)
      paddle[player].y -= min(12,paddle[player].y-y);
    else if (y > paddle[player].y)
      paddle[player].y += min(12,y-paddle[player].y);
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
                       &player))
      return 1;

  }
  return 0;
}

//===========================================================================
void ConnectIdleProc () {

  // IF WE ARE STILL WAITING TO CONNECT, UPDATE THE WAITING BANNER AND RETURN
  {
    DWORD activeplayers = 0;
    SNetGetNumPlayers(NULL,NULL,&activeplayers);
    if (activeplayers < 2) {
      static BYTE intensity = 0;
      static int  direction = 1;
      PALETTEENTRY pe = {intensity,intensity,0x80+(intensity >> 1),0};
      SDrawUpdatePalette(128,1,&pe);
      intensity += direction;
      if (intensity >= 127)
        direction = -1;
      else if (!intensity)
        direction = 1;
      Sleep(1);
      return;
    }
  }

  // START THE GAME
  CreateSinTable();
  StartGame();
  started = 1;

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
                        programdata->maxplayers,
                        playerdata->playername,
                        NULL,
                        playerid);
}

//===========================================================================
void CreateSinTable () {
  for (int loop = 0; loop < 256; ++loop) {
    double angle = (loop*3.14159265359)/128.0;
    sintable[loop] = (FIXEDINT)(sin(angle)*0x10000);
  }
}

//===========================================================================
void DrawScreen () {
  LPBYTE videobuffer;
  int    videopitch;
  SDrawClearSurface(SDRAW_SURFACE_BACK);
  if (SDrawLockSurface(SDRAW_SURFACE_BACK,NULL,&videobuffer,&videopitch)) {
    SGdiSetPitch(videopitch);

    // DRAW THE PADDLES
    {
      for (int loop = 0; loop <= 1; ++loop)
        if (!missed[loop])
          SGdiRectangle(videobuffer,
                        paddle[loop].x-paddlesize.cx,
                        paddle[loop].y-paddlesize.cy,
                        paddle[loop].x+paddlesize.cx,
                        paddle[loop].y+paddlesize.cy,
                        PALETTEINDEX(255));
    }

    // DRAW THE BRICKS
    {
      for (int row = 0; row < BRICKROWS; ++row)
        for (int col = 0; col < BRICKCOLS; ++col)
          if (brick[col][row])
            SGdiRectangle(videobuffer,
                          col*25+240,
                          row*20+2,
                          col*25+250,
                          row*20+17,
                          PALETTEINDEX(248+col));
    }

    // DRAW THE BALLS
    {
      for (int loop = 0; loop <= 1; ++loop)
        if (ballwait[loop] < 0)
          SGdiRectangle(videobuffer,
                        (ballpos[loop].x/0x10000)-ballsize.cx,
                        (ballpos[loop].y/0x10000)-ballsize.cy,
                        (ballpos[loop].x/0x10000)+ballsize.cx,
                        (ballpos[loop].y/0x10000)+ballsize.cy,
                        PALETTEINDEX(255));
    }

    // DRAW LIVES LEFT
    {
      for (DWORD loop = 0; loop <= 1; ++loop)
        if (missed[loop])
          if (lives[loop]) {
            char buffer[64];
            wsprintf(buffer,
                     "%u %s LEFT",
                     lives[loop],
                     (lives[loop] == 1) ? "LIFE" : "LIVES");
            SGdiTextOut(videobuffer,
                        loop*400+100,
                        235,
                        PALETTEINDEX(255),
                        buffer);
          }
          else if (loop == player)
            SGdiTextOut(videobuffer,
                        loop*400+100,
                        235,
                        PALETTEINDEX(255),
                        "LOSER!");
          else
            SGdiTextOut(videobuffer,
                        (!loop)*400+100,
                        235,
                        PALETTEINDEX(255),
                        "WINNER!");
    }

    SDrawUnlockSurface(SDRAW_SURFACE_BACK,videobuffer);
    SDrawFlipPage();
  }
}

//===========================================================================
void ExecuteGameLoop () {

  // MOVE THE BALLS
  {
    for (int loop = 0; loop < 2; ++loop)
      if (ballwait[loop] < 0) {
        POINT lastpos = {ballpos[loop].x/0x10000,
                         ballpos[loop].y/0x10000};
        POINT nextpos = {(ballpos[loop].x+FixedMul(COS(ballangle[loop]),speed[loop]))/0x10000,
                         (ballpos[loop].y+FixedMul(SIN(ballangle[loop]),speed[loop]))/0x10000};

        // IF THE BALL HAS MOVED TO WHERE IT MAY OR MAY NOT HAVE COLLIDED
        // WITH THE REMOTE PLAYER'S PADDLE, PUT THE BALL ON HOLD UNTIL WE
        // FIND OUT FROM THE REMOTE PLAYER WHAT HAPPENED
        if ((  player  && (nextpos.x-ballsize.cx < paddle[0].x+paddlesize.cx)) ||
            ((!player) && (nextpos.x+ballsize.cx > paddle[1].x-paddlesize.cx))) {
          ballwait[loop] = !player;
          DWORD paramarray[2] = {NETID_CHECKSYNC,loop};
          SendNetworkMessage(&paramarray[0],2);
          continue;
        }

        // CHECK FOR COLLISION WITH THE LOCAL PLAYER'S PADDLE
        if ((!missed[player]) &&
            (nextpos.x-ballsize.cx < paddle[player].x+paddlesize.cx) &&
            (nextpos.x+ballsize.cx > paddle[player].x-paddlesize.cx) &&
            (nextpos.y-ballsize.cy < paddle[player].y+paddlesize.cy) &&
            (nextpos.y+ballsize.cy > paddle[player].y-paddlesize.cy)) {
          int ydir = 0;
          if (nextpos.y-ballsize.cy < paddle[player].y-paddlesize.cy)
            ydir = -1;
          else if (nextpos.y+ballsize.cy > paddle[player].y+paddlesize.cy)
            ydir = 1;
          if (Bounce(&ballangle[loop],player ? -1 : 1,ydir)) {
            ANGLE target = (ANGLE)((nextpos.y-paddle[player].y)*32/paddlesize.cy);
            if (player)
              target = 128-target;
            ballangle[loop] += ((char)(target-ballangle[loop]))/2;
            DWORD paramarray[6] = {NETID_BALLPOS,
                                   loop,
                                   ballpos[loop].x,
                                   ballpos[loop].y,
                                   ballangle[loop],
                                   speed[loop]};
            SendNetworkMessage(&paramarray[0],6);
            continue;
          }
        }

        // CHECK FOR COLLISION WITH THE EDGE OF THE SCREEN, INDICATING THAT
        // THE PLAYER MISSED THE BALL
        if ((  player  && (nextpos.x+ballsize.cx > 639)) ||
            ((!player) && (nextpos.x-ballsize.cx < 0))) {
          ballwait[loop] = player;
          if (missed[player])
            missed[player] = 30;
          else {
            --lives[player];
            missed[player] = 100;
          }
          DWORD paramarray[4] = {NETID_MISSED,
                                 loop,
                                 missed[player],
                                 lives[player]};
          SendNetworkMessage(&paramarray[0],4);
          continue;
        }

        // CHECK FOR COLLISION WITH THE TOP OR BOTTOM OF THE SCREEN
        if (nextpos.y-ballsize.cy < 0)
          Bounce(&ballangle[loop],0,1);
        if (nextpos.y+ballsize.cy > 479)
          Bounce(&ballangle[loop],0,-1);

        // CHECK FOR COLLISION WITH A BRICK
        {
          for (int col = 0; col < BRICKCOLS; ++col)
            if ((nextpos.x-ballsize.cx <= col*25+250) &&
                (nextpos.x+ballsize.cx >= col*25+240))
              for (int row = 0; row < BRICKROWS; ++row)
                if (brick[col][row] &&
                    ((nextpos.y-ballsize.cy <= row*20+17) &&
                     (nextpos.y+ballsize.cy >= row*20+2))) {
                  int xdir = 0;
                  int ydir = 0;
                  if ((lastpos.x+ballsize.cx <  col*25+240) &&
                      (nextpos.x+ballsize.cx >= col*25+240))
                    xdir = -1;
                  else if ((lastpos.x-ballsize.cx >  col*25+250) &&
                           (nextpos.x-ballsize.cx <= col*25+250))
                    xdir = 1;
                  if ((lastpos.y+ballsize.cy <  row*20+2) &&
                      (nextpos.y+ballsize.cy >= row*20+2))
                    ydir = -1;
                  else if ((lastpos.y-ballsize.cy >  row*20+17) &&
                           (nextpos.y-ballsize.cy <= row*20+17))
                    ydir = 1;
                  Bounce(&ballangle[loop],xdir,ydir);
                  brick[col][row] = 0;
                }
        }

        ballpos[loop].x += FixedMul(COS(ballangle[loop]),speed[loop]);
        ballpos[loop].y += FixedMul(SIN(ballangle[loop]),speed[loop]);
        speed[loop]     += 48;
      }
  }

  // IF THE PLAYER RECENTLY MISSED THE BALL, CHECK TO SEE IF HE CAN
  // REENTER THE GAME
  if (missed[player])
    if (missed[player] > 1)
      --missed[player];
    else {
      DWORD side[2];
      for (int loop = 0; loop < 2; ++loop) {
        if (ballwait[loop] >= 0)
          side[loop] = -1;
        else if ((ballpos[loop].x < 320*0x10000) && (COS(ballangle[loop]) < 0))
          side[loop] = 0;
        else if ((ballpos[loop].x > 320*0x10000) && (COS(ballangle[loop]) > 0))
          side[loop] = 1;
        else
          side[loop] = -1;
      }
      if ((side[0] != player) && (side[1] != player)) {
        missed[player] = 0;
        if (ballwait[0] == (int)player) {
          StartBall(0,player);
          if (ballwait[1] == (int)player)
            StartBall(1,!player);
        }
        else if (ballwait[1] == (int)player)
          StartBall(1,player);
      }
    }

  // MOVE THE PLAYER'S PADDLE
  if (computer)
    ComputerMovePaddle();
  else {
    POINT pt;
    GetCursorPos(&pt);
    paddle[player].y = pt.y;
  }
  paddle[player].y = max(paddlesize.cy,min(479-paddlesize.cy,paddle[player].y));

}

//===========================================================================
FIXEDINT FixedMul (FIXEDINT a, FIXEDINT b) {
  return MulDiv(a,b,0x10000);
}

//===========================================================================
void GameIdleProc () {
  DWORD currtime = GetTickCount();

  // PROCESS NETWORK MESSAGES
  BOOL processed = ProcessNetworkMessages();

  // EXECUTE THE GAME LOOP 60 TIMES PER SECOND
  if (lives[0] && lives[1]) {
    static DWORD lasttime = GetTickCount();
    static DWORD cycle    = 0;
    if (currtime-lasttime > 1000)
      lasttime = currtime;
    else
      while (currtime-lasttime >= 17) {
        ExecuteGameLoop();
        processed = 1;
        lasttime += cycle++ ? 17 : 16;
        if (cycle > 2)
          cycle = 0;
      }
  }

  // SEND OUT OUR PADDLE POSITION 10 TIMES PER SECOND
  {
    static DWORD lasttime = GetTickCount();
    if (currtime-lasttime > 1000)
      lasttime = currtime;
    else
      while (currtime-lasttime >= 100) {
        DWORD paramarray[2] = {NETID_PADDLEPOS,paddle[player].y};
        SendNetworkMessage(&paramarray[0],2);
        lasttime += 100;
      }
  }

  // UPDATE THE SCREEN 
  if (processed)
    DrawScreen();

}

//===========================================================================
BOOL CALLBACK IdleProc (DWORD) {
  if (started)
    GameIdleProc();
  else
    ConnectIdleProc();
  return 1;
}

//===========================================================================
BOOL LoadFont () {
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
void CALLBACK OnKeyDown (LPPARAMS params) {
  if (params->wparam == VK_ESCAPE) {
    if (started) {
      DWORD param = NETID_QUIT;
      SendNetworkMessage(&param,1);
    }
    SDrawPostClose();
  }
  else if (started && (!lives[0]) || (!lives[1])) {
    DWORD param = NETID_RESTART;
    SendNetworkMessage(&param,1);
    StartGame();
  }
}

//===========================================================================
void CALLBACK OnPaint (LPPARAMS params) {
  if (!started) {
    LPBYTE videobuffer;
    int    videopitch;
    if (SDrawLockSurface(SDRAW_SURFACE_FRONT,NULL,&videobuffer,&videopitch)) {
      SGdiSetPitch(videopitch);
      SGdiRectangle(videobuffer,0,0,639,479,PALETTEINDEX(128));
      SGdiTextOut(videobuffer,240,235,PALETTEINDEX(255),"Waiting to connect...");
      SDrawUnlockSurface(SDRAW_SURFACE_FRONT,videobuffer);
    }
  }
}

//===========================================================================
BOOL ProcessNetworkMessages () {
  BOOL   processed = 0;
  LPVOID data      = NULL;
  DWORD  databytes = 0;
  while (SNetReceiveMessage(NULL,&data,&databytes))
    if (databytes >= sizeof(DWORD)) {
      LPDWORD param = (LPDWORD)data;
      switch (*param) {

        case NETID_BALLPOS:
          ballpos[*(param+1)].x = *(param+2);
          ballpos[*(param+1)].y = *(param+3);
          ballangle[*(param+1)] = (ANGLE)*(param+4);
          speed[*(param+1)]     = *(param+5);
          ballwait[*(param+1)]  = -1;
          missed[!player]       = 0;
          break;

        case NETID_CHECKSYNC:
          if (ballwait[*(param+1)] == !player)
            StartBall(*(param+1),player);
          break;

        case NETID_MISSED:
          ballwait[*(param+1)] = !player;
          missed[!player]      = *(param+2);
          lives[!player]       = *(param+3);
          break;

        case NETID_PADDLEPOS:
          paddle[!player].y = *(param+1);
          break;

        case NETID_QUIT:
          SDrawPostClose();
          break;

        case NETID_RESTART:
          StartGame();
          break;

      }
      processed = 1;
    }
  return processed;
}

//===========================================================================
void SendNetworkMessage (LPDWORD paramarray, DWORD paramcount) {
  SNetSendMessage(!player,
                  paramarray,
                  paramcount*sizeof(DWORD));
}

//===========================================================================
void StartBall (int number, DWORD player) {
  ballangle[number] = (rand() & 63)+(player ? 224 : 96);
  ballpos[number].x = (player ? 420 : 220)*0x10000;
  int besty = paddle[player].y-(abs((ballpos[number].x >> 16)-paddle[player].x)*SIN(ballangle[number]) >> 16);
  ballpos[number].y = max(0,min(479,besty))*0x10000;
  ballwait[number]  = -1;
  DWORD paramarray[6] = {NETID_BALLPOS,
                         number,
                         ballpos[number].x,
                         ballpos[number].y,
                         ballangle[number],
                         speed[number]};
  SendNetworkMessage(&paramarray[0],6);
}

//===========================================================================
void StartGame () {
  srand(GetTickCount());
  FillMemory(brick,BRICKCOLS*BRICKROWS,1);
  for (int loop = 0; loop < 2; ++loop) {
    lives[loop]  = 3;
    missed[loop] = 0;
    speed[loop]  = 0x50000;
  }
  StartBall(player,player);
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR cmdline, int) {
  if (cmdline && *cmdline)
    computer = 1;
  if (!SDrawAutoInitialize(instance,
                           TITLE "_CLASS",
                           TITLE,
                           NULL,
                           SDRAW_SERVICE_PAGEFLIP)) {
    SDrawMessageBox("Unable to initialize DirectDraw.",TITLE,0);
    return 1;
  }
  if (!SFileOpenArchive("artwork.mpq",0,0,&archive)) {
    SDrawMessageBox("Unable to open artwork.",TITLE,0);
    return 1;
  }
  if (!Connect()) {
    SDrawMessageBox("Unable to initialize networking.",TITLE,0);
    return 1;
  }
  if (!LoadFont()) {
    SDrawMessageBox("Unable to load font.",TITLE,0);
    return 1;
  }
  SMsgRegisterMessage(NULL,WM_KEYDOWN,OnKeyDown);
  SMsgRegisterMessage(NULL,WM_PAINT  ,OnPaint);
  return SMsgDoMessageLoop(IdleProc);
}
