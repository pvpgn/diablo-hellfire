# Microsoft Developer Studio Project File - Name="stormdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=stormdll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stormdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stormdll.mak" CFG="stormdll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stormdll - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stormdll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stormdll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "h" /I "smacker\h" /I "pkware\h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib comdlg32.lib version.lib implode.lib oldnames.lib msvcrtd.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"crtdll.lib" /nodefaultlib /libpath:"source" /libpath:"pkware\lib"

!ELSEIF  "$(CFG)" == "stormdll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "h" /I "smacker\h" /I "pkware\h" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR"./debug/" /Fp"./debug/stormdll.pch" /YX /Fo"./debug/" /Fd"./debug/" /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo"../windebug/Storm.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /o"../windebug/stormdll.bsc"
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib comdlg32.lib version.lib implode.lib oldnames.lib msvcrtd.lib /nologo /base:"0x15000000" /subsystem:windows /dll /incremental:no /pdb:"../windebug/storm.pdb" /map:"../windebug/storm.map" /debug /machine:I386 /nodefaultlib:"crtdll.lib" /nodefaultlib /def:".\Source\Exports.def" /out:"../windebug/storm.dll" /implib:"../windebug/storm.lib" /pdbtype:sept /libpath:"source" /libpath:"pkware\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "stormdll - Win32 Release"
# Name "stormdll - Win32 Debug"
# Begin Group "source"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\Source\Exports.def

!IF  "$(CFG)" == "stormdll - Win32 Release"

!ELSEIF  "$(CFG)" == "stormdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\Sblt.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Sbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Scmd.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Scode.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Scomp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Sdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Sdraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Serr.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Sevt.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Sfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Sgdi.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Slog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Smem.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Snet.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Sreg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Srgn.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Srtl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Sstr.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Storm.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Storm.rc
# End Source File
# Begin Source File

SOURCE=.\Source\STrans.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Svid.cpp
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\Source\Pch.h
# End Source File
# Begin Source File

SOURCE=.\Source\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Source\SIntern.h
# End Source File
# End Group
# Begin Group "h"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\H\Bnetart.h
# End Source File
# Begin Source File

SOURCE=.\H\Storm.h
# End Source File
# End Group
# Begin Group "pkware"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pkware\Obj\Crc32.obj
# End Source File
# Begin Source File

SOURCE=.\pkware\Obj\Crcfast.obj
# End Source File
# Begin Source File

SOURCE=.\pkware\Obj\Crcstd.obj
# End Source File
# Begin Source File

SOURCE=.\pkware\Obj\Expfast.obj
# End Source File
# Begin Source File

SOURCE=.\pkware\Obj\Explode.obj
# End Source File
# Begin Source File

SOURCE=.\pkware\Obj\Expstd.obj
# End Source File
# Begin Source File

SOURCE=.\pkware\Obj\Impfast.obj
# End Source File
# Begin Source File

SOURCE=.\pkware\Obj\Implode.obj
# End Source File
# Begin Source File

SOURCE=.\pkware\Obj\Impstd.obj
# End Source File
# End Group
# End Target
# End Project
