/****************************************************************************
*
*  SFILE.CPP
*  Storm file I/O functions
*
*  By Michael O'Brien (6/23/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  CDROM_FALSE            0
#define  CDROM_USEDFORDDA       1
#define  CDROM_TRUE             2

#define  FILL_UNREQUESTED       0
#define  FILL_REQUESTED         1
#define  FILL_PLAYING           2
#define  FILL_CLOSING           3

#define  HASH_INDEX             0
#define  HASH_CHECK0            1
#define  HASH_CHECK1            2
#define  HASH_ENCRYPTKEY        3
#define  HASH_ENCRYPTDATA       4

#define  MPQ_COMPRESSED_PKWARE  0x00000100
#define  MPQ_COMPRESSED_SCOMP   0x00000200
#define  MPQ_COMPRESSEDMASK     0x0000FF00
#define  MPQ_ENCRYPTED          0x00010000
#define  MPQ_ENCRYPTED_FIXLOC   0x00020000
#define  MPQ_ALLOCATED          0x80000000

#define  RS_IDLE                0
#define  RS_READING             1
#define  RS_SEEKING             2

#define  DEALLOCATED            0xFFFFFFFE
#define  NOBLOCK                0xFFFFFFFF
#define  SIGNATURE              0x1A51504D
#define  DATACHUNKSIZE          0x20000
#define  WAVECHUNKSIZE          0x4000
#define  READAHEAD              0x1000

#define  KEYCONTAINER           "Blizzard_Storm"
#define  SIGNATUREFILE          "(signature)"
#define  LISTFILE               "(listfile)"
#define  AUTHCOMPANIES          1

typedef struct _AUTHCOMPANYINFO {
    LPCTSTR keyname;
    DWORD   authresult;
} AUTHCOMPANYINFO, *AUTHCOMPANYINFOPTR;

typedef struct _CRYPTOAPI {
    BOOL (APIENTRY *CryptAcquireContext )(HCRYPTPROV *,LPCTSTR,LPCTSTR,DWORD,DWORD);
    BOOL (APIENTRY *CryptCreateHash     )(HCRYPTPROV,ALG_ID,HCRYPTKEY,DWORD,HCRYPTHASH *);
    BOOL (APIENTRY *CryptDestroyHash    )(HCRYPTHASH);
    BOOL (APIENTRY *CryptDestroyKey     )(HCRYPTKEY);
    BOOL (APIENTRY *CryptHashData       )(HCRYPTHASH,BYTE *,DWORD,DWORD);
    BOOL (APIENTRY *CryptImportKey      )(HCRYPTPROV,BYTE *,DWORD,HCRYPTKEY,DWORD,HCRYPTKEY *);
    BOOL (APIENTRY *CryptReleaseContext )(HCRYPTPROV,DWORD);
    BOOL (APIENTRY *CryptSignHash       )(HCRYPTHASH,DWORD,LPCTSTR,DWORD,BYTE *,DWORD *);
    BOOL (APIENTRY *CryptVerifySignature)(HCRYPTHASH,BYTE *,DWORD,HCRYPTKEY,LPCTSTR,DWORD);
} CRYPTOAPI, *CRYPTOAPIPTR;

typedef struct _SIGNATUREHEADER {
    DWORD companyid;
    DWORD reserved;
} SIGNATUREHEADER, *SIGNATUREHEADERPTR;

typedef struct _FILEHEADER {
    DWORD signature;
    DWORD headersize;
    DWORD filesize;
    WORD  version;
    WORD  sectorsizeid;
    DWORD hashoffset;
    DWORD blockoffset;
    DWORD hashcount;
    DWORD blockcount;
} FILEHEADER, *FILEHEADERPTR;

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

NODEDECL(ARCHIVEREC) {
    char          name[MAX_PATH];
    HANDLE        handle;
    BOOL          cdrom;
    int           priority;
    LPVOID        sectorfile;
    DWORD         sectorlocation;
    DWORD         sectorsize;
    LPBYTE        sectorbuffer;
    DWORD         sectorbytesread;
    DWORD         startinglocation;
    BLOCKENTRYPTR blocktable;
    FILEHEADERPTR fileheader;
    HASHENTRYPTR  hashtable;
    DWORD         lastlocation;
} *ARCHIVEPTR;

NODEDECL(FILEREC) {
    char          name[MAX_PATH];
    HANDLE        handle;
    ARCHIVEPTR    archive;
    BLOCKENTRYPTR block;
    DWORD         key;
    DWORD         location;
    DWORD         lastlocation;
    DWORD         sectors;
    LPDWORD       sectoroffsettable;
    BOOL          sectoroffsettablevalid;
    BOOL          dda;
    LPVOID        readaheadbuffer;
    DWORD         readaheadoffset;
    DWORD         readaheadbytes;
} *FILEPTR;

typedef struct CKINFO {
    DWORD size;
    DWORD offset;
} CKINFO;

NODEDECL(AUDIOSTREAM) {
    FILEPTR             file;
    DWORD               nextwrite;
    DWORD               bytespersecond;
    BOOL                loop;
    DWORD               fillstatus;
    DWORD               bytespastend;
    DWORD               startinglocation;
    DWORD               totalsize;
    LONG                volume;
    LONG                pan;
    LPDIRECTSOUNDBUFFER soundbuffer;
    DWORD               soundbuffersize;
    BYTE                fillvalue;
} *AUDIOSTREAMPTR;

NODEDECL(REQUEST) {
    HANDLE              event;
    FILEPTR             file;
    DWORD               location;
    DWORD               approxarchivelocation;
    DWORD               requiredcompletiontime;
    LPVOID              buffer;
    LPDIRECTSOUNDBUFFER soundbuffer;
    DWORD               soundbufferoffset;
    AUDIOSTREAMPTR      stream;
    DWORD               bytestoread;
    BOOL                autodelrequest;
    DWORD               bytesread;
    DWORD               sequence;
    DWORD               dependentsequence;
} *REQUESTPTR;

static const AUTHCOMPANYINFO s_authcompany[AUTHCOMPANIES] = {{"BLIZZARDKEY",SFILE_AUTH_AUTHENTICBLIZZARD}};

static LIST(ARCHIVEREC)  s_archivelist;
static char              s_basepath[MAX_PATH] = "";
static HANDLE            s_cdevent            = INVALID_HANDLE_VALUE;
static LIST(REQUEST)     s_cdreqlist;
static BOOL              s_cdshutdown         = 0;
static HANDLE            s_cdthread           = INVALID_HANDLE_VALUE;
static CCritSect         s_critsect;
static LPDIRECTSOUND     s_directsound        = NULL;
static BOOL              s_enabledirect       = 0;
static LPVOID            s_explodebuffer      = NULL;
static LIST(FILEREC)     s_filelist;
static LPDWORD           s_hashsource         = NULL;
static DWORD             s_ioerrormode        = SFILE_ERRORMODE_FATAL;
static SFILEERRORPROC    s_ioerrorproc        = NULL;
static LCID              s_lcid               = MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT);
static LPVOID            s_soundreadbuffer    = NULL;
static LIST(AUDIOSTREAM) s_streamlist;

#ifdef _DEBUG
static CSLog s_log("Internal","SFile Trace File");
#define  TRACEHANDLE  s_log.GetHandle()
#define  TRACEOUT     SLogWrite
#else
#define  TRACEHANDLE  0
#define  TRACEOUT
#endif

/****************************************************************************
*
*  DATA DECOMPRESSION (PKWARE) SUPPORT
*
***/

typedef struct _DECOMPRESSIONINFO {
    LPVOID sourcebuffer;
    DWORD  sourceoffset;
    LPVOID destbuffer;
    DWORD  destoffset;
    DWORD  bytes;
} DECOMPRESSIONINFO, *DECOMPRESSIONPTR;

//===========================================================================
static UINT __cdecl DecompressLzw_BufferRead (LPSTR   buffer,
                                              UINT   *size,
                                              LPVOID  param) {
  DECOMPRESSIONPTR infoptr = (DECOMPRESSIONPTR)param;
  UINT bytes = min(*size,infoptr->bytes-infoptr->sourceoffset);
  CopyMemory(buffer,(LPSTR)infoptr->sourcebuffer+infoptr->sourceoffset,bytes);
  infoptr->sourceoffset += bytes;
  return bytes;
}

//===========================================================================
static void __cdecl DecompressLzw_BufferWrite (LPSTR   buffer,
                                               UINT   *size,
                                               LPVOID  param) {
  DECOMPRESSIONPTR infoptr = (DECOMPRESSIONPTR)param;
  CopyMemory((LPSTR)infoptr->destbuffer+infoptr->destoffset,buffer,*size);
  infoptr->destoffset += *size;
}

//===========================================================================
static BOOL DecompressLzw (LPBYTE dest, LPBYTE source, DWORD sourcebytes) {

  // CREATE A DECOMPRESSION BUFFER IF ONE DOES NOT ALREADY EXIST
  if (!s_explodebuffer)
    s_explodebuffer = ALLOC(EXP_BUFFER_SIZE);

  // CREATE AN INFORMATION RECORD
  DECOMPRESSIONINFO info;
  info.sourcebuffer = source;
  info.sourceoffset = 0;
  info.destbuffer   = dest;
  info.destoffset   = 0;
  info.bytes        = sourcebytes;

  // PERFORM THE DECOMPRESSION
  explode(DecompressLzw_BufferRead,
          DecompressLzw_BufferWrite,
          (LPSTR)s_explodebuffer,
          &info);

  return 1;
}

/****************************************************************************
*
*  DATA DECRYPTION SUPPORT
*
***/

//===========================================================================
static void inline Decrypt (LPDWORD data, DWORD bytes, DWORD key) {
  DWORD adjust = 0xEEEEEEEE;
  DWORD iter   = bytes >> 2;
  while (iter--) {
    adjust += *(s_hashsource+(HASH_ENCRYPTDATA << 8)+(key & 0xFF));
    adjust += (*data++ ^= adjust+key)+(adjust << 5)+3;
    key     = (key >> 11) | ((key << 21) ^ 0xFFE00000)+0x11111111;
  }
}

/****************************************************************************
*
*  MOPAQ FILE PROCESSING FUNCTIONS
*
***/

static BOOL ReadFileChecked (HANDLE        file,
                             LPVOID        buffer,
                             DWORD         bytestoread,
                             DWORD        *bytesread,
                             LPOVERLAPPED  overlapped,
                             LPCTSTR       filename);

//===========================================================================
static DWORD inline Hash (LPCSTR filename, int hashtype) {
  DWORD result = 0x7FED7FED;
  DWORD adjust = 0xEEEEEEEE;
  while (filename && *filename) {
    char origchar = toupper(*filename++);
    result  = (result+adjust) ^ *(s_hashsource+(hashtype << 8)+origchar);
    adjust += origchar+result+(adjust << 5)+3;
  }
  return result;
}

//===========================================================================
static DWORD inline Hash (LPCWSTR filename, int hashtype) {
  DWORD result = 0x7FED7FED;
  DWORD adjust = 0xEEEEEEEE;
  while (filename && *filename) {
    char origchar = toupper((char)((*filename++) & 0xFF));
    result  = (result+adjust) ^ *(s_hashsource+(hashtype << 8)+origchar);
    adjust += origchar+result+(adjust << 5)+3;
  }
  return result;
}

//===========================================================================
static void inline InitializeHashSource () {
  if (!s_hashsource)
    return;
  DWORD seed = 0x100001;
  for (int loop1 = 0; loop1 < 256; ++loop1)
    for (int loop2 = 0; loop2 < 5; ++loop2) {
      seed = (seed*0x7D+3) % 0x2AAAAB;
      DWORD rand1 = seed & 0xFFFF;
      seed = (seed*0x7D+3) % 0x2AAAAB;
      DWORD rand2 = seed & 0xFFFF;
      *(s_hashsource+(loop2 << 8)+loop1) = (rand1 << 16) | rand2;
    }
}

//===========================================================================
static DWORD InternalReadAligned (FILEPTR file,
                                  DWORD   location,
                                  LPVOID  buffer,
                                  DWORD   bytes) {

  // ENSURE THAT THE READ IS SECTOR ALIGNED
  VALIDATEBEGIN;
  VALIDATE(!(location & (file->archive->sectorsize-1)));
  VALIDATE(!(bytes    & (file->archive->sectorsize-1)));
  VALIDATEEND;

  // CHECK FOR A NULL READ
  if (!bytes)
    return FALSE;

  // IF THIS FILE IS COMPRESSED AND WE HAVEN'T YET READ THE SECTOR
  // OFFSET TABLE, READ IT NOW
  if ((file->block->flags & MPQ_COMPRESSEDMASK) &&
      !file->sectoroffsettablevalid) {
    if (file->block->offset != file->archive->lastlocation)
      SetFilePointer(file->archive->handle,file->block->offset,NULL,FILE_BEGIN);
    DWORD bytesread = 0;
    ReadFileChecked(file->archive->handle,
                    file->sectoroffsettable,
                    (file->sectors+1)*sizeof(DWORD),
                    &bytesread,
                    NULL,
                    file->archive->name);
    if (file->block->flags & MPQ_ENCRYPTED)
      Decrypt((LPDWORD)file->sectoroffsettable,
              bytesread,
              file->key+0xFFFFFFFF);
    file->sectoroffsettablevalid = TRUE;
    file->archive->lastlocation  = file->block->offset+bytesread;
  }

  // DETERMINE THE DISK LOCATION AND SIZE, GIVEN THE UNCOMPRESSED LOCATION
  // AND SIZE
  DWORD disklocation = location;
  DWORD diskbytes    = bytes;
  if (file->block->flags & MPQ_COMPRESSEDMASK) {
    disklocation = *(file->sectoroffsettable+location/file->archive->sectorsize);
    diskbytes    = *(file->sectoroffsettable+(location+bytes)/file->archive->sectorsize)
                     -disklocation;
  }

  // PROVIDE A SECONDARY BUFFER IF NECESSARY FOR DECOMPRESSION
  LPVOID diskbuffer = buffer;
  if (file->block->flags & MPQ_COMPRESSEDMASK)
    diskbuffer = ALLOC(diskbytes+sizeof(DWORD));

  // SET THE ARCHIVE FILE POINTER IF NECESSARY
  DWORD archivelocation = file->block->offset+disklocation;
  if (archivelocation != file->archive->lastlocation)
    SetFilePointer(file->archive->handle,archivelocation,NULL,FILE_BEGIN);

  // PERFORM THE READ
  DWORD diskbytesread = 0;
  ReadFileChecked(file->archive->handle,
                  diskbuffer,
                  diskbytes,
                  &diskbytesread,
                  NULL,
                  file->archive->name);

  // SAVE THE NEW ARCHIVE FILE POINTER
  file->archive->lastlocation = archivelocation+diskbytesread;

  // IF WE WERE NOT ABLE TO READ THE ENTIRE REQUEST, DETERMINE A NUMBER OF
  // UNCOMPRESSED BYTES THAT WE KNOW AT A MINIMUM WE READ SUCCESSFULLY
  DWORD bytesread = bytes;
  if (diskbytesread < diskbytes)
    if (file->block->flags & MPQ_COMPRESSEDMASK) {
      bytesread = 0;
      DWORD checksector = location/file->archive->sectorsize+1;
      while ((checksector   <= file->sectors) &&
             (diskbytesread >= (*(file->sectoroffsettable+checksector)-disklocation))) {
        ++checksector;
        bytesread += file->archive->sectorsize;
      }
    }
    else
      bytesread = diskbytesread;

  // DECRYPT THE DATA IF NECESSARY
  if (file->block->flags & MPQ_ENCRYPTED) {
    DWORD offset  = 0;
    DWORD sector  = location/file->archive->sectorsize;
    DWORD sectors = (bytesread+file->archive->sectorsize-1)/file->archive->sectorsize;
    while ((sector < file->sectors) && sectors--) {
      DWORD processbytes;
      if (file->block->flags & MPQ_COMPRESSEDMASK)
        processbytes = *(file->sectoroffsettable+sector+1)-*(file->sectoroffsettable+sector);
      else {
        processbytes = min(file->archive->sectorsize,bytesread-offset);
        if ((sector == file->sectors-1) &&
            (file->block->sizefile & (file->archive->sectorsize-1)))
          processbytes = min(processbytes,file->block->sizefile & (file->archive->sectorsize-1));
      }
      Decrypt((LPDWORD)((LPBYTE)diskbuffer+offset),
              processbytes & 0xFFFFFFFC,
              file->key+sector);
      ++sector;
      offset += processbytes;
    }
  }

  // DECOMPRESS THE DATA IF NECESSARY
  if (file->block->flags & MPQ_COMPRESSEDMASK) {
    DWORD destoffset   = 0;
    DWORD sourceoffset = 0;
    DWORD sector       = location/file->archive->sectorsize;
    DWORD sectors      = (bytesread+file->archive->sectorsize-1)/file->archive->sectorsize;
    while ((sector < file->sectors) && sectors--) {
      DWORD sourcebytes = *(file->sectoroffsettable+sector+1)-*(file->sectoroffsettable+sector);
      DWORD targetbytes = (sector == file->sectors-1)
                            ? (file->block->sizefile && !(file->block->sizefile & (file->archive->sectorsize-1)))
                              ? file->archive->sectorsize
                              : (file->block->sizefile & (file->archive->sectorsize-1))
                            : file->archive->sectorsize;
      if (targetbytes > sourcebytes)
        switch (file->block->flags & MPQ_COMPRESSEDMASK) {

          case MPQ_COMPRESSED_PKWARE:
            DecompressLzw((LPBYTE)buffer+destoffset,
                          (LPBYTE)diskbuffer+sourceoffset,
                          sourcebytes);
          break;

          case MPQ_COMPRESSED_SCOMP:
            {
              DWORD destsize = targetbytes;
              SCompDecompress((LPBYTE)buffer+destoffset,
                              &destsize,
                              (LPBYTE)diskbuffer+sourceoffset,
                              sourcebytes);
            }
          break;

        }
      else if (diskbuffer != buffer)
        CopyMemory((LPBYTE)buffer+destoffset,
                   (LPBYTE)diskbuffer+sourceoffset,
                   targetbytes);
      destoffset   += targetbytes;  
      sourceoffset += sourcebytes;
      ++sector;
    }
  }

  // FREE THE DECOMPRESSION BUFFER IF NECESSARY
  if (diskbuffer != buffer)
    FREE(diskbuffer);

  // RETURN THE NUMBER OF BYTES READ
  return bytesread;
}

//===========================================================================
static DWORD InternalReadAlignedSector (FILEPTR file,
                                        DWORD   location) {

  // IF THIS SECTOR IS ALREADY IN THE SECTOR BUFFER, JUST RETURN THE
  // PREVIOUS RESULTS
  if ((file->archive->sectorfile     == file) &&
      (file->archive->sectorlocation == location))
    return file->archive->sectorbytesread;

  // OTHERWISE, READ THE SECTOR
  file->archive->sectorfile      = file;
  file->archive->sectorlocation  = location;
  file->archive->sectorbytesread = InternalReadAligned(file,
                                                       location,
                                                       file->archive->sectorbuffer,
                                                       file->archive->sectorsize);

  return file->archive->sectorbytesread;
}

//===========================================================================
static DWORD InternalReadUnaligned (FILEPTR file,
                                    DWORD   location,
                                    LPVOID  buffer,
                                    DWORD   bytes) {

  // CLIP THE READ SIZE AGAINST THE END OF THE FILE
  if (file->block->sizefile <= location)
    return 0;
  else
    bytes = min(bytes,file->block->sizefile-location);

  // READ THE FIRST SECTOR
  DWORD firstbytesread = 0;
  if (location & (file->archive->sectorsize-1)) {
    DWORD sectorbytesread = InternalReadAlignedSector(file,location & ~(file->archive->sectorsize-1));
    DWORD unalignedbytes  = min(bytes,file->archive->sectorsize-(location & (file->archive->sectorsize-1)));
    CopyMemory(buffer,
               file->archive->sectorbuffer+(location & (file->archive->sectorsize-1)),
               unalignedbytes);
    buffer  = (LPBYTE)buffer+unalignedbytes;
    bytes  -= unalignedbytes;
    if ((sectorbytesread != file->archive->sectorsize) || !bytes)
      if (sectorbytesread < (location & (file->archive->sectorsize-1)))
        return 0;
      else
        return min(unalignedbytes,sectorbytesread-(location & (file->archive->sectorsize-1)));
    else
      firstbytesread = unalignedbytes;
    location += unalignedbytes;
  }

  // READ IN THE MIDDLE SECTORS
  DWORD middlebytesread = 0;
  if (bytes) {
    DWORD middlebytes = bytes & ~(file->archive->sectorsize-1);
    middlebytesread = InternalReadAligned(file,
                                          location,
                                          buffer,
                                          bytes & ~(file->archive->sectorsize-1));
    buffer    = (LPBYTE)buffer+middlebytes;
    bytes    -= middlebytes;
    location += middlebytes;
    if ((middlebytes != middlebytesread) || !bytes)
      return firstbytesread+middlebytesread;
  }

  // READ IN THE LAST SECTOR
  DWORD lastbytesread = 0;
  if (bytes) {
    DWORD sectorbytesread = InternalReadAlignedSector(file,location);
    CopyMemory(buffer,
               file->archive->sectorbuffer,
               bytes);
    lastbytesread = min(bytes,sectorbytesread);
  }

  return firstbytesread+middlebytesread+lastbytesread;
}

//===========================================================================
static BOOL ReadFileChecked (HANDLE        file,
                             LPVOID        buffer,
                             DWORD         bytestoread,
                             DWORD        *bytesread,
                             LPOVERLAPPED  overlapped,
                             LPCTSTR       filename) {
  for (;;) {
    BOOL result = ReadFile(file,
                           buffer,
                           bytestoread,
                           bytesread,
                           overlapped);
    if (result && (*bytesread == bytestoread))
      return TRUE;
    DWORD errorcode = GetLastError();
    switch (s_ioerrormode) {

      case SFILE_ERRORMODE_RETURNCODE:
      return result;

      case SFILE_ERRORMODE_CUSTOM:
        if ((!s_ioerrorproc) ||
            (!s_ioerrorproc(filename,errorcode)))
          return result;
      break;

      case SFILE_ERRORMODE_FATAL:
        SErrDisplayError(errorcode,
                         filename,
                         SERR_LINECODE_FILE,
                         NULL,
                         FALSE,
                         1);
      break;

    }
  }
}

//===========================================================================
static DWORD inline SearchHashTable (ARCHIVEPTR archive, LPCTSTR filename, LCID lcid) {
  DWORD hashindex  = Hash(filename,HASH_INDEX);
  DWORD hashcheck0 = Hash(filename,HASH_CHECK0);
  DWORD hashcheck1 = Hash(filename,HASH_CHECK1);
  DWORD entry      = hashindex & (archive->fileheader->hashcount-1);
  DWORD firstentry = entry;
  DWORD found      = 0xFFFFFFFF;
  while ((archive->hashtable+entry)->block != NOBLOCK) {
    if (((archive->hashtable+entry)->hashcheck[0] == hashcheck0) &&
        ((archive->hashtable+entry)->hashcheck[1] == hashcheck1) &&
        ((archive->hashtable+entry)->block != DEALLOCATED))
      if ((archive->hashtable+entry)->lcid == lcid)
        return entry;
      else if ((archive->hashtable+entry)->lcid == MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT))
        found = entry;
    entry = (entry+1) & (archive->fileheader->hashcount-1);
    if (entry == firstentry)
      break;
  }
  return found;
}

/****************************************************************************
*
*  STREAMING AND OVERLAPPED I/O FUNCTIONS
*
***/

static REQUESTPTR IssueRequest (FILEPTR              file,
                                DWORD                locationoffset,
                                LPVOID               buffer,
                                LPDIRECTSOUNDBUFFER  soundbuffer,
                                DWORD                soundbufferoffset,
                                AUDIOSTREAMPTR       stream,
                                DWORD                bytestoread,
                                DWORD                requiredms,
                                DWORD                dependentsequence,
                                HANDLE               event,
                                BOOL                 autodelrequest,
                                BOOL                 triggerreadthread,
                                DWORD               *sequence);

//===========================================================================
static inline BOOL CanProcessRequest (REQUESTPTR request) {
  DWORD dependentsequence = request->dependentsequence;
  if (dependentsequence)
    ITERATELIST(REQUEST,s_cdreqlist,curr)
      if (curr->sequence == dependentsequence)
        return FALSE;
  return TRUE;
}

//===========================================================================
static DWORD CALLBACK CdThreadProc (LPVOID) {
  ARCHIVEPTR lastarchive         = NULL;
  DWORD      lastarchivelocation = 0;
  while (!s_cdshutdown) {
    BOOL processed;
    do {
      processed = 0;

      // IF THERE ARE ANY ACTIVE AUDIO STREAMS, ISSUE READ REQUESTS AS
      // NECESSARY TO KEEP THEIR BUFFERS FULL
      s_critsect.Enter();
      {
        AUDIOSTREAMPTR curr = s_streamlist.Head();
        while (curr) {

          // IF THIS STREAM HAS REACHED THE END OF THE WAVE FILE, EITHER LOOP
          // OR WAIT UNTIL THE BUFFER IS DONE PLAYING BEFORE CLOSING THE
          // STREAM
          if ((curr->fillstatus == FILL_CLOSING) ||
              ((curr->fillstatus == FILL_PLAYING) &&
               (curr->file->location >= curr->totalsize)))
            if (curr->loop)
              curr->file->location = curr->startinglocation;
            else {
              DWORD playcursor  = 0;
              DWORD writecursor = 0;
              curr->soundbuffer->GetCurrentPosition(&playcursor,&writecursor);
              if (((playcursor-curr->nextwrite) % curr->soundbuffersize) >= WAVECHUNKSIZE) {
                DWORD  sizeneeded = min(WAVECHUNKSIZE,curr->soundbuffersize-curr->nextwrite);
                LPVOID buffer     = NULL;
                DWORD  bytes      = sizeneeded;
                if (curr->soundbuffer->Lock(curr->nextwrite,
                                            sizeneeded,
                                            &buffer,
                                            &bytes,
                                            NULL,
                                            NULL,
                                            0) == DS_OK) {
                  FillMemory(buffer,bytes,curr->fillvalue);
                  curr->soundbuffer->Unlock(buffer,
                                            bytes,
                                            NULL,
                                            0);
                  curr->bytespastend += bytes;
                  curr->nextwrite    += bytes;
                  if (curr->nextwrite >= curr->soundbuffersize)
                    curr->nextwrite -= curr->soundbuffersize;
                }
              }
              curr->fillstatus = FILL_CLOSING;
            }

          // IF THIS STREAM IS CURRENTLY PLAYING, CHECK WHETHER THERE IS ROOM
          // TO LOAD ANOTHER CHUNK OF DATA
          if (curr->fillstatus == FILL_PLAYING) {
            DWORD playcursor  = 0;
            DWORD writecursor = 0;
            curr->soundbuffer->GetCurrentPosition(&playcursor,&writecursor);
            if (((playcursor-curr->nextwrite) % curr->soundbuffersize) >= WAVECHUNKSIZE) {
              DWORD sizeneeded = min(WAVECHUNKSIZE,curr->soundbuffersize-curr->nextwrite);
              sizeneeded = min(sizeneeded,curr->totalsize-curr->file->location);
              IssueRequest(curr->file,
                           curr->file->location,
                           NULL,
                           curr->soundbuffer,
                           curr->nextwrite,
                           curr,
                           sizeneeded,
                           ((curr->soundbuffersize-0x20000)*1000)/curr->bytespersecond,
                           0,
                           NULL,
                           TRUE,
                           FALSE,
                           NULL);
              curr->file->location += sizeneeded;
              curr->nextwrite      += (curr->loop ? sizeneeded : WAVECHUNKSIZE);
              if (curr->nextwrite >= curr->soundbuffersize)
                curr->nextwrite -= curr->soundbuffersize;
            }
          }

          // IF THIS STREAM IS NOT YET PLAYING, ISSUE THE INITIAL REQUESTS
          // TO FILL THE BUFFER
          else if (curr->fillstatus == FILL_UNREQUESTED) {
            DWORD offset     = 0;
            DWORD maxoffset  = min(curr->soundbuffersize,curr->totalsize);
            DWORD timeoffset = 0;
            while (offset < maxoffset) {
              IssueRequest(curr->file,
                           curr->file->location,
                           NULL,
                           curr->soundbuffer,
                           offset,
                           curr,
                           min(WAVECHUNKSIZE,maxoffset-offset),
                           1000+(timeoffset += 500),
                           0,
                           NULL,
                           TRUE,
                           FALSE,
                           NULL);
              curr->file->location += WAVECHUNKSIZE;
              offset               += WAVECHUNKSIZE;
            }
            curr->fillstatus = FILL_REQUESTED;
            curr->nextwrite  = offset % curr->soundbuffersize;
          }

          curr = curr->Next();
        }
      }

      // FIND THE MOST URGENT REQUEST, AND THE REQUEST WHICH IS NEXT IN
      // SEEK ORDER
      REQUESTPTR nextreq         = NULL;
      REQUESTPTR urgentreq       = NULL;
      DWORD      lowcompletetime = UINT_MAX;
      {
        DWORD      lowseektime = UINT_MAX;
        DWORD      currtime    = GetTickCount();
        ITERATELIST(REQUEST,s_cdreqlist,curr)
          if (CanProcessRequest(curr)) {

            // CHECK THIS REQUEST FOR SEEK ORDER
            {
              DWORD adjlastlocation = 0;
              if (lastarchivelocation > curr->file->archive->sectorsize)
                adjlastlocation = lastarchivelocation-curr->file->archive->sectorsize;
              DWORD seektime;
              if (curr->file->archive == lastarchive)
                if (curr->approxarchivelocation < adjlastlocation)
                  seektime = 0x8000000+curr->approxarchivelocation;
                else
                  seektime = curr->approxarchivelocation-adjlastlocation;
              else
                seektime = UINT_MAX-1;
              if (seektime <= lowseektime) {
                lowseektime = seektime;
                nextreq     = curr;
              }
            }

            // CHECK THIS REQUEST FOR URGENCY
            {
              DWORD completetime;
              if (currtime-curr->requiredcompletiontime < 0x7FFFFFFF)
                completetime = 0;
              else
                completetime = curr->requiredcompletiontime-currtime;
              if (completetime <= lowcompletetime) {
                lowcompletetime = completetime;
                urgentreq       = curr;
              }
            }

          }
      }
      s_critsect.Leave();

      // IF THE MOST URGENT REQUEST MUST COMPLETE IN THE NEXT
      // 700 MILLISECONDS, PROCESS THAT REQUEST.  OTHERWISE, PROCESS
      // THE NEXT REQUEST IN SEEK ORDER
      REQUESTPTR request = NULL;
      if (urgentreq && (lowcompletetime <= 700))
        request = urgentreq;
      else if (nextreq)
        request = nextreq;

      // PERFORM THE REQUEST
      if (request) {

        // READ THE DATA FROM CD
        lastarchive         = request->file->archive;
        lastarchivelocation = request->approxarchivelocation;
        LPVOID readbuffer = request->buffer;
        if (request->soundbuffer)
          readbuffer = s_soundreadbuffer;
        if (readbuffer)
          request->bytesread = InternalReadUnaligned(request->file,
                                                     request->location,
                                                     readbuffer,
                                                     request->bytestoread);
        else
          request->bytesread = 0;

        // IF THE DATA IS GOING INTO A SOUND BUFFER, LOCK THE SOUND BUFFER
        // AND MOVE THE DATA
        if (request->soundbuffer) {
          request->soundbuffer->Lock(request->soundbufferoffset,
                                     request->bytestoread,
                                     &request->buffer,
                                     &request->bytestoread,
                                     NULL,
                                     NULL,
                                     0);
          CopyMemory(request->buffer,readbuffer,request->bytesread);
          DWORD fullbytes = min(WAVECHUNKSIZE,request->stream->soundbuffersize-request->soundbufferoffset);
          if ((request->bytesread < fullbytes) && !request->stream->loop)
            FillMemory((LPBYTE)request->buffer+request->bytesread,fullbytes-request->bytesread,request->stream->fillvalue);
          request->soundbuffer->Unlock(request->buffer,
                                       request->bytestoread,
                                       NULL,
                                       0);
          if ((request->stream->fillstatus == FILL_REQUESTED) &&
              ((request->location+request->bytestoread >= request->stream->soundbuffersize) ||
               (request->location+request->bytestoread-request->stream->startinglocation >= request->stream->totalsize))) {
            request->stream->soundbuffer->Play(0,0,DSBPLAY_LOOPING);
            request->stream->fillstatus = FILL_PLAYING;
          }
        }

        // REMOVE THIS REQUEST
        s_critsect.Enter();
        processed = TRUE;
        HANDLE event = request->event;
        if (request->autodelrequest)
          s_cdreqlist.DeleteNode(request);
        else
          s_cdreqlist.UnlinkNode(request);
        s_critsect.Leave();
        if (event)
          SetEvent(event);

      }

    } while (processed);
    WaitForSingleObject(s_cdevent,s_streamlist.IsEmpty() ? INFINITE : 250);
  }
  s_cdshutdown = FALSE;
  _endthreadex(0);
  return 0;
}

//===========================================================================
static void CreateCdThread () {
  if (s_cdthread != INVALID_HANDLE_VALUE)
    return;
  s_cdevent  = CreateEvent((LPSECURITY_ATTRIBUTES)NULL,
                               0,
                               0,
                               NULL);
  DWORD threadid;
  s_cdthread = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)NULL,
                                          0,
                                          CdThreadProc,
                                          NULL,
                                          0,
                                          &threadid);
  SetThreadPriority(s_cdthread,THREAD_PRIORITY_ABOVE_NORMAL);
}

//===========================================================================
static REQUESTPTR IssueRequest (FILEPTR              file,
                                DWORD                offset,
                                LPVOID               buffer,
                                LPDIRECTSOUNDBUFFER  soundbuffer,
                                DWORD                soundbufferoffset,
                                AUDIOSTREAMPTR       stream,
                                DWORD                bytestoread,
                                DWORD                requiredms,
                                DWORD                dependentsequence,
                                HANDLE               event,
                                BOOL                 autodelrequest,
                                BOOL                 triggerreadthread,
                                DWORD               *sequence) {
  VALIDATEBEGIN;
  VALIDATE(buffer || soundbuffer);
  VALIDATEEND;

  // CREATE A REQUEST RECORD
  s_critsect.Enter();
  static DWORD currsequence = 0;
  REQUESTPTR request         = s_cdreqlist.NewNode();
  DWORD      requestsequence = 0;
  request->event                  = event;
  request->file                   = file;
  request->location               = offset;
  request->approxarchivelocation  = file->block->offset+offset/2;
  request->requiredcompletiontime = GetTickCount()+requiredms;
  request->buffer                 = buffer;
  request->soundbuffer            = soundbuffer;
  request->soundbufferoffset      = soundbufferoffset;
  request->stream                 = stream;
  request->bytestoread            = bytestoread;
  request->autodelrequest         = autodelrequest;
  request->dependentsequence      = dependentsequence;
  do
    requestsequence = request->sequence = ++currsequence;
  while (!requestsequence);
  s_critsect.Leave();

  // TRIGGER THE CD READ THREAD
  if (triggerreadthread)
    SetEvent(s_cdevent);

  if (sequence)
    *sequence = requestsequence;
  return request;
}

/****************************************************************************
*
*  SUPPORT/UTILITY FUNCTIONS
*
***/

//===========================================================================
static void BuildDefaultBasePath () {
  GetModuleFileName(GetModuleHandle(NULL),s_basepath,MAX_PATH);
  {
    LPTSTR separator = SStrChr(s_basepath,'\\');
    while (separator && SStrChr(separator+1,'\\'))
      separator = SStrChr(separator+1,'\\');
    if (separator)
      *separator = 0;
  }
  SStrPack(s_basepath,"\\",MAX_PATH);
}

//===========================================================================
static BOOL CheckArchiveHandle (HSARCHIVE archive, ARCHIVEPTR *archiveptr) {

  // CONVERT THE FILE HANDLE INTO AN ARCHIVE POINTER
  *archiveptr = (ARCHIVEPTR)archive;

  // DETERMINE WHETHER THE ARCHIVE POINTER IS VALID
  BOOL valid = FALSE;
  ITERATELIST(ARCHIVEREC,s_archivelist,currptr)
    if (currptr == *archiveptr) {
      valid = TRUE;
      break;
    }

  // IF IT IS VALID, RETURN SUCCESS
  if (valid)
    return TRUE;

  // OTHERWISE, SET THE ERROR CODE
  SErrSetLastError(SFILE_ERROR_INVALID_HANDLE);

  // GENERATE AN ERROR MESSAGE
  switch (s_ioerrormode) {

    case SFILE_ERRORMODE_CUSTOM:
      if (s_ioerrorproc)
        s_ioerrorproc("",
                      SFILE_ERROR_INVALID_HANDLE);
    break;

    case SFILE_ERRORMODE_FATAL:
      SErrDisplayError(SFILE_ERROR_INVALID_HANDLE,
                       "HSARCHIVE",
                       SERR_LINECODE_HANDLE,
                       NULL,
                       TRUE,
                       1);
    break;

  }

  return FALSE;
}

//===========================================================================
static BOOL CheckFileHandle (HSFILE file, FILEPTR *fileptr) {

  // CONVERT THE FILE HANDLE INTO A FILE POINTER
  *fileptr = (FILEPTR)file;

  // DETERMINE WHETHER THE FILE POINTER IS VALID
  BOOL valid = FALSE;
  ITERATELIST(FILEREC,s_filelist,currptr)
    if (currptr == *fileptr) {
      valid = TRUE;
      break;
    }

  // IF IT IS VALID, RETURN SUCCESS
  if (valid)
    return TRUE;

  // OTHERWISE, SET THE ERROR CODE
  SErrSetLastError(SFILE_ERROR_INVALID_HANDLE);

  // GENERATE AN ERROR MESSAGE
  switch (s_ioerrormode) {

    case SFILE_ERRORMODE_CUSTOM:
      if (s_ioerrorproc)
        s_ioerrorproc("",
                      SFILE_ERROR_INVALID_HANDLE);
    break;

    case SFILE_ERRORMODE_FATAL:
      SErrDisplayError(SFILE_ERROR_INVALID_HANDLE,
                       "HSFILE",
                       SERR_LINECODE_HANDLE,
                       NULL,
                       TRUE,
                       1);
    break;

  }

  return FALSE;
}

//===========================================================================
static BOOL inline CheckForCdRom (LPCTSTR path) {

  // DETERMINE THE ROOT PATH OF THE DRIVE
  char rootpath[4];
  SStrCopy(rootpath,path,4);

  // GET THE DRIVE TYPE
  UINT drivetype = GetDriveType(rootpath);

  // GET THE FILE SYSTEM INFORMATION.  RETURN FAILURE IF THERE IS NO MEDIA
  // IN THE DRIVE.
  DWORD fsflags = 0;
  char  fsname[MAX_PATH];
  ZeroMemory(fsname,MAX_PATH);
  if (!GetVolumeInformation(rootpath,NULL,0,NULL,NULL,&fsflags,fsname,MAX_PATH))
    return 0;

  // GET THE SECTOR FORMAT AND NUMBER OF FREE SECTORS
  DWORD sectorspercluster = 0;
  DWORD bytespersector    = 0;
  DWORD freeclusters      = 0;
  DWORD totalclusters     = 0;
  if (!GetDiskFreeSpace(rootpath,&sectorspercluster,&bytespersector,&freeclusters,&totalclusters))
    return 0;

  // CONFIRM THAT:
  // 1. THE DRIVE TYPE IS CD-ROM
  // 2. THE FILE SYSTEM IS CDFS OR UNKNOWN (BLANK)
  // 3. THE FILE SYSTEM DOES NOT SUPPORT UNICODE
  // 4. THE SECTORS ARE 2048 BYTES EACH
  // 5. THERE ARE NO FREE CLUSTERS ON THE DISK
  DWORD value = drivetype ^ (*(LPDWORD)fsname)
                          ^ (fsflags & FS_UNICODE_STORED_ON_DISK)
                          ^ bytespersector
                          ^ freeclusters;
  WORD  check = LOWORD(value) ^ HIWORD(value);
  return ((check == 0x1F00) || (check == 0x0805));
}

//===========================================================================
static void ConvertRelativePathName (LPCTSTR inputpath, LPTSTR outputpath) {

  // DETERMINE THE BASE PATH
  if (!s_basepath[0])
    BuildDefaultBasePath();

  // COMBINE THE BASE PATH AND THE INPUT PATH TO FORM THE OUTPUT PATH
  char absolutepath[MAX_PATH] = "";
  wsprintf(absolutepath,"%s%s",s_basepath,inputpath);
  _fullpath(outputpath,absolutepath,MAX_PATH);

}

//===========================================================================
static BOOL FindChunk (HSFILE handle, FOURCC ckid, CKINFO *pck) {
  struct {
      FOURCC ckid;
      DWORD  size;
  } ckhdr;

  for (;;) {
    if (!SFileReadFile(handle,&ckhdr,sizeof(ckhdr),NULL,NULL))
      return 0;
    if (ckhdr.ckid == ckid)
      break;
    if (SFileSetFilePointer(handle,ckhdr.size,NULL,FILE_CURRENT) == 0xFFFFFFFF)
      return 0;
  }

  pck->size   = ckhdr.size;
  pck->offset = SFileSetFilePointer(handle,0,NULL,FILE_CURRENT);
  return (pck->offset != 0xFFFFFFFF);
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SFileAuthenticateArchive (HSARCHIVE  handle,
                                        DWORD     *extendedresult) {
  TRACEOUT(TRACEHANDLE,
           "SFileAuthenticateArchive(0x%x,*extendedresult)",
           handle);

  if (extendedresult)
    *extendedresult = SFILE_AUTH_UNABLETOAUTHENTICATE;

  // CHECK THE ARCHIVE HANDLE
  ARCHIVEPTR archiveptr;
  if (!CheckArchiveHandle(handle,&archiveptr))
    return FALSE;

  // DO A PRELIMINARY CHECK FOR A SIGNATURE BLOCK IN THIS ARCHIVE BEFORE
  // WE GO TO ALL THE TROUBLE OF BINDING TO THE CRYPTO API AND SECURITY
  // SUPPORT PROVIDER
  LCID defaultlcid = MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT);
  if (SearchHashTable(archiveptr,SIGNATUREFILE,defaultlcid) == 0xFFFFFFFF) {
    if (extendedresult)
      *extendedresult = SFILE_AUTH_NOSIGNATURE;
    SErrSetLastError(SFILE_ERROR_NOT_AUTHENTICATED);
    return 0;
  }

  CRYPTOAPIPTR cryptoapi = NULL;
  HCRYPTHASH   hash      = (HCRYPTHASH)0;
  HCRYPTKEY    key       = (HCRYPTKEY)0;
  HINSTANCE    lib       = (HINSTANCE)0;
  HANDLE       map       = (HANDLE)0;
  HCRYPTPROV   provider  = (HCRYPTPROV)0;
  BOOL         result    = 0;
  LPVOID       view      = NULL;

  TRY {

    // BIND TO THE CRYPTOGRAPHY LIBRARY
    lib = LoadLibrary("advapi32.dll");
    if (!lib)
      LEAVE;
    cryptoapi = (CRYPTOAPIPTR)ALLOC(sizeof(CRYPTOAPI));
#define  BIND(a,b)  *(void **)&cryptoapi->##a = GetProcAddress(lib,b); \
                    if (!cryptoapi->##a) LEAVE;
    BIND(CryptAcquireContext ,"CryptAcquireContextA" );
    BIND(CryptCreateHash     ,"CryptCreateHash"      );
    BIND(CryptDestroyHash    ,"CryptDestroyHash"     );
    BIND(CryptDestroyKey     ,"CryptDestroyKey"      );
    BIND(CryptHashData       ,"CryptHashData"        );
    BIND(CryptImportKey      ,"CryptImportKey"       );
    BIND(CryptReleaseContext ,"CryptReleaseContext"  );
    BIND(CryptSignHash       ,"CryptSignHashA"       );
    BIND(CryptVerifySignature,"CryptVerifySignatureA");
#undef  BIND

    // INITIALIZE SECURITY SUPPORT
    if (!cryptoapi->CryptAcquireContext(&provider,KEYCONTAINER,MS_DEF_PROV,PROV_RSA_FULL,0))
      if (!cryptoapi->CryptAcquireContext(&provider,KEYCONTAINER,MS_DEF_PROV,PROV_RSA_FULL,CRYPT_NEWKEYSET))
        LEAVE;

    // MAP THE ENTIRE ARCHIVE FILE INTO MEMORY
    map  = CreateFileMapping(archiveptr->handle,NULL,PAGE_READONLY | SEC_COMMIT,0,0,NULL);
    if (!map)
      LEAVE;
    view = MapViewOfFile(map,FILE_MAP_READ,0,0,0);
    if (!view)
      LEAVE;

    // DETERMINE THE LOCATION OF THE SIGNATURE BLOCK
    DWORD index = SearchHashTable(archiveptr,SIGNATUREFILE,defaultlcid);
    if (index == 0xFFFFFFFF)
      LEAVE;
    BLOCKENTRYPTR block = archiveptr->blocktable+(archiveptr->hashtable+index)->block;
    if ((block->flags & (MPQ_COMPRESSEDMASK | MPQ_ENCRYPTED)) ||
        (block->sizefile <= sizeof(SIGNATUREHEADER)) ||
        !block->offset) {
      if (extendedresult)
        *extendedresult = SFILE_AUTH_BADSIGNATURE;
      LEAVE;
    }

    // DETERMINE THE COMPANY ID
    DWORD companyid = ((SIGNATUREHEADERPTR)((LPBYTE)view+block->offset))->companyid;
    if (companyid >= AUTHCOMPANIES) {
      if (extendedresult)
        *extendedresult = SFILE_AUTH_UNKNOWNSIGNATURE;
      LEAVE;
    }

    // LOAD THE APPROPRIATE COMPANY'S PUBLIC KEY INTO THE KEY DATABASE
    {
      HRSRC   resource = FindResource(StormGetInstance(),s_authcompany[companyid].keyname,"#256");
      HGLOBAL handle   = LoadResource(StormGetInstance(),resource);
      LPVOID  ptr      = LockResource(handle);
      if (ptr)
        cryptoapi->CryptImportKey(provider,
                                  (LPBYTE)ptr,
                                  SizeofResource(StormGetInstance(),resource),
                                  NULL,
                                  0,
                                  &key);
      FreeResource(handle);
    }
    if (!key)
      LEAVE;

    // HASH THE ARCHIVE FILE, REPLACING THE SIGNATURE BLOCK WITH ZEROES.
    // IF THIS ARCHIVE FILE IS EMBEDDED IN A LARGER FILE, ONLY HASH THE
    // MOPAQ PORTION.
    if (!cryptoapi->CryptCreateHash(provider,CALG_MD5,NULL,0,&hash))
      LEAVE;
    if (!cryptoapi->CryptHashData(hash,
                                  (LPBYTE)view+archiveptr->startinglocation,
                                  block->offset-archiveptr->startinglocation,
                                  0))
      LEAVE;
    {
      LPVOID zeroblock  = ALLOCZERO(block->sizefile);
      BOOL   hashresult = cryptoapi->CryptHashData(hash,
                                                   (LPBYTE)zeroblock,
                                                   block->sizefile,
                                                   0);
      FREE(zeroblock);
      if (!hashresult)
        LEAVE;
    }
    if (block->offset+block->sizefile < archiveptr->startinglocation+archiveptr->fileheader->filesize)
      if (!cryptoapi->CryptHashData(hash,
                                    (LPBYTE)view+block->offset+block->sizefile,
                                    archiveptr->startinglocation+archiveptr->fileheader->filesize
                                      -(block->offset+block->sizefile),
                                    0))
        LEAVE;

    // VERIFY THE SIGNATURE BLOCK AGAINST THE HASH VALUE
    if (cryptoapi->CryptVerifySignature(hash,
                                        (LPBYTE)view+block->offset+sizeof(SIGNATUREHEADER),
                                        block->sizefile-sizeof(SIGNATUREHEADER),
                                        key,
                                        NULL,
                                        0)) {
      result = 1;
      if (extendedresult)
        *extendedresult = s_authcompany[companyid].authresult;
    }
    else if (extendedresult)
      *extendedresult = SFILE_AUTH_BADSIGNATURE;

  }
  FINALLY {
    if (hash)
      cryptoapi->CryptDestroyHash(hash);
    if (key)
      cryptoapi->CryptDestroyKey(key);
    if (view)
      UnmapViewOfFile(view);
    if (map)
      CloseHandle(map);
    if (provider) {
      cryptoapi->CryptReleaseContext(provider,0);
      cryptoapi->CryptAcquireContext(&provider,KEYCONTAINER,MS_DEF_PROV,PROV_RSA_FULL,CRYPT_DELETEKEYSET);
    }
    if (cryptoapi)
      FREE(cryptoapi);
    if (lib)
      FreeLibrary(lib);
  }

  if (!result)
    SErrSetLastError(SFILE_ERROR_NOT_AUTHENTICATED);
  return result;
}

//===========================================================================
BOOL APIENTRY SFileCloseArchive (HSARCHIVE handle) {
  TRACEOUT(TRACEHANDLE,
           "SFileCloseArchive(0x%x)",
           handle);

  // CHECK THE ARCHIVE HANDLE
  ARCHIVEPTR archiveptr;
  if (!CheckArchiveHandle(handle,&archiveptr))
    return FALSE;

  // CLOSE ALL FILES ASSOCIATED WITH THIS ARCHIVE
  {
    FILEPTR curr = s_filelist.Head();
    while (curr) {
      FILEPTR next = curr->Next();
      if (curr->archive == archiveptr)
        SFileCloseFile((HSFILE)curr);
      curr = next;
    }
  }

  // FREE MEMORY
  if (archiveptr->sectorbuffer)
    FREE(archiveptr->sectorbuffer);
  if (archiveptr->blocktable)
    FREE(archiveptr->blocktable);
  if (archiveptr->fileheader)
    DEL(archiveptr->fileheader);
  if (archiveptr->hashtable)
    FREE(archiveptr->hashtable);

  // CLOSE THE ARCHIVE FILE
  CloseHandle(archiveptr->handle);

  // UNLINK AND FREE THE ARCHIVE
  s_critsect.Enter();
  s_archivelist.DeleteNode(archiveptr);
  s_critsect.Leave();

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileCloseFile (HSFILE handle) {
  TRACEOUT(TRACEHANDLE,
           "SFileCloseFile(0x%x)",
           handle);

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(handle,&fileptr))
    return FALSE;

  // END ANY DDA STREAMS ASSOCIATED WITH THIS FILE
  if (fileptr->dda)
    SFileDdaEnd(handle);

  // WAIT UNTIL ALL OUTSTANDING READ OPERATIONS FOR THIS FILE COMPLETE.
  // IF WE ARE IN DEBUG MODE, DISPLAY A WARNING IF THE APPLICATION TRIES
  // TO CLOSE A FILE WHILE THERE ARE OUTSTANDING OVERLAPPED READ OPERATIONS
  // ON THAT FILE.
  s_critsect.Enter();
  for (;;) {
    BOOL outstanding = FALSE;
    ITERATELIST(REQUEST,s_cdreqlist,curr)
      if (curr->file == fileptr) {
        outstanding = TRUE;
        break;
      }
    if (outstanding) {
#ifdef _DEBUG
      SErrDisplayError(ERROR_IO_PENDING,
                       "SFileCloseFile()",
                       SERR_LINECODE_FUNCTION,
                       NULL,
                       TRUE);
#endif
      Sleep(1);
    }
    else
      break;
  }

  // CLOSE THE FILE HANDLE IF USED
  if (fileptr->handle != INVALID_HANDLE_VALUE)
    CloseHandle(fileptr->handle);

  // CLEAR THE SECTOR CACHE
  if (fileptr->archive)
    fileptr->archive->sectorfile = NULL;

  // FREE THE SECTOR OFFSET TABLE
  if (fileptr->sectoroffsettable)
    FREE(fileptr->sectoroffsettable);

  // FREE THE READ AHEAD BUFFER
  if (fileptr->readaheadbuffer)
    FREE(fileptr->readaheadbuffer);

  // UNLINK AND FREE THE FILE
  s_filelist.DeleteNode(fileptr);
  s_critsect.Leave();

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileDdaBegin (HSFILE handle,
                             DWORD  buffersize,
                             DWORD  flags) {
  TRACEOUT(TRACEHANDLE,
           "SFileDdaBegin(0x%x,%u,0x%x)",
           handle,buffersize,flags);

  return SFileDdaBeginEx(handle,
                         buffersize,
                         flags,
                         0,
                         0x7FFFFFFF,
                         0x7FFFFFFF,
                         NULL);
}

//===========================================================================
BOOL APIENTRY SFileDdaBeginEx (HSFILE handle,
                               DWORD  buffersize,
                               DWORD  flags,
                               DWORD  offset,
                               LONG   volume,
                               LONG   pan,
                               LPVOID reserved) {
  VALIDATEBEGIN;
  VALIDATE(buffersize);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileDdaBeginEx(0x%x,%u,0x%x,%u,%d,%d,%u)",
           handle,buffersize,flags,offset,volume,pan,reserved);

  if (!s_directsound) {
    SErrSetLastError(SFILE_ERROR_NOT_INITIALIZED);
    return 0;
  }
  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(handle,&fileptr))
    return FALSE;

  // STOP PLAYING THIS FILE IF IT IS ALREADY PLAYING
  if (fileptr->dda)
    SFileDdaEnd(handle);

  // ENSURE THAT THE FILE IS IN AN ARCHIVE
  if ((fileptr->handle != INVALID_HANDLE_VALUE) || !fileptr->archive) {
    SErrSetLastError(SFILE_ERROR_NOT_IN_ARCHIVE);
    return 0;
  }

  // MARK THIS FILE AS PLAYING
  fileptr->dda = 1;

  // CREATE A SOUND READ BUFFER IF NECESSARY
  if (!s_soundreadbuffer)
    s_soundreadbuffer = ALLOC(WAVECHUNKSIZE);

  // IF THE FILE'S ARCHIVE IS NOT ON CD, START PROCESSING IT AS A CD ARCHIVE
  // ANYWAY, BECAUSE THE CD READ THREAD DOES THE WORK OF READING BACKGROUND
  // STREAM DATA
  if (fileptr->archive->cdrom == CDROM_FALSE) {
    fileptr->archive->cdrom = CDROM_USEDFORDDA;
    if (s_cdthread == INVALID_HANDLE_VALUE)
      CreateCdThread();
  }

  // ADJUST THE BUFFER SIZE SO IT IS A MULTIPLE OF 64K AND AT LEAST 128K
  if (buffersize & (WAVECHUNKSIZE-1))
    buffersize += WAVECHUNKSIZE-(buffersize & (WAVECHUNKSIZE-1));
  if (buffersize < 2*WAVECHUNKSIZE)
    buffersize = 2*WAVECHUNKSIZE;

  // READ THE WAVE FILE HEADER
  WAVEFORMATEX format;
  DWORD        startinglocation;
  DWORD        totalsize;
  {
    SFileSetFilePointer(handle,0,NULL,FILE_BEGIN);
    MMCKINFO mmck;
    if (!SFileReadFile(handle,&mmck,sizeof(FOURCC)*2+sizeof(DWORD),NULL,NULL))
      return 0;
    if ((mmck.ckid != FOURCC_RIFF) || (mmck.fccType != mmioFOURCC('W','A','V','E'))) {
      SErrSetLastError(SFILE_ERROR_INVALID_DATA);
      return 0;
    }
    CKINFO info;
    if (!FindChunk(handle,mmioFOURCC('f','m','t',' '),&info)) {
      SErrSetLastError(SFILE_ERROR_INVALID_DATA);
      return 0;
    }
    if (info.size < sizeof(PCMWAVEFORMAT)) {
      SErrSetLastError(SFILE_ERROR_INVALID_DATA);
      return 0;
    }
    PCMWAVEFORMAT pcm;
    if (!SFileReadFile(handle,&pcm,sizeof(PCMWAVEFORMAT),NULL,NULL))
      return 0;
    if (SFileSetFilePointer(handle,info.size-sizeof(PCMWAVEFORMAT),NULL,FILE_CURRENT) == 0xFFFFFFFF)
      return 0;
    format.wFormatTag      = pcm.wf.wFormatTag;
    format.nChannels       = pcm.wf.nChannels;
    format.nSamplesPerSec  = pcm.wf.nSamplesPerSec;
    format.nAvgBytesPerSec = pcm.wf.nAvgBytesPerSec;
    format.nBlockAlign     = pcm.wf.nBlockAlign;
    format.wBitsPerSample  = pcm.wBitsPerSample;
    format.cbSize          = 0;
    if (!FindChunk(handle,mmioFOURCC('d','a','t','a'),&info)) {
      SErrSetLastError(SFILE_ERROR_INVALID_DATA);
      return 0;
    }
    startinglocation = info.offset;
    totalsize        = info.size;
  }

  // CREATE A STREAM RECORD
  s_critsect.Enter();
  AUDIOSTREAMPTR stream = s_streamlist.NewNode();
  stream->file             = fileptr;
  stream->soundbuffersize  = buffersize;
  stream->bytespersecond   = format.nAvgBytesPerSec;
  stream->loop             = ((flags & SFILE_DDA_LOOP) != 0);
  stream->fillstatus       = FILL_UNREQUESTED;
  stream->startinglocation = startinglocation+min(offset,totalsize-startinglocation);
  stream->totalsize        = totalsize;
  stream->volume           = volume;
  stream->pan              = pan;
  stream->fillvalue        = (format.wBitsPerSample == 8) ? 0x80 : 0x00;

  // CREATE A SECONDARY SOUND BUFFER
  {
    DSBUFFERDESC desc;
    ZeroMemory(&desc,sizeof(DSBUFFERDESC));
    desc.dwSize        = sizeof(DSBUFFERDESC);
    desc.dwFlags       = DSBCAPS_CTRLDEFAULT;
    desc.dwBufferBytes = buffersize;
    desc.lpwfxFormat   = &format;
    s_directsound->CreateSoundBuffer(&desc,&stream->soundbuffer,NULL);
    if (stream->soundbuffer) {
      if (stream->volume != 0x7FFFFFFF)
        stream->soundbuffer->SetVolume(stream->volume);
      if (stream->pan != 0x7FFFFFFF)
        stream->soundbuffer->SetPan(stream->pan);
    }
  }
  s_critsect.Leave();

  // TRIGGER THE CD READ THREAD
  SetEvent(s_cdevent);

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileDdaDestroy () {
  TRACEOUT(TRACEHANDLE,
           "SFileDdaDestroy()");

  // FREE ALL STREAMS
  s_critsect.Enter();
  {
    AUDIOSTREAMPTR curr;
    while ((curr = s_streamlist.Head()) != NULL) {
      curr->file->dda = TRUE;
      s_critsect.Leave();
      SFileDdaEnd((HSFILE)curr->file);
      s_critsect.Enter();
    }
  }
  s_critsect.Leave();

  // DELETE THE POINTER TO THE DIRECTSOUND OBJECT
  s_directsound = NULL;

  // FREE THE SOUND READ BUFFER
  if (s_soundreadbuffer) {
    FREE(s_soundreadbuffer);
    s_soundreadbuffer = NULL;
  }

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileDdaEnd (HSFILE handle) {
  TRACEOUT(TRACEHANDLE,
           "SFileDdaEnd(0x%x)",
           handle);

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(handle,&fileptr))
    return FALSE;

  // FREE ALL STREAMS ASSOCIATED WITH THIS FILE
  BOOL again;
  do {
    again = FALSE;
    s_critsect.Enter();
    AUDIOSTREAMPTR curr = s_streamlist.Head();
    while (curr) {
      AUDIOSTREAMPTR next = curr->Next();
      if (curr->file == fileptr) {
        curr->fillstatus = FILL_CLOSING;

        // CHECK WHETHER THERE ARE OUTSTANDING READ REQUESTS FOR THE STREAM
        BOOL       found   = FALSE;
        REQUESTPTR currreq = s_cdreqlist.Head();
        while (currreq && !found) {
          if ((currreq->stream      == curr) ||
              (currreq->soundbuffer == curr->soundbuffer))
            found = TRUE;
          currreq = currreq->Next();
        }

        // IF NOT, DELETE THE STREAM
        if (found)
          again = TRUE;
        else {
          if (curr->soundbuffer)
            curr->soundbuffer->Release();
          next = s_streamlist.DeleteNode(curr);
        }

      }
      curr = next;
    }
    s_critsect.Leave();
    if (again)
      Sleep(1);
  } while (again);

  fileptr->dda = FALSE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SFileDdaGetPos (HSFILE  handle,
                              DWORD  *position,
                              DWORD  *maxposition) {
  TRACEOUT(TRACEHANDLE,
           "SFileDdaGetPos(0x%x,*position,*maxposition)",
           handle);

  if (position)
    *position = 0;
  if (maxposition)
    *maxposition = 0;

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(handle,&fileptr))
    return FALSE;

  // ENTER THE CRITICAL SECTION
  s_critsect.Enter();

  // FIND THE STREAM ASSOCIATED WITH THIS FILE
  AUDIOSTREAMPTR curr = s_streamlist.Head();
  while (curr && (curr->file != fileptr))
    curr = curr->Next();
  if (!curr) {
    s_critsect.Leave();
    SErrSetLastError(SFILE_ERROR_NOT_PLAYING);
    return 0;
  }

  // DETERMINE THE CURRENT PLAY POSITION
  DWORD playposition = 0;
  if ((curr->fillstatus == FILL_PLAYING) ||
      (curr->fillstatus == FILL_CLOSING)) {
    DWORD playcursor  = 0;
    DWORD writecursor = 0;
    curr->soundbuffer->GetCurrentPosition(&playcursor,&writecursor);
    DWORD writeposition = curr->file->location+curr->bytespastend-curr->startinglocation;
    playposition = (writeposition/curr->soundbuffersize)*curr->soundbuffersize+playcursor;
    if (playposition >= writeposition)
      if (playposition > curr->soundbuffersize)
        playposition -= curr->soundbuffersize;
      else
        playposition = 0;
    playposition = min(playposition,curr->totalsize-curr->startinglocation);
  }

  // RETURN THE CURRENT POSITIONS
  if (position)
    *position = playposition;
  if (maxposition)
    *maxposition = curr->totalsize-curr->startinglocation;

  // LEAVE THE CRITICAL SECTION
  s_critsect.Leave();

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileDdaGetVolume (HSFILE  handle,
                                 LONG   *volume,
                                 LONG   *pan) {
  TRACEOUT(TRACEHANDLE,
           "SFileDdaGetVolume(0x%x,*volume,*pan)",
           handle);

  if (volume)
    *volume = 0;
  if (pan)
    *pan = 0;

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(handle,&fileptr))
    return FALSE;

  // ENTER THE CRITICAL SECTION
  s_critsect.Enter();

  // FIND THE STREAM ASSOCIATED WITH THIS FILE
  AUDIOSTREAMPTR curr = s_streamlist.Head();
  while (curr && (curr->file != fileptr))
    curr = curr->Next();
  if (!curr) {
    s_critsect.Leave();
    SErrSetLastError(SFILE_ERROR_NOT_PLAYING);
    return 0;
  }

  // IF WE DON'T KNOW WHAT THE VOLUME OR PAN OF THIS AUDIO STREAM IS,
  // DETERMINE IT NOW
  if (curr->volume == 0x7FFFFFFF)
    curr->soundbuffer->GetVolume(&curr->volume);
  if (curr->pan == 0x7FFFFFFF)
    curr->soundbuffer->GetPan(&curr->pan);

  // RETURN THE REQUESTED VALUES
  if (volume)
    *volume = curr->volume;
  if (pan)
    *pan = curr->pan;

  // LEAVE THE CRITICAL SECTION
  s_critsect.Leave();

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileDdaInitialize (LPDIRECTSOUND directsound) {
  VALIDATEBEGIN;
  VALIDATE(directsound);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileDdaInitialize(0x%x)",
           directsound);

  s_directsound = directsound;
  return 1;
}

//===========================================================================
BOOL APIENTRY SFileDdaSetVolume (HSFILE handle,
                                 LONG   volume,
                                 LONG   pan) {
  TRACEOUT(TRACEHANDLE,
           "SFileDdaSetVolume(0x%x,%d,%d)",
           handle,volume,pan);

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(handle,&fileptr))
    return FALSE;

  // ENTER THE CRITICAL SECTION
  s_critsect.Enter();

  // FIND THE STREAM ASSOCIATED WITH THIS FILE
  AUDIOSTREAMPTR curr = s_streamlist.Head();
  while (curr && (curr->file != fileptr))
    curr = curr->Next();
  if (!curr) {
    s_critsect.Leave();
    SErrSetLastError(SFILE_ERROR_NOT_PLAYING);
    return 0;
  }

  // ADJUST THE SETTINGS
  if ((volume != 0x7FFFFFFF) && (volume != curr->volume)) {
    curr->volume = volume;
    curr->soundbuffer->SetVolume(volume);
  }
  if ((pan != 0x7FFFFFFF) && (pan != curr->pan)) {
    curr->pan = pan;
    curr->soundbuffer->SetPan(pan);
  }

  // LEAVE THE CRITICAL SECTION
  s_critsect.Leave();

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileDestroy () {
  TRACEOUT(TRACEHANDLE,
           "SFileDestroy()");

  // WAIT FOR ALL OUTSTANDING REQUESTS TO COMPLETE
  while (!s_cdreqlist.IsEmpty())
    Sleep(10);

  // SHUT DOWN THE CD THREAD READ ROUTINE
  if (s_cdthread != INVALID_HANDLE_VALUE) {
    s_cdshutdown = 1;
    SetEvent(s_cdevent);
    WaitForSingleObject(s_cdthread,INFINITE);
    if (s_cdevent != INVALID_HANDLE_VALUE) {
      CloseHandle(s_cdevent);
      s_cdevent = INVALID_HANDLE_VALUE;
    }
    if (s_cdthread != INVALID_HANDLE_VALUE) {
      CloseHandle(s_cdthread);
      s_cdthread = INVALID_HANDLE_VALUE;
    }
  }

  // DESTROY DDA
  SFileDdaDestroy();

  // DELETE THE OPEN FILES AND ARCHIVES
  {
    FILEPTR curr;
    while ((curr = s_filelist.Head()) != NULL) {
      REPORTRESOURCELEAK(HSFILE);
      SFileCloseFile((HSFILE)curr);
    }
  }
  {
    ARCHIVEPTR curr;
    while ((curr = s_archivelist.Head()) != NULL) {
      REPORTRESOURCELEAK(HSARCHIVE);
      SFileCloseArchive((HSARCHIVE)curr);
    }
  }

  // DELETE THE HASH TABLES
  if (s_hashsource) {
    FREE(s_hashsource);
    s_hashsource = NULL;
  }

  // DELETE THE DECOMPRESSION DATA
  if (s_explodebuffer) {
    FREE(s_explodebuffer);
    s_explodebuffer = NULL;
  }

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileEnableDirectAccess (BOOL enable) {
  TRACEOUT(TRACEHANDLE,
           "SFileEnableDirectAccess(%u)",
           enable);

  s_enabledirect = enable;
  return 1;
}

//===========================================================================
BOOL APIENTRY SFileGetArchiveInfo (HSARCHIVE  archive,
                                   int       *priority,
                                   BOOL      *cdrom) {
  if (priority)
    *priority = 0;
  if (cdrom)
    *cdrom = FALSE;

  // CHECK THE ARCHIVE HANDLE
  ARCHIVEPTR archiveptr;
  if (!CheckArchiveHandle(archive,&archiveptr))
    return FALSE;

  // RETURN THE INFORMATION
  if (priority)
    *priority = archiveptr->priority;
  if (cdrom)
    *cdrom = (archiveptr->cdrom == CDROM_TRUE);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SFileGetArchiveName (HSARCHIVE archive,
                                   LPTSTR    buffer,
                                   DWORD     bufferchars) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(buffer);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileGetArchiveName(0x%x,0x%x,%u)",
           archive,buffer,bufferchars);

  // CHECK THE ARCHIVE HANDLE
  ARCHIVEPTR archiveptr;
  if (!CheckArchiveHandle(archive,&archiveptr))
    return FALSE;

  SStrCopy(buffer,archiveptr->name,bufferchars);
  return 1;
}

//===========================================================================
BOOL APIENTRY SFileGetBasePath (LPTSTR buffer,
                                DWORD  bufferchars) {
  TRACEOUT(TRACEHANDLE,
           "SFileGetBasePath(0x%x,%u)",
           buffer,bufferchars);

  if (!s_basepath[0])
    BuildDefaultBasePath();
  SStrCopy(buffer,s_basepath,bufferchars);
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SFileGetFileArchive (HSFILE     file,
                                   HSARCHIVE *archive) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(archive);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileGetFileArchive(0x%x,*archive)",
           file);

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(file,&fileptr))
    return FALSE;

  *archive = (HSARCHIVE)(fileptr->archive);
  return 1;
}

//===========================================================================
BOOL APIENTRY SFileGetFileName (HSFILE file,
                                LPTSTR buffer,
                                DWORD  bufferchars) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(buffer);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileGetFileName(0x%x,0x%x,%u)",
           file,buffer,bufferchars);

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(file,&fileptr))
    return FALSE;

  SStrCopy(buffer,fileptr->name,bufferchars);
  return 1;
}

//===========================================================================
DWORD APIENTRY SFileGetFileSize (HSFILE  handle,
                                 LPDWORD filesizehigh) {
  TRACEOUT(TRACEHANDLE,
           "SFileGetFileSize(0x%x,*filesizehigh)",
           handle);

  if (filesizehigh)
    *filesizehigh = NULL;

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(handle,&fileptr))
    return 0xFFFFFFFF;

  // IF THIS HANDLE REPRESENTS A REAL FILE, FORWARD THE REQUEST TO THE
  // STANDARD WIN32 API
  if (fileptr->handle != INVALID_HANDLE_VALUE)
    return GetFileSize(fileptr->handle,filesizehigh);

  // RETURN THE FILE SIZE
  return fileptr->block->sizefile;
}

//===========================================================================
BOOL APIENTRY SFileOpenArchive (LPCTSTR    archivename,
                                int        priority,
                                BOOL       cdonly,
                                HSARCHIVE *handle) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(handle);
  VALIDATE(archivename);
  VALIDATE(*archivename);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileOpenArchive(\"%s\",%d,%u,*handle)",
           archivename,priority,cdonly);

  // INITIALIZE THE HASH TABLES IF NECESSARY
  if (!s_hashsource) {
    s_hashsource = (LPDWORD)ALLOC(5*256*sizeof(DWORD));
    InitializeHashSource();
  }

  // IF THE ARCHIVE FILENAME DOES NOT CONTAIN AN ABSOLUTE PATH AND THE
  // ARCHIVE DOES NOT EXIST IN THE GIVEN LOCATION RELATIVE TO THE CURRENT
  // DIRECTORY, CHECK FOR IT IN THE GIVEN LOCATION RELATIVE TO THE
  // EXECUTABLE DIRECTORY
  char localarchivename[MAX_PATH];
  SStrCopy(localarchivename,archivename,MAX_PATH);
  localarchivename[MAX_PATH-1] = 0;
  if (GetFileAttributes(localarchivename) & FILE_ATTRIBUTE_DIRECTORY)
    if ((*archivename == '\\') ||
        strstr(archivename,":\\") ||
        strstr(archivename,"\\\\"))
      SStrCopy(localarchivename,archivename,MAX_PATH);
    else
      ConvertRelativePathName(archivename,localarchivename);

  // DETERMINE WHETHER THIS ARCHIVE IS ON A CD-ROM DRIVE
  BOOL cdrom = CheckForCdRom(localarchivename);
  if (cdonly && !cdrom) {
    SErrSetLastError(SFILE_ERROR_INVALID_DRIVE);
    return 0;
  }

  // OPEN THE ARCHIVE FILE
  HANDLE archivehandle = CreateFile(localarchivename,
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    (LPSECURITY_ATTRIBUTES)NULL,
                                    OPEN_EXISTING,
                                    0,
                                    NULL);
  if (archivehandle == INVALID_HANDLE_VALUE)
    return 0;

  // CREATE A RECORD FOR THIS ARCHIVE
  ARCHIVEPTR archiveptr = s_archivelist.NewNode(LIST_UNLINKED);
  SStrCopy(archiveptr->name,localarchivename,MAX_PATH);
  archiveptr->handle   = archivehandle;
  archiveptr->cdrom    = cdrom ? CDROM_TRUE : CDROM_FALSE;
  archiveptr->priority = priority;

  // SINCE THE MOPAQ FILE MAY BE TACKED ON BELOW OTHER FILE DATA, SEARCH
  // FOR THE SIGNATURE TO DETERMINE THE STARTING LOCATION OF THE MOPAQ
  // FILE
  archiveptr->fileheader = NEW(FILEHEADER);
  DWORD bytesread;
  archiveptr->startinglocation = 0;
  do {
    SetFilePointer(archivehandle,archiveptr->startinglocation,NULL,FILE_BEGIN);
    bytesread = 0;
    ReadFile(archivehandle,
             archiveptr->fileheader,
             sizeof(FILEHEADER),
             &bytesread,
             NULL);
    if (bytesread != sizeof(FILEHEADER)) {
      DEL(archiveptr->fileheader);
      s_archivelist.DeleteNode(archiveptr);
      CloseHandle(archivehandle);
      SErrSetLastError(SFILE_ERROR_NOT_ARCHIVE);
      return 0;
    }
    if (archiveptr->fileheader->signature != SIGNATURE)
      archiveptr->startinglocation += 512;
  } while (archiveptr->fileheader->signature != SIGNATURE);

  // DETERMINE THE SECTOR SIZE AND ALLOCATE A SECTOR BUFFER
  archiveptr->sectorsize   = (512 << archiveptr->fileheader->sectorsizeid);
  archiveptr->sectorbuffer = (LPBYTE)ALLOC(archiveptr->sectorsize);

  // READ THE HASH TABLE
  archiveptr->hashtable = (HASHENTRYPTR)ALLOC(archiveptr->fileheader->hashcount*sizeof(HASHENTRY));
  SetFilePointer(archivehandle,archiveptr->startinglocation+archiveptr->fileheader->hashoffset,NULL,FILE_BEGIN);
  ReadFileChecked(archivehandle,
                  archiveptr->hashtable,
                  archiveptr->fileheader->hashcount*sizeof(HASHENTRY),
                  &bytesread,
                  NULL,
                  archiveptr->name);
  Decrypt((LPDWORD)archiveptr->hashtable,
          archiveptr->fileheader->hashcount*sizeof(HASHENTRY),
          Hash("(hash table)",HASH_ENCRYPTKEY));

  // READ THE BLOCK TABLE
  archiveptr->blocktable = (BLOCKENTRYPTR)ALLOC(archiveptr->fileheader->blockcount*sizeof(BLOCKENTRY));
  SetFilePointer(archivehandle,archiveptr->startinglocation+archiveptr->fileheader->blockoffset,NULL,FILE_BEGIN);
  ReadFileChecked(archivehandle,
                  archiveptr->blocktable,
                  archiveptr->fileheader->blockcount*sizeof(BLOCKENTRY),
                  &bytesread,
                  NULL,
                  archiveptr->name);
  Decrypt((LPDWORD)archiveptr->blocktable,
          archiveptr->fileheader->blockcount*sizeof(BLOCKENTRY),
          Hash("(block table)",HASH_ENCRYPTKEY));

  // IF THE STARTING LOCATION IS NOT ZERO, OFFSET ALL OF THE BLOCKS IN THE
  // BLOCK TABLE
  if (archiveptr->startinglocation) {
    for (DWORD block = 0; block < archiveptr->fileheader->blockcount; ++block)
      if ((archiveptr->blocktable+block)->offset)
        (archiveptr->blocktable+block)->offset += archiveptr->startinglocation;
  }

  // ADD THE ARCHIVE TO OUR LINKED LIST IN PRIORITY ORDER
  s_critsect.Enter();
  {
    ARCHIVEPTR curr = s_archivelist.Head();
    while (curr && (curr->priority > priority))
      curr = curr->Next();
    s_archivelist.LinkNode(archiveptr,LIST_LINK_BEFORE,curr);
  }
  s_critsect.Leave();

  // IF THIS ARCHIVE IS ON A CD-ROM AND THERE IS NO CD-ROM READ THREAD YET,
  // CREATE ONE
  if (cdrom && (s_cdthread == INVALID_HANDLE_VALUE))
    CreateCdThread();
  else if (cdonly && !cdrom) {
    SFileCloseArchive((HSARCHIVE)archiveptr);
    SErrSetLastError(SFILE_ERROR_INVALID_DRIVE);
    return 0;
  }

  // RETURN A HANDLE TO THE ARCHIVE
  *handle = (HSARCHIVE)archiveptr;

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileOpenFile (LPCTSTR  filename,
                             HSFILE  *handle) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(handle);
  VALIDATE(filename);
  VALIDATE(*filename);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileOpenFile(\"%s\",*handle)",
           filename);

  // IF THIS FILE IS AVAILABLE OUTSIDE OF AN ARCHIVE, OPEN IT DIRECTLY
  if (s_enabledirect || s_archivelist.IsEmpty()) {
    char localfilename[MAX_PATH] = "";
    if ((*filename == '\\') ||
        strstr(filename,":\\") ||
        strstr(filename,"\\\\"))
      SStrCopy(localfilename,filename,MAX_PATH);
    else
      ConvertRelativePathName(filename,localfilename);
    HANDLE filehandle = CreateFile(localfilename,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   (LPSECURITY_ATTRIBUTES)NULL,
                                   OPEN_EXISTING,
                                   FILE_FLAG_SEQUENTIAL_SCAN,
                                   NULL);
    if (filehandle != INVALID_HANDLE_VALUE) {
      s_critsect.Enter();
      FILEPTR fileptr = s_filelist.NewNode();
      fileptr->handle = filehandle;
      s_critsect.Leave();
      *handle = (HSFILE)fileptr;
      return TRUE;
    }
  }

  // OTHERWISE, SEARCH FOR THE REQUESTED FILE IN THE CURRENTLY OPEN ARCHIVES
  if (s_archivelist.IsEmpty() || !s_hashsource) {
    SErrSetLastError(SFILE_ERROR_FILE_NOT_FOUND);
    return 0;
  }
  ARCHIVEPTR archiveptr = s_archivelist.Head();
  DWORD      index;
  do {
    index = SearchHashTable(archiveptr,filename,s_lcid);
    if (index == 0xFFFFFFFF)
      archiveptr = archiveptr->Next();
  } while (archiveptr && (index == 0xFFFFFFFF));
  if (!archiveptr) {
    SErrSetLastError(SFILE_ERROR_FILE_NOT_FOUND);
    return 0;
  }

  return SFileOpenFileEx((HSARCHIVE)archiveptr,filename,0,handle);
}

//===========================================================================
BOOL APIENTRY SFileOpenFileEx (HSARCHIVE  archivehandle,
                               LPCTSTR    filename,
                               DWORD      flags,
                               HSFILE    *handle) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(handle);
  VALIDATE(filename);
  VALIDATE(*filename);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileOpenFileEx(0x%x,\"%s\",0x%x,*handle)",
           archivehandle,filename,flags);

  // CHECK THE ARCHIVE HANDLE
  ARCHIVEPTR archiveptr;
  if (!CheckArchiveHandle(archivehandle,&archiveptr))
    return FALSE;

  // DETERMINE THE HASH INDEX
  DWORD index = SearchHashTable(archiveptr,filename,s_lcid);
  if (index == 0xFFFFFFFF) {
    SErrSetLastError(SFILE_ERROR_FILE_NOT_FOUND);
    return 0;
  }

  // DETERMINE THE BLOCK POINTER
  BLOCKENTRYPTR block = archiveptr->blocktable+(archiveptr->hashtable+index)->block;

  // CONFIRM THAT THIS IS A VALID FILE
  if ((!block->sizefile) ||
      !(block->flags & MPQ_ALLOCATED)) {
    SErrSetLastError(SFILE_ERROR_FILE_INVALID);
    return 0;
  }

  // DETERMINE THE FILE NAME PORTION OF THE REQUESTED FILE
  LPCTSTR name = filename;
  while (SStrChr(name,':'))
    name = SStrChr(name,':')+1;
  while (SStrChr(name,'\\'))
    name = SStrChr(name,'\\')+1;

  // CREATE A DECRYPTION KEY BASED ON THE FILE NAME
  DWORD key = Hash(name,HASH_ENCRYPTKEY);
  if (block->flags & MPQ_ENCRYPTED_FIXLOC)
    key = (key + (block->offset-archiveptr->startinglocation)) ^ block->sizefile;

  // CREATE A RECORD FOR THE FILE
  DWORD   sectors           = (block->sizefile+archiveptr->sectorsize-1)/archiveptr->sectorsize;
  LPDWORD sectoroffsettable = NULL;
  if (block->flags & MPQ_COMPRESSEDMASK)
    sectoroffsettable = (LPDWORD)ALLOC((sectors+1)*sizeof(DWORD));
  s_critsect.Enter();
  FILEPTR fileptr = s_filelist.NewNode();
  SStrCopy(fileptr->name,filename,MAX_PATH);
  fileptr->handle            = INVALID_HANDLE_VALUE;
  fileptr->archive           = archiveptr;
  fileptr->block             = block;
  fileptr->key               = key;
  fileptr->sectors           = sectors;
  fileptr->sectoroffsettable = sectoroffsettable;
  fileptr->readaheadbuffer   = ALLOC(READAHEAD);
  s_critsect.Leave();

  // RETURN A HANDLE TO THE FILE
  *handle = (HSFILE)fileptr;

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileOpenFileWin32 (LPCTSTR  filename,
                                  HANDLE  *handle) {
  VALIDATEBEGIN;
  VALIDATEANDBLANK(handle);
  VALIDATE(filename);
  VALIDATE(*filename);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileOpenFileWin32(\"%s\",*handle)",
           filename);

  // IF THIS FILE IS AVAILABLE OUTSIDE OF AN ARCHIVE, OPEN IT DIRECTLY
  {
    char localfilename[MAX_PATH] = "";
    if ((*filename == '\\') ||
        strstr(filename,":\\") ||
        strstr(filename,"\\\\"))
      SStrCopy(localfilename,filename,MAX_PATH);
    else
      ConvertRelativePathName(filename,localfilename);
    *handle = CreateFile(localfilename,
                         GENERIC_READ,
                         FILE_SHARE_READ,
                         (LPSECURITY_ATTRIBUTES)NULL,
                         OPEN_EXISTING,
                         FILE_FLAG_SEQUENTIAL_SCAN,
                         NULL);
    if (*handle != INVALID_HANDLE_VALUE)
      return 1;
  }

  // OTHERWISE, SEARCH FOR THE REQUESTED FILE IN THE CURRENTLY OPEN ARCHIVES
  if (s_archivelist.IsEmpty() || !s_hashsource) {
    SErrSetLastError(SFILE_ERROR_FILE_NOT_FOUND);
    return 0;
  }
  ARCHIVEPTR archiveptr = s_archivelist.Head();
  DWORD      index;
  do {
    index = SearchHashTable(archiveptr,filename,s_lcid);
    if (index == 0xFFFFFFFF)
      archiveptr = archiveptr->Next();
  } while (archiveptr && (index == 0xFFFFFFFF));
  if (!archiveptr) {
    SErrSetLastError(SFILE_ERROR_FILE_NOT_FOUND);
    return 0;
  }

  // DETERMINE THE BLOCK POINTER
  BLOCKENTRYPTR block = archiveptr->blocktable+(archiveptr->hashtable+index)->block;

  // CONFIRM THAT THIS IS A VALID FILE, AND IS NOT COMPRESSED OR ENCRYPTED.
  // WE CAN'T RETURN WIN32 HANDLES TO COMPRESSED OR ENCRYPTED FILES.
  if ((!block->sizefile) ||
      (block->flags & (MPQ_COMPRESSEDMASK | MPQ_ENCRYPTED)) ||
      !(block->flags & MPQ_ALLOCATED)) {
    SErrSetLastError(SFILE_ERROR_FILE_INVALID);
    return 0;
  }

  // OPEN THE ARCHIVE THAT CONTAINS THE FILE
  *handle = CreateFile(archiveptr->name,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       (LPSECURITY_ATTRIBUTES)NULL,
                       OPEN_EXISTING,
                       FILE_FLAG_SEQUENTIAL_SCAN,
                       NULL);
  if (*handle == INVALID_HANDLE_VALUE)
    return 0;

  // SEEK TO THE BEGINNING OF THE FILE
  SetFilePointer(*handle,block->offset,0,FILE_BEGIN);

  return 1;
}

//===========================================================================
BOOL APIENTRY SFileReadFile (HSFILE       handle,
                             LPVOID       buffer,
                             DWORD        bytestoread,
                             LPDWORD      bytesread,
                             LPOVERLAPPED overlapped) {
  if (bytesread)
    *bytesread = 0;

  VALIDATEBEGIN;
  VALIDATE(buffer);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileReadFile(0x%x,0x%x,%u,*bytesread,0x%x)",
           handle,buffer,bytestoread,overlapped);

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(handle,&fileptr))
    return FALSE;

  if (!bytestoread)
    return TRUE;

  // IF THIS HANDLE REPRESENTS A REAL FILE, FORWARD THE REQUEST TO THE
  // STANDARD WIN32 API
  if (fileptr->handle != INVALID_HANDLE_VALUE) {
    DWORD  localbytesread = 0;
    HANDLE event          = NULL;
    if (overlapped) {
      event = overlapped->hEvent;
      overlapped->hEvent = NULL;
    }
    ReadFileChecked(fileptr->handle,
                    buffer,
                    bytestoread,
                    &localbytesread,
                    overlapped,
                    fileptr->name);
    if (event)
      SetEvent(event);
    if (bytesread)
      *bytesread = localbytesread;
    return (localbytesread == bytestoread);
  }

  // TRY TO SATISFY, OR PARTIALLY SATISFY, THIS REQUEST FROM THE READ
  // AHEAD BUFFER
  if (fileptr->readaheadbuffer &&
      (fileptr->readaheadoffset < fileptr->readaheadbytes) &&
      !overlapped) {
    DWORD satisfybytes = min(bytestoread,fileptr->readaheadbytes-fileptr->readaheadoffset);
    CopyMemory(buffer,(LPBYTE)fileptr->readaheadbuffer+fileptr->readaheadoffset,satisfybytes);
    fileptr->location        += satisfybytes;
    fileptr->readaheadoffset += satisfybytes;
    buffer                    = (LPBYTE)buffer+satisfybytes;
    bytestoread              -= satisfybytes;
    if (bytesread)
      *bytesread = satisfybytes;
    if (!bytestoread)
      return TRUE;
  }

  // DETERMINE THE AMOUNT OF DATA TO READ
  // (NO READ-AHEAD IS DONE FOR OVERLAPPED I/O, BECAUSE THE EVENT WILL
  //  BE TRIGGERED BEFORE WE HAVE A CHANCE TO DO THE BUFFER COPY)
  LPVOID readbuffer = buffer;
  DWORD  readbytes  = bytestoread;
  if (!overlapped) {
    DWORD desiredreadahead = min(READAHEAD,
                                 fileptr->archive->sectorsize-(fileptr->location & (fileptr->archive->sectorsize-1)));
    if (bytestoread < desiredreadahead) {
      readbuffer = fileptr->readaheadbuffer;
      readbytes  = desiredreadahead;
    }
  }

  // IF THERE IS NOTHING TO READ, RETURN
  if (!readbytes)
    return TRUE;

  // IF THIS FILE IS ON A CD-ROM DRIVE, QUEUE THE REQUEST FOR THE CD-ROM
  // I/O THREAD
  DWORD location       = overlapped ? overlapped->Offset : fileptr->location;
  DWORD totalbytesread = 0;
  if (fileptr->archive->cdrom != CDROM_FALSE) {
    REQUESTPTR requestptr;
    HANDLE     event = overlapped ? overlapped->hEvent
                                  : CreateEvent(NULL,TRUE,FALSE,NULL);
    DWORD blocks  = (readbytes+DATACHUNKSIZE-1)/DATACHUNKSIZE;
    DWORD prevseq = 0;
    DWORD loop;
    for (loop = 0; loop < blocks; ++loop) {
      BOOL  lastblock = (loop == blocks-1);
      DWORD offset    = loop*DATACHUNKSIZE;
      DWORD bytes     = min(DATACHUNKSIZE,readbytes-offset);
      requestptr = IssueRequest(fileptr,
                                location+loop*DATACHUNKSIZE,
                                (LPBYTE)readbuffer+offset,
                                NULL,
                                0,
                                NULL,
                                bytes,
                                INT_MAX,
                                prevseq,
                                lastblock ? event : NULL,
                                overlapped || !lastblock,
                                lastblock,
                                &prevseq);
      if (!lastblock)
        totalbytesread += bytes;
    }
    if (overlapped) {
      SErrSetLastError(ERROR_IO_PENDING);
      return FALSE;
    }
    WaitForSingleObject(event,INFINITE);
    CloseHandle(event);
    totalbytesread += requestptr->bytesread;
    FREE(requestptr);
  }

  // OTHERWISE, READ THE REQUESTED DATA
  else {
    totalbytesread = InternalReadUnaligned(fileptr,
                                           location,
                                           readbuffer,
                                           readbytes);
    if (overlapped && overlapped->hEvent)
      SetEvent(overlapped->hEvent);
  }

  // COPY THE READ DATA TO THE USER'S BUFFER
  DWORD userbytesread = min(bytestoread,totalbytesread);
  if (readbuffer != buffer) {
    CopyMemory(buffer,readbuffer,userbytesread);
    fileptr->readaheadoffset = userbytesread;
    fileptr->readaheadbytes  = totalbytesread;
  }

  // UPDATE THE FILE POINTER
  if (!overlapped)
    fileptr->location += userbytesread;

  // RETURN THE NUMBER OF BYTES READ
  if (bytesread)
    *bytesread += userbytesread;

  if (userbytesread == bytestoread)
    return TRUE;
  else {
    SErrSetLastError(SFILE_ERROR_HANDLE_EOF);
    return FALSE;
  }
}

//===========================================================================
BOOL APIENTRY SFileSetBasePath (LPCTSTR path) {
  VALIDATEBEGIN;
  VALIDATE(path);
  VALIDATEEND;

  TRACEOUT(TRACEHANDLE,
           "SFileSetBasePath(\"%s\")",
           path);

  // IF WE WERE GIVEN A NULL PATH, RESET THE BASE PATH VARIABLE.  IT WILL
  // BE AUTOMATICALLY SET ON NEXT USE TO THE PROGRAM PATH.
  if (!*path)
    s_basepath[0] = 0;

  // OTHERWISE, SET THE BASE PATH TO THE GIVEN PATH
  else {
    BOOL terminated = (*(path+SStrLen(path)-1) == '\\');
    if ((SStrLen(path)+1+!terminated) > MAX_PATH) {
      SErrSetLastError(SFILE_ERROR_BAD_PATHNAME);
      return 0;
    }
    SStrCopy(s_basepath,path,MAX_PATH);
    if (!terminated)
      SStrPack(s_basepath,"\\",MAX_PATH);
  }

  return TRUE;
}

//===========================================================================
DWORD APIENTRY SFileSetFilePointer (HSFILE handle,
                                    LONG   distancetomove,
                                    PLONG  distancetomovehigh,
                                    DWORD  movemethod) {
  TRACEOUT(TRACEHANDLE,
           "SFileSetFilePointer(0x%x,%d,0x%x,%u)",
           handle,distancetomove,distancetomovehigh,movemethod);

  if (distancetomovehigh && *distancetomovehigh) {
    SErrSetLastError(SFILE_ERROR_INVALID_PARAMETER);
    return 0xFFFFFFFF;
  }

  // CHECK THE FILE HANDLE
  FILEPTR fileptr;
  if (!CheckFileHandle(handle,&fileptr))
    return 0xFFFFFFFF;

  // IF THIS HANDLE REPRESENTS A REAL FILE, FORWARD THE REQUEST TO THE
  // STANDARD WIN32 API
  if (fileptr->handle != INVALID_HANDLE_VALUE)
    return SetFilePointer(fileptr->handle,distancetomove,NULL,movemethod);

  // SET THE NEW FILE LOCATION
  switch (movemethod) {

    case FILE_BEGIN:
      fileptr->location = distancetomove;
      break;

    case FILE_CURRENT:
      if ((distancetomove < 0) && (fileptr->location < (DWORD)-distancetomove))
        fileptr->location = 0;
      else
        fileptr->location += distancetomove;
      break;

    case FILE_END:
      if ((distancetomove < 0) && (fileptr->block->sizefile < (DWORD)-distancetomove))
        fileptr->location = 0;
      else
        fileptr->location = fileptr->block->sizefile+distancetomove;
      break;

  }
  fileptr->location        = min(fileptr->location,fileptr->block->sizefile-1);
  fileptr->readaheadoffset = 0;
  fileptr->readaheadbytes  = 0;

  // RETURN THE NEW LOCATION
  return fileptr->location;
}

//===========================================================================
BOOL APIENTRY SFileSetIoErrorMode (DWORD          errormode,
                                   SFILEERRORPROC errorproc) {
  TRACEOUT(TRACEHANDLE,
           "SFileSetIoErrorMode(%u,0x%x)",
           errormode,errorproc);

  s_ioerrormode = errormode;
  s_ioerrorproc = errorproc;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SFileSetLocale (LCID lcid) {
  TRACEOUT(TRACEHANDLE,
           "SFileSetLocale(0x%x)",
           lcid);

  s_lcid = lcid;
  return 1;
}
