/****************************************************************************
*
*  CACHE.CPP
*  battle.net file caching functions
*
*  By Michael O'Brien (11/24/96)
*
***/

#include "pch.h"

#define  ARCHIVENAME       "bncache.dat"
#define  DEFENTRIES        1024  // must be a power of two
#define  DEFLCID           MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT)
#define  MAXFILESIZE       0x01000000
#define  FILEFLAGS         0xC0000000
#define  SIGNATURE         0x1A434E42
#define  DEALLOCATED       0xFFFFFFFE
#define  NOBLOCK           0xFFFFFFFF

#define  HASH_INDEX        0
#define  HASH_CHECK0       1
#define  HASH_CHECK1       2
#define  HASH_ENCRYPTKEY   3
#define  HASH_ENCRYPTDATA  4

typedef struct _ARCHIVEHEADER {
    DWORD signature;
    DWORD headersize;
    DWORD archivesize;
    DWORD version;
    DWORD hashoffset;
    DWORD blockoffset;
    DWORD hashentries;
    DWORD blockentries;
} ARCHIVEHEADER, *ARCHIVEHEADERPTR;

typedef struct _HASHENTRY {
    DWORD hashcheck[2];
    LCID  lcid;
    DWORD block;
} HASHENTRY, *HASHENTRYPTR;

typedef struct _BLOCKENTRY {
    DWORD offset;
    DWORD sizealloc;
    DWORD sizefile;
    DWORD flags;
} BLOCKENTRY, *BLOCKENTRYPTR;

typedef struct _FILEHEADER {
    DWORD    headersize;    // must be first field
    DWORD    headerversion;  
    DWORD    attributes;
    DWORD    reserved1;
    FILETIME lastaccesstime;
    FILETIME lastwritetime;
    FILETIME creationtime;
    FILETIME minexpiretime;
    FILETIME maxexpiretime;
    char     filename[MAX_PATH];
} FILEHEADER, *FILEHEADERPTR;

static HANDLE           cache_archivefile   = INVALID_HANDLE_VALUE;
static ARCHIVEHEADERPTR cache_archiveheader = NULL;
static BLOCKENTRYPTR    cache_blocktable    = NULL;
static CCritSect        cache_critsect;
static LPDWORD          cache_hashsource    = NULL;
static HASHENTRYPTR     cache_hashtable     = NULL;
static BOOL             cache_initialized   = 0;

//===========================================================================
static void inline Decrypt (LPDWORD data, DWORD bytes, DWORD key) {
  DWORD adjust = 0xEEEEEEEE;
  DWORD iter   = bytes >> 2;
  while (iter--) {
    adjust += *(cache_hashsource+(HASH_ENCRYPTDATA << 8)+(key & 0xFF));
    adjust += (*data++ ^= adjust+key)+(adjust << 5)+3;
    key     = (key >> 11) | ((key << 21) ^ 0xFFE00000)+0x11111111;
  }
}

//===========================================================================
static void inline Encrypt (LPDWORD data, DWORD bytes, DWORD key) {
  DWORD adjust = 0xEEEEEEEE;
  DWORD iter   = bytes >> 2;
  while (iter--) {
    DWORD origdata = *data;
    adjust  += *(cache_hashsource+(HASH_ENCRYPTDATA << 8)+(key & 0xFF));
    *data++  = origdata ^ (adjust+key);
    adjust  += origdata + (adjust << 5)+3;
    key      = (key >> 11) | ((key << 21) ^ 0xFFE00000)+0x11111111;
  }
}

//===========================================================================
static FILETIME *GetCurrentFileTime (DWORD offsetseconds) {
  static FILETIME filetime;
  static DWORD    lastoffset = 0;
  static DWORD    lastcall   = 0;
  if ((offsetseconds  != lastoffset) ||
      (GetTickCount() != lastcall)) {
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    SystemTimeToFileTime(&systime,&filetime);
    __int64 offset = 10000000ul;
    offset *= (__int64)offsetseconds;
    *(__int64 *)&filetime += offset;
    lastoffset = offsetseconds;
    lastcall   = GetTickCount();
  }
  return &filetime;
}

//===========================================================================
static DWORD inline Hash (LPCSTR filename, int hashtype) {
  DWORD result = 0x7FED7FED;
  DWORD adjust = 0xEEEEEEEE;
  while (filename && *filename) {
    char origchar = toupper(*filename++);
    result  = (result+adjust) ^ *(cache_hashsource+(hashtype << 8)+origchar);
    adjust += origchar+result+(adjust << 5)+3;
  }
  return result;
}

//===========================================================================
static void inline InitializeHashSource () {
  if (cache_hashsource) {
    DWORD seed = 0x100001;
    for (int loop1 = 0; loop1 < 256; ++loop1)
      for (int loop2 = 0; loop2 < 5; ++loop2) {
        seed = (seed*0x7D+3) % 0x2AAAAB;
        DWORD rand1 = seed & 0xFFFF;
        seed = (seed*0x7D+3) % 0x2AAAAB;
        DWORD rand2 = seed & 0xFFFF;
        *(cache_hashsource+(loop2 << 8)+loop1) = (rand1 << 16) | rand2;
      }
  }
}

//===========================================================================
static DWORD inline SearchHashTable (LPCSTR filename, LCID lcid) {
  DWORD hashindex  = Hash(filename,HASH_INDEX);
  DWORD hashcheck0 = Hash(filename,HASH_CHECK0);
  DWORD hashcheck1 = Hash(filename,HASH_CHECK1);
  DWORD entry      = hashindex & (cache_archiveheader->hashentries-1);
  DWORD firstentry = entry;
  DWORD found      = 0xFFFFFFFF;
  while ((cache_hashtable+entry)->block != NOBLOCK) {
    if (((cache_hashtable+entry)->hashcheck[0] == hashcheck0) &&
        ((cache_hashtable+entry)->hashcheck[1] == hashcheck1) &&
        ((cache_hashtable+entry)->block != DEALLOCATED))
      if ((cache_hashtable+entry)->lcid == lcid)
        return entry;
      else if ((cache_hashtable+entry)->lcid == MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT))
        found = entry;
    entry = (entry+1) & (cache_archiveheader->hashentries-1);
    if (entry == firstentry)
      break;
  }
  return found;
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
void CacheDestroy () {
  cache_critsect.Enter();
  cache_initialized = 0;
  if (cache_blocktable) {
    FREE(cache_blocktable);
    cache_blocktable = NULL;
  }
  if (cache_hashtable) {
    FREE(cache_hashtable);
    cache_hashtable = NULL;
  }
  if (cache_archiveheader) {
    FREE(cache_archiveheader);
    cache_archiveheader = NULL;
  }
  if (cache_archivefile) {
    CloseHandle(cache_archivefile);
    cache_archivefile = INVALID_HANDLE_VALUE;
  }
  if (cache_hashsource) {
    FREE(cache_hashsource);
    cache_hashsource = NULL;
  }
  cache_critsect.Leave();
}

//===========================================================================
BOOL CacheFree (LPVOID data, DWORD databytes) {
  if (!data)
    return 0;
  FREE(data);
  return 1;
}

//===========================================================================
BOOL CacheInitialize () {
  cache_critsect.Enter();
  if (cache_initialized)
    CacheDestroy();
  __try {

    // INITIALIZE THE HASH TABLES 
    cache_hashsource = (LPDWORD)ALLOC(5*256*sizeof(DWORD));
    if (cache_hashsource)
      InitializeHashSource();
    else
      __leave;

    // DETERMINE THE PATH TO THE ARCHIVE FILE
    TCHAR archivefilename[MAX_PATH] = "";
    GetModuleFileName(GetModuleHandle(NULL),archivefilename,MAX_PATH);
    {
      LPTSTR separator = strchr(archivefilename,'\\');
      while (separator && strchr(separator+1,'\\'))
        separator = strchr(separator+1,'\\');
      if (separator)
        *separator = 0;
    }
    strcat(archivefilename,"\\" ARCHIVENAME);

    // OPEN THE ARCHIVE FILE
    cache_archivefile = CreateFile(archivefilename,
                                   GENERIC_READ | GENERIC_WRITE,
                                   0,
                                   NULL,
                                   OPEN_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    if (cache_archivefile == INVALID_HANDLE_VALUE)
      __leave;

    // LOAD THE ARCHIVE HEADER
    cache_archiveheader = NEW(ARCHIVEHEADER);
    if (!cache_archiveheader)
      __leave;
    {
      DWORD bytesread = 0;
      ReadFile(cache_archivefile,cache_archiveheader,sizeof(ARCHIVEHEADER),&bytesread,NULL);
    }
    if (cache_archiveheader->signature != SIGNATURE) {
      ZeroMemory(cache_archiveheader,sizeof(ARCHIVEHEADER));
      cache_archiveheader->signature  = SIGNATURE;
      cache_archiveheader->headersize = sizeof(ARCHIVEHEADER);
    }

    // LOAD THE HASH TABLE
    DWORD hashentries = max(DEFENTRIES,cache_archiveheader->hashentries);
    cache_hashtable = (HASHENTRYPTR)ALLOC(hashentries*sizeof(HASHENTRY));
    if (cache_hashtable)
      ZeroMemory(cache_hashtable,hashentries*sizeof(HASHENTRY));
    else
      __leave;
    if (cache_archiveheader->hashoffset || cache_archiveheader->hashentries) {
      SetFilePointer(cache_archivefile,cache_archiveheader->hashoffset,NULL,FILE_BEGIN);
      DWORD bytesread;
      ReadFile(cache_archivefile,cache_hashtable,hashentries*sizeof(HASHENTRY),&bytesread,NULL);
      Decrypt((LPDWORD)cache_hashtable,hashentries*sizeof(HASHENTRY),Hash("(hash table)",HASH_ENCRYPTKEY));
    }
    else {
      cache_archiveheader->hashoffset  = sizeof(ARCHIVEHEADER);
      cache_archiveheader->hashentries = DEFENTRIES;
      cache_archiveheader->archivesize = max(cache_archiveheader->archivesize,
                                             cache_archiveheader->hashoffset
                                               +DEFENTRIES*sizeof(HASHENTRY));
      for (DWORD loop = 0; loop < DEFENTRIES; ++loop)
        (cache_hashtable+loop)->block = NOBLOCK;
    }

    // LOAD THE BLOCK TABLE
    DWORD blockentries = max(DEFENTRIES,cache_archiveheader->blockentries);
    cache_blocktable = (BLOCKENTRYPTR)ALLOC(blockentries*sizeof(BLOCKENTRY));
    if (cache_blocktable)
      ZeroMemory(cache_blocktable,DEFENTRIES*sizeof(BLOCKENTRY));
    else
      __leave;
    if (cache_archiveheader->blockoffset || cache_archiveheader->blockentries) {
      SetFilePointer(cache_archivefile,cache_archiveheader->blockoffset,NULL,FILE_BEGIN);
      DWORD bytesread;
      ReadFile(cache_archivefile,cache_blocktable,blockentries*sizeof(BLOCKENTRY),&bytesread,NULL);
      Decrypt((LPDWORD)cache_blocktable,blockentries*sizeof(BLOCKENTRY),Hash("(block table)",HASH_ENCRYPTKEY));
    }
    else {
      cache_archiveheader->blockoffset = sizeof(ARCHIVEHEADER)+DEFENTRIES*sizeof(HASHENTRY);
      cache_archiveheader->archivesize = max(cache_archiveheader->archivesize,
                                             cache_archiveheader->blockoffset
                                               +DEFENTRIES*sizeof(BLOCKENTRY));
    }

  }
  __finally {
    if (cache_archiveheader && cache_blocktable && cache_hashtable)
      cache_initialized = 1;
    else
      CacheDestroy();
  }
  cache_critsect.Leave();
  return 1;
}

//===========================================================================
BOOL CacheLoadFile (LPCSTR    filename,
                    FILETIME *lastwritetime,
                    LPVOID   *data,
                    DWORD    *databytes) {
  if (lastwritetime)
    ZeroMemory(lastwritetime,sizeof(FILETIME));
  if (data)
    *data = NULL;
  if (databytes)
    *databytes = 0;
  if (!(filename && *filename && data && databytes && cache_initialized))
    return 0;
  cache_critsect.Enter();

  // FIND THE REQUESTED FILE
  DWORD index = SearchHashTable(filename,DEFLCID);
  if (index == 0xFFFFFFFF) {
    cache_critsect.Leave();
    return 0;
  }
  DWORD block = (cache_hashtable+index)->block;
  if (block >= cache_archiveheader->blockentries) {
    cache_critsect.Leave();
    return 0;
  }

  // LOAD ITS DATA BLOCK
  FILEHEADER fileheader;
  ZeroMemory(&fileheader,sizeof(FILEHEADER));
  if ((cache_blocktable+block)->flags & FILEFLAGS) {
    SetFilePointer(cache_archivefile,
                   (cache_blocktable+block)->offset,
                   NULL,
                   FILE_BEGIN);
    DWORD bytesread;
    ReadFile(cache_archivefile,
             &fileheader.headersize,
             sizeof(DWORD),
             &bytesread,
             NULL);
    ReadFile(cache_archivefile,
             &fileheader.headersize+1,
             min(fileheader.headersize,sizeof(FILEHEADER))-sizeof(DWORD),
             &bytesread,
             NULL);
  }

  // VERIFY THAT IT IS THE CORRECT FILE AND THAT IT HASN'T EXPIRED
  if (_stricmp(filename,fileheader.filename)) {
    cache_critsect.Leave();
    return 0;
  }
  if (CompareFileTime(GetCurrentFileTime(0),&fileheader.maxexpiretime) > 0) {
    cache_critsect.Leave();
    return 0;
  }

  // VERIFY THAT THE SIZE IS REASONABLE
  if ((cache_blocktable+block)->sizefile > MAXFILESIZE) {
    cache_critsect.Leave();
    return 0;
  }

  // GET THE LAST WRITE TIME
  if (lastwritetime)
    CopyMemory(lastwritetime,&fileheader.lastwritetime,sizeof(FILETIME));

  // UPDATE THE LAST ACCESS TIME
  CopyMemory(&fileheader.lastaccesstime,GetCurrentFileTime(0),sizeof(FILETIME));
  SetFilePointer(cache_archivefile,
                 (cache_blocktable+block)->offset,
                 NULL,
                 FILE_BEGIN);
  {
    DWORD byteswritten;
    WriteFile(cache_archivefile,
              &fileheader,
              min(sizeof(FILEHEADER),fileheader.headersize),
              &byteswritten,
              NULL);
  }

  // ALLOCATE A BUFFER AND READ THE FILE DATA
  *data = ALLOC((cache_blocktable+block)->sizefile);
  if (!*data) {
    cache_critsect.Leave();
    return 0;
  }
  SetFilePointer(cache_archivefile,
                 (cache_blocktable+block)->offset+fileheader.headersize,
                 NULL,
                 FILE_BEGIN);
  ReadFile(cache_archivefile,
           *data,
           (cache_blocktable+block)->sizefile,
           databytes,
           NULL);

  cache_critsect.Leave();
  return 1;
}

//===========================================================================
BOOL CacheSaveFile (LPCSTR    filename,
                    LPVOID    data,
                    DWORD     databytes,
                    FILETIME *lastwritetime,
                    DWORD     minexpireseconds,
                    DWORD     maxexpireseconds) {
  if (!(filename && *filename && data && databytes && lastwritetime &&
        maxexpireseconds && cache_initialized))
    return 0;
  cache_critsect.Enter();

  // REMOVE ANY EXISTING INSTANCES OF THIS FILE
  {
    DWORD index = SearchHashTable(filename,DEFLCID);
    while (index != 0xFFFFFFFF) {
      DWORD block = (cache_hashtable+index)->block;
      if (block < cache_archiveheader->blockentries) {
        (cache_blocktable+block)->sizefile = 0;
        (cache_blocktable+block)->flags    = 0;
      }
      (cache_hashtable+index)->block = DEALLOCATED;
      index = SearchHashTable(filename,DEFLCID);
    }
  }

  // SEARCH FOR A FREE BLOCK WHICH IS BIG ENOUGH TO STORE THE FILE
  DWORD minalloc = sizeof(FILEHEADER)+databytes;
  DWORD block    = 0;
  while ((block < cache_archiveheader->blockentries) &&
         ((cache_blocktable+block)->flags ||
          ((cache_blocktable+block)->sizealloc < minalloc)))
    ++block;

  // IF WE DIDN'T FIND ONE, THEN SEARCH FOR A BLOCK WHICH IS PAST ITS
  // MINIMUM EXPIRATION TIME AND IS BIG ENOUGH TO STORE THE FILE
  if (block >= cache_archiveheader->blockentries) {
    block = 0;
    while (block < cache_archiveheader->blockentries) {
      if ((cache_blocktable+block)->sizealloc >= minalloc) {
        SetFilePointer(cache_archivefile,
                       (cache_blocktable+block)->offset,
                       NULL,
                       FILE_BEGIN);
        FILEHEADER fileheader;
        ZeroMemory(&fileheader,sizeof(FILEHEADER));
        DWORD bytesread;
        ReadFile(cache_archivefile,
                 &fileheader.headersize,
                 sizeof(DWORD),
                 &bytesread,
                 NULL);
        ReadFile(cache_archivefile,
                 &fileheader.headersize+1,
                 min(fileheader.headersize,sizeof(FILEHEADER))-sizeof(DWORD),
                 &bytesread,
                 NULL);
        if (CompareFileTime(GetCurrentFileTime(0),&fileheader.minexpiretime) > 0)
          break;
      }
      ++block;
    }
  }

  // IF WE FOUND A BLOCK TO USE, REMOVE ANY HASH TABLE REFERENCES TO IT
  if (block < cache_archiveheader->blockentries) {
    for (DWORD index = 0; index < cache_archiveheader->hashentries; ++index)
      if ((cache_hashtable+index)->block == block)
        (cache_hashtable+index)->block = DEALLOCATED;
  }

  // OTHERWISE, TRY TO CREATE A NEW BLOCK
  else {
    if (block >= DEFENTRIES)
      return 0;
    else {
      (cache_blocktable+block)->offset    = cache_archiveheader->archivesize;
      (cache_blocktable+block)->sizealloc = minalloc;
      cache_archiveheader->archivesize   += minalloc;
      cache_archiveheader->blockentries   = block+1;
    }
  }

  // ADD A HASH TABLE ENTRY FOR THE BLOCK
  {
    DWORD hashindex  = Hash(filename,HASH_INDEX);
    DWORD hashcheck0 = Hash(filename,HASH_CHECK0);
    DWORD hashcheck1 = Hash(filename,HASH_CHECK1);
    DWORD entry      = hashindex & (cache_archiveheader->hashentries-1);
    DWORD firstentry = entry;
    DWORD found      = NOBLOCK;
    while (((cache_hashtable+entry)->block != NOBLOCK) &&
           ((cache_hashtable+entry)->block != DEALLOCATED)) {
      entry = (entry+1) & (cache_archiveheader->hashentries-1);
      if (entry == firstentry)
        break;
    }
    if (((cache_hashtable+entry)->block == NOBLOCK) ||
        ((cache_hashtable+entry)->block == DEALLOCATED)) {
      (cache_hashtable+entry)->hashcheck[0] = hashcheck0;
      (cache_hashtable+entry)->hashcheck[1] = hashcheck1;
      (cache_hashtable+entry)->lcid         = DEFLCID;
      (cache_hashtable+entry)->block        = block;
    }
    else {
      cache_critsect.Leave();
      return 0;
    }
  }

  // FILL IN THE FILE'S BLOCK ENTRY
  (cache_blocktable+block)->sizefile = databytes;
  (cache_blocktable+block)->flags    = FILEFLAGS;

  // FILL IN THE FILE HEADER
  FILEHEADER fileheader;
  ZeroMemory(&fileheader,sizeof(FILEHEADER));
  fileheader.headersize = sizeof(FILEHEADER);
  CopyMemory(&fileheader.creationtime  ,GetCurrentFileTime(0)               ,sizeof(FILETIME));
  CopyMemory(&fileheader.lastwritetime ,lastwritetime                       ,sizeof(FILETIME));
  CopyMemory(&fileheader.minexpiretime ,GetCurrentFileTime(minexpireseconds),sizeof(FILETIME));
  CopyMemory(&fileheader.maxexpiretime ,GetCurrentFileTime(maxexpireseconds),sizeof(FILETIME));
  CopyMemory(&fileheader.lastaccesstime,GetCurrentFileTime(0)               ,sizeof(FILETIME));
  strcpy(fileheader.filename,filename);

  // SAVE THE FILE
  SetFilePointer(cache_archivefile,
                 (cache_blocktable+block)->offset,
                 NULL,
                 FILE_BEGIN);
  {
    DWORD byteswritten;
    WriteFile(cache_archivefile,
              &fileheader,
              sizeof(FILEHEADER),
              &byteswritten,
              NULL);
    WriteFile(cache_archivefile,
              data,
              databytes,
              &byteswritten,
              NULL);
  }

  // SAVE THE FILE SYSTEM DATA
  {
    DWORD byteswritten;
    SetFilePointer(cache_archivefile,0,NULL,FILE_BEGIN);
    WriteFile(cache_archivefile,
              cache_archiveheader,
              sizeof(ARCHIVEHEADER),
              &byteswritten,
              NULL);
    DWORD  hashsize  = max(DEFENTRIES,cache_archiveheader->hashentries )*sizeof(HASHENTRY);
    DWORD  blocksize = max(DEFENTRIES,cache_archiveheader->blockentries)*sizeof(BLOCKENTRY);
    LPVOID buffer    = ALLOC(max(hashsize,blocksize));
    if (buffer) {
      SetFilePointer(cache_archivefile,cache_archiveheader->hashoffset,NULL,FILE_BEGIN);
      CopyMemory(buffer,cache_hashtable,hashsize);
      Encrypt((LPDWORD)buffer,hashsize,Hash("(hash table)",HASH_ENCRYPTKEY));
      WriteFile(cache_archivefile,
                buffer,
                hashsize,
                &byteswritten,
                NULL);
      SetFilePointer(cache_archivefile,cache_archiveheader->blockoffset,NULL,FILE_BEGIN);
      CopyMemory(buffer,cache_blocktable,blocksize);
      Encrypt((LPDWORD)buffer,blocksize,Hash("(block table)",HASH_ENCRYPTKEY));
      WriteFile(cache_archivefile,
                buffer,
                blocksize,
                &byteswritten,
                NULL);
      FREE(buffer);
    }
  }

  cache_critsect.Leave();
  return 1;
}
