#include <win32.cs>
set crtlib=crtdll.lib
set deffile=exports.def
set extralib=comdlg32.lib implode.lib version.lib
set linkopt=%linkopt% -base:0x15000000
set .obj=.dll

// DETERMINE THE PROJECT NAME
if %debug% set project=%project%d

// COPY THE FILES TO THE OUTPUT DIRECTORIES
!copy %project%.lib ..\lib > NUL:
!copy %project%.dll ..\bin > NUL:
!if exist *.bak del *.bak
!if exist %project%.dll del %project%.dll
