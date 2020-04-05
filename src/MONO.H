
/* ========================================================================
   Copyright (c) 1990,1997	Synergistic Software
   All Rights Reserved
   Author: 
   ======================================================================== */
   

#ifndef _MONO_H_
#define _MONO_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

//#ifdef __BORLANDC__
//#pragma option -a4
//#endif
//#ifdef _MSC_VER
//#pragma pack(push,4)
//#endif

// Define __cdecl for non Microsoft compilers.
#if (!defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif

// This class is to help put text strings out to a monochrome monitor under
// Windows95.
// It requires the DARKMONO.VXD driver to be installed to work.
// It does do line wrapping.
// There is no stored buffer of the text, so no scrolling.

class MonoDevice
{
private:
   // Helper class to do auto initialization and to keep from opening the 
   // driver more than once.
   class DeviceData {
   private:
	   friend class MonoDevice;
	   explicit DeviceData();
	   inline bool Status() const;
	   
	   
	   HANDLE fhDevice;
	   bool   fEnabled;
	   int    fNextRow;
	   int    fNextCol;

   protected:
   public:
	   ~DeviceData();
   };
   
   enum MonoDimisions
      {
      WIDTH = 80,
      HEIGHT = 24
      };
      
   enum MonoFunctions 
      {
      MONO_VERSION = 1, 
      CURSOR_ON = 2, 
      CURSOR_OFF = 3,
      SET_ATTRIBUTE = 4,
      CLEAR_SCREEN = 5,
      PUT_CHAR = 6,
      PUT_STRING = 7
      };
   
   // Can't create or distroy one of these.
   MonoDevice();
   ~MonoDevice();
   
   // Can only have one Monochrome display.
   static DeviceData MDA;
public:
   typedef enum Attrib
   {
         ERASE     =0x00,NORM     =0x07, NORM_BLNK     =0x87,
         INVRS     =0x70,UNDRLN   =0x01, UNDRLN_BLNK   =0x81,
         INVRS_BLNK=0xF0,HI       =0x0F, HI_BLNK       =0x8F, 
         HI_UNDRLN=0x09, HI_UNDRLN_BLNK=0x89
   };

   
   static inline bool Status() { return MDA.Status(); }
   static inline bool IsEnabled();
   static inline void Enable();
   static inline void Disable();
   static inline void SetAttribute( Attrib attribute );
   static inline void CursorOn();
   static inline void CursorOff();
   static inline void ClearScreen( char fill = ERASE);
   static inline bool PutChar( int row, int col, char value );
   static inline bool PutChar( char value );
   static int  PutString( int row, int col, char const * const string );
   static inline int  PutString( char const * const string );
   static void __cdecl Printf( int row, int col, char const * const format, ... );
   static void __cdecl Printf(char const * const format, ... );
};

inline bool MonoDevice::DeviceData::Status() const
{
   return fhDevice != INVALID_HANDLE_VALUE && fEnabled;
}

inline bool MonoDevice::IsEnabled()
{
   return MDA.fEnabled;
}

inline void MonoDevice::Enable()
{
   MDA.fEnabled = true;
}

inline void MonoDevice::Disable()
{
   MDA.fEnabled = false;
}

inline void MonoDevice::SetAttribute( Attrib attribute )
{
   if( Status() )
   {
	  unsigned char attr = static_cast<unsigned char>(attribute);
      DeviceIoControl(MDA.fhDevice, SET_ATTRIBUTE, 
         &attr, sizeof( attr ), 
         NULL, 0, NULL, NULL);
	}
}

inline void MonoDevice::CursorOn()
{
   if( Status() )
      DeviceIoControl(MDA.fhDevice, CURSOR_ON, 
         NULL, 0, NULL, 0, NULL, NULL);
}

inline void MonoDevice::CursorOff()
{
   if( Status() )
      DeviceIoControl(MDA.fhDevice, CURSOR_OFF, 
         NULL, 0, NULL, 0, NULL, NULL);
}

inline void MonoDevice::ClearScreen( char fill )
{
   if( Status() )
   {
      DeviceIoControl(MDA.fhDevice, CLEAR_SCREEN, 
         &fill, sizeof( fill ), NULL, 0, NULL, NULL);
      MDA.fNextRow = 0;
      MDA.fNextCol = 0;
   }
}

inline bool MonoDevice::PutChar( int row, int col, char value )
{
   bool Result = false;
   
   if( Status() && row < HEIGHT && col < WIDTH)
   {
      short buff[3];
      buff[0] = static_cast<short>(row);
      buff[1] = static_cast<short>(col);
      buff[2] = value;  // high byte doesn't matter
      DeviceIoControl(MDA.fhDevice, PUT_CHAR, 
         &(buff[0]), sizeof( buff ), NULL, 0, NULL, NULL);
   	  Result = true;
   }
   return Result;
}

inline bool MonoDevice::PutChar( char value )
{
   bool const Result = PutChar (MDA.fNextCol, MDA.fNextRow, value);
   if (Result)
   {
	   ++MDA.fNextCol;
	   if (MDA.fNextCol > WIDTH)
	   {
	       MDA.fNextCol = 0;
	       ++MDA.fNextRow;
	       if (MDA.fNextRow > HEIGHT)
	       {
	          MDA.fNextRow = 0;
	       }
	   }
   }
   return Result;
}

inline int MonoDevice::PutString( char const * const string )
{
   int const Result = PutString(MDA.fNextRow, MDA.fNextCol, string);
   MDA.fNextCol += Result % WIDTH;
   if (MDA.fNextCol > WIDTH)
   {
       MDA.fNextCol %= WIDTH;
       ++MDA.fNextRow;
   }
   MDA.fNextRow += Result / WIDTH;
   if (MDA.fNextRow > HEIGHT)
   {
       MDA.fNextRow %= HEIGHT;
   }
   return Result;
}


//#ifdef __BORLANDC__
//#pragma option -a.
//#endif
//#ifdef _MSC_VER
//#pragma pack(pop)
//#endif

#endif
