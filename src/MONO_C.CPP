#ifdef _DEBUG
#include <fcntl.h>
#include <sys\stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "mono_c.h"



//---------------------------------------------------------------------------


bool mono_getExist( void )
{
   return MonoDevice::Status() ? true : false;  // Returns TRUE, if device detected
}                                       // else returns FALSE

int mono_getEnable( void )
{
   return MonoDevice::IsEnabled() ? true : false; // Returns ON, if device enabled
}                                         // else returns OFF

void mono_setEnable( int flag )    // flag = TRUE  - use mono device
{                                  // flag = FALSE - don't use 
   if( flag == TRUE )
      MonoDevice::Enable();
   else
      MonoDevice::Disable();
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void mono_cls(void)
{
   MonoDevice::ClearScreen( ' ' );
}


//---------------------------------------------------------------------------

void mono_putc( short x, short y, char c)
{
   MDA.PutChar( y, x, c );
}


//---------------------------------------------------------------------------

void mono_puts( short x, short y, char const * const str )
{
   MDA.PutString( y, x, str );   
}


//---------------------------------------------------------------------------

void __cdecl mono_printf( short x, short y, char const * const format, ... )
{
   char  strbuf [ 256 ];
   va_list argptr;
   va_start(argptr,format);
   vsprintf(strbuf,format,argptr);
   va_end(argptr);
   MDA.PutString( y, x, strbuf );
}


//---------------------------------------------------------------------------

void mono_dump  ( short port )
{
/*
   if( MDA.IsEnabled() && MDA.Status() )
   {
	   char	linefeed[3] = "\r\n";
	   int	fp;
      char lptPort[] = "LPTx";
      lptPort[3] = port + '0';               //set LPT port

      short *ptr = 0;//(short *)(_x386_zero_base_ptr + MONOBASE);
      short i;
      short ch;
      short attribute;
      short col = 0;
 	   if ((fp = open(lptPort, O_WRONLY) ) >= 0)
	   {
		   // Output screen
         for (i=0; i<MONOHEIGHT*MONOWIDTH; i++)
         {
            attribute = (*ptr)>>8;
            ch        = (*ptr) & 0x00ff;
            (*ptr) = 0x0fdb;                 //display a progress character

            if (attribute == MONO_NORM)
               write(fp, "(s0B", 5);        //Normal on
            else
               write(fp, "(s7B", 5);        //BOLD on

            if (!ch)
               ch = ' ';   // char 0 is also a space on the mono
            write(fp, &ch, 1);               //write the character

            if (++col == MONOWIDTH)          //Next line?
            {
               col = 0;
		         write(fp,&linefeed,2);
            }
            (*ptr) = (attribute<<8)+ch;      //remove progress character
            ptr++;
         }

		   close(fp);
	   }
   }
*/
}


//---------------------------------------------------------------------------

void mono_setAttrib( MonoAttrib attrib )
{
   MDA.SetAttribute( attrib );
}

MonoAttrib mono_getAttrib()
{
   return MONO_NORM;
}

//---------------------------------------------------------------------------

void  mono_setHardwareCursor( int flag )    // flag = TRUE  - use mono device
{                                            // flag = FALSE - don't use 
   if( flag )
      MDA.CursorOn();
   else
      MDA.CursorOff();
}

#endif
