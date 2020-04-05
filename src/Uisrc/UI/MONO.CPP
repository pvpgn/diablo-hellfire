#ifdef _DEBUG
#include "mono.h"


// Allocate space for the MonoDriver data.
// and Call the constructor for one MonoDevice data object.
MonoDevice::DeviceData MonoDevice::MDA;

MonoDevice::DeviceData::DeviceData() :
   fhDevice (CreateFile("\\\\.\\DARKMONO.VXD", 0, 0, NULL, NULL,
                        FILE_FLAG_DELETE_ON_CLOSE, NULL)),
   fEnabled((fhDevice != INVALID_HANDLE_VALUE)),
   fNextRow(0),
   fNextCol(0)
{
   ClearScreen();
}   

MonoDevice::DeviceData::~DeviceData()
{
   if( Status() )
   {
      CloseHandle(fhDevice);
	  fEnabled = false;
	  fhDevice = INVALID_HANDLE_VALUE;
	  fNextRow = 0;
	  fNextCol = 0;
   }
}

int MonoDevice::PutString( int row, int col, char const * string )
{
   int Result = 0;
   if( Status()  && row < HEIGHT && col < WIDTH)
   {
      short buff[3 + 41];  // allow WIDTH character string
      int len = strlen( string );
      Result = len;
      int bufferLen = 0;
      
      for (;len > 0; len -= WIDTH, ++row, col=0, string += bufferLen )
      {
	      bufferLen = ((len + col) > WIDTH)? WIDTH - (len + col) : len;
	      if (row >= HEIGHT)
	      	row = 0;
	      buff[0] = static_cast<short>(row);
	      buff[1] = static_cast<short>(col);
	      buff[2] = static_cast<short>(bufferLen);
	      if( buff[2] > (WIDTH - col) )
	      {
	         buff[2] = (WIDTH - col);
	         strncpy( reinterpret_cast<char *>(buff + 3), string, buff[2] );
	      }
	      else
	         strcpy(  reinterpret_cast<char *>(buff + 3), string );
	
	      DeviceIoControl(MDA.fhDevice, PUT_STRING, 
	         &(buff[0]), sizeof( buff ), NULL, 0, NULL, NULL);
      }
      
   }
   return Result;
}

void __cdecl MonoDevice::Printf( int row, int col, char const * const format, ... )
{
   if( Status() )
      {
      char  strbuf [ 256 ];
      va_list argptr;

      va_start(argptr,format);
      vsprintf(strbuf,format,argptr);
      va_end(argptr);

      PutString( row, col, strbuf );
      }
}

void __cdecl MonoDevice::Printf( char const * const format, ... )
{
   if( Status() )
      {
      char  strbuf [ 256 ];
      va_list argptr;

      va_start(argptr,format);
      vsprintf(strbuf,format,argptr);
      va_end(argptr);

      PutString( strbuf );
      }
}

#endif
