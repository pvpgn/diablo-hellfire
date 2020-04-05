#ifdef _DEBUG
#ifndef  _MONO_C_H_
#define  _MONO_C_H_

#include "mono.h"

//같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같
//
// Global Function Declarations
//
//같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같

void       mono_setAttrib( MonoAttrib aVal );
MonoAttrib mono_getAttrib( void );



int   mono_getExist ( void      );
int   mono_getEnable( void      );
void  mono_setEnable( int flag );

void  mono_cls   ( void );

void  mono_putc  ( short x, short y, char c          );
void  mono_puts  ( short x, short y, char const * const str );
void  __cdecl mono_printf( short x, short y, char const * const format, ... );

void  mono_dump  ( short port = 1);

void  mono_setHardwareCursor( int flag );



#endif
#endif
