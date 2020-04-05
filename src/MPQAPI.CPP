//******************************************************************
// MPQapi.cpp
// File pack API
// By Michael O'Brien (6/1/96) && Patrick Wyatt (6/24/96)
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include "storm.h"
#include "engine.h"
#include "mpqapi.h"


//******************************************************************
// debugging
//******************************************************************
#define DUMP_BLOCK	0	// 0 in final
#ifdef NDEBUG
#undef DUMP_BLOCK
#define DUMP_BLOCK 0
#endif


//******************************************************************
// extern
//******************************************************************
extern BYTE gbMaxPlayers;


//******************************************************************
// public
//******************************************************************
char gszProgKey[] = "Hellfire";


//******************************************************************
// private
//******************************************************************
#define MPQ_HIDE_ATTR (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)

static HANDLE sghArchive = INVALID_HANDLE_VALUE;
static BOOL sgbChanged;

// extra header -- non-standard .MPQ stuff
#define FILE_EXHDR_SIZE		72
#define FILE_EXHDR_OFFSET	sizeof(FILEHEADER)

// block table
#define BLOCK_ENTRIES		2048
#define BLOCK_TBL_SIZE		(BLOCK_ENTRIES * sizeof(BLOCKENTRY))
#define BLOCK_TBL_OFFSET	(FILE_EXHDR_OFFSET + FILE_EXHDR_SIZE)
static BLOCKENTRYPTR 		sgpBlockTbl;

// hash table
#define HASH_ENTRIES		2048	// must be pow2
#define HASH_TBL_SIZE		(HASH_ENTRIES * sizeof(HASHENTRY))
#define HASH_TBL_OFFSET		(BLOCK_TBL_OFFSET + BLOCK_TBL_SIZE)
static HASHENTRYPTR 		sgpHashTbl;

// offset past last file where next added file will start
#define FIRST_FILE_START	(HASH_TBL_OFFSET + HASH_TBL_SIZE)
static DWORD 				sgdwNextFileStart;

// minimum space we will allow as standalone "free" block in file
#define MIN_FREE_SIZE		1024

static BYTE sgbSaveCreationKey = FALSE;

#if IS_VERSION(SHAREWARE)
static char sgszArchiveKey[] = "Audio Playback ";
#else
static char sgszArchiveKey[] = "Video Player ";
#endif


#define CREATION_TIME	0
#define LASTWRITE_TIME	1
#define NUM_TIMES		2

typedef struct EXFILEHEADER {
	BYTE bData[FILE_EXHDR_SIZE];
} EXFILEHEADER;

typedef struct FULLHEADER {
	FILEHEADER hdr;
	EXFILEHEADER exhdr;
} FULLHEADER;


//******************************************************************
// imported functions
//******************************************************************
void InitializeHashSource();
void Decrypt(LPDWORD data, DWORD bytes, DWORD key);
void Encrypt(LPDWORD data, DWORD bytes, DWORD key);
DWORD Hash(const char *filename, int hashtype);
DWORD Compress(LPBYTE data, DWORD bytes);


//******************************************************************
//******************************************************************
static void xor_timestamp(FILETIME * pTime) {
	DWORD dwKey = 0xf0761ab;
	LPBYTE lpStamp = (LPBYTE) pTime;
	DWORD dwBytes = sizeof(FILETIME);
	while (dwBytes--) {
		*lpStamp++ ^= (BYTE) dwKey;
		dwKey = _rotl(dwKey,1);
	}
}


//******************************************************************
//******************************************************************
static BOOL reg_get_stamps(FILETIME * pft,DWORD dwSize) {
	ZeroMemory(pft,dwSize);
	DWORD dwBytes;
	if (! SRegLoadData(gszProgKey,sgszArchiveKey,0,pft,dwSize,&dwBytes))
		return FALSE;
	if (dwBytes != dwSize)
		return FALSE;

	while (dwSize >= sizeof FILETIME) {
		xor_timestamp(pft++);
		dwSize -= sizeof FILETIME;
	}
	return TRUE;
}


//******************************************************************
//******************************************************************
static void reg_set_stamps(FILETIME * pft,DWORD dwSize) {
	// only update registry stamps for multiplayer mode
	app_assert(gbMaxPlayers != 1);

	FILETIME * pft2 = pft;
	for (DWORD d = dwSize; d >= sizeof FILETIME; d -= sizeof FILETIME)
		xor_timestamp(pft2++);

// pjw.patch1.start
//	SRegSaveData(gszProgKey,sgszArchiveKey,SREG_FLAG_FLUSHTODISK,pft,dwSize);
	SRegSaveData(gszProgKey,sgszArchiveKey,0,pft,dwSize);
// pjw.patch1.end
}


//******************************************************************
//******************************************************************
/* pjw.patch1.start
static int stamp_compare(FILETIME * pf1,FILETIME * pf2) {
	return ! memcmp(pf1,pf2,sizeof FILETIME);
}
pjw.patch1.end */


//******************************************************************
//******************************************************************
BOOL MPQSetAttributes(const char * pszArchive,BOOL bHide) {
	app_assert(pszArchive);

	// get the attributes for the file
	DWORD dwAttr;
	if (0xffffffff == (dwAttr = GetFileAttributes(pszArchive))) {
		if (ERROR_FILE_NOT_FOUND == GetLastError())
			return TRUE;
		return FALSE;
	}

	DWORD dwWantAttr = bHide ? MPQ_HIDE_ATTR : 0;
	if (dwAttr == dwWantAttr) return TRUE;
	return SetFileAttributes(pszArchive,dwWantAttr);
}


//******************************************************************
//******************************************************************
BOOL MPQCompareTimeStamps(const char * pszArchive,DWORD dwChar) {
/* pjw.patch1.start
	app_assert(pszArchive);
	app_assert(dwChar < MAX_CHARACTERS);

	// get archive timestamps from registry
	FILETIME ft[MAX_CHARACTERS][NUM_TIMES];
	if (! reg_get_stamps(&ft[0][0],sizeof ft)) return FALSE;

	// get archive file timestamps
	WIN32_FIND_DATA finddata;
	HANDLE findhandle = FindFirstFile(pszArchive,&finddata);
	if (findhandle == INVALID_HANDLE_VALUE) return FALSE;
	FindClose(findhandle);

	return
		stamp_compare(&finddata.ftCreationTime,&ft[dwChar][CREATION_TIME])
		&&
		stamp_compare(&finddata.ftLastWriteTime,&ft[dwChar][LASTWRITE_TIME]);
pjw.patch1.end */
	return TRUE;
}


//******************************************************************
//******************************************************************
void MPQMungeStamps(DWORD dwChar) {
/* pjw.patch1.start
	// only update registry stamps for multiplayer mode
	if (gbMaxPlayers == 1) return;
	app_assert(dwChar < MAX_CHARACTERS);

	FILETIME ft[MAX_CHARACTERS][NUM_TIMES];
	reg_get_stamps(&ft[0][0],sizeof ft);
	ft[dwChar][0].dwHighDateTime = 0x78341348;
	reg_set_stamps(&ft[0][0],sizeof ft);
pjw.patch1.end */
}


//******************************************************************
//******************************************************************
static void MPQUpdateLastWriteTimeStamp(const char * pszArchive,DWORD dwChar) {
	// only update registry stamps for multiplayer mode
	if (gbMaxPlayers == 1) return;

	app_assert(pszArchive);
	app_assert(dwChar < MAX_CHARACTERS);

	// get archive timestamps from registry
	FILETIME ft[MAX_CHARACTERS][NUM_TIMES];
	reg_get_stamps(&ft[0][0],sizeof ft);

	// get archive file timestamps
	WIN32_FIND_DATA finddata;
	HANDLE findhandle = FindFirstFile(pszArchive,&finddata);
	if (findhandle == INVALID_HANDLE_VALUE) return;
	FindClose(findhandle);

	// update timestamp
	ft[dwChar][LASTWRITE_TIME] = finddata.ftLastWriteTime;

	// save back to registry
	reg_set_stamps(&ft[0][0],sizeof ft);
}


//******************************************************************
//******************************************************************
void MPQUpdateCreationTimeStamp(const char * pszArchive,DWORD dwChar) {
	// only update registry stamps for multiplayer mode
	if (gbMaxPlayers == 1) return;
	app_assert(pszArchive);
	app_assert(dwChar < MAX_CHARACTERS);

	// get archive timestamps from registry
	FILETIME ft[MAX_CHARACTERS][NUM_TIMES];
	reg_get_stamps(&ft[0][0],sizeof ft);

	// get archive file timestamps
	WIN32_FIND_DATA finddata;
	HANDLE findhandle = FindFirstFile(pszArchive,&finddata);
	if (findhandle == INVALID_HANDLE_VALUE) return;
	FindClose(findhandle);

	// update timestamp
	ft[dwChar][CREATION_TIME] = finddata.ftCreationTime;

	reg_set_stamps(&ft[0][0],sizeof ft);
}


//******************************************************************
//******************************************************************
#if DUMP_BLOCK
static const char * fflags_2_str(DWORD dwFlags) {
	if (dwFlags & MPQ_ADD_COMPRESSED) {
		if (dwFlags & MPQ_ADD_ENCRYPTED)
			return "ENCRYPT+COMPRESS";
		return "COMPRESS";
	}

	if (dwFlags & MPQ_ADD_ENCRYPTED)
		return "ENCRYPT";
	return "NONE";
}
#endif


//******************************************************************
//******************************************************************
#if DUMP_BLOCK
static void dump_blocktbl(void) {

	FILE * f = fopen("c:\\block.txt","wb");
	if (! f) return;

	fprintf(f,"   start +     size =      end\r\n");
	DWORD dwPosition = FIRST_FILE_START;
	while (dwPosition != sgdwNextFileStart) {
		BLOCKENTRY * pBlk = sgpBlockTbl;
		for (DWORD i = BLOCK_ENTRIES; i--; pBlk++) {
			if (pBlk->offset != dwPosition) continue;
			fprintf(f,"%8d + %8d = %8d   %s\r\n",
				pBlk->offset,
				pBlk->sizealloc,
				pBlk->offset + pBlk->sizealloc,
				fflags_2_str(pBlk->flags)
			);
			dwPosition = pBlk->offset + pBlk->sizealloc;
			break;
		}
		if (i == -1) {
			fclose(f);
			void myDebugBreak();
			myDebugBreak();
			app_fatal("Bad block table");
		}
	}

	fprintf(f,"next file start: %d\r\n",sgdwNextFileStart);
	fclose(f);
}
#endif


//******************************************************************
//******************************************************************
static BLOCKENTRY * get_free_block(DWORD * pdwBlockIndex) {
	BLOCKENTRY * pBlk = sgpBlockTbl;
	for (DWORD i = 0; i < BLOCK_ENTRIES; i++, pBlk++) {
		if (pBlk->offset) continue;
		if (pBlk->sizealloc) continue;
		if (pBlk->flags) continue;
		if (pBlk->sizefile) continue;
		if (pdwBlockIndex) *pdwBlockIndex = i;
		return pBlk;
	}

	app_fatal("Out of free block entries");
	return NULL;
}


//******************************************************************
//******************************************************************
static void add_free_block(DWORD dwOffset,DWORD dwSize) {
	// see if we can merge this block with an existing free block
	BLOCKENTRY * pBlk = sgpBlockTbl;
	for (DWORD i = BLOCK_ENTRIES; i--; pBlk++) {
		// is this block unused?
		if (pBlk->offset == 0) continue;
		if (pBlk->flags != 0) continue;
		if (pBlk->sizefile != 0) continue;

		if (pBlk->offset + pBlk->sizealloc == dwOffset) {
			dwOffset = pBlk->offset;
			dwSize += pBlk->sizealloc;
		}
		else if (dwOffset + dwSize == pBlk->offset) {
			dwSize += pBlk->sizealloc;
		}
		else {
			continue;
		}

		// try adding the new larger block.
		// NOTE: in the worst case, we free a block which is between
		// two free blocks, which causes only one deep recursion
		ZeroMemory(pBlk,sizeof(*pBlk));
		add_free_block(dwOffset,dwSize);
		return;
	}

	if (dwOffset + dwSize > sgdwNextFileStart)
		app_fatal("MPQ free list error");

	// is this block at the end of the file?
	if (dwOffset + dwSize == sgdwNextFileStart) {
		sgdwNextFileStart = dwOffset;
		return;
	}

	// create a new block entry
	pBlk = get_free_block(NULL);
	pBlk->offset = dwOffset;
	pBlk->sizealloc = dwSize;
	pBlk->sizefile = 0;
	pBlk->flags = 0;
}


//******************************************************************
//******************************************************************
static DWORD get_free_space(DWORD dwSize,DWORD * pdwSizeAlloc) {
	DWORD dwOffset;

	// see if there is a space large enough in an existing block
	BLOCKENTRY * pBlk = sgpBlockTbl;
	for (DWORD i = BLOCK_ENTRIES; i--; pBlk++) {
		// is this block unused?
		if (pBlk->offset == 0) continue;
		if (pBlk->flags != 0) continue;
		if (pBlk->sizefile != 0) continue;
		if (pBlk->sizealloc < dwSize) continue;

		// use a portion of this block
		dwOffset = pBlk->offset;
		*pdwSizeAlloc = dwSize;

		// fixup this block
		pBlk->offset += dwSize;
		pBlk->sizealloc -= dwSize;

		// did we use the entire block?
		if (! pBlk->sizealloc) ZeroMemory(pBlk,sizeof(BLOCKENTRY));

		return dwOffset;
	}

	// use free space at end of .MPQ file
	*pdwSizeAlloc = dwSize;
	dwOffset = sgdwNextFileStart;
	sgdwNextFileStart += dwSize;
	return dwOffset;
}


//******************************************************************
//******************************************************************
static DWORD SearchHashEntry(
	DWORD hashindex,
	DWORD hashcheck0,
	DWORD hashcheck1,
	LCID lcid
) {
	DWORD dwCount = HASH_ENTRIES;
	for (
		DWORD entry = hashindex & (HASH_ENTRIES - 1);
		(sgpHashTbl+entry)->block != HASH_BLOCK_UNUSED;
		entry = (entry + 1) & (HASH_ENTRIES - 1)
	) {
		if (! dwCount--) break;

		if ((sgpHashTbl+entry)->hashcheck[0] != hashcheck0)
			continue;
		if ((sgpHashTbl+entry)->hashcheck[1] != hashcheck1)
			continue;
		if ((sgpHashTbl+entry)->lcid != lcid)
			continue;
		if ((sgpHashTbl+entry)->block == HASH_BLOCK_FREED)
			continue;

		return entry;
	}

	return HASH_ENTRY_UNUSED;
}


//******************************************************************
//******************************************************************
static BOOL WriteFileHeader() {
	FULLHEADER fhdr;

	// initialize header
	ZeroMemory(&fhdr,sizeof(fhdr));

	// fill in .mpq header
	fhdr.hdr.signature = SIGNATURE;
	fhdr.hdr.headersize = sizeof(FILEHEADER);
	fhdr.hdr.filesize = GetFileSize(sghArchive,NULL);
	fhdr.hdr.version = VERSION;
	fhdr.hdr.sectorsizeid = SECTORSIZEID;
	fhdr.hdr.hashoffset = HASH_TBL_OFFSET;
	fhdr.hdr.blockoffset = BLOCK_TBL_OFFSET;
	fhdr.hdr.hashcount = HASH_ENTRIES;
	fhdr.hdr.blockcount = BLOCK_ENTRIES;

	DWORD dwTemp;
	if (0xffffffff == SetFilePointer(sghArchive,0,NULL,FILE_BEGIN))
		return FALSE;
	if (! WriteFile(sghArchive,&fhdr.hdr,sizeof(fhdr),&dwTemp,NULL))
		return FALSE;
	return sizeof(fhdr) == dwTemp;
}


//******************************************************************
//******************************************************************
static BOOL WriteBlockTable() {
	#if DUMP_BLOCK
	dump_blocktbl();
	#endif

	if (0xffffffff == SetFilePointer(sghArchive,BLOCK_TBL_OFFSET,NULL,FILE_BEGIN))
		return FALSE;

	DWORD dwTemp;
	Encrypt((LPDWORD) sgpBlockTbl,BLOCK_TBL_SIZE,Hash("(block table)",HASH_ENCRYPTKEY));
	BOOL bResult = WriteFile(sghArchive,sgpBlockTbl,BLOCK_TBL_SIZE,&dwTemp,NULL);
	Decrypt((LPDWORD) sgpBlockTbl,BLOCK_TBL_SIZE,Hash("(block table)",HASH_ENCRYPTKEY));

	return bResult && BLOCK_TBL_SIZE == dwTemp;
}


//******************************************************************
//******************************************************************
static BOOL WriteHashTable() {
	if (0xffffffff == SetFilePointer(sghArchive,HASH_TBL_OFFSET,NULL,FILE_BEGIN))
		return FALSE;

	DWORD dwTemp;
	Encrypt((LPDWORD) sgpHashTbl,HASH_TBL_SIZE,Hash("(hash table)",HASH_ENCRYPTKEY));
	BOOL bResult = WriteFile(sghArchive,sgpHashTbl,HASH_TBL_SIZE,&dwTemp,NULL);
	Decrypt((LPDWORD) sgpHashTbl,HASH_TBL_SIZE,Hash("(hash table)",HASH_ENCRYPTKEY));

	return bResult && HASH_TBL_SIZE == dwTemp;
}


//******************************************************************
//******************************************************************
static BOOL SetEOF() {
	if (0xffffffff == SetFilePointer(sghArchive,sgdwNextFileStart,NULL,FILE_BEGIN))
		return FALSE;
	return SetEndOfFile(sghArchive);
}


//******************************************************************
//******************************************************************
static BOOL read_mpq_file_hdr(FULLHEADER * pHdr,DWORD * pdwNextFileStart) {
	BOOL bError;
	DWORD dwBytes;
	app_assert(pHdr);
	app_assert(pdwNextFileStart);

	// read file hdr
	DWORD dwFileSize = GetFileSize(sghArchive,NULL);
	*pdwNextFileStart = dwFileSize;
	if (dwFileSize == 0xffffffff)
		bError = TRUE;
	else if (sizeof(*pHdr) > dwFileSize)
		bError = TRUE;
	else if (! ReadFile(sghArchive,pHdr,sizeof(*pHdr),&dwBytes,NULL))
		bError = TRUE;
	else if (sizeof(*pHdr) != dwBytes)
		bError = TRUE;
	else if (pHdr->hdr.signature != SIGNATURE)
		bError = TRUE;
	else if (pHdr->hdr.headersize != sizeof(FILEHEADER))
		bError = TRUE;
	else if (pHdr->hdr.version > VERSION)
		bError = TRUE;
	else if (pHdr->hdr.sectorsizeid != SECTORSIZEID)
		bError = TRUE;
	else if (pHdr->hdr.filesize != dwFileSize)
		bError = TRUE;
	else if (pHdr->hdr.hashoffset != HASH_TBL_OFFSET)
		bError = TRUE;
	else if (pHdr->hdr.blockoffset != BLOCK_TBL_OFFSET)
		bError = TRUE;
	else if (pHdr->hdr.hashcount != HASH_ENTRIES)
		bError = TRUE;
	else if (pHdr->hdr.blockcount != BLOCK_ENTRIES)
		bError = TRUE;
	else // NO ERROR (finally)
		bError = FALSE;

	if (bError) {
		// kill off any existing file hdr information
		if (0xffffffff == SetFilePointer(sghArchive,0,NULL,FILE_BEGIN))
			return FALSE;
		if (! SetEndOfFile(sghArchive))
			return FALSE;

		// initialize file header
		ZeroMemory(pHdr,sizeof(*pHdr));

		// fill in .mpq header
		pHdr->hdr.signature = SIGNATURE;
		pHdr->hdr.headersize = sizeof(FILEHEADER);
		pHdr->hdr.sectorsizeid = SECTORSIZEID;
		pHdr->hdr.version = VERSION;

		// set location of start of next file
		*pdwNextFileStart = FIRST_FILE_START;

		// modified the file
		sgbChanged = TRUE;
		sgbSaveCreationKey = TRUE;
	}

	return TRUE;
}


//******************************************************************
//******************************************************************
static DWORD SearchHashName(const char * pszName) {
	return SearchHashEntry(
		Hash(pszName,HASH_INDEX),
		Hash(pszName,HASH_CHECK0),
		Hash(pszName,HASH_CHECK1),
		MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT)
	);
}


//******************************************************************
//******************************************************************
static void mpq_delete_file(const char * pszName) {
	DWORD dwEntry = SearchHashName(pszName);
	if (dwEntry == HASH_ENTRY_UNUSED)
		return;

	HASHENTRY * pHash = sgpHashTbl + dwEntry;
	BLOCKENTRY * pBlk = sgpBlockTbl + pHash->block;

	// we can't set the hash block to HASH_BLOCK_UNUSED, because
	// we use closed hashing.  Another file may have hashed
	// to the same spot, and so was added to the hash table
	// after this hash entry.  Therefore, just mark the hash
	// entry as freed, but not unused
	pHash->block = HASH_BLOCK_FREED;

	// free the block entry and give back the memory
	DWORD dwOffset = pBlk->offset;
	DWORD dwSize = pBlk->sizealloc;
	ZeroMemory(pBlk,sizeof(*pBlk));
	add_free_block(dwOffset,dwSize);

	// modified the file
	sgbChanged = TRUE;
}


//******************************************************************
//******************************************************************
void MPQDeleteFile(const char * pszName) {
	app_assert(sghArchive != INVALID_HANDLE_VALUE);
	app_assert(pszName);

	// if the specified file is not part of the archive,
	// then consider the delete operation successful
	mpq_delete_file(pszName);
}


//******************************************************************
//******************************************************************
void MPQDeleteFiles(TGetNameFcn fnGetName) {
	app_assert(sghArchive != INVALID_HANDLE_VALUE);
	app_assert(fnGetName);

	DWORD dwIndex = 0;
	char szName[MAX_PATH];
	while (fnGetName(dwIndex++,szName))
		mpq_delete_file(szName);
}


//******************************************************************
//******************************************************************
static BLOCKENTRY * InsertIntoHash(const char * pszName,BLOCKENTRY * pBlk,DWORD dwBlock) {
	DWORD hashindex		= Hash(pszName,HASH_INDEX);
	DWORD hashcheck0	= Hash(pszName,HASH_CHECK0);
	DWORD hashcheck1	= Hash(pszName,HASH_CHECK1);

	if (HASH_ENTRY_UNUSED != SearchHashEntry(
		hashindex,
		hashcheck0,
		hashcheck1,
		MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT)))
		app_fatal("Hash collision between \"%s\" and existing file\n",pszName);

	// find free slot in hash table
	long lCount = HASH_ENTRIES;
	DWORD entry = hashindex & (HASH_ENTRIES - 1);
	while (lCount--) {
		if ((sgpHashTbl+entry)->block == HASH_BLOCK_UNUSED) break;
		if ((sgpHashTbl+entry)->block == HASH_BLOCK_FREED) break;
		entry = (entry + 1) & (HASH_ENTRIES - 1);
	}
	if (lCount < 0) app_fatal("Out of hash space");

	if (! pBlk) pBlk = get_free_block(&dwBlock);
	(sgpHashTbl+entry)->hashcheck[0]	= hashcheck0;
	(sgpHashTbl+entry)->hashcheck[1]	= hashcheck1;
	(sgpHashTbl+entry)->lcid			= MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT);
	(sgpHashTbl+entry)->block			= dwBlock;
	return pBlk;
}


//******************************************************************
//******************************************************************
static BOOL WriteFileData(
	const char * pszName,
	const BYTE * pbData,
	DWORD dwLen,
	BLOCKENTRY * pBlk
) {
	// DETERMINE THE FILE NAME PORTION OF THE PATH NAME
	const char * pszTemp;
	while (NULL != (pszTemp = strchr(pszName,':')))
		pszName = pszTemp + 1;
	while (NULL != (pszTemp = strchr(pszName,'\\')))
		pszName = pszTemp + 1;

	// CREATE AN ENCRYPTION KEY BASED ON THE FILE NAME
	DWORD key = Hash(pszName,HASH_ENCRYPTKEY);
	DWORD sectors = (dwLen + SECTORSIZE - 1) / SECTORSIZE;
	DWORD sectoroffsetsize = (sectors + 1) * sizeof(DWORD);

	// find free space for file
	DWORD dwSizeAllocGuess = dwLen + sectoroffsetsize;
	pBlk->offset = get_free_space(dwSizeAllocGuess,&pBlk->sizealloc);
	pBlk->sizefile = dwLen;
	pBlk->flags = MPQ_ADD_COMPRESSED | MPQ_ADD_ALLOCATED;
	if (0xffffffff == SetFilePointer(sghArchive,pBlk->offset,NULL,FILE_BEGIN))
		return FALSE;

	DWORD sector = 0;
	DWORD destsize = 0;
	static BYTE buffer[SECTORSIZE];
	LPDWORD sectoroffsettable = NULL;
	while (dwLen) {
		for (int loop = 0; loop < SECTORSIZE; ++loop)
			buffer[loop] += 0xAA;

		// PERFORM COMPRESSION
		DWORD bytes = min(dwLen,SECTORSIZE);
		CopyMemory(buffer,pbData,bytes);
		pbData += bytes;
		bytes = Compress(buffer,bytes);

		// LEAVE SPACE FOR THE SECTOR OFFSET TABLE.
		if (! sector) {
			sectoroffsetsize = (sectors+1)*sizeof(DWORD);
			sectoroffsettable = (LPDWORD) DiabloAllocPtrSig(sectoroffsetsize,'MPQt');
			ZeroMemory(sectoroffsettable,sectoroffsetsize);
			if (! WriteFile(sghArchive,sectoroffsettable,sectoroffsetsize,&sectoroffsetsize,NULL))
				goto error;
			destsize += sectoroffsetsize;
		}

		// SAVE THE SECTOR OFFSET
		app_assert(sectoroffsettable);
		*(sectoroffsettable+sector) = destsize;

		// PERFORM ENCRYPTION -- not encrypted for save files
		// Encrypt((LPDWORD)buffer,bytes & 0xFFFFFFFC,key+sector);

		// WRITE THE SECTOR
		if (! WriteFile(sghArchive,buffer,bytes,&bytes,NULL))
			goto error;

		++sector;
		if (dwLen > SECTORSIZE)
			dwLen -= SECTORSIZE;
		else
			dwLen = 0;
		destsize += bytes;
	}

	app_assert(sectoroffsettable);
	*(sectoroffsettable+sector) = destsize;
	// bug in mopaq 1.91 -- fixed in 1.92
	// sector table is not supposed to be encrypted for unencrypted files
	// Encrypt(sectoroffsettable,sectoroffsetsize,key+0xFFFFFFFF);
	if (0xffffffff == SetFilePointer(sghArchive,-(LONG)destsize,NULL,FILE_CURRENT))
		goto error;
	if (! WriteFile(sghArchive,sectoroffsettable,sectoroffsetsize,&sectoroffsetsize,NULL))
		goto error;
	if (0xffffffff == SetFilePointer(sghArchive,destsize-sectoroffsetsize,NULL,FILE_CURRENT))
		goto error;
	DiabloFreePtr(sectoroffsettable);
	
	// make sure the file fit into the hole we provided
	app_assert(destsize <= pBlk->sizealloc);

	// give back any extra space we might have allocated
	if (destsize < pBlk->sizealloc) {
		DWORD dwLeftover = pBlk->sizealloc - destsize;
		if (dwLeftover >= MIN_FREE_SIZE) {
			pBlk->sizealloc = destsize;
			add_free_block(pBlk->offset + destsize, dwLeftover);
		}
	}

	return TRUE;
error:
	if (sectoroffsettable) DiabloFreePtr(sectoroffsettable);
	return FALSE;
}


//******************************************************************
//******************************************************************
BOOL MPQAddFile(const char * pszName,const BYTE * pbData,DWORD dwLen) {
	app_assert(sghArchive != INVALID_HANDLE_VALUE);
	app_assert(pszName);
	app_assert(pbData);
	app_assert(dwLen);

	// think positive
	BOOL bResult = TRUE;
	sgbChanged = TRUE;

	// delete any existing file with the same name
	mpq_delete_file(pszName);

	// insert the new file data
	BLOCKENTRY * pBlk = InsertIntoHash(pszName,NULL,0);
	if (! WriteFileData(pszName,pbData,dwLen,pBlk)) {
		mpq_delete_file(pszName);
		return FALSE;
	}

	return TRUE;
}


//******************************************************************
//******************************************************************
void MPQRenameFile(const char * pszOld,const char * pszNew) {
	app_assert(sghArchive != INVALID_HANDLE_VALUE);
	app_assert(pszOld);
	app_assert(pszNew);
	DWORD dwEntry = SearchHashName(pszOld);
	if (dwEntry == HASH_ENTRY_UNUSED)
		return;

	HASHENTRY * pHash = sgpHashTbl + dwEntry;
	BLOCKENTRY * pBlk = sgpBlockTbl + pHash->block;
	DWORD dwBlock = pHash->block;

	// we can't set the hash block to HASH_BLOCK_UNUSED, because
	// we use closed hashing.  Another file may have hashed
	// to the same spot, and so was added to the hash table
	// after this hash entry.  Therefore, just mark the hash
	// entry as freed, but not unused
	pHash->block = HASH_BLOCK_FREED;

	// create a new hash entry which references the existing block
	InsertIntoHash(pszNew,pBlk,dwBlock);

	// modified the file
	sgbChanged = TRUE;
}


//******************************************************************
//******************************************************************
BOOL MPQFileExists(const char * pszName) {
	app_assert(sghArchive != INVALID_HANDLE_VALUE);
	app_assert(pszName);
	return SearchHashName(pszName) != HASH_ENTRY_UNUSED;
}


//******************************************************************
//******************************************************************
static void CloseArchive(const char * pszArchive,BOOL bFree,DWORD dwChar) {

	if (bFree) {
		DiabloFreePtr(sgpBlockTbl);
		DiabloFreePtr(sgpHashTbl);
	}

	if (sghArchive != INVALID_HANDLE_VALUE) {
		CloseHandle(sghArchive);
		sghArchive = INVALID_HANDLE_VALUE;
	}

	if (sgbChanged) {
		// since we wrote the file, update the last write timestamp
		sgbChanged = FALSE;
		MPQUpdateLastWriteTimeStamp(pszArchive,dwChar);
	}

	if (sgbSaveCreationKey) {
		// update the file creation timestamp if the archive was just created
		sgbSaveCreationKey = FALSE;
		MPQUpdateCreationTimeStamp(pszArchive,dwChar);
	}
}


//******************************************************************
//******************************************************************
BOOL MPQOpenArchive(const char * pszArchive,BOOL bHide,DWORD dwChar) {
	DWORD dwTemp;
	app_assert(pszArchive);
	app_assert(sghArchive == INVALID_HANDLE_VALUE);
	InitializeHashSource();

	if (! MPQSetAttributes(pszArchive,bHide))
		return FALSE;

	DWORD dwFlags = gbMaxPlayers > 1 ? FILE_FLAG_WRITE_THROUGH : 0;

	sgbSaveCreationKey = FALSE;
	sghArchive = CreateFile(
		pszArchive,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		dwFlags,
		NULL
	);

	if (sghArchive == INVALID_HANDLE_VALUE) {
		DWORD dwAttr = bHide ? MPQ_HIDE_ATTR : 0;
		if (INVALID_HANDLE_VALUE == (sghArchive = CreateFile(
			pszArchive,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			dwAttr | dwFlags,
			NULL
		))) return FALSE;
		sgbSaveCreationKey = TRUE;
		sgbChanged = TRUE;
	}

	// if the file is still loaded in memory, skip reading file
	if (sgpBlockTbl && sgpHashTbl)
		return TRUE;

	FULLHEADER fhdr;
	ZeroMemory(&fhdr,sizeof(fhdr));
	if (! read_mpq_file_hdr(&fhdr,&sgdwNextFileStart))
		goto error;

	// allocate enough memory for all the blocks we might need
	app_assert(! sgpBlockTbl);
	sgpBlockTbl = (BLOCKENTRYPTR) DiabloAllocPtrSig(BLOCK_TBL_SIZE,'MPQt');
	ZeroMemory(sgpBlockTbl,BLOCK_TBL_SIZE);

	// read in block table
	if (fhdr.hdr.blockcount) {
		app_assert(fhdr.hdr.blockcount == BLOCK_ENTRIES);
		app_assert(fhdr.hdr.blockoffset == BLOCK_TBL_OFFSET);
		if (0xffffffff == SetFilePointer(sghArchive,BLOCK_TBL_OFFSET,NULL,FILE_BEGIN))
			goto error;
		if (! ReadFile(sghArchive,sgpBlockTbl,BLOCK_TBL_SIZE,&dwTemp,NULL))
			goto error;
		app_assert(BLOCK_TBL_SIZE == dwTemp);
		Decrypt(
			(LPDWORD) sgpBlockTbl,
			BLOCK_TBL_SIZE,
			Hash("(block table)",
			HASH_ENCRYPTKEY)
		);
	}

	// allocate enough memory for hash table
	app_assert(! sgpHashTbl);
	sgpHashTbl = (HASHENTRYPTR) DiabloAllocPtrSig(HASH_TBL_SIZE,'MPQt');
	FillMemory(sgpHashTbl,HASH_TBL_SIZE,0xff);

	// read in hash table
	if (fhdr.hdr.hashcount) {
		app_assert(fhdr.hdr.hashcount == HASH_ENTRIES);
		app_assert(fhdr.hdr.hashoffset == HASH_TBL_OFFSET);
		if (0xffffffff == SetFilePointer(sghArchive,HASH_TBL_OFFSET,NULL,FILE_BEGIN))
			goto error;
		if (! ReadFile(sghArchive,sgpHashTbl,HASH_TBL_SIZE,&dwTemp,NULL))
			goto error;
		app_assert(HASH_TBL_SIZE == dwTemp);
		Decrypt(
			(LPDWORD) sgpHashTbl,
			HASH_TBL_SIZE,
			Hash("(hash table)",
			HASH_ENCRYPTKEY)
		);
	}

	return TRUE;

error:
	CloseArchive(pszArchive,TRUE,dwChar);
	return FALSE;
}


//******************************************************************
//******************************************************************
BOOL MPQCloseArchive(const char * pszArchive,BOOL bFree,DWORD dwChar) {
	// write archive data only if the file open & changed
	BOOL bResult;
	if (sghArchive == INVALID_HANDLE_VALUE) bResult = TRUE;
	else if (! sgbChanged) bResult = TRUE;
	else if (! SetEOF()) bResult = FALSE;
	else if (! WriteFileHeader()) bResult = FALSE;
	else if (! WriteBlockTable()) bResult = FALSE;
	else if (! WriteHashTable()) bResult = FALSE;
	else bResult = TRUE;

	CloseArchive(pszArchive,bFree,dwChar);
	return bResult;
}
