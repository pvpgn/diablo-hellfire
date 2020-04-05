//****************************************************************************
//	SelGame.cpp
//	Diablo UI select multiplayer game
//	
//	By Frank Pearce
//	created 11.5.96
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
extern BOOL UiSelectIPXGame (DWORD               flags,
							 SNETPROGRAMDATAPTR  programdata,
							 SNETPLAYERDATAPTR   playerdata,
							 SNETUIDATAPTR       interfacedata,
							 SNETVERSIONDATAPTR  versiondata,
							 DWORD              *playerid);
extern BOOL UiDirectLinkGame (DWORD               flags,
							  SNETPROGRAMDATAPTR  programdata,
							  SNETPLAYERDATAPTR   playerdata,
							  SNETUIDATAPTR       interfacedata,
							  SNETVERSIONDATAPTR  versiondata,
							  DWORD              *playerid);
extern BOOL UiModemGame (DWORD               flags,
						 SNETPROGRAMDATAPTR  programdata,
						 SNETPLAYERDATAPTR   playerdata,
						 SNETUIDATAPTR       interfacedata,
						 SNETVERSIONDATAPTR  versiondata,
						 DWORD              *playerid);



//****************************************************************************
//****************************************************************************
	#define ID_NETWORK		'IPXN'
	#define ID_MODEM		'MODM'
	#define ID_SERIAL		'SCBL'


//****************************************************************************
//****************************************************************************
static DWORD sgProviderID;
static BOOL sgBasicInterface;
void UiSetBasicInterface(BOOL basic) {
	sgBasicInterface = basic;
}
BOOL UiIsBasicInterface(void) {
	return sgBasicInterface;
}
void UiSetProviderID(DWORD id) {
	sgProviderID = id;
}
DWORD UiGetProviderID(void) {
	return sgProviderID;
}


//****************************************************************************
//****************************************************************************
BOOL APIENTRY UiSelectGame (DWORD               flags,
                            SNETPROGRAMDATAPTR  programdata,
                            SNETPLAYERDATAPTR   playerdata,
                            SNETUIDATAPTR       interfacedata,
                            SNETVERSIONDATAPTR  versiondata,
                            DWORD              *playerid) {
	BOOL success;

	if (UiIsBasicInterface()) {
		return UiSelectIPXGame(
			flags,
			programdata,
			playerdata,
			interfacedata,
			versiondata,
			playerid
		);
	}

	switch (UiGetProviderID()) {
		case ID_NETWORK:
			success = UiSelectIPXGame(
				flags,
				programdata,
				playerdata,
				interfacedata,
				versiondata,
				playerid
			);
		break;

		case ID_MODEM:
			success = UiModemGame(
				flags,
				programdata,
				playerdata,
				interfacedata,
				versiondata,
				playerid
			);
		break;

		case ID_SERIAL:
			success = UiDirectLinkGame(
				flags,
				programdata,
				playerdata,
				interfacedata,
				versiondata,
				playerid
			);
		break;

		default:
			success = SNetSelectGame(
				flags,
				programdata,
				playerdata,
				interfacedata,
				versiondata,
				playerid
			);
		break;
	}

	return success;
}
