//******************************************************************
// codec.cpp
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include "engine.h"


//******************************************************************
// externs
//******************************************************************
void DesDestroy ();
void DesEncrypt (int, const void *, void *);
void DesInitialize (int, BOOL, const void *);
void IdeaDestroy ();
void IdeaEncrypt (int, const void *, void *);
void IdeaInitialize (int, BOOL, const void *);
void ShaDestroy ();
void ShaGetLastHash (int, void *);
void ShaHash (int, const void *, void *);
void ShaInitialize (int);


//******************************************************************
// set encryption methods
//******************************************************************
#define IDEA	0
#define DES		0


//******************************************************************
// private
//******************************************************************
#define	BLOCKSIZE	64
#define	VERSION		0

typedef struct _appendrec {
	DWORD checkvalue;
	BYTE	version;
	BYTE	lastblocksize;
	WORD	reserved;
} appendrec, *appendptr;

typedef struct _keyrec {
	WORD ideakey[3][8];
	BYTE deskey[3][8];
	BYTE shainitvect[64];
} keyrec, *keyptr;


//******************************************************************
//******************************************************************
static void DestroyKeys () {
	#if DES
	DesDestroy();
	#endif
	#if IDEA
	IdeaDestroy();
	#endif
	ShaDestroy();
}


//******************************************************************
//******************************************************************
static void InitializeKeys (BOOL encrypt, const char *password) {
	keyrec keyset;

	// generate a key
	srand(SAVE_GAME_KEY);
	BYTE * pb = (BYTE *) &keyset;
	for (int i = sizeof(keyset); i--; )
		*pb++ = (BYTE) rand();

	// HASH THE PASSWORD AND MIX IT WITH THE KEY
	{
		BYTE originalpassword[64];
		BYTE hashedpassword[20];
		{
			int passchar = 0;
			for (int loop = 0; loop < 64; ++loop) {
				if (!*(password+passchar))
					passchar = 0;
				originalpassword[loop] = *(password+passchar++);
			}
		}

		ShaInitialize(0);
		ShaHash(0,originalpassword,hashedpassword);
		ShaDestroy();

		{
			LPBYTE keysptr = (LPBYTE)&keyset;
			for (int loop = 0; loop < sizeof(keyset); ++loop)
				*(keysptr+loop) ^= hashedpassword[loop % 20];
		}

		ZeroMemory(originalpassword, sizeof originalpassword);
		ZeroMemory(hashedpassword, sizeof hashedpassword);
	}

	// INITIALIZE THE ENCRYPTION ALGORITHMS
	for (int loop = 0; loop < 3; ++loop) {
		#if DES
		DesInitialize(loop,encrypt,keyset.deskey[loop]);
		#endif
		#if IDEA
		IdeaInitialize(loop,encrypt,keyset.ideakey[loop]);
		#endif
		ShaInitialize(loop);
		ShaHash(loop,keyset.shainitvect,NULL);
	}

	// WIPE OUT THE LOCAL COPY OF THE KEYS
	ZeroMemory(&keyset,sizeof(keyrec));
}


//******************************************************************
//******************************************************************
DWORD DecodeFile(BYTE * pbSrcDst,DWORD dwDstBytes,const char * pszPassword) {
	app_assert(pbSrcDst);
	app_assert(pszPassword);

	// initialize encryption keys
	InitializeKeys(0,pszPassword);

	// make sure the length is correct
	if (dwDstBytes <= sizeof(appendrec)) return 0;
	dwDstBytes -= sizeof(appendrec);
	if (dwDstBytes & (BLOCKSIZE-1)) return 0;
	DWORD dwBytesLeft = dwDstBytes;

	// DECRYPT THE FILE BLOCK BY BLOCK
	BYTE buffer[2][BLOCKSIZE];
	while (dwBytesLeft) {

		// get the next chunk
		CopyMemory(&buffer[0][0],pbSrcDst,BLOCKSIZE);

		// DECRYPT THE BLOCK
		{
			BYTE hash[20];
			ShaGetLastHash(0,hash);
			int loop;

			#if IDEA
			for (loop = 0; loop < BLOCKSIZE; loop += 8) {
				IdeaEncrypt(2,&buffer[0][loop],&buffer[1][loop]);
				IdeaEncrypt(1,&buffer[1][loop],&buffer[0][loop]);
				IdeaEncrypt(0,&buffer[0][loop],&buffer[1][loop]);
			}
			for (loop = 0; loop < BLOCKSIZE; loop++)
				buffer[0][loop] = buffer[1][loop]-hash[(BLOCKSIZE-(loop+1)) % 20];
			#endif

			#if DES
			for (loop = 0; loop < BLOCKSIZE; loop += 8) {
				DesEncrypt(2,&buffer[0][loop],&buffer[1][loop]);
				DesEncrypt(1,&buffer[1][loop],&buffer[0][loop]);
				DesEncrypt(0,&buffer[0][loop],&buffer[1][loop]);
			}
			for (loop = 0; loop < BLOCKSIZE; loop++)
				buffer[0][loop] = hash[loop % 20] ^ buffer[1][loop];
			#endif

			#if ! DES && ! IDEA
			for (loop = 0; loop < BLOCKSIZE; loop++)
				buffer[0][loop] = hash[loop % 20] ^ buffer[0][loop];
			#endif

			ShaHash(0,buffer[0],NULL);
			ZeroMemory(hash,sizeof(hash));
		}


		// WRITE THE BLOCK
		CopyMemory(pbSrcDst,&buffer[0][0],BLOCKSIZE);

		// next block
		pbSrcDst += BLOCKSIZE;
		dwBytesLeft	-= BLOCKSIZE;
	}

	// hide buffer contents
	ZeroMemory(buffer,sizeof(buffer));

	// CHECK THE FILE TERMINATION RECORD
	const appendrec * append = (const appendrec *) pbSrcDst;

	// CHECK THE ENCRYPTION VERSION
	if (append->version > VERSION)
		goto error;

	// CONFIRM THAT THE KEY AND PASSWORD WERE VALID
	{
		BYTE hash[20];
		ShaGetLastHash(0,hash);
		if (append->checkvalue != * (LPDWORD) &hash[0]) {
			ZeroMemory(hash,20);
			goto error;
		}
	}

	// SET THE EXACT OUTPUT SIZE
	dwDstBytes -= BLOCKSIZE - append->lastblocksize;
	DestroyKeys();
	return dwDstBytes;

error:
	DestroyKeys();
	return 0;
}


//******************************************************************
// CalcEncodeDstBytes()
// -- calculate the number of bytes required to hold the encoded
// file information including the append record
//******************************************************************
DWORD CalcEncodeDstBytes(DWORD dwSrcBytes) {
	app_assert(dwSrcBytes);
	if (dwSrcBytes & (BLOCKSIZE-1))
		dwSrcBytes += BLOCKSIZE - (dwSrcBytes & (BLOCKSIZE-1));
	dwSrcBytes += sizeof(appendrec);
	return dwSrcBytes;
}


//******************************************************************
//******************************************************************
void EncodeFile(BYTE * pbSrcDst,DWORD dwSrcBytes,DWORD dwDstBytes,const char * pszPassword) {
	app_assert(pbSrcDst);
	app_assert(pszPassword);

	// make sure the user allocated enough bytes for the destination
	if (dwDstBytes != CalcEncodeDstBytes(dwSrcBytes))
		app_fatal("Invalid encode parameters");

	// initialize encryption keys
	InitializeKeys(1,pszPassword);

	// ENCRYPT THE FILE BLOCK BY BLOCK
	DWORD lastblocksize = 0;
	BYTE buffer[2][BLOCKSIZE];

	while (dwSrcBytes) {

		// get the next src data chunk
		DWORD blocksize = min(dwSrcBytes,BLOCKSIZE);
		CopyMemory(&buffer[0][0],pbSrcDst,blocksize);

		// blank out any unused portion of the buffer
		if (blocksize < BLOCKSIZE)
			ZeroMemory(&buffer[0][blocksize],BLOCKSIZE - blocksize);

		// ENCRYPT THE BLOCK
		{
			BYTE hash[20];
			ShaGetLastHash(0,hash);
			ShaHash(0,buffer[0],NULL);
			int loop;

			#if ! DES && ! IDEA
			for (loop = 0; loop < BLOCKSIZE; loop++)
				buffer[0][loop] = hash[loop % 20] ^ buffer[0][loop];
			#endif

			#if DES
			for (loop = 0; loop < BLOCKSIZE; loop++)
				buffer[1][loop] = hash[loop % 20] ^ buffer[0][loop];
			for (loop = 0; loop < BLOCKSIZE; loop += 8) {
				DesEncrypt(0,&buffer[1][loop],&buffer[0][loop]);
				DesEncrypt(1,&buffer[0][loop],&buffer[1][loop]);
				DesEncrypt(2,&buffer[1][loop],&buffer[0][loop]);
			}
			#endif

			#if IDEA
			for (loop = 0; loop < BLOCKSIZE; loop++)
				buffer[1][loop] = buffer[0][loop]+hash[(BLOCKSIZE-(loop+1)) % 20];
			for (loop = 0; loop < BLOCKSIZE; loop += 8) {
				IdeaEncrypt(0,&buffer[1][loop],&buffer[0][loop]);
				IdeaEncrypt(1,&buffer[0][loop],&buffer[1][loop]);
				IdeaEncrypt(2,&buffer[1][loop],&buffer[0][loop]);
			}
			#endif

			// hide hash info
			ZeroMemory(hash,sizeof(hash));
		}

		// write encrypted chunk to destination
		CopyMemory(pbSrcDst,&buffer[0][0],BLOCKSIZE);

		// next block
		pbSrcDst += BLOCKSIZE;
		dwSrcBytes -= blocksize;
		lastblocksize = blocksize;
	}

	// hide buffer
	ZeroMemory(buffer,sizeof(buffer));

	// APPEND THE TERMINATION RECORD
	BYTE hash[20];
	appendrec * append = (appendrec *) pbSrcDst;
	ShaGetLastHash(0,hash);
	append->checkvalue = * (LPDWORD) &hash[0];
	append->version = VERSION;
	append->lastblocksize = (BYTE) lastblocksize;
	append->reserved = 0;

	DestroyKeys();
}
