#define  _beginthreadex   CreateThread
#define  _endthreadex(a)

/****************************************************************************
*
*  Storm functions
*
***/

HINSTANCE StormGetInstance ();

/****************************************************************************
*
*  SFile functions
*
***/

BOOL APIENTRY SFileOpenFileWin32 (LPCTSTR  filename,
                                  HANDLE  *handle);
