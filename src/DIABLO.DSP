# Microsoft Developer Studio Project File - Name="Diablo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Diablo - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Diablo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Diablo.mak" CFG="Diablo - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Diablo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Diablo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Diablo - Win32 FinalFinal" (based on "Win32 (x86) Application")
!MESSAGE "Diablo - Win32 Shareware FinalFinal" (based on\
 "Win32 (x86) Application")
!MESSAGE "Diablo - Win32 Shareware Release" (based on\
 "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Diablo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\WinRel"
# PROP Intermediate_Dir ".\WinRel"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D PROGRAM_VERSION=RETAIL /FAcs /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib /out:".\WinRel/hellfire.exe"
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\WinDebug"
# PROP Intermediate_Dir ".\WinDebug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /G5 /Gr /MTd /W3 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D PROGRAM_VERSION=RETAIL /D "DEBUG_MEM" /D "_MULTITEST" /FAcs /Fr /YX /FD /c
# SUBTRACT CPP /Gy
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /profile /map /debug /machine:I386 /nodefaultlib /out:".\WinDebug/Hellfire.exe"
# SUBTRACT LINK32 /force

!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Diablo__"
# PROP BASE Intermediate_Dir ".\Diablo__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\WinFinal"
# PROP Intermediate_Dir ".\WinFinal"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D PROGRAM_VERSION=RETAIL /D "_MULTITEST" /FAcs /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winspool.lib libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib
# ADD LINK32 libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib /out:".\WinFinal/Hellfire.exe"
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Diablo__"
# PROP BASE Intermediate_Dir ".\Diablo__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\SFinal"
# PROP Intermediate_Dir ".\SFinal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D PROGRAM_VERSION=SHAREWARE /FAcs /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib
# ADD LINK32 libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Diablo_0"
# PROP BASE Intermediate_Dir ".\Diablo_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\SRel"
# PROP Intermediate_Dir ".\SRel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D PROGRAM_VERSION=SHAREWARE /FAcs /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib
# ADD LINK32 libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib
# SUBTRACT LINK32 /incremental:yes

!ENDIF 

# Begin Target

# Name "Diablo - Win32 Release"
# Name "Diablo - Win32 Debug"
# Name "Diablo - Win32 FinalFinal"
# Name "Diablo - Win32 Shareware FinalFinal"
# Name "Diablo - Win32 Shareware Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\appfat.cpp
# End Source File
# Begin Source File

SOURCE=.\automap.cpp
# End Source File
# Begin Source File

SOURCE=.\capture.cpp
# End Source File
# Begin Source File

SOURCE=.\CODEC.CPP
# End Source File
# Begin Source File

SOURCE=.\CONTROL.CPP
# End Source File
# Begin Source File

SOURCE=.\CURSOR.CPP
# End Source File
# Begin Source File

SOURCE=.\ddraw.lib
# End Source File
# Begin Source File

SOURCE=.\DEAD.CPP
# End Source File
# Begin Source File

SOURCE=.\DEBUG.CPP
# End Source File
# Begin Source File

SOURCE=.\DIABLO.CPP
# End Source File
# Begin Source File

SOURCE=.\diablo.rc

!IF  "$(CFG)" == "Diablo - Win32 Release"

!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\doom.cpp
# End Source File
# Begin Source File

SOURCE=.\DRLG_L1.CPP
# End Source File
# Begin Source File

SOURCE=.\DRLG_L2.CPP
# End Source File
# Begin Source File

SOURCE=.\DRLG_L3.CPP
# End Source File
# Begin Source File

SOURCE=.\drlg_l4.cpp
# End Source File
# Begin Source File

SOURCE=.\dsound.lib
# End Source File
# Begin Source File

SOURCE=.\dthread.cpp
# End Source File
# Begin Source File

SOURCE=.\dx.cpp
# End Source File
# Begin Source File

SOURCE=.\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\encrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\ENGINE.CPP
# End Source File
# Begin Source File

SOURCE=.\error.cpp
# End Source File
# Begin Source File

SOURCE=.\except.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\GENDUNG.CPP
# End Source File
# Begin Source File

SOURCE=.\gmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\WinDebug\hellfrui.lib
# End Source File
# Begin Source File

SOURCE=.\help.cpp
# End Source File
# Begin Source File

SOURCE=.\implode.lib
# End Source File
# Begin Source File

SOURCE=.\init.cpp
# End Source File
# Begin Source File

SOURCE=.\Interfac.cpp
# End Source File
# Begin Source File

SOURCE=.\inv.cpp
# End Source File
# Begin Source File

SOURCE=.\itemdat.cpp
# End Source File
# Begin Source File

SOURCE=.\ITEMS.CPP
# End Source File
# Begin Source File

SOURCE=.\LIGHTING.CPP
# End Source File
# Begin Source File

SOURCE=.\loadsave.cpp
# End Source File
# Begin Source File

SOURCE=.\mainmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\minitext.cpp
# End Source File
# Begin Source File

SOURCE=.\misdat.cpp
# End Source File
# Begin Source File

SOURCE=.\misdat.h
# End Source File
# Begin Source File

SOURCE=.\MISSILES.CPP
# End Source File
# Begin Source File

SOURCE=.\Mono.cpp
# End Source File
# Begin Source File

SOURCE=.\MONSTDAT.CPP
# End Source File
# Begin Source File

SOURCE=.\MONSTER.CPP
# End Source File
# Begin Source File

SOURCE=.\movie.cpp
# End Source File
# Begin Source File

SOURCE=.\mpqapi.cpp
# End Source File
# Begin Source File

SOURCE=.\msg.cpp
# End Source File
# Begin Source File

SOURCE=.\multi.cpp
# End Source File
# Begin Source File

SOURCE=.\nthread.cpp
# End Source File
# Begin Source File

SOURCE=.\objdat.cpp
# End Source File
# Begin Source File

SOURCE=.\OBJECTS.CPP
# End Source File
# Begin Source File

SOURCE=.\packplr.cpp
# End Source File
# Begin Source File

SOURCE=.\PALETTE.CPP
# End Source File
# Begin Source File

SOURCE=.\path.cpp
# End Source File
# Begin Source File

SOURCE=.\pfile.cpp
# End Source File
# Begin Source File

SOURCE=.\PLAYER.CPP
# End Source File
# Begin Source File

SOURCE=.\plrmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\portal.cpp
# End Source File
# Begin Source File

SOURCE=.\Quests.cpp
# End Source File
# Begin Source File

SOURCE=.\SCROLLRT.CPP
# End Source File
# Begin Source File

SOURCE=.\SetMaps.cpp
# End Source File
# Begin Source File

SOURCE=.\SHA.CPP
# End Source File
# Begin Source File

SOURCE=.\SOUND.CPP
# End Source File
# Begin Source File

SOURCE=.\Spelldat.cpp
# End Source File
# Begin Source File

SOURCE=.\SPELLS.CPP
# End Source File
# Begin Source File

SOURCE=.\stores.cpp
# End Source File
# Begin Source File

SOURCE=.\WinDebug\storm.lib
# End Source File
# Begin Source File

SOURCE=.\sync.cpp
# End Source File
# Begin Source File

SOURCE=.\Textdat.cpp
# End Source File
# Begin Source File

SOURCE=.\themes.cpp
# End Source File
# Begin Source File

SOURCE=.\tmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\TOWN.CPP
# End Source File
# Begin Source File

SOURCE=.\towners.cpp
# End Source File
# Begin Source File

SOURCE=.\track.cpp
# End Source File
# Begin Source File

SOURCE=.\TRIGS.CPP
# End Source File
# Begin Source File

SOURCE=.\wave.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\automap.h
# End Source File
# Begin Source File

SOURCE=.\Control.h
# End Source File
# Begin Source File

SOURCE=.\Cursor.h
# End Source File
# Begin Source File

SOURCE=.\d3dtypes.h
# End Source File
# Begin Source File

SOURCE=.\Ddraw.h
# End Source File
# Begin Source File

SOURCE=.\Dead.h
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=.\Diablo.h
# End Source File
# Begin Source File

SOURCE=.\Diabloui.h
# End Source File
# Begin Source File

SOURCE=.\doom.h
# End Source File
# Begin Source File

SOURCE=.\Drlg_l1.h
# End Source File
# Begin Source File

SOURCE=.\Drlg_l2.h
# End Source File
# Begin Source File

SOURCE=.\Drlg_l3.h
# End Source File
# Begin Source File

SOURCE=.\Drlg_l4.h
# End Source File
# Begin Source File

SOURCE=.\Dsound.h
# End Source File
# Begin Source File

SOURCE=.\Effects.h
# End Source File
# Begin Source File

SOURCE=.\Engine.h
# End Source File
# Begin Source File

SOURCE=.\error.h
# End Source File
# Begin Source File

SOURCE=.\Gamemenu.h
# End Source File
# Begin Source File

SOURCE=.\Gendung.h
# End Source File
# Begin Source File

SOURCE=.\help.h
# End Source File
# Begin Source File

SOURCE=.\implode.h
# End Source File
# Begin Source File

SOURCE=.\Interfac.h
# End Source File
# Begin Source File

SOURCE=.\Inv.h
# End Source File
# Begin Source File

SOURCE=.\itemdat.h
# End Source File
# Begin Source File

SOURCE=.\Items.h
# End Source File
# Begin Source File

SOURCE=.\Lighting.h
# End Source File
# Begin Source File

SOURCE=.\mainmenu.h
# End Source File
# Begin Source File

SOURCE=.\MiniText.h
# End Source File
# Begin Source File

SOURCE=.\Missiles.h
# End Source File
# Begin Source File

SOURCE=.\Mono.h
# End Source File
# Begin Source File

SOURCE=.\Monstdat.h
# End Source File
# Begin Source File

SOURCE=.\Monster.h
# End Source File
# Begin Source File

SOURCE=.\monstint.h
# End Source File
# Begin Source File

SOURCE=.\mpqapi.h
# End Source File
# Begin Source File

SOURCE=.\msg.h
# End Source File
# Begin Source File

SOURCE=.\Multi.h
# End Source File
# Begin Source File

SOURCE=.\objdat.h
# End Source File
# Begin Source File

SOURCE=.\Objects.h
# End Source File
# Begin Source File

SOURCE=.\packplr.h
# End Source File
# Begin Source File

SOURCE=.\Palette.h
# End Source File
# Begin Source File

SOURCE=.\path.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\portal.h
# End Source File
# Begin Source File

SOURCE=.\Quests.h
# End Source File
# Begin Source File

SOURCE=.\regconst.h
# End Source File
# Begin Source File

SOURCE=.\Sclass.h
# End Source File
# Begin Source File

SOURCE=.\scrlasm.h
# End Source File
# Begin Source File

SOURCE=.\Scrollrt.h
# End Source File
# Begin Source File

SOURCE=.\Setmaps.h
# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\spelldat.h
# End Source File
# Begin Source File

SOURCE=.\Spells.h
# End Source File
# Begin Source File

SOURCE=.\stores.h
# End Source File
# Begin Source File

SOURCE=.\Storm.h
# End Source File
# Begin Source File

SOURCE=.\textdat.h
# End Source File
# Begin Source File

SOURCE=.\themes.h
# End Source File
# Begin Source File

SOURCE=.\Town.h
# End Source File
# Begin Source File

SOURCE=.\towners.h
# End Source File
# Begin Source File

SOURCE=.\Trigs.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\WinRel\Scroll.obj
# End Source File
# End Target
# End Project
