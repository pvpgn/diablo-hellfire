/****************************************************************************
*
*  TRACE.H
*  Modem trace public interface
*
*  Written by Jeff Strain (1/9/97)
*
***/
	
void TraceBegin (BOOL writepending);
void TraceDestroy ();
void TraceDumpAddr (LPCSTR addrname, LPVOID addr, unsigned size);
void __cdecl TraceOut (const char *format, ...);
void __cdecl TracePend (const char *format, ...);
