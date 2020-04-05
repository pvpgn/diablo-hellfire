//******************************************************************
// ENCRYPT.CPP
// File pack utility
// By Michael O'Brien (6/1/96) && Patrick Wyatt (6/24/96)
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include "engine.h"
#include "mpqapi.h"
#include "implode.h"


//******************************************************************
// private
//******************************************************************
	typedef struct _COMPRESSIONINFO {
		LPVOID sourcebuffer;
		DWORD sourceoffset;
		LPVOID destbuffer;
		DWORD destoffset;
		DWORD bytes;
	} COMPRESSIONINFO, *COMPRESSIONPTR;

	static DWORD hashsource[5][256];


//******************************************************************
//******************************************************************
void Decrypt(LPDWORD data, DWORD bytes, DWORD key) {
	DWORD adjust = 0xEEEEEEEE;
	DWORD iter = bytes >> 2;
	while (iter--) {
		adjust += hashsource[HASH_ENCRYPTDATA][key & 0xFF];
		adjust += (*data++ ^= adjust+key)+(adjust << 5)+3;
		key     = (key >> 11) | ((key << 21) ^ 0xFFE00000)+0x11111111;
  }
}


//******************************************************************
//******************************************************************
void Encrypt(LPDWORD data, DWORD bytes, DWORD key) {
	DWORD adjust = 0xEEEEEEEE;
	DWORD iter	= bytes >> 2;
	while (iter--) {
		DWORD origdata = *data;
		adjust += hashsource[HASH_ENCRYPTDATA][key & 0xFF];
		*data++ = origdata ^ (adjust+key);
		adjust += origdata + (adjust << 5)+3;
		key		= (key >> 11) | ((key << 21) ^ 0xFFE00000)+0x11111111;
	}
}


//******************************************************************
//******************************************************************
DWORD Hash(const char *filename, int hashtype) {
	DWORD result = 0x7FED7FED;
	DWORD adjust = 0xEEEEEEEE;
	while (filename && *filename) {
		char origchar = toupper(*filename++);
		result	= (result+adjust) ^ hashsource[hashtype][origchar];
		adjust += origchar+result+(adjust << 5)+3;
	}

	return result;
}


//******************************************************************
//******************************************************************
void InitializeHashSource() {
	DWORD seed = 0x100001;
	for (int loop1 = 0; loop1 < 256; ++loop1) {
		for (int loop2 = 0; loop2 < 5; ++loop2) {
			seed = (seed*0x7D+3) % 0x2AAAAB;
			DWORD rand1 = seed & 0xFFFF;
			seed = (seed*0x7D+3) % 0x2AAAAB;
			DWORD rand2 = seed & 0xFFFF;
			hashsource[loop2][loop1] = (rand1 << 16) | rand2;
		}
	}
}


//******************************************************************
//******************************************************************
static UINT __cdecl CompBufferRead(LPSTR buffer, UINT *size, LPVOID param) {
	COMPRESSIONPTR infoptr = (COMPRESSIONPTR) param;
	UINT bytes = min(*size,infoptr->bytes-infoptr->sourceoffset);
	CopyMemory(buffer,(LPSTR)infoptr->sourcebuffer+infoptr->sourceoffset,bytes);
	infoptr->sourceoffset += bytes;
	return bytes;
}


//******************************************************************
//******************************************************************
static void __cdecl CompBufferWrite(LPSTR buffer, UINT *size, LPVOID param) {
	COMPRESSIONPTR infoptr = (COMPRESSIONPTR)param;
	CopyMemory((LPSTR)infoptr->destbuffer+infoptr->destoffset,buffer,*size);
	infoptr->destoffset += *size;
}


//******************************************************************
//******************************************************************
DWORD Compress(LPBYTE data, DWORD bytes) {
	// ALLOCATE COMPRESSION BUFFERS
	LPVOID implodebuffer = DiabloAllocPtrSig(CMP_BUFFER_SIZE,'CMPt');
	LPVOID destbuffer = DiabloAllocPtrSig(max(SECTORSIZE*2,bytes*2),'CMPt');

	// CREATE AN INFORMATION RECORD
	COMPRESSIONINFO info;
	info.sourcebuffer = data;
	info.sourceoffset = 0;
	info.destbuffer	= destbuffer;
	info.destoffset	= 0;
	info.bytes = bytes;

	// PERFORM THE COMPRESSION
	UINT comptype = CMP_BINARY;
	UINT dictsize = max(512,min(4096,SECTORSIZE));
	implode(CompBufferRead,CompBufferWrite,(LPSTR)implodebuffer,&info,&comptype,&dictsize);

	// IF THE DATA WAS NOT COMPRESSABLE, RETURN THE SOURCE DATA
	// OTHERWISE, RETURN THE COMPRESSED DATA
	if (info.destoffset < bytes) {
		CopyMemory(data,destbuffer,info.destoffset);
		bytes = info.destoffset;
	}

	DiabloFreePtr(implodebuffer);
	DiabloFreePtr(destbuffer);
	return bytes;
}


//******************************************************************
//******************************************************************
void Expand(LPBYTE data, DWORD bytes, DWORD dwMaxBytes) {
	// ALLOCATE COMPRESSION BUFFERS
	LPVOID implodebuffer = DiabloAllocPtrSig(CMP_BUFFER_SIZE,'CMPt');
	LPVOID destbuffer = DiabloAllocPtrSig(dwMaxBytes,'CMPt');

	// CREATE AN INFORMATION RECORD
	COMPRESSIONINFO info;
	info.sourcebuffer = data;
	info.sourceoffset = 0;
	info.destbuffer	= destbuffer;
	info.destoffset	= 0;
	info.bytes = bytes;

	// PERFORM THE DECOMPRESSION
	explode(CompBufferRead,CompBufferWrite,(LPSTR)implodebuffer,&info);
	app_assert(info.destoffset <= dwMaxBytes);

	// copy back into the original buffer
	CopyMemory(data,destbuffer,info.destoffset);

	DiabloFreePtr(implodebuffer);
	DiabloFreePtr(destbuffer);
}
