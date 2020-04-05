//****************************************************************************
//
//  TESTUI.CPP
//  Diablo UI test program
//
//  By Michael O'Brien (9/12/96)
//
//****************************************************************************

#include <windows.h>
#include "storm.h"
#include "ui\diabloui.h"

//****************************************************************************
//****************************************************************************
#define CLASS  TEXT("TESTUICLASS")
#define TITLE  TEXT("Diablo UI Test")
#define PROGRAMID        'DTST'
#define VERSIONID        1
#define MAXPLAYERS       4
							    
char szName[MAX_COMPUTERNAME_LENGTH+1] = "TestUI";


//****************************************************************************
//****************************************************************************
BOOL Initialize (HINSTANCE instance) {

	// FOR DEBUG VERSIONS OF THE PROGRAM, SETUP STORM'S MOPAQ ARCHIVE SUPPORT
	// SO THAT IT CAN READ FILES DIRECTLY FROM THE DISK (RATHER THAN ONLY FROM
	// MOPAQ FILES) AND SO THAT FILE ACCESS IS BASED IN THE CURRENT DIRECTORY
	// RATHER THAN THE EXECUTABLE DIRECTORY
	{
		TCHAR directory[MAX_PATH] = TEXT("");
		GetCurrentDirectory(MAX_PATH,directory);
		SFileSetBasePath(directory);
		SFileEnableDirectAccess(1);
	}

	// INITIALIZE DIRECTDRAW
	if (!SDrawAutoInitialize(instance,CLASS,TITLE)) {
		SDrawMessageBox("Unable to initialize DirectDraw.",TITLE,0);
		return 1;
	}

	// REGISTER MESSAGE HANDLERS FOR THOSE MESSAGES THAT WILL BE HANDLED
	// BY THE FRAME WINDOW
	SMsgRegisterMessage(NULL,WM_PAINT,UiOnPaint); 

	return 1;
}


//****************************************************************************
//****************************************************************************
BOOL APIENTRY Connect (SNETPROGRAMDATAPTR programdata, SNETPLAYERDATAPTR playerdata, DWORD *pdwLocalPlayer) {
	// BUILD AN INTERFACE DATA RECORD
	SNETUIDATA Interfacedata;

	ZeroMemory(&Interfacedata,sizeof(SNETUIDATA));
	Interfacedata.size           = sizeof(SNETUIDATA);
	Interfacedata.parentwindow   = SDrawGetFrameWindow();
	Interfacedata.artcallback    = UiArtCallback;
	Interfacedata.createcallback = UiCreateGameCallback;
	Interfacedata.drawdesccallback = UiDrawDescCallback;
	Interfacedata.messageboxcallback = UiMessageBoxCallback;
	Interfacedata.soundcallback  = UiSoundCallback;
	Interfacedata.authcallback   = UiAuthCallback;
	Interfacedata.getdatacallback = UiGetDataCallback;
	Interfacedata.categorycallback = UiCategoryCallback;
	
	
	// SELECT A NETWORK PROVIDER
	while (UiSelectProvider(NULL,
	                          programdata,
	                          playerdata,
	                          &Interfacedata,
	                          NULL,
	                          NULL)) {

	  // SELECT A GAME
	  if (SNetSelectGame(SNET_SF_ALLOWCREATE,
	                     programdata,
	                     playerdata,
	                     &Interfacedata,
	                     NULL,
	                     pdwLocalPlayer)) {
		MessageBox(SDrawGetFrameWindow(), "Test", "Test", MB_OK);
	    return 1;
	  }
	
	}

	return 0;
}


//****************************************************************************
//****************************************************************************
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int) {

	DWORD dwLocalPlayer;
	char szDesc[256];
	TUIHEROINFO HeroInfo;


	ShowCursor(FALSE);

	ZeroMemory(&HeroInfo, sizeof(HeroInfo));
	HeroInfo.level = 5;
	HeroInfo.herorank = 3;

	if (!Initialize(instance)) {
		ShowCursor(TRUE);
		return 1;
	}


	// Must be called after storm is initialized
	UiInitialize();

	// BUILD A PROGRAM DATA RECORD
	SNETPROGRAMDATA programdata;
	ZeroMemory(&programdata,sizeof(SNETPROGRAMDATA));
	programdata.size        = sizeof(SNETPROGRAMDATA);
	programdata.programname = TITLE;
	programdata.programid   = PROGRAMID;
	programdata.versionid   = VERSIONID;
	programdata.maxplayers  = MAXPLAYERS;
	programdata.initdata    = NULL;			
	programdata.initdatabytes = 0;
	
	
	// BUILD A PLAYER DATA RECORD
	SNETPLAYERDATA playerdata;
	ZeroMemory(&playerdata,sizeof(SNETPLAYERDATA));
	playerdata.size       = sizeof(SNETPLAYERDATA);
	playerdata.playername = szName;
	playerdata.playerdescription = (LPSTR)szDesc;

	// Create player description string   
	UiCreatePlayerDescription(&HeroInfo, PROGRAMID, (LPSTR)szDesc);

	Connect(&programdata, &playerdata, &dwLocalPlayer);

	UiDestroy();
	StormDestroy();

	ShowCursor(TRUE);
	return 0;
}
