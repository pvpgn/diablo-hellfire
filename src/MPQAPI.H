//******************************************************************
// MPQapi.H
// File pack API
// By Michael O'Brien (6/1/96) && Patrick Wyatt (6/24/96) && Dan Liebgold (9/26/96)
//******************************************************************


#ifndef _MPQAPI
#define _MPQAPI


// diablo specific
#define MAX_CHARACTERS 10	// saved on disk


//******************************************************************
// constants
//******************************************************************
	#define MPQ_ADD_COMPRESSED	0x00000100
	#define MPQ_ADD_ENCRYPTED	0x00010000
	#define MPQ_ADD_ALLOCATED	0x80000000

	#define HASH_INDEX			0
	#define HASH_CHECK0			1
	#define HASH_CHECK1			2
	#define HASH_ENCRYPTKEY		3
	#define HASH_ENCRYPTDATA	4
	#define HASH_ENTRY_UNUSED	0xFFFFFFFF

	// possible values for HASHENTRY.block
	#define HASH_BLOCK_FREED	0xFFFFFFFE
	#define HASH_BLOCK_UNUSED	0xFFFFFFFF

	#define SIGNATURE			0x1A51504D
	#define SIGNATURELENGTH		72
	#define VERSION				0
	#define SECTORSIZEID		3
	#define SECTORSIZE			(512 << SECTORSIZEID)


//******************************************************************
// structures used in output file
//******************************************************************
	typedef struct _FILEHEADER {
		DWORD signature;
		DWORD headersize;
		DWORD filesize;
		WORD version;
		WORD sectorsizeid;
		DWORD hashoffset;
		DWORD blockoffset;
		DWORD hashcount;
		DWORD blockcount;
	} FILEHEADER, *FILEHEADERPTR;

	typedef struct _HASHENTRY {
		DWORD hashcheck[2];
		LCID lcid;
		DWORD block;
	} HASHENTRY, *HASHENTRYPTR;

	typedef struct _BLOCKENTRY {
		DWORD offset;
		DWORD sizealloc;
		DWORD sizefile;
		DWORD flags;
	} BLOCKENTRY, *BLOCKENTRYPTR;


	BOOL MPQOpenArchive(const char * pszArchive,BOOL bHide,DWORD dwChar);
	BOOL MPQCloseArchive(const char * pszArchive,BOOL bFree,DWORD dwChar);

	typedef BOOL (CALLBACK * TGetNameFcn)(DWORD dwIndex,char szPath[MAX_PATH]);
	void MPQDeleteFiles(TGetNameFcn fnGetName);
	void MPQDeleteFile(const char * pszName);
	BOOL MPQAddFile(const char * pszName,const BYTE * pbData,DWORD dwLen);
	void MPQRenameFile(const char * pszOld,const char * pszNew);
	BOOL MPQFileExists(const char * pszName);

	BOOL MPQCompareTimeStamps(const char * pszArchive,DWORD dwChar);
	void MPQUpdateCreationTimeStamp(const char * pszArchive,DWORD dwChar);
	void MPQMungeStamps(DWORD dwChar);


#endif
