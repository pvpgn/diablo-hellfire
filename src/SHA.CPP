/****************************************************************************
*
*  SHA.CPP
*  Secure Hash Algorithm
*
*  Implementation copyright (C) 1995, Michael O'Brien.  All rights reserved.
*
*           key size - N/A
*   input block size - 512 bits
*  output block size - 160 bits
*
***/


#include "diablo.h"
#pragma hdrstop


//******************************************************************
//******************************************************************
#define  SHA_BLOCKSIZE   64
#define  SHA_DIGESTSIZE  20

#define  F1(x,y,z)       ((x & y) | (~x & z))
#define  F2(x,y,z)       (x ^ y ^ z)
#define  F3(x,y,z)       ((x & y) | (x & z) | (y & z))
#define  F4(x,y,z)       (x ^ y ^ z)

#define  K1              0x5A827999
#define  K2              0x6ED9EBA1
#define  K3              0x8f1BBCDC
#define  K4              0xCA62C1D6

#define  H0INIT          0x67452301
#define  H1INIT          0xEFCDAB89
#define  H2INIT          0x98BADCFE
#define  H3INIT          0x10325476
#define  H4INIT          0xC3D2E1F0

#define  S(n,x)          ((x << n) | (x >> (32 - n)))

#define  EXPAND(c)       w[c] = w[c-3] ^ w[c-8] ^ w[c-14] ^ w[c-16]

#define  SUBROUND1(num)  temp = S(5,a) + F1(b,c,d) + e + w[num] + K1
#define  SUBROUND2(num)  temp = S(5,a) + F2(b,c,d) + e + w[num] + K2
#define  SUBROUND3(num)  temp = S(5,a) + F3(b,c,d) + e + w[num] + K3
#define  SUBROUND4(num)  temp = S(5,a) + F4(b,c,d) + e + w[num] + K4

typedef struct _shainfo {
  int digest[5];
  int countlo;
  int counthi;
  int data[16];
} shainfo, *shainfoptr;


//******************************************************************
//******************************************************************
static void InitializeHash (shainfoptr infoptr) {
  infoptr->digest[0] = H0INIT;
  infoptr->digest[1] = H1INIT;
  infoptr->digest[2] = H2INIT;
  infoptr->digest[3] = H3INIT;
  infoptr->digest[4] = H4INIT;
  infoptr->countlo   = 0;
  infoptr->counthi   = 0;
}


//******************************************************************
//******************************************************************
static void TransformHash (shainfoptr infoptr) {
  int w[80];
  int loop;
  for (loop = 0; loop < 16; loop++)
    w[loop] = infoptr->data[loop];
  for (loop = 16; loop < 80; loop++)
    EXPAND(loop);

  int a = infoptr->digest[0];
  int b = infoptr->digest[1];
  int c = infoptr->digest[2];
  int d = infoptr->digest[3];
  int e = infoptr->digest[4];

  int temp;
  for (loop = 0; loop < 20; loop++) {
    SUBROUND1(loop);
    e = d;
    d = c;
    c = S(30,b);
    b = a;
    a = temp;
  }
  for (loop = 20; loop < 40; loop++) {
    SUBROUND2(loop);
    e = d;
    d = c;
    c = S(30,b);
    b = a;
    a = temp;
  }
  for (loop = 40; loop < 60; loop++) {
    SUBROUND3(loop);
    e = d;
    d = c;
    c = S(30,b);
    b = a;
    a = temp;
  }
  for (loop = 60; loop < 80; loop++) {
    SUBROUND4(loop);
    e = d;
    d = c;
    c = S(30,b);
    b = a;
    a = temp;
  }

  infoptr->digest[0] += a;
  infoptr->digest[1] += b;
  infoptr->digest[2] += c;
  infoptr->digest[3] += d;
  infoptr->digest[4] += e;

  for (loop = 0; loop < 80; loop++)
    w[loop] = 0;
  a = b = c = d = e = 0;
}


//******************************************************************
//******************************************************************
static void UpdateHash (shainfoptr infoptr, const BYTE *buffer, int bytes) {
  if ((infoptr->countlo + (bytes << 3)) < infoptr->countlo)
    infoptr->counthi++;
  infoptr->countlo += bytes << 3;
  infoptr->counthi += bytes >> 29;

  while (bytes >= SHA_BLOCKSIZE) {
    CopyMemory(infoptr->data,buffer,SHA_BLOCKSIZE);
    TransformHash(infoptr);
    buffer += SHA_BLOCKSIZE;
    bytes  -= SHA_BLOCKSIZE;
  }
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

static shainfo currinfo[3];


//******************************************************************
//******************************************************************
void ShaDestroy () {
  ZeroMemory(&currinfo,3*sizeof(shainfo));
}


//******************************************************************
//******************************************************************
void ShaGetLastHash (int streamnum, void *outptr) {
  if (outptr) {
    DWORD *outptr32 = (DWORD *)outptr;
    int loop = 0;
    while (loop < 5)
      *(outptr32++) = currinfo[streamnum].digest[loop++];
  }
}


//******************************************************************
//******************************************************************
void ShaHash (int streamnum, const void *inptr, void *outptr) {
  UpdateHash(&currinfo[streamnum],(const BYTE *)inptr,SHA_BLOCKSIZE);
  if (outptr)
    ShaGetLastHash(streamnum,outptr);
}


//******************************************************************
//******************************************************************
void ShaInitialize (int streamnum) {
  InitializeHash(&currinfo[streamnum]);
}
