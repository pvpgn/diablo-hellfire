/***************************************************************
 PKWARE Data Compression Library (R) for Win32
 Copyright 1991,1992,1994,1995 PKWARE Inc.  All Rights Reserved.
 PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off.
***************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "implode.h"

/* Define a structure containing data to be passed to the callback
   functions through the user defined parameter.
*/

struct PassedParam
{
   unsigned int CmpPhase;
   FILE *InFile;
   FILE *OutFile;
   unsigned long CRC;
};

/*-------------------------------------------------------------------
   Routine to supply data to the implode() or explode() routines.
   When this routine returns 0 bytes read, the implode() or explode()
   routines will terminate.  Also calculate the CRC-32 on the original
   uncompressed data during the implode() call.
*/

unsigned int ReadData(char *Buff, unsigned int *Size, void *Param)
{
   size_t Read;
   struct PassedParam *Par = (struct PassedParam *)Param;

   Read = fread(Buff, 1, *Size, Par->InFile);
   if (Par->CmpPhase)
      Par->CRC = crc32(Buff, (unsigned int *)&Read, &Par->CRC);

   return (unsigned int)Read;
}

/*-------------------------------------------------------------------
   Routine to write compressed data output from implode() or
   uncompressed data from explode().  Also calculate the CRC on
   the uncompressed data during the explode() call.
*/

void WriteData(char *Buff, unsigned int *Size, void *Param)
{
   struct PassedParam *Par = (struct PassedParam *)Param;

   fwrite(Buff, 1, *Size, Par->OutFile);
   if (!Par->CmpPhase)
      Par->CRC = crc32(Buff, Size, &Par->CRC);
}

int cdecl main(void)
{
   char *WorkBuff;               /* buffer for compression tables */
   unsigned int Error;
   unsigned int type;            /* ASCII or Binary compression   */
   unsigned int dsize;           /* Dictionary Size. 1,2 or 4K    */
   unsigned long OrgCRC;         /* CRC of original input file    */
   struct PassedParam Param;     /* Parameters passed to callback functions */

      /* Open the input file */
   Param.InFile = fopen("test.in","rb");
   if (Param.InFile == NULL)
   {
      puts("Unable to open input file");
      return 1;
   }

      /* Create the output compressed file */
   Param.OutFile = fopen("test.cmp","wb");

      /* Allocate memory for implode work buffer */
   WorkBuff = (char *)malloc(CMP_BUFFER_SIZE);
   if (WorkBuff == NULL)
   {
      puts("Unable to allocate work buffer");
      return 1;
   }

      /* Initialize CRC */
   Param.CmpPhase = 1;
   Param.CRC = (unsigned long) -1;

   type  = CMP_ASCII;                  /* Use ASCII compression */
   dsize = 4096;                       /* Use 4K dictionary     */

   puts("Calling Implode");
   implode(ReadData, WriteData, WorkBuff, &Param, &type, &dsize);
   puts("Done Compressing");

   OrgCRC = ~Param.CRC;
   free(WorkBuff);

   fclose(Param.InFile);
   fclose(Param.OutFile);

       /* Compression done, now try extracting the compressed file */

   WorkBuff = (char *)malloc(EXP_BUFFER_SIZE);
   if (WorkBuff == NULL)
   {
      puts("Unable to allocate work buffer");
      return 1;
   }

   Param.InFile  = fopen("test.cmp","rb");      /* Compressed file    */
   Param.OutFile = fopen("test.ext","wb");      /* File to extract to */

      /* Initialize CRC */
   Param.CmpPhase = 0;
   Param.CRC = (unsigned long) -1;

   puts("Calling Explode");
   Error = explode(ReadData, WriteData, WorkBuff, &Param);

   Param.CRC = ~Param.CRC;
   free(WorkBuff);
   fclose(Param.InFile);
   fclose(Param.OutFile);

   printf("Uncompression completed - Error %d\n", Error);
   printf("Original CRC=%08lx  Uncompressed CRC=%08lx\n",OrgCRC,Param.CRC);

   return 0;
}

