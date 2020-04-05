/*
 *******************************************************************
 *** Important information for use with the                      ***
 *** PKWARE Data Compression Library (R) for Win32               ***
 *** Copyright 1994,1995 by PKWARE Inc. All Rights Reserved.     ***
 *** PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off. ***
 *******************************************************************
 */
// compdlg.cpp : implementation file
//


#include "stdafx.h"
#include <stdio.h>
#include "dcl.h"

// Start, Added by PKWARE
#include "implode.h"
#include "PKstruct.h"
#include "compdlg.h"                                     

static char CancelCompression[] = "Cancel Compression";

#define DO_CRC_INSTREAM          1
#define DO_CRC_OUTSTREAM         2

void ProcessOutBuffer(PCHAR buffer, UINT *iSize, void *Param);
UINT ProcessInBuffer(PCHAR buffer, UINT *iSize, void *pParam);

void PKCompressFile(void);
// End, added by PKWARE

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompDlg dialog


CCompDlg::CCompDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CCompDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CCompDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void CCompDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCompDlg)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCompDlg, CDialog)
   //{{AFX_MSG_MAP(CCompDlg)
   ON_BN_CLICKED(IDC_BUTTON1, OnCompressButton)
   ON_BN_CLICKED(IDC_BUTTON2, OnTestButton)
   ON_BN_CLICKED(IDC_BUTTON3, OnClearButton)
   ON_BN_CLICKED(IDC_BUTTON4, OnDebugButton)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCompDlg message handlers
BOOL CCompDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   SetDlgItemText(IDC_EDIT1, "TEST.IN");    
   SetDlgItemText(IDC_EDIT9, "?");    
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON2), FALSE);         // Disable the Extract button 
   DebugMessages = FALSE;
   return TRUE;  // return TRUE  unless you set the focus to a control
}

// Function to add an item to the listbox.  The listbox is then scrolled to display this item
void CCompDlg::PKLBMessage(char *message)
{
   SendDlgItemMessage(IDC_LIST1, LB_ADDSTRING, 0, (LONG)(PCHAR)(const char *)message);
   int Items = (int)SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0,0);
   SendDlgItemMessage(IDC_LIST1, LB_SETCURSEL, Items-1, 0);           // Show this item
}

void CCompDlg::OnCompressButton()
{
   // TODO: Add your control notification handler code here
   ClearMessages();        // Clear listbox and filesizes                           
   PKCompressFile();       // Start compression
}

void CCompDlg::OnTestButton()
{
   // TODO: Add your control notification handler code here
   SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0L);         // Clear the listbox
   PKLBMessage("Extracting file..."); 
   PKExtractFile();                                               // Start extraction
}

void CCompDlg::OnClearButton()
{
   // TODO: Add your control notification handler code here
   ClearMessages();
}

void CCompDlg::OnCancel()
{
   // TODO: Add extra cleanup here
   CDialog::OnCancel();
}

// Function to calculate the percentage of compression without using floating point math
int GetPercent(unsigned long top, unsigned long bottom)
{
   int percent;
   unsigned long bot;

   if (top == 0L)
      return(0);

   bot = bottom ? bottom : 1L;

   if (top > 400000000L)                          // 400,000,000
      percent = (int)(top / (bot / 100L));
   else if (top > 40000000L)                      //  40,000,000
      percent = int((top * 10L) / (bot / 10L));
   else
      percent = int((top * 100L) / bot);

   return(percent > 100 ? 100 : percent);
}
            
void CCompDlg::ClearMessages()
{
   SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0L);   // Clear the listbox
   SetDlgItemText(IDC_EDIT2, (PCHAR)"");                    // Clear all the edit boxes
   SetDlgItemText(IDC_EDIT3, (PCHAR)"");
   SetDlgItemText(IDC_EDIT4, (PCHAR)"");
   SetDlgItemText(IDC_EDIT5, (PCHAR)"");
   SetDlgItemText(IDC_EDIT6, (PCHAR)"");
   SetDlgItemText(IDC_EDIT7, (PCHAR)"");
   SetDlgItemText(IDC_EDIT8, (PCHAR)"");
}
                     
// This function is called by the implode and explode functions.
UINT ProcessInBuffer(PCHAR buffer, UINT *iSize, void *pParam)
{
   PIOFILEBLOCK pFileIOBlock;
   unsigned int iRead;

   pFileIOBlock = (PIOFILEBLOCK) pParam;

   if (pFileIOBlock->PKAbortOperation)             // Set this variable to abort compression or extraction
      return 0;

   iRead = fread( buffer, 1, *iSize, pFileIOBlock->InFile );   // Read data from disk

   if (pFileIOBlock->DebugMessages)                                    // Debugging messages on ?
   {  
      char s[80];
      wsprintf(s, "Asked to read %u bytes, actually read %u bytes", *iSize, iRead);
      SendDlgItemMessage(pFileIOBlock->hWindow, IDC_LIST1, LB_ADDSTRING, 0, (LONG)(PCHAR)(const char *)s);
      int Items = (int)SendDlgItemMessage(pFileIOBlock->hWindow, IDC_LIST1, LB_GETCOUNT, 0, 0);
      SendDlgItemMessage(pFileIOBlock->hWindow, IDC_LIST1, LB_SETCURSEL, Items-1, 0);
   }

   if( iRead > 0 && pFileIOBlock->bDoCRC == DO_CRC_INSTREAM)
      pFileIOBlock->dwCRC = crc32(buffer, &iRead, &pFileIOBlock->dwCRC);

   return iRead;
}

// This function is called by the implode and explode functions.
void ProcessOutBuffer(PCHAR buffer, UINT *iSize, void *pParam)
{
   PIOFILEBLOCK pFileIOBlock;
   unsigned int iWrite;

   pFileIOBlock = (PIOFILEBLOCK) pParam;

   if (pFileIOBlock->PKAbortOperation)             // Set this variable to abort compression or extraction
      return;

   iWrite = fwrite( buffer, 1, *iSize, pFileIOBlock->OutFile );   // Write the data to disk

   if (pFileIOBlock->DebugMessages)                                       // Debugging messages on ?
   { 
      char s[80];
      wsprintf(s, "Asked to write %u bytes, actually wrote %u bytes", iSize, iWrite);
      SendDlgItemMessage(pFileIOBlock->hWindow, IDC_LIST1, LB_ADDSTRING, 0, (LONG)(PCHAR)(const char *)s);
      int Items = (int)SendDlgItemMessage(pFileIOBlock->hWindow, IDC_LIST1, LB_GETCOUNT, 0, 0);
      SendDlgItemMessage(pFileIOBlock->hWindow, IDC_LIST1, LB_SETCURSEL, Items-1, 0);
   }
   
   if( iWrite > 0 && pFileIOBlock->bDoCRC == DO_CRC_OUTSTREAM)
      pFileIOBlock->dwCRC = crc32(buffer, &iWrite, &pFileIOBlock->dwCRC);

   return;
}

void CCompDlg::PKCompressFile()
{
   int iStatus;
   UINT CompType = CMP_ASCII, DictSize;
   char szVerbose[128], FileName[80];
   fpos_t FileSize = 0, CompressedFileSize;
   static char *Comp[] = { "Binary", "ASCII" };
    
   FileIOBlock.hWindow = m_hWnd;                       // Used in the read and write routines
   FileIOBlock.DebugMessages = DebugMessages;       
   FileIOBlock.PKAbortOperation = FALSE;
   
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), FALSE);      // Disable buttons
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON2), FALSE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON3), FALSE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON4), FALSE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDCANCEL), FALSE);
    
   HANDLE hScratchPad;
   PCHAR  pScratchPad;

   // allocate the memory block for the scratch pad
   if( (hScratchPad = GlobalAlloc(GHND, CMP_BUFFER_SIZE)) == NULL )
   {
      return;
   }

   if( (pScratchPad = (PCHAR) GlobalLock(hScratchPad)) == NULL )
   {
      GlobalFree(hScratchPad);
      return;
   }

   int LoopCnt;

   for( LoopCnt=0, CompType = CMP_ASCII; LoopCnt < 2; LoopCnt++, CompType = CMP_BINARY )
   {
      for (DictSize = 1024; DictSize <= 4096; DictSize *= 2)               // Dictionary size
      {
         FileIOBlock.InFile = FileIOBlock.OutFile = NULL;                  // Initialize

         GetDlgItemText(IDC_EDIT1, FileName, 80);
         FileIOBlock.InFile = fopen(FileName, "rb" );       // Open the source file
   
         if( FileIOBlock.InFile == NULL )                // Error opening file
         {
            char s[80];
            wsprintf(s, "Error opening file %s.", (PCHAR)FileName);
            PKLBMessage(s);
            break;
         }

         if( FileSize == 0 )
         {
            fseek(FileIOBlock.InFile, 0L, SEEK_END);        // Get the filesize
            fgetpos(FileIOBlock.InFile, &FileSize );        // Get the filesize
            fseek(FileIOBlock.InFile, 0L, SEEK_SET);                   // Rewind the file
            wsprintf(szVerbose, "%ld", (PCHAR)FileSize);                   // Uncompressed size
            SetDlgItemText(IDC_EDIT9, szVerbose);
         }

         FileIOBlock.OutFile = fopen( "Test.cmp", "wb" );
         FileIOBlock.bDoCRC = DO_CRC_INSTREAM;
         FileIOBlock.dwCRC = ~((DWORD)0);                                  // Pre-condition CRC

         if( (FileIOBlock.InFile  != NULL) && (FileIOBlock.OutFile != NULL))
         {
            iStatus = implode( ProcessInBuffer, ProcessOutBuffer, pScratchPad,
                               &FileIOBlock, &CompType, &DictSize );

            // Check the value of iStatus for errors

            // Post-condition CRC
            if (FileIOBlock.bDoCRC == DO_CRC_INSTREAM)
            {
               CString CRCvalue;
               FileIOBlock.dwCRC = ~FileIOBlock.dwCRC;
               InputCRC = FileIOBlock.dwCRC;
               wsprintf(szVerbose, "%lX", FileIOBlock.dwCRC);
               SetDlgItemText(IDC_EDIT8, szVerbose);                          // Display CRC
            }

            fgetpos(FileIOBlock.OutFile, &CompressedFileSize );               // Get the filesize

            fclose(FileIOBlock.OutFile);                                  // Close the files
            fclose(FileIOBlock.InFile);

            wsprintf(szVerbose, "%ld", CompressedFileSize);

            if (CompType == CMP_ASCII)
            {
               if (DictSize == 1024)
                  SetDlgItemText(IDC_EDIT2, szVerbose);
               else if (DictSize == 2048)
                  SetDlgItemText(IDC_EDIT3, szVerbose);
               else
                  SetDlgItemText(IDC_EDIT4, szVerbose);
            }
            else   // Binary
            {
               if (DictSize == 1024)
                  SetDlgItemText(IDC_EDIT5, szVerbose);
               else if (DictSize == 2048)
                  SetDlgItemText(IDC_EDIT6, szVerbose);
               else
                  SetDlgItemText(IDC_EDIT7, szVerbose);
            }
            wsprintf(szVerbose, "Using %d dictionary, %s compression.  File Compressed %d%%.",
                  DictSize, (PCHAR)Comp[CompType], 100 - GetPercent(CompressedFileSize, FileSize));
            PKLBMessage(szVerbose);
         }
      }
   }

   GlobalUnlock(hScratchPad);
   GlobalFree(hScratchPad);

   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), TRUE);          // Reset the buttons
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON2), TRUE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON3), TRUE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON4), TRUE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDCANCEL), TRUE);
}

void CCompDlg::PKExtractFile()
{
   int iStatus;

   FileIOBlock.hWindow = m_hWnd;                       // Used in the read and write routines
   FileIOBlock.DebugMessages = DebugMessages; 
   FileIOBlock.PKAbortOperation = FALSE;

   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), FALSE);      // Disable the buttons
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON2), FALSE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON3), FALSE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON4), FALSE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDCANCEL), FALSE);

   HANDLE hScratchPad;
   PCHAR  pScratchPad;

   // allocate the memory block for the scratch pad
   if( (hScratchPad = GlobalAlloc(GHND, EXP_BUFFER_SIZE)) == NULL )
   {
      ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), TRUE);
      ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON2), TRUE);
      ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON3), TRUE);
      ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON4), TRUE);
      ::EnableWindow(::GetDlgItem(m_hWnd, IDCANCEL), TRUE);
      return;
   }

   if( (pScratchPad = (PCHAR) GlobalLock(hScratchPad)) == NULL )
   {
      GlobalFree(hScratchPad);
      ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), TRUE);
      ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON2), TRUE);
      ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON3), TRUE);
      ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON4), TRUE);
      ::EnableWindow(::GetDlgItem(m_hWnd, IDCANCEL), TRUE);
      return;
   }

   FileIOBlock.InFile = FileIOBlock.OutFile = NULL;

   // setup structure used by ProcessReadBuffer() and ProcessWriteBuffer()
   FileIOBlock.InFile = fopen( "Test.cmp", "rb" );

   if( FileIOBlock.InFile == NULL )
   {
      int Items = (int)SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0,0);
      SendDlgItemMessage(IDC_LIST1, LB_INSERTSTRING, Items, (LONG)(PCHAR)(const char *)"The file TEST.CMP was not found, you must compress a file first.");
   }
   else
   {
      FileIOBlock.OutFile = fopen( "Test.ext", "wb" );
      FileIOBlock.bDoCRC = DO_CRC_OUTSTREAM;
      FileIOBlock.dwCRC = ~((DWORD)0);                      // Pre-condition CRC

      iStatus = explode( ProcessInBuffer, ProcessOutBuffer, pScratchPad, &FileIOBlock );

      // Check the value of iStatus for errors

      // Post-condition CRC
      if (FileIOBlock.bDoCRC == DO_CRC_OUTSTREAM)
      {
         FileIOBlock.dwCRC = ~FileIOBlock.dwCRC;
         if (InputCRC != FileIOBlock.dwCRC)
            PKLBMessage("File fails the CRC check!");
         else
            PKLBMessage("File tests OK.");
      }

      if( FileIOBlock.OutFile != NULL )
         fclose(FileIOBlock.OutFile);
      if( FileIOBlock.InFile != NULL )
         fclose(FileIOBlock.InFile);

      PKLBMessage("Done extracting.");
   }

   GlobalUnlock(hScratchPad);
   GlobalFree(hScratchPad);

   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), TRUE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON2), TRUE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON3), TRUE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON4), TRUE);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDCANCEL), TRUE);
}
                     
void CCompDlg::OnDebugButton()
{
   // TODO: Add your control notification handler code here

   DebugMessages = !DebugMessages;   
   ::SetWindowText(::GetDlgItem(m_hWnd, IDC_BUTTON4), DebugMessages ? (PCHAR)"Debug Messages = On" : (PCHAR)"Debug Messages = Off");
}

