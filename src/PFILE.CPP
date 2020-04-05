//******************************************************************
// $Header: /Diablo/PFILE.CPP 3     2/04/97 11:17a Pwyatt $
//******************************************************************


#include	"diablo.h"
#pragma hdrstop
#include	<stddef.h>
#include	"storm.h"
#include	"diabloui.h"
#include	"items.h"
#include	"gendung.h"
#include	"player.h"
#include	"engine.h"
#include	"multi.h"
#include	"spells.h"
#include	"packplr.h"
#include	"gamemenu.h"
#include	"mpqapi.h"


//******************************************************************
// compiler constants
//******************************************************************
// pjw.patch1.start -- remove retribution code
#define PREVENT_CHEATING	0	// 0 in final
#ifdef NDEBUG
#undef PREVENT_CHEATING
#define PREVENT_CHEATING 0
#endif
// pjw.patch1.end


#define ENCRYPT_WITH_NAME	1	// 1 in final
#ifdef NDEBUG
#undef ENCRYPT_WITH_NAME
#define ENCRYPT_WITH_NAME 1
#endif

#if !ENCRYPT_WITH_NAME
#define ENCRYPTNAME "BROPER"	// replace name to debug specific file
#endif


//******************************************************************
// extern
//******************************************************************
extern char gszHero[];
extern char gszProgKey[];
extern char gszArchiveKey[];
extern int StrengthTbl[NUM_CLASSES];
extern int MagicTbl[NUM_CLASSES];
extern int DexterityTbl[NUM_CLASSES];
extern int VitalityTbl[NUM_CLASSES];

DWORD CalcEncodeDstBytes(DWORD dwSrcBytes);
void EncodeFile(BYTE * pbSrcDst,DWORD dwSrcBytes,DWORD dwDstBytes,const char * pszPassword);
DWORD DecodeFile(BYTE * pbSrcDst,DWORD dwDstBytes,const char * pszPassword);
void PackClearCheater(PkPlayerStruct *pPack);
void DiskFreeErrorDlg(const char * pszDir);


//******************************************************************
// public
//******************************************************************
BOOL gbValidSaveFile = FALSE;
BOOL gbSaveFileExists = FALSE;


//******************************************************************
// private
//******************************************************************
// signature name for machines with no network name
#ifndef ENCRYPTNAME
#define ENCRYPTNAME "xrgyrkj1"
#endif

#define ARCHIVE_PRIORITY	0x7000

static char sgszCharNames[MAX_CHARACTERS+1][PLR_NAME_LEN];

static const char sgszSaveGame[] = "game";
static const char sgszSaveChar[] = "hero";
static const char sgszPermSaveLevel_d[] = "perml%02d";
static const char sgszPermSaveSLevel_d[] = "perms%02d";
static const char sgszTempSaveLevel_d[] = "templ%02d";
static const char sgszTempSaveSLevel_d[] = "temps%02d";

typedef enum {
	ARCHIVE_DIABLO,
	ARCHIVE_HELLFIRE
	} ARCHIVE_TYPE;


//******************************************************************
//******************************************************************
static void check_disk_space_priv(char * pszDir) {
	app_assert(pszDir);

	// remove any trailing path from the drive name
	char * pszTemp = pszDir;
	while (*pszTemp) {
		if (*pszTemp++ != '\\') continue;
		*pszTemp = 0;
		break;
	}

	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwNumberOfFreeClusters;
	DWORD dwTotalNumberOfClusters;
	BOOL bResult = GetDiskFreeSpace(
		pszDir,
		&dwSectorsPerCluster,
		&dwBytesPerSector,
		&dwNumberOfFreeClusters,
		&dwTotalNumberOfClusters
	);

	if (bResult) {
		__int64 i64BytesFree = dwNumberOfFreeClusters;
		i64BytesFree *= dwSectorsPerCluster;
		i64BytesFree *= dwBytesPerSector;
		if (i64BytesFree < 10 * 1024 * 1024) bResult = FALSE;
	}

	if (! bResult) DiskFreeErrorDlg(pszDir);
}


//******************************************************************
//******************************************************************
void check_disk_space() {
	char szDir[MAX_PATH];

	#if IS_VERSION(BETA)
	if (! GetSystemDirectory(szDir,MAX_PATH)) goto error;
	#else
	if (! GetWindowsDirectory(szDir,MAX_PATH)) goto error;
	#endif
	check_disk_space_priv(szDir);

	if (! GetModuleFileName(ghInst,szDir,MAX_PATH)) goto error;
	check_disk_space_priv(szDir);

	return;
error:
	app_fatal("Unable to initialize save directory");
}


//******************************************************************
//******************************************************************
static void archive_name(char * pszBuf,DWORD dwBufSize,DWORD dwChar, ARCHIVE_TYPE arType) {
	app_assert(pszBuf);
	app_assert(dwBufSize >= MAX_PATH);
	const char * pszSaveFile_d;
	UINT uiResult;

	if (gbMaxPlayers > 1) {
		#if IS_VERSION(SHAREWARE)
		pszSaveFile_d = "\\slinfo_%d.drv";
		#else
		if (arType == ARCHIVE_DIABLO)
			pszSaveFile_d = "\\dlinfo_%d.drv";
		else
			pszSaveFile_d = "\\hrinfo_%d.drv";
		#endif

		#if IS_VERSION(BETA)
		uiResult = GetSystemDirectory(pszBuf,MAX_PATH);
		#else
		uiResult = GetWindowsDirectory(pszBuf,MAX_PATH);
		#endif
	}
	else {
		#if IS_VERSION(SHAREWARE)
		pszSaveFile_d = "\\spawn_%d.sv";
		#else
		if (arType == ARCHIVE_DIABLO)
			pszSaveFile_d = "\\single_%d.sv";
		else
			pszSaveFile_d = "\\single_%d.hsv";
		#endif

		uiResult = GetModuleFileName(ghInst,pszBuf,MAX_PATH);
		char * pszExeName = strrchr(pszBuf,'\\');
		if (pszExeName) *pszExeName = 0;
	}

	// make sure we were able to get the parent directory
	if (! uiResult) app_fatal("Unable to get save directory");

	char szTemp[MAX_PATH];
	sprintf(szTemp,pszSaveFile_d,dwChar);
	strcat(pszBuf,szTemp);
	_strlwr(pszBuf);
}


//******************************************************************
//******************************************************************
static DWORD name_2_file_index(const char * pszName) {
	for (DWORD d = 0; d < MAX_CHARACTERS; d++) {
		if (! _stricmp(sgszCharNames[d],pszName))
			break;
	}

	return d;
}


//******************************************************************
//******************************************************************
static BOOL CALLBACK GetPlayerFileNames(DWORD dwIndex,char szPath[MAX_PATH]) {
	const char * pszFmt;

	if (gbMaxPlayers > 1) {
		if (dwIndex) return FALSE;
		pszFmt = sgszSaveChar;
	}
	else if (dwIndex < NUMLEVELS) {
		// level file
		pszFmt = sgszPermSaveLevel_d;
	}
	else if (dwIndex < NUMLEVELS*2) {
		// slevel file
		dwIndex -= NUMLEVELS;
		pszFmt = sgszPermSaveSLevel_d;
	}
	else if (dwIndex == NUMLEVELS*2) {
		// game file
		pszFmt = sgszSaveGame;
	}
	else if (dwIndex == NUMLEVELS*2+1) {
		// character file
		pszFmt = sgszSaveChar;
	}
	else {
		return FALSE;
	}

	sprintf(szPath,pszFmt,dwIndex);
	return TRUE;
}


//******************************************************************
//******************************************************************
static BOOL LoadCharacter(HSARCHIVE hsArchive,PkPlayerStruct * pPack) {
	app_assert(pPack);

	// open the character file
	HSFILE hsFile;
	if (! SFileOpenFileEx(hsArchive,sgszSaveChar,0,&hsFile))
		return FALSE;

	// setup everything in case of error
	BYTE * pbFile = NULL;
	BOOL bResult = FALSE;

	// get password
	char computername[MAX_COMPUTERNAME_LENGTH+1] = ENCRYPTNAME;
	DWORD size = MAX_COMPUTERNAME_LENGTH+1;
	#if ENCRYPT_WITH_NAME
	if (gbMaxPlayers > 1) GetComputerName(computername,&size);
	#endif

	// read character data
	DWORD dwSize = SFileGetFileSize(hsFile,NULL);
	if (! dwSize) goto error;
	pbFile = DiabloAllocPtrSig(dwSize,'SAVt');
	DWORD dwBytes;
	if (! SFileReadFile(hsFile,pbFile,dwSize,&dwBytes,NULL))
		goto error;
	app_assert(dwBytes == dwSize);

	// decode file using password
	dwBytes = DecodeFile(pbFile,dwSize,computername);
	if (dwBytes != sizeof(*pPack)) goto error;

	// success!
	CopyMemory(pPack,pbFile,sizeof(*pPack));
	bResult = TRUE;

error:	// CLEANUP
	if (pbFile) DiabloFreePtr(pbFile);
	SFileCloseFile(hsFile);
	return bResult;
}


//******************************************************************
//******************************************************************
static void SaveCharacter(const PkPlayerStruct * pPack) {
	// get password
	char computername[MAX_COMPUTERNAME_LENGTH+1] = ENCRYPTNAME;
	DWORD size = MAX_COMPUTERNAME_LENGTH+1;
	#if ENCRYPT_WITH_NAME
	if (gbMaxPlayers > 1) GetComputerName(computername,&size);
	#endif

	// encrypt character data
	DWORD dwDstBytes = CalcEncodeDstBytes(sizeof(*pPack));
	BYTE * pbSrcDst = DiabloAllocPtrSig(dwDstBytes,'SAVt');
	CopyMemory(pbSrcDst,pPack,sizeof(*pPack));
	EncodeFile(pbSrcDst,sizeof(*pPack),dwDstBytes,computername);

	// add file to archive
	MPQAddFile(sgszSaveChar,pbSrcDst,dwDstBytes);

	// cleanup
	DiabloFreePtr(pbSrcDst);
}


//******************************************************************
//******************************************************************
#if PREVENT_CHEATING
static BOOL FixCheaters(BOOL * pbMsgBox,DWORD dwChar) {
	app_assert(dwChar < MAX_CHARACTERS);

	// only perform cheat detection/correction for multiplayer chars
	if (gbMaxPlayers == 1) return TRUE;

	// compare current state of archive time stamps to expected values
	char szSaveArchive[MAX_PATH];
	archive_name(szSaveArchive,MAX_PATH,dwChar, ARCHIVE_HELLFIRE);
	if (MPQCompareTimeStamps(szSaveArchive,dwChar))
		return TRUE;

	HSARCHIVE hsArchive = NULL;
	if (! SFileOpenArchive(szSaveArchive,ARCHIVE_PRIORITY,0,&hsArchive))
		return TRUE;

	// load character
	PkPlayerStruct pack;
	if (! LoadCharacter(hsArchive,&pack))
		pack.pName[0] = 0;
	else
		PackClearCheater(&pack);
	SFileCloseArchive(hsArchive);
	hsArchive = NULL;

	// write all characters
	if (pack.pName[0]) {
		if (! MPQOpenArchive(szSaveArchive,TRUE,dwChar)) return FALSE;
		SaveCharacter(&pack);
		MPQCloseArchive(szSaveArchive,TRUE,dwChar);
		MPQUpdateCreationTimeStamp(szSaveArchive,dwChar);
	}
	else {
		BOOL MPQSetAttributes(const char * pszArchive,BOOL bHide);
		MPQSetAttributes(szSaveArchive,FALSE);
		DeleteFile(szSaveArchive);
	}

	if (pbMsgBox && *pbMsgBox) {
		*pbMsgBox = FALSE;
		UiMessageBoxCallback(
			ghMainWnd,
			"A problem with your saved game file has been detected."
			"The file has been fixed, but changes to it may have occurred.",
			"Diablo",
			MB_OK
		);
	}

	return TRUE;
}
#endif


//******************************************************************
//******************************************************************
static BOOL open_archive_write(BOOL bMungeOnError,DWORD dwChar) {
	// make sure nobody is screwing with our file
	#if PREVENT_CHEATING
	if (FixCheaters(NULL,dwChar))
	#endif
	{
		char szSaveArchive[MAX_PATH];
		archive_name(szSaveArchive,MAX_PATH,dwChar, ARCHIVE_HELLFIRE);
		if (MPQOpenArchive(szSaveArchive,gbMaxPlayers > 1,dwChar))
			return TRUE;
	}
	
	if (bMungeOnError && gbMaxPlayers > 1)
		MPQMungeStamps(dwChar);
	return FALSE;
}


//******************************************************************
//******************************************************************
static void close_archive_write(BOOL bFree,DWORD dwChar) {
	char szSaveArchive[MAX_PATH];
	archive_name(szSaveArchive,MAX_PATH,dwChar, ARCHIVE_HELLFIRE);
	MPQCloseArchive(szSaveArchive,bFree,dwChar);
}


//******************************************************************
//******************************************************************
static HSARCHIVE open_archive_read(BOOL * pbMsgBox,DWORD dwChar) {
	// make sure nobody is screwing with our file
	#if PREVENT_CHEATING
	if (! FixCheaters(pbMsgBox,dwChar))
		return FALSE;
	#endif

	// open the archive for reading
	HSARCHIVE hsArchive;
	char szSaveArchive[MAX_PATH];
	archive_name(szSaveArchive,MAX_PATH,dwChar, ARCHIVE_HELLFIRE);
	if (! SFileOpenArchive(szSaveArchive,ARCHIVE_PRIORITY,0,&hsArchive))
		{
		//archive_name(szSaveArchive,MAX_PATH,dwChar, ARCHIVE_DIABLO);
		//if (! SFileOpenArchive(szSaveArchive,ARCHIVE_PRIORITY,0,&hsArchive))
			return NULL;
		}
	return hsArchive;
}


//******************************************************************
//******************************************************************
static void close_archive_read(HSARCHIVE hsArchive) {
	app_assert(hsArchive);
	SFileCloseArchive(hsArchive);
}


//******************************************************************
//******************************************************************
static BOOL check_valid_save(HSARCHIVE hsArchive,DWORD dwChar) {
	app_assert(dwChar < MAX_CHARACTERS);
	
	BOOL Result = FALSE;

	gbSaveFileExists = FALSE;
	
	// only single player games can have save games
	if (gbMaxPlayers != 1)
		return Result;

	HSFILE hsFile;
	if (! SFileOpenFileEx(hsArchive,sgszSaveGame,0,&hsFile))
		return Result;

	// allocate a buffer large enough for the file	
	DWORD dwLen = SFileGetFileSize(hsFile,NULL);
	if (! dwLen) app_fatal("Invalid save file");
	BYTE * pbData = DiabloAllocPtrSig(dwLen + 8,'SAVt');

		// decode overwrites the headers, so give it extra room
    BYTE *pbHackData = 4 + pbData;
	
	DWORD dwBytes;
	if (SFileReadFile(hsFile, pbHackData, dwLen, &dwBytes, NULL))
	{
		if (dwBytes == dwLen)
		{
			char computername[MAX_COMPUTERNAME_LENGTH+1] = ENCRYPTNAME;
			DWORD size = MAX_COMPUTERNAME_LENGTH+1;
			#if ENCRYPT_WITH_NAME
			if (gbMaxPlayers > 1) GetComputerName(computername,&size);
			#endif
		
			gbSaveFileExists = TRUE;
		
			dwLen = DecodeFile(pbHackData,dwLen,computername);
			if (dwLen)
			{
				long id;
				id = *pbHackData << 24;
				pbHackData++;
				id |= *pbHackData << 16;
				pbHackData++;
				id |= *pbHackData << 8;
				pbHackData++;
				id |= *pbHackData;
				
				// Test first 4 bytes for our header.
				if ('HELF' == id)
				{
					Result = TRUE;
				}
			}
		}
	}
	
	if (pbData) DiabloFreePtr(pbData);
	SFileCloseFile(hsFile);
	return Result;
}


//******************************************************************
//******************************************************************
void UpdatePlayerFile() {
	app_assert(myplr >= 0 && myplr < MAX_PLRS);
	DWORD dwChar = name_2_file_index(plr[myplr]._pName);
	if (! open_archive_write(TRUE,dwChar)) return;

	// pack and save player
	PkPlayerStruct pack;
	PackPlayer(&pack,myplr);
	SaveCharacter(&pack);

	// close archive -- in single player mode, release
	// the file memory.  In multiplayer mode, leave the
	// file info in memory so we don't have to reload
	close_archive_write(gbMaxPlayers == 1,dwChar);
}


//******************************************************************
//******************************************************************
void ReleasePlayerFile() {
	// free any memory associated with player file
	DWORD dwChar = name_2_file_index(plr[myplr]._pName);
	close_archive_write(TRUE,dwChar);
}


//******************************************************************
//******************************************************************
static char ui_2_game_class(int heroclass) {
	if (heroclass == UI_WARRIOR)
		return CLASS_WARRIOR;
	if (heroclass == UI_ROGUE)
		return CLASS_ROGUE;
	if (heroclass == UI_MONK)
		return CLASS_MONK;
	if (heroclass == UI_BARD)
		return CLASS_BARD;
	if (heroclass == UI_BARBARIAN)
		return CLASS_BARBARIAN;
	return CLASS_SORCEROR;
}


//******************************************************************
//******************************************************************
static BYTE game_2_ui_class(const PlayerStruct * p) {
	if (p->_pClass == CLASS_WARRIOR)
		return UI_WARRIOR;
	if (p->_pClass == CLASS_ROGUE)
		return UI_ROGUE;
	if (p->_pClass == CLASS_MONK)
		return UI_MONK;
	if (p->_pClass == CLASS_BARD)
		return UI_BARD;
	if (p->_pClass == CLASS_BARBARIAN)
		return UI_BARBARIAN;
	return UI_SORCERER;
}


//******************************************************************
//******************************************************************
void game_2_ui_player(const PlayerStruct * p,TPUIHEROINFO heroinfo,BOOL bHasSaveFile) {
	ZeroMemory(heroinfo,sizeof(TUIHEROINFO));
	strncpy(heroinfo->name,p->_pName,MAX_NAME_LEN-1);
	heroinfo->name[MAX_NAME_LEN-1] = 0;
	heroinfo->level = p->_pLevel;
	heroinfo->heroclass = game_2_ui_class(p);
	heroinfo->strength = p->_pStrength;
	heroinfo->magic = p->_pMagic;
	heroinfo->dexterity = p->_pDexterity;
	heroinfo->vitality = p->_pVitality;
	heroinfo->gold = p->_pGold;
	heroinfo->hassaved = bHasSaveFile;
	heroinfo->herorank = (BYTE) p->pDiabloKillLevel;
	heroinfo->spawned = IS_VERSION(SHAREWARE);
}


//******************************************************************
//******************************************************************
BOOL CALLBACK UiEnumHeroes(ENUMHEROPROC enumproc) {
	// assume there is no archive
	ZeroMemory(sgszCharNames,sizeof(sgszCharNames));

	// get the character name field and save into global
	BOOL bMsgBox = TRUE;
	for (DWORD dwChar = 0; dwChar < MAX_CHARACTERS; dwChar++) {
		HSARCHIVE hsArchive;
		if (NULL == (hsArchive = open_archive_read(&bMsgBox,dwChar)))
			continue;

		PkPlayerStruct pack;
		TUIHEROINFO heroinfo;
		BOOL bResult = LoadCharacter(hsArchive,&pack);
		if (bResult) {
			app_assert(sizeof sgszCharNames[dwChar] == sizeof pack.pName);
			strcpy(sgszCharNames[dwChar],pack.pName);
			UnPackPlayer(&pack,0,FALSE);
			game_2_ui_player(&plr[0],&heroinfo,check_valid_save(hsArchive,dwChar));
			enumproc(&heroinfo);
		}

		// close archive
		close_archive_read(hsArchive);
	}

	return 1;
}


//******************************************************************
//******************************************************************
BOOL CALLBACK UiGetDefaultCharStats(int heroclass, TPUIDEFSTATS defaultstats) {
	int nClass = ui_2_game_class(heroclass);
	defaultstats->strength = StrengthTbl[nClass];
	defaultstats->magic = MagicTbl[nClass];
	defaultstats->dexterity = DexterityTbl[nClass];
	defaultstats->vitality = VitalityTbl[nClass];
	return TRUE;
}


//******************************************************************
//******************************************************************
BOOL CALLBACK UiCreateHero(TPUIHEROINFO heroinfo) {
	app_assert(heroinfo->name[0]);

	// find out if this guy already exists
	// or find a free hero slot
	DWORD dwChar = name_2_file_index(heroinfo->name);
	if (dwChar >= MAX_CHARACTERS) {
		for (dwChar = 0; dwChar < MAX_CHARACTERS; dwChar++)
			if (! sgszCharNames[dwChar][0])
				break;
	}
	if (dwChar >= MAX_CHARACTERS)
		return FALSE;

	// open archive to save character
	if (! open_archive_write(FALSE,dwChar)) return FALSE;

	// we are going to overwrite a save slot.  If the player
	// has done any hacking, there may be invalid save files
	// from a previous player in the slot.  Remove those files
	MPQDeleteFiles(GetPlayerFileNames);

	// fix up the name table
	strncpy(sgszCharNames[dwChar],heroinfo->name,PLR_NAME_LEN);
	sgszCharNames[dwChar][PLR_NAME_LEN - 1] = 0;

	// create character in player slot 0
	// heroinfo->heroclass = UI_BARD;
	CreatePlayer(0,ui_2_game_class(heroinfo->heroclass));
	strncpy(plr[0]._pName,heroinfo->name,PLR_NAME_LEN);
	plr[0]._pName[PLR_NAME_LEN - 1] = 0;

	// pack character and save
	PkPlayerStruct pack;
	PackPlayer(&pack,0);
	SaveCharacter(&pack);

	// convert character into something the UI can use

	game_2_ui_player(&plr[0],heroinfo,FALSE);

	close_archive_write(TRUE,dwChar);

	return TRUE;
}


//******************************************************************
//******************************************************************
BOOL CALLBACK UiDeleteHero(TPUIHEROINFO heroinfo) {
	DWORD dwChar = name_2_file_index(heroinfo->name);
	if (dwChar >= MAX_CHARACTERS) return TRUE;

	// delete character
	sgszCharNames[dwChar][0] = 0;

	// delete archive
	char szSaveArchive[MAX_PATH];
	archive_name(szSaveArchive,MAX_PATH,dwChar, ARCHIVE_HELLFIRE);
	DeleteFile(szSaveArchive);

	return TRUE;
}


//******************************************************************
//******************************************************************
void SetupLocalPlayer() {
	// load the player information we want into our player slot
	app_assert(myplr >= 0 && myplr < MAX_PLRS);
	DWORD dwChar = name_2_file_index(gszHero);
	app_assert(dwChar != MAX_CHARACTERS);

	// open the main archive
	HSARCHIVE hsArchive;
	PkPlayerStruct pack;
	if (NULL == (hsArchive = open_archive_read(NULL,dwChar)))
		app_fatal("Unable to open archive");
	if (! LoadCharacter(hsArchive,&pack))
		app_fatal("Unable to load character");
	UnPackPlayer(&pack,myplr,FALSE);
	gbValidSaveFile = check_valid_save(hsArchive,dwChar);
	close_archive_read(hsArchive);
}


//******************************************************************
//******************************************************************
void CreateSaveLevelName(char szName[MAX_PATH]) {
	// make sure the character is currently valid
	DWORD dwChar = name_2_file_index(plr[myplr]._pName);
	app_assert(dwChar < MAX_CHARACTERS);
	app_assert(gbMaxPlayers == 1);

	// saving a level always saves it as a "temporary" save file
	if (setlevel) sprintf(szName,sgszTempSaveSLevel_d,setlvlnum);
	else sprintf(szName,sgszTempSaveLevel_d,currlevel);
}


//******************************************************************
//******************************************************************
void CreateLoadLevelName(char szName[MAX_PATH]) {
	// make sure the character is currently valid
	DWORD dwChar = name_2_file_index(plr[myplr]._pName);
	app_assert(dwChar < MAX_CHARACTERS);
	app_assert(gbMaxPlayers == 1);

	// try "temporary" save file name
	CreateSaveLevelName(szName);

	if (! open_archive_write(FALSE,dwChar))
		app_fatal("Unable to read to save file archive");
	BOOL bResult = MPQFileExists(szName);
	close_archive_write(TRUE,dwChar);
	if (bResult) return;

	// create "permanent" save file name
	if (setlevel) sprintf(szName,sgszPermSaveSLevel_d,setlvlnum);
	else sprintf(szName,sgszPermSaveLevel_d,currlevel);
}


//******************************************************************
//******************************************************************
void CreateSaveGameName(char szName[MAX_PATH]) {
	DWORD dwChar = name_2_file_index(plr[myplr]._pName);
	app_assert(dwChar < MAX_CHARACTERS);
	app_assert(gbMaxPlayers == 1);
	strcpy(szName,sgszSaveGame);
}


//******************************************************************
//******************************************************************
static BOOL CALLBACK GetPermSaveNames(DWORD dwIndex,char szPath[MAX_PATH]) {
	const char * pszFmt;

	app_assert(gbMaxPlayers == 1);
	if (dwIndex < NUMLEVELS) {
		// level file
		pszFmt = sgszPermSaveLevel_d;
	}
	else if (dwIndex < NUMLEVELS*2) {
		// slevel file
		dwIndex -= NUMLEVELS;
		pszFmt = sgszPermSaveSLevel_d;
	}
	else {
		return FALSE;
	}

	sprintf(szPath,pszFmt,dwIndex);
	return TRUE;
}


//******************************************************************
//******************************************************************
static BOOL CALLBACK GetTempSaveNames(DWORD dwIndex,char szPath[MAX_PATH]) {
	const char * pszFmt;

	app_assert(gbMaxPlayers == 1);
	if (dwIndex < NUMLEVELS) {
		// level file
		pszFmt = sgszTempSaveLevel_d;
	}
	else if (dwIndex < NUMLEVELS*2) {
		// slevel file
		dwIndex -= NUMLEVELS;
		pszFmt = sgszTempSaveSLevel_d;
	}
	else {
		return FALSE;
	}

	sprintf(szPath,pszFmt,dwIndex);
	return TRUE;
}


//******************************************************************
//******************************************************************
void DestroyTempSaves() {
	if (gbMaxPlayers > 1) return;
	DWORD dwChar = name_2_file_index(plr[myplr]._pName);
	app_assert(dwChar < MAX_CHARACTERS);
	if (! open_archive_write(FALSE,dwChar))
		app_fatal("Unable to write to save file archive");
	MPQDeleteFiles(GetTempSaveNames);
	close_archive_write(TRUE,dwChar);
}


//******************************************************************
//******************************************************************
void MoveTempSavesToPermanent() {
	DWORD dwChar = name_2_file_index(plr[myplr]._pName);
	app_assert(dwChar < MAX_CHARACTERS);
	app_assert(gbMaxPlayers == 1);

	if (! open_archive_write(FALSE,dwChar))
		app_fatal("Unable to write to save file archive");

	DWORD dwIndex = 0;
	char szTemp[MAX_PATH];
	char szPerm[MAX_PATH];
	while (GetTempSaveNames(dwIndex,szTemp)) {
		BOOL bResult = GetPermSaveNames(dwIndex,szPerm);
		app_assert(bResult);
		dwIndex++;

		// is there a temp file?
		if (! MPQFileExists(szTemp)) continue;

		// delete permanent file so we can rename temp file
		if (MPQFileExists(szPerm)) MPQDeleteFile(szPerm);

		MPQRenameFile(szTemp,szPerm);
	}
	app_assert(! GetPermSaveNames(dwIndex,szPerm));

	close_archive_write(TRUE,dwChar);
}


//******************************************************************
//******************************************************************
void WriteSaveFile(const char * pszName,BYTE * pbData,DWORD dwLen,DWORD dwDstBytes) {
	app_assert(pszName);
	app_assert(pbData);
	app_assert(dwLen);
	app_assert(gbMaxPlayers == 1);

	DWORD dwChar = name_2_file_index(plr[myplr]._pName);
	app_assert(dwChar < MAX_CHARACTERS);

	// encode file
	char computername[MAX_COMPUTERNAME_LENGTH+1] = ENCRYPTNAME;
	DWORD size = MAX_COMPUTERNAME_LENGTH+1;
	#if ENCRYPT_WITH_NAME
	if (gbMaxPlayers > 1) GetComputerName(computername,&size);
	#endif
	EncodeFile(pbData,dwLen,dwDstBytes,computername);

	if (! open_archive_write(FALSE,dwChar))
		app_fatal("Unable to write to save file archive");
	MPQAddFile(pszName,pbData,dwDstBytes);
	close_archive_write(TRUE,dwChar);
}
		

//******************************************************************
//******************************************************************
BYTE * ReadSaveFile(const char * pszName,DWORD * pdwLen) {
	app_assert(pszName);
	app_assert(pdwLen);
	app_assert(gbMaxPlayers == 1);

	DWORD dwChar = name_2_file_index(plr[myplr]._pName);
	app_assert(dwChar < MAX_CHARACTERS);

	HSARCHIVE hsArchive;
	if (NULL == (hsArchive = open_archive_read(NULL,dwChar)))
		app_fatal("Unable to open save file archive");

	HSFILE hsFile;
	if (! SFileOpenFileEx(hsArchive,pszName,0,&hsFile))
		app_fatal("Unable to open save file");

	// allocate a buffer large enough for the file	
	*pdwLen = SFileGetFileSize(hsFile,NULL);
	if (! *pdwLen) app_fatal("Invalid save file");
	BYTE * pbData = DiabloAllocPtrSig(*pdwLen,'SAVt');
	
	DWORD dwBytes;
	if (! SFileReadFile(hsFile,pbData,*pdwLen,&dwBytes,NULL))
		app_fatal("Unable to read save file");
	app_assert(dwBytes == *pdwLen);

	SFileCloseFile(hsFile);
	close_archive_read(hsArchive);

	char computername[MAX_COMPUTERNAME_LENGTH+1] = ENCRYPTNAME;
	DWORD size = MAX_COMPUTERNAME_LENGTH+1;
	#if ENCRYPT_WITH_NAME
	if (gbMaxPlayers > 1) GetComputerName(computername,&size);
	#endif
	*pdwLen = DecodeFile(pbData,*pdwLen,computername);
	if (! *pdwLen) app_fatal("Invalid save file");

	return pbData;
}


//******************************************************************
//******************************************************************
void TimedUpdatePlayerFile(BOOL bForce) {
	#define SAVE_TICKS	(60*1000)	// 60 seconds
	static long slSaveTime = 0;
	if (gbMaxPlayers == 1) return;

	long lCurrTime = (long) GetTickCount();
	if (bForce || lCurrTime - slSaveTime > SAVE_TICKS) {
		slSaveTime = lCurrTime;
		UpdatePlayerFile();
	}
}
