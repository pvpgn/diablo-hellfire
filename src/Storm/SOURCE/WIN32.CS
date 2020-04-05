// IF THE USER TRIES TO RUN THIS SCRIPT DIRECTLY, INSTEAD OF USING THIS 
// SCRIPT BY INCLUDING IT FROM ANOTHER ONE, DISPLAY AN ERROR
if not %project%==win32 goto start
echo You must specify a project script.
echo Type "C /?" for help.
halt
:start

// SET UP AUTO-DEPENDENCY CHECKING 
set -autodependencies  =.c;.cpp;.h;.hpp
set -ignoredependencies=windows.h;windowsx.h;ole2.h;rpc.h;ddraw.h;dsound.h

// SET THE DEFAULT COMMAND LINE OPTIONS FOR THE COMPILER AND LINKER
if %debug% set clopt=-D_DEBUG -Zi
if %debug% set linkopt=-debug -debugtype:cv -pdb:none
if %debug% set crtlib=libcmtd.lib
if %debug% set debugchar=d
if not %debug% set clopt=-DNDEBUG -Ox -GBFry
if not %debug% set crtlib=libcmt.lib
if not %debug% set debugchar=
set mlopt=-Cx
set subsystem=windows
set baselib=%baselib% kernel32.lib user32.lib gdi32.lib advapi32.lib

// CREATE A TEMPORARY DIRECTORY TO USE FOR PRECOMPILED HEADERS
if not exist %temp%\pch\. md %temp%\pch
if not exist %temp%\pch\%username%\. md %temp%\pch\%username%
set precompileddir=%temp%\pch\%username%

// SET THE DEFAULT BUILD RULES
set .asm=.obj
set .c  =.obj
set .cpp=.obj
set .def=.exp
set .rc =.res
if exist %project%.def set .obj=.dll
if exist %project%.def set deffile=%project%.def
if not exist %project%.def set .obj=.exe
set .asm.obj=ml -nologo -c -coff %mlopt% %file%
set .c.obj  =cl -nologo -c -D_X86_ -D_MT -DWIN32 %clopt% -W3 -Fp%precompileddir%\%project%.pch -YX %file%
set .cpp.obj=cl -nologo -c -D_X86_ -D_MT -DWIN32 %clopt% -W3 -Fp%precompileddir%\%project%.pch -YX %file%
set .def.exp=lib -nologo -machine:i386 -def:%deffile% -name:%project%.dll -out:%project%.lib *.obj
set .rc.res =rc -r %rcopt% %file%
set .obj.lib=lib -nologo -machine:i386 -out:%project%.lib *.obj *.res %extralib%
set .obj.dll=link -nologo -machine:i386 -subsystem:%subsystem% %linkopt% -map -dll -out:%project%.dll *.obj *.res %project%.exp %extraobj% -nod %crtlib% %baselib% %extralib%
set .obj.exe=link -nologo -machine:i386 -subsystem:%subsystem% %linkopt% -map -out:%project%.exe *.obj *.res %extraobj% -nod %crtlib% %baselib% %extralib%
