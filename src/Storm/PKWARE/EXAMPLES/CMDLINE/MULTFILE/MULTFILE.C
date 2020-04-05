/*************************************************************************
   Example to interface the PKWARE Data Compression Library (R)
   Copyright 1995 PKWARE Inc. All Rights Reserved.
   PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off
   Version 1.11

   This example compresses a set of one or more files into a single
   output file.  The set of files is taken from the command line.  Each
   file is compressed, and written to the output file.  A record 
   holding information for each file is written to the output file along
   with the compressed data.  The name of the compressed output file
   created by this program is PKWDCL.CMP.
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "implode.h"

#define BUFSIZE 2048                       /* Work buffer                    */
#define TEMPNAME "PKWDCL.TMP"              /* Temporary file                 */
#define COMPRESSED_FILE_NAME "PKWDCL.CMP"  /* Name of compressed output file */
#define FALSE    0
#define TRUE     (!FALSE)


/*
** Structure definitions
*/

typedef struct FileHeader
{
   char signature[4];                   /* Signature in case of errors */
   char filename[13];                   /* File name                   */
   unsigned long CompSize;              /* Compressed size of file     */
   unsigned long UnCompSize;            /* Original size of file       */
   unsigned long Crc;                   /* Crc value for file          */
} HEADER;
/*
** This structure will be written to the compressed output file and
** will record information for each input file that is compressed
** into the output file.
*/

typedef struct PassedParam 
{
   FILE *InFile;                      /* Pointer to file for reading data  */
   FILE *OutFile;                     /* Pointer to file for writing data  */
   FILE *Destination;                 /* Pointer to compressed output file */
   int Imploding;                     /* Flag indicating compression or    */
                                      /* uncompression is in progress.     */
   unsigned long Crc;                 /* CRC value for current file.       */ 
   unsigned long OrigCrc;             /* Original CRC for a file           */
   unsigned long CompressedFileSize;  /* Size of compressed file           */
   unsigned long UnCompressedFileSize;/* Original file size                */
} PARAM;
/* 
** This structure is used to pass values shared between the main
** application and the callback functions called by implode() and
** explode().
*/


/* 
** Function Prototypes 
*/
unsigned long FileSize(FILE *);
void ReadHeader(FILE *, HEADER *);
void SkipFile(FILE *, unsigned long);
void Expand(char *,int);
void CompressFile(char *, char *, PARAM *);
void AppendFile(FILE *,unsigned long);
void WriteHeader(char *, PARAM *);
void Compress(char *,int, char **);


/* The ReadBuff function is used by the implode() and explode()
** functions to read a stream of data that will be either
** compressed, or uncompressed.
*/

unsigned int
ReadBuff(char *buff, unsigned int *size, void *Param)
{
   PARAM *Ptr = (PARAM *) Param;
   unsigned int Read = 0;

   /* This function may ask for up to 4K of data at a time. If your archive
   ** file contains several compressed files, you may read too much.  For 
   ** example, your first compressed file in the archive may be 100 bytes.  
   ** So you do not want to read more than 100 bytes or you'll be unable to 
   ** uncompress the second file, since you will not be located at the 
   ** beginning of the file any longer. We will use the variable 
   ** "CompressedFileSize" to check for this condition.
   */

   if (Ptr->Imploding == FALSE)
   {
      /* If we are exploding data, and the number of bytes left in the 
      ** compressed file is less than the number of bytes requested, then 
      ** set the number of bytes requested to the bytes remaining in the 
      ** compressed file.
      */

      /* Set size to bytes left */

      if ((unsigned long)*size > Ptr->CompressedFileSize)
      {
         *size = (unsigned)Ptr->CompressedFileSize;  
      }

      /* Subtract the number of bytes read from the total size of the
      ** compressed file.
      */

      Ptr->CompressedFileSize -= (unsigned long)*size;
   }

   /* Read 'size' bytes from input source */
   Read = fread(buff, 1, *size, Ptr->InFile);

   if (Ptr->Imploding == FALSE)
   {
      /* Check the CRC value of data as it's uncompressed. */
      Ptr->Crc = crc32(buff, &Read, &Ptr->Crc);
   }

   /* Return the number of bytes read from the input source */
   return(Read);
}

/* The WriteBuff function is used by the implode() and explode()
** functions to write a stream of data that has been either
** compressed, or uncompressed.
*/

void 
WriteBuff(char *buff, unsigned int *size, void *Param)
{
   /* If compressing data, add the number of bytes to 'CompressedFileSize'.
   ** We need to keep track of the size of the compressed file.
   */

   PARAM *Ptr = (PARAM *) Param;
   int Written;

   if (Ptr->Imploding)
   {
      Ptr->CompressedFileSize += (unsigned long)*size;
   }

   /* Write the data to the file.  If we are Imploding, this is compressed
   ** data.  Otherwise it is uncompressed data.
   */

   Written = fwrite((void *)buff, 1, *size, Ptr->OutFile);
   if (Written != *size)
   {
      puts("Failed to write compressed data");
   }
   if (Ptr->Imploding == TRUE)
   {
      /* Calculate the CRC value of data as it's compressed. */
      Ptr->Crc = crc32(buff, size, &Ptr->Crc);
   }
}

/* The ReadHeader function is used to read a HEADER data structure
** from the compressed output file.  This HEADER record contains the
** information about the compressed file.
*/
void 
ReadHeader(FILE *pFile, HEADER *header)
{
   fread(header, 1, sizeof(HEADER), pFile);
}

/* The SkipFile function is used to skip over a file that is in
** the compressed output file if that file is not to be uncompressed.
*/
void 
SkipFile(FILE *pFile, unsigned long Size)
{
   fseek(pFile, Size, SEEK_CUR);
}

/* The FileSize function is used to determine the number of
** bytes in a file that is to be compressed.
*/
unsigned long
FileSize(FILE *pFile)
{
   unsigned long Size;

   if (fseek(pFile, 0L, SEEK_END))
   {
      puts("Unable to determine input file size.");
   }
   Size = ftell(pFile);
   fseek(pFile, 0L, SEEK_SET);

   return(Size);
}

/* The Expand function is used to uncompress the files that were
** written to the compressed output file.  A prompt is displayed
** for each file allowing the user to skip the file if it is not
** to be uncompressed.
*/
void 
Expand(char *WorkBuff, int Files)
{
   HEADER header;
   PARAM Param;
   int error;
   int i;
   int FileCount;
   char ch;
   char s[80];

   memset( &Param, 0, sizeof(Param) );

   Param.InFile = fopen(COMPRESSED_FILE_NAME, "rb");
   if (Param.InFile == NULL)
   {
      printf("Unable to open compressed output file %s\n",COMPRESSED_FILE_NAME);
   }

   for (FileCount = 1; FileCount < Files; FileCount++)
   {
      Param.CompressedFileSize = 0L;
      ReadHeader(Param.InFile,&header);

      /* Display a message and ask if you wish to extract this file */
      sprintf(s,"Extract file %s ? File is %lu bytes. (Y/N)",
              header.filename, header.UnCompSize);
      puts(s);

      /* We need to remember how many bytes are in this compressed data
      ** stream.  We don't want to read too many bytes.
      */
      Param.CompressedFileSize = header.CompSize;
      Param.OrigCrc = header.Crc;
      Param.Imploding = FALSE;

      do
      {
         ch = toupper(getchar());
      }
      while(ch != 'Y' && ch != 'N');
      if (ch == 'Y')
      {
         /* If the file is to be uncompressed, create new, empty file
         ** of the same name where the uncompressed contents of the
         ** file will be written.
         */
         Param.OutFile = fopen(header.filename, "wb+");
         if (Param.OutFile == NULL)
         {
            printf("Unable to open output data file %s\n",header.filename);
            /* Skip past this file and go on to the next one. */
            SkipFile(Param.InFile,Param.CompressedFileSize); 
         }
         else 
         {
            /* Call explode to uncompress the file. */
            Param.Crc = (unsigned long) -1;
            error = explode(ReadBuff,WriteBuff,WorkBuff,&Param);
            Param.Crc = ~Param.Crc;
            if (error || (Param.OrigCrc != Param.Crc))
            {
               printf("Error in compressed file %s!\n",header.filename);
            }
            printf("Expanding file %s Original CRC = %lx Uncompressed CRC = %lx\n",header.filename,Param.OrigCrc,Param.Crc);
            /* Close the file we just created */
            fclose(Param.OutFile);
         }
      }
      else
      {
         /* Skip past this file and go on to the next one. */
         SkipFile(Param.InFile,Param.CompressedFileSize); 
      }
   }
   fclose(Param.InFile);
}

/* The CompressFile function is used to compress a file into a
** temporary file.  
*/
void 
CompressFile(char *file, char *WorkBuff, PARAM *Param)
{
   unsigned int type;           /* Compression type  */
   unsigned int dsize;          /* Dictionary  size  */

   /* Set the compression type to BINARY compression */
   type = CMP_BINARY;                         

   /* Set the compression dictionary size to 4K */
   dsize = 4096;                             

   /* Open the file to be compressed. */
   Param->InFile = fopen(file, "rb");
   if (Param->InFile == NULL)
   {
      puts("Unable to open input file");
      return;
   }

   /* Open the temporary file where the compressed data will be written. */
   Param->OutFile = fopen(TEMPNAME, "wb+");
   if (Param->OutFile == NULL)
   {
      printf("Unable to open temporary output file %s\n",Param->OutFile);
   }
   else
   {
      Param->Crc = (unsigned long) -1; 
      Param->UnCompressedFileSize = FileSize(Param->InFile);
   
      /* Call implode() to compress the file. */
      implode(ReadBuff,WriteBuff,WorkBuff,Param,&type,&dsize);
      printf("Compressing file %s", file);
      printf("  File size = %ld bytes  Compressed size = %ld bytes\n",
               Param->UnCompressedFileSize,Param->CompressedFileSize);
   
      /* Close the temp file and the file being compressed */
      Param->OrigCrc = ~Param->Crc;
      fclose(Param->InFile);
      fclose(Param->OutFile);
   }
}

/* The AppendFile function is used to copy the compressed data from
** the temporary file to the compressed output file after the
** header record for the file has been written.
*/

void 
AppendFile(FILE *pDest,unsigned long Size)
{
   char buf[BUFSIZE];
   unsigned long left;
   unsigned int Read;
   unsigned int written;
   FILE *pFile;

   /* Keep track of the number of bytes that need to be appended */
   left = Size;

   /* Open the temporary file containing the compressed input file
   ** data.  The contents of this file are then written to the
   ** final output file.
   */
   if ((pFile = fopen(TEMPNAME, "rb")) != NULL)
   {
      do
      {
         Read = fread(buf, 1, BUFSIZE, pFile);
         written = fwrite(buf, 1, Read, pDest);
         left -= (unsigned long)written;      
      }
      while (left && written);               
      fclose(pFile);                        
   }
   else
   {
      printf("Unable to open temporary file %s\n",TEMPNAME);
   }
}

/* The WriteHeader function is used to format and write the
** record header for each file compressed into the compressed
** output file.
*/
void 
WriteHeader(char *filename, PARAM *Param)
{
   HEADER header;
   int Written;

   /* Save the filename and compressed file size in the structure */
   strcpy(header.filename, filename);
   header.CompSize = Param->CompressedFileSize;
   header.UnCompSize = Param->UnCompressedFileSize;
   header.Crc = Param->OrigCrc;

   /* Save a signature, can be used to help rebuild a damaged file */
   header.signature[0] = 'D';
   header.signature[1] = 'H';
   header.signature[2] = 9;
   header.signature[3] = 2;

   /* Write the data to the compressed archive file */
   Written = fwrite(&header, 1, sizeof(HEADER), Param->Destination);
   if (Written != sizeof(HEADER))
   {
      puts("Failed to write compressed file header record.");
   }
}

/* The Compress function is used to compress each file specified
** on the command line.
*/
void 
Compress(char *WorkBuff, int Files, char** FileList)
{
   PARAM Param;
   int FileCount;

   memset( &Param, 0, sizeof(Param) );

   /* Open the file that will be the final output file for all
   ** of the compressed input files.
   */
   Param.Destination = fopen(COMPRESSED_FILE_NAME, "wb+");
   if (Param.Destination == NULL)
   {
      printf("Unable to open compressed output file %s\n",COMPRESSED_FILE_NAME);
   }
   else 
   {
      /* For each file specified on the command line, compress the file,
      ** write a header record for the file, then write the compressed
      ** file data to the output file.
      */
      for (FileCount = 1; FileCount < Files; FileCount++)
      {
         Param.Imploding = TRUE;
         Param.CompressedFileSize = 0L; 
         Param.OrigCrc = 0L;
         CompressFile(FileList[FileCount], WorkBuff,&Param);
         WriteHeader(FileList[FileCount], &Param);
         AppendFile(Param.Destination,Param.CompressedFileSize);
      }

      if( Param.InFile )
         fclose(Param.InFile);
      fclose(Param.Destination);
      unlink(TEMPNAME);                            
   }
}

/* The main function simply allocates a work buffer needed for
** calling the implode() and explode() functions, and then
** calls the functions will compress and uncompress the
** files specified on the commandline.
*/
int
main(int argc, char *argv[])
{
   char *WorkBuff;                     /* Buffer for compression tables */

   if( argc < 2 )
   {
      printf( "Usage: multfile filename(s)\n" );
      return 0;
   }

   /* Allocate the memory needed for implode(). The explode() routine
   ** will use this same buffer, although it can generally use a 
   ** smaller sized buffer.
   */
   WorkBuff = (char *)malloc(CMP_BUFFER_SIZE);

   if (WorkBuff == NULL)
   {
      puts("Unable to allocate work buffer");
      return 1;
   }
   Compress(WorkBuff,argc,argv); /* Call Compress, pass the allocated memory */
   Expand(WorkBuff,argc);        /* Call Expand, pass the allocated memory.  */
   free(WorkBuff);               /* Free the allocated memory */

   return 0;
}
