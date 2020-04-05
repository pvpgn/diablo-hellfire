/*************************************************************************
   Example to interface the PKWARE Data Compression Library (R)
   Copyright 1995 PKWARE Inc. All Rights Reserved.
   PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off
   Version 1.11

   This example takes a file from the disk (file must be <= 62K), 
   compresses the file to memory, and then expands the compressed 
   data back into another memory buffer.  This data is then written 
   to a file called test.ext, that can be compared to the original file.
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "implode.h"

#define BUFFERSIZE   ((long)(62 * 1024))               /* File size limit */
#define FALSE          0
#define TRUE           (!FALSE)

typedef struct PassedParam
{
   char *pSource;                   /* Pointer to source buffer           */
   char *pDestination;              /* Pointer to destination buffer      */
   unsigned long SourceOffset;      /* Offset into the source buffer      */
   unsigned long DestinationOffset; /* Offset into the destination buffer */
   unsigned long CompressedSize;    /* Need this for extracting!          */
   unsigned long UnCompressedSize;  /* Size of uncompressed data file     */
   unsigned long BufferSize;
   unsigned long Crc;               /* Calculated CRC value               */
   unsigned long OrigCrc;           /* Original CRC value of data         */
} PARAM;


/*
** BufferSize defines the maximum size allowed for output of compressed
** data from implode(), or uncompressed data from explode().  Notice that
** if you are compressing files that compress to a size greater than 
** BufferSize, an error message will result.  You can modify the value of
** BufferSize if you need a bigger buffer.  If you know your maximum file 
** size, you can adjust BUFFERSIZE.  This will allow for larger, or smaller
** maximum values of BufferSize.  
*/

/* Routine to read uncompressed data.  Used only by implode().
** This routine reads the data that is to be compressed.
*/

unsigned int 
ReadUnCompressed(char *buff, unsigned int *size, void *Param)
{
   PARAM *Ptr = (PARAM *) Param;

   if (Ptr->UnCompressedSize == 0L)  
   {
      /* This will terminate the compression or extraction process */
      return(0);
   }

   if (Ptr->UnCompressedSize < (unsigned long)*size)
   {
      *size = (unsigned int)Ptr->UnCompressedSize;
   }

   memcpy(buff, Ptr->pSource + Ptr->SourceOffset, *size);
   Ptr->SourceOffset += (unsigned long)*size;
   Ptr->UnCompressedSize -= (unsigned long)*size;
   Ptr->Crc = crc32(buff, size, &Ptr->Crc);

   return(*size);
}

/* Routine to read compressed data.  Used only by explode(). 
** This routine reads the compressed data that is to be uncompressed.
*/

unsigned int
ReadCompressed(char *buff, unsigned int *size, void *Param)
{
   PARAM *Ptr = (PARAM *) Param;

   if (Ptr->CompressedSize == 0L)  
   {
      /* This will terminate the compression or extraction process */
      return(0);
   }

   if (Ptr->CompressedSize < (unsigned long)*size)
   {
      *size = (unsigned int)Ptr->CompressedSize;
   }

   memcpy(buff, Ptr->pSource + Ptr->SourceOffset, *size);
   Ptr->SourceOffset += (unsigned long)*size;
   Ptr->CompressedSize -= (unsigned long)*size;

   return(*size);
}

/* Routime to write compressed data.  Used only by implode(). 
** This routine writes the compressed data to a memory buffer.
*/

void 
WriteCompressed(char *buff, unsigned int *size, void *Param)
{
   PARAM *Ptr = (PARAM *) Param;

   if (Ptr->CompressedSize + (unsigned long)*size > Ptr->BufferSize)
   {
      puts("Compressed data will overflow buffer. Increase size of buffer!");
      exit(1);
   }
   memcpy(Ptr->pDestination + Ptr->DestinationOffset, buff, *size);
   Ptr->DestinationOffset += (unsigned long)*size;
   Ptr->CompressedSize += (unsigned long)*size;
}

/* Routine to write uncompressed data. Used only by explode().
** This routine writes the uncompressed data to a memory buffer.
*/

void 
WriteUnCompressed(char *buff, unsigned int *size, void *Param)
{
   PARAM *Ptr = (PARAM *) Param;

   if (Ptr->CompressedSize + (unsigned long)*size > Ptr->BufferSize)
   {
      puts("Compressed data will overflow buffer. Increase size of buffer!");
      exit(1);
   }
   memcpy(Ptr->pDestination + Ptr->DestinationOffset, buff, *size);
   Ptr->DestinationOffset += (unsigned long)*size;
   Ptr->UnCompressedSize += (unsigned long)*size;
   Ptr->Crc = crc32(buff, size, &Ptr->Crc);
}

void 
main(int argc, char *argv[])
{
   char *WorkBuff;               /* Buffer for compression tables */
   char *InFileName;
   char *temp;
   unsigned int error;
   unsigned int bytes_read;
   unsigned int type;
   unsigned int dsize;
   unsigned int written;
   PARAM Param;
   FILE *InFile;
   FILE *OutFile;

   /* Use the first command line argument as the name of the file
   ** to read as the data source.  If no filename is given, default
   ** to use "test.in"
   */
   if (argc > 1)
   {
      InFileName = argv[1];
   }
   else
   {
      InFileName = "test.in";
   }

   /* Open the file so it's contents can be read into memory. */
   if ((InFile = fopen(InFileName, "rb")) == NULL)
   {
      puts("Unable to open input file.");
      exit(1);
   }

   /* Determine the size of the file, and rewind to beginning of file. */
   if (fseek(InFile, 0L, SEEK_END))
   {
      puts("Unable to determine input file size.");
      exit(1);
   }
   Param.UnCompressedSize = ftell(InFile);
   fseek(InFile, 0L, SEEK_SET);

   if (Param.UnCompressedSize > BUFFERSIZE)
   {
      fclose(InFile);
      printf("Cannot compress files larger than %d.\n",BUFFERSIZE);
      exit(1);
   }
   Param.BufferSize     = Param.UnCompressedSize;
   Param.CompressedSize = 0L;

   /* Allocate memory buffers to hold the compressed and uncompressed
   ** contents of the data file.
   */
   Param.pSource      = (char *)malloc(Param.BufferSize);
   Param.pDestination = (char *)malloc(Param.BufferSize);

   /* We make the destination buffer the same size as the source buffer.
   ** You should determine what compression ratios you achieve with your
   ** specific data.  You may only need a destination buffer about one half
   ** the size of the source buffer (assuming 50% compression).
   */

   if (Param.pSource == NULL || Param.pDestination == NULL)
   {
      puts("Unable to allocate source & destination buffers.");
      exit(1);
   }

   /* Read the contents of the file into the uncompressed data buffer. */
   fread((void *)Param.pSource, 1, Param.UnCompressedSize, InFile);
   fclose(InFile);

   /* Allocate the buffer used by implode() for compression tables. */
   WorkBuff = (char *)malloc(CMP_BUFFER_SIZE);
   if (WorkBuff == NULL)
   {
      puts("Unable to allocate work buffer.");
      return;
   }

   puts("Calling Implode");
   type  = CMP_ASCII;
   dsize = 1024;

   Param.SourceOffset      = 0L;
   Param.DestinationOffset = 0L;
   Param.Crc               = (unsigned long) -1;
   implode(ReadUnCompressed,WriteCompressed,WorkBuff,&Param,&type,&dsize); 
   Param.OrigCrc           = ~Param.Crc;
   free(WorkBuff);

   /* Since the imploding is done, the data in the compressed buffer 
   ** will be used as the source for the exploding process. 
   */
   temp = Param.pSource;
   Param.pSource = Param.pDestination;
   Param.pDestination = temp;

   /* Clear buffer containing original uncompressed data */
   memset(Param.pDestination, 0, Param.UnCompressedSize);

   /* Allocate the buffer used by explode() for compression tables. */
   WorkBuff = (char *)malloc(EXP_BUFFER_SIZE);
   if (WorkBuff == NULL)
   {
      puts("Unable to allocate work buffer.");
      return;
   }
   Param.SourceOffset      = 0L;
   Param.DestinationOffset = 0L;
   Param.UnCompressedSize  = 0L;
   Param.Crc               = (unsigned long) -1;

   /* Now try extracting the compressed file data */
   puts("Calling Explode");
   error = explode(ReadCompressed,WriteUnCompressed,WorkBuff,&Param);
   Param.Crc = ~Param.Crc;

   if (error || (Param.Crc != Param.OrigCrc))
   {
      puts("Error in compressed data!");
   }
   printf("Original CRC=%lx Uncompressed CRC=%lx\n",Param.OrigCrc, Param.Crc);

   /* The uncompressed data is now in pDestination.  Lets write this to a file 
   ** called test.ext.  We can compare this buffer to the original input file.
   */

   if ((OutFile = fopen("test.ext", "wb+")) == NULL)
   {
      puts("Unable to open output file.");
   }
   else
   {
      fwrite((void *)Param.pDestination, 1, Param.UnCompressedSize, OutFile);
      fclose(OutFile);
   }

   /* Free buffers */
   free(WorkBuff);
   free(Param.pSource);
   free(Param.pDestination);
}
