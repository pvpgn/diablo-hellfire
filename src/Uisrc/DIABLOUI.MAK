# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Standard - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Standard - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DiabloUI - Win32 Release" && "$(CFG)" !=\
 "DiabloUI - Win32 Debug" && "$(CFG)" != "ui - Win32 Release" && "$(CFG)" !=\
 "ui - Win32 Debug" && "$(CFG)" != "battle - Win32 Release" && "$(CFG)" !=\
 "battle - Win32 Debug" && "$(CFG)" != "Standard - Win32 Release" && "$(CFG)" !=\
 "Standard - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Diabloui.mak" CFG="Standard - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DiabloUI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DiabloUI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "battle - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "battle - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Standard - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Standard - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Standard - Win32 Debug"

!IF  "$(CFG)" == "DiabloUI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "Standard - Win32 Release" "battle - Win32 Release" "ui - Win32 Release"\
 ".\bin\DiabloUI.exe"

CLEAN : 
	-@erase "$(INTDIR)\Testui.obj"
	-@erase "$(OUTDIR)\DiabloUI.map"
	-@erase ".\bin\DiabloUI.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /G5 /MT /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Diabloui.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Diabloui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib wsock32.lib storm.lib ui\release\diabloui.lib battle\release\battle.lib standard\release\standard.lib /nologo /subsystem:windows /map /machine:I386 /out:"bin/DiabloUI.exe"
# SUBTRACT LINK32 /incremental:yes
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib\
 wsock32.lib storm.lib ui\release\diabloui.lib battle\release\battle.lib\
 standard\release\standard.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/DiabloUI.pdb" /map:"$(INTDIR)/DiabloUI.map" /machine:I386\
 /out:"bin/DiabloUI.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Testui.obj" \
	".\battle\Release\battle.lib" \
	".\Standard\Release\Standard.lib" \
	".\STORM.LIB" \
	".\ui\Release\diabloui.lib"

".\bin\DiabloUI.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DiabloUI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "Standard - Win32 Debug" "battle - Win32 Debug" "ui - Win32 Debug"\
 ".\bin\DiabloUI.exe"

CLEAN : 
	-@erase "$(INTDIR)\Testui.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\DiabloUI.map"
	-@erase "$(OUTDIR)\DiabloUI.pdb"
	-@erase ".\bin\DiabloUI.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Diabloui.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Diabloui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib wsock32.lib storm.lib ui\debug\diabloui.lib battle\debug\battle.lib standard\debug\standard.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386 /out:"bin/DiabloUI.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib\
 wsock32.lib storm.lib ui\debug\diabloui.lib battle\debug\battle.lib\
 standard\debug\standard.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/DiabloUI.pdb" /map:"$(INTDIR)/DiabloUI.map" /debug\
 /machine:I386 /out:"bin/DiabloUI.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Testui.obj" \
	".\battle\debug\battle.lib" \
	".\Standard\Debug\Standard.lib" \
	".\STORM.LIB" \
	".\ui\Debug\diabloui.lib"

".\bin\DiabloUI.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ui\Release"
# PROP BASE Intermediate_Dir "ui\Release"
# PROP BASE Target_Dir "ui"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ui\Release"
# PROP Intermediate_Dir "ui\Release"
# PROP Target_Dir "ui"
OUTDIR=.\ui\Release
INTDIR=.\ui\Release

ALL : ".\bin\diabloui.dll"

CLEAN : 
	-@erase "$(INTDIR)\artfont.obj"
	-@erase "$(INTDIR)\connect.obj"
	-@erase "$(INTDIR)\CopyProt.obj"
	-@erase "$(INTDIR)\cr8game.obj"
	-@erase "$(INTDIR)\CreaDung.obj"
	-@erase "$(INTDIR)\CreaHero.obj"
	-@erase "$(INTDIR)\credits.obj"
	-@erase "$(INTDIR)\DiabEdit.obj"
	-@erase "$(INTDIR)\Diabloui.obj"
	-@erase "$(INTDIR)\DiabloUI.res"
	-@erase "$(INTDIR)\DirLink.obj"
	-@erase "$(INTDIR)\disclaim.obj"
	-@erase "$(INTDIR)\Doom.obj"
	-@erase "$(INTDIR)\EntrDial.obj"
	-@erase "$(INTDIR)\EntrName.obj"
	-@erase "$(INTDIR)\Fade.obj"
	-@erase "$(INTDIR)\Focus.obj"
	-@erase "$(INTDIR)\local.obj"
	-@erase "$(INTDIR)\Logon.obj"
	-@erase "$(INTDIR)\MainMenu.obj"
	-@erase "$(INTDIR)\Modem.obj"
	-@erase "$(INTDIR)\ModmStat.obj"
	-@erase "$(INTDIR)\OkCancel.obj"
	-@erase "$(INTDIR)\Progress.obj"
	-@erase "$(INTDIR)\Sbar.obj"
	-@erase "$(INTDIR)\SelClass.obj"
	-@erase "$(INTDIR)\SelConn.obj"
	-@erase "$(INTDIR)\SelDial.obj"
	-@erase "$(INTDIR)\SelGame.obj"
	-@erase "$(INTDIR)\SelHero.obj"
	-@erase "$(INTDIR)\SelIPX.obj"
	-@erase "$(INTDIR)\SelList.obj"
	-@erase "$(INTDIR)\SelLoad.obj"
	-@erase "$(INTDIR)\SelModem.obj"
	-@erase "$(INTDIR)\SelYesNo.obj"
	-@erase "$(INTDIR)\Title.obj"
	-@erase "$(INTDIR)\UiSnd.obj"
	-@erase "$(OUTDIR)\diabloui.exp"
	-@erase "$(OUTDIR)\diabloui.lib"
	-@erase "$(OUTDIR)\diabloui.map"
	-@erase ".\bin\diabloui.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /G5Fry /c
CPP_PROJ=/nologo /MT /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/ui.pch" /YX /Fo"$(INTDIR)/" /G5Fry /c 
CPP_OBJS=.\ui\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DiabloUI.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 tapi32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib wsock32.lib storm.lib /nologo /base:0x20000000 /subsystem:windows /dll /map /machine:I386 /out:"bin/diabloui.dll"
# SUBTRACT LINK32 /incremental:yes
LINK32_FLAGS=tapi32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib\
 shell32.lib wsock32.lib storm.lib /nologo /base:0x20000000 /subsystem:windows\
 /dll /incremental:no /pdb:"$(OUTDIR)/diabloui.pdb"\
 /map:"$(INTDIR)/diabloui.map" /machine:I386 /def:".\Ui\Diabloui.def"\
 /out:"bin/diabloui.dll" /implib:"$(OUTDIR)/diabloui.lib" 
DEF_FILE= \
	".\Ui\Diabloui.def"
LINK32_OBJS= \
	"$(INTDIR)\artfont.obj" \
	"$(INTDIR)\connect.obj" \
	"$(INTDIR)\CopyProt.obj" \
	"$(INTDIR)\cr8game.obj" \
	"$(INTDIR)\CreaDung.obj" \
	"$(INTDIR)\CreaHero.obj" \
	"$(INTDIR)\credits.obj" \
	"$(INTDIR)\DiabEdit.obj" \
	"$(INTDIR)\Diabloui.obj" \
	"$(INTDIR)\DiabloUI.res" \
	"$(INTDIR)\DirLink.obj" \
	"$(INTDIR)\disclaim.obj" \
	"$(INTDIR)\Doom.obj" \
	"$(INTDIR)\EntrDial.obj" \
	"$(INTDIR)\EntrName.obj" \
	"$(INTDIR)\Fade.obj" \
	"$(INTDIR)\Focus.obj" \
	"$(INTDIR)\local.obj" \
	"$(INTDIR)\Logon.obj" \
	"$(INTDIR)\MainMenu.obj" \
	"$(INTDIR)\Modem.obj" \
	"$(INTDIR)\ModmStat.obj" \
	"$(INTDIR)\OkCancel.obj" \
	"$(INTDIR)\Progress.obj" \
	"$(INTDIR)\Sbar.obj" \
	"$(INTDIR)\SelClass.obj" \
	"$(INTDIR)\SelConn.obj" \
	"$(INTDIR)\SelDial.obj" \
	"$(INTDIR)\SelGame.obj" \
	"$(INTDIR)\SelHero.obj" \
	"$(INTDIR)\SelIPX.obj" \
	"$(INTDIR)\SelList.obj" \
	"$(INTDIR)\SelLoad.obj" \
	"$(INTDIR)\SelModem.obj" \
	"$(INTDIR)\SelYesNo.obj" \
	"$(INTDIR)\Title.obj" \
	"$(INTDIR)\UiSnd.obj" \
	".\STORM.LIB"

".\bin\diabloui.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ui\ui___Win"
# PROP BASE Intermediate_Dir "ui\ui___Win"
# PROP BASE Target_Dir "ui"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ui\Debug"
# PROP Intermediate_Dir "ui\Debug"
# PROP Target_Dir "ui"
OUTDIR=.\ui\Debug
INTDIR=.\ui\Debug

ALL : ".\bin\diabloui.dll"

CLEAN : 
	-@erase "$(INTDIR)\artfont.obj"
	-@erase "$(INTDIR)\connect.obj"
	-@erase "$(INTDIR)\CopyProt.obj"
	-@erase "$(INTDIR)\cr8game.obj"
	-@erase "$(INTDIR)\CreaDung.obj"
	-@erase "$(INTDIR)\CreaHero.obj"
	-@erase "$(INTDIR)\credits.obj"
	-@erase "$(INTDIR)\DiabEdit.obj"
	-@erase "$(INTDIR)\Diabloui.obj"
	-@erase "$(INTDIR)\DiabloUI.res"
	-@erase "$(INTDIR)\DirLink.obj"
	-@erase "$(INTDIR)\disclaim.obj"
	-@erase "$(INTDIR)\Doom.obj"
	-@erase "$(INTDIR)\EntrDial.obj"
	-@erase "$(INTDIR)\EntrName.obj"
	-@erase "$(INTDIR)\Fade.obj"
	-@erase "$(INTDIR)\Focus.obj"
	-@erase "$(INTDIR)\local.obj"
	-@erase "$(INTDIR)\Logon.obj"
	-@erase "$(INTDIR)\MainMenu.obj"
	-@erase "$(INTDIR)\Modem.obj"
	-@erase "$(INTDIR)\ModmStat.obj"
	-@erase "$(INTDIR)\OkCancel.obj"
	-@erase "$(INTDIR)\Progress.obj"
	-@erase "$(INTDIR)\Sbar.obj"
	-@erase "$(INTDIR)\SelClass.obj"
	-@erase "$(INTDIR)\SelConn.obj"
	-@erase "$(INTDIR)\SelDial.obj"
	-@erase "$(INTDIR)\SelGame.obj"
	-@erase "$(INTDIR)\SelHero.obj"
	-@erase "$(INTDIR)\SelIPX.obj"
	-@erase "$(INTDIR)\SelList.obj"
	-@erase "$(INTDIR)\SelLoad.obj"
	-@erase "$(INTDIR)\SelModem.obj"
	-@erase "$(INTDIR)\SelYesNo.obj"
	-@erase "$(INTDIR)\Title.obj"
	-@erase "$(INTDIR)\UiSnd.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\diabloui.exp"
	-@erase "$(OUTDIR)\diabloui.lib"
	-@erase "$(OUTDIR)\diabloui.map"
	-@erase "$(OUTDIR)\diabloui.pdb"
	-@erase ".\bin\diabloui.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/ui.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\ui\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DiabloUI.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 tapi32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib wsock32.lib storm.lib /nologo /base:0x20000000 /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"bin/diabloui.dll"
LINK32_FLAGS=tapi32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib\
 shell32.lib wsock32.lib storm.lib /nologo /base:0x20000000 /subsystem:windows\
 /dll /incremental:no /pdb:"$(OUTDIR)/diabloui.pdb"\
 /map:"$(INTDIR)/diabloui.map" /debug /machine:I386 /def:".\Ui\Diabloui.def"\
 /out:"bin/diabloui.dll" /implib:"$(OUTDIR)/diabloui.lib" 
DEF_FILE= \
	".\Ui\Diabloui.def"
LINK32_OBJS= \
	"$(INTDIR)\artfont.obj" \
	"$(INTDIR)\connect.obj" \
	"$(INTDIR)\CopyProt.obj" \
	"$(INTDIR)\cr8game.obj" \
	"$(INTDIR)\CreaDung.obj" \
	"$(INTDIR)\CreaHero.obj" \
	"$(INTDIR)\credits.obj" \
	"$(INTDIR)\DiabEdit.obj" \
	"$(INTDIR)\Diabloui.obj" \
	"$(INTDIR)\DiabloUI.res" \
	"$(INTDIR)\DirLink.obj" \
	"$(INTDIR)\disclaim.obj" \
	"$(INTDIR)\Doom.obj" \
	"$(INTDIR)\EntrDial.obj" \
	"$(INTDIR)\EntrName.obj" \
	"$(INTDIR)\Fade.obj" \
	"$(INTDIR)\Focus.obj" \
	"$(INTDIR)\local.obj" \
	"$(INTDIR)\Logon.obj" \
	"$(INTDIR)\MainMenu.obj" \
	"$(INTDIR)\Modem.obj" \
	"$(INTDIR)\ModmStat.obj" \
	"$(INTDIR)\OkCancel.obj" \
	"$(INTDIR)\Progress.obj" \
	"$(INTDIR)\Sbar.obj" \
	"$(INTDIR)\SelClass.obj" \
	"$(INTDIR)\SelConn.obj" \
	"$(INTDIR)\SelDial.obj" \
	"$(INTDIR)\SelGame.obj" \
	"$(INTDIR)\SelHero.obj" \
	"$(INTDIR)\SelIPX.obj" \
	"$(INTDIR)\SelList.obj" \
	"$(INTDIR)\SelLoad.obj" \
	"$(INTDIR)\SelModem.obj" \
	"$(INTDIR)\SelYesNo.obj" \
	"$(INTDIR)\Title.obj" \
	"$(INTDIR)\UiSnd.obj" \
	".\STORM.LIB"

".\bin\diabloui.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "battle - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "battle\Release"
# PROP BASE Intermediate_Dir "battle\Release"
# PROP BASE Target_Dir "battle"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "battle\Release"
# PROP Intermediate_Dir "battle\Release"
# PROP Target_Dir "battle"
OUTDIR=.\battle\Release
INTDIR=.\battle\Release

ALL : ".\bin\battle.snp"

CLEAN : 
	-@erase "$(INTDIR)\ad.obj"
	-@erase "$(INTDIR)\Battle.obj"
	-@erase "$(INTDIR)\Battle.res"
	-@erase "$(INTDIR)\bnetlogo.obj"
	-@erase "$(INTDIR)\Cache.obj"
	-@erase "$(INTDIR)\cancel.obj"
	-@erase "$(INTDIR)\chatchnl.obj"
	-@erase "$(INTDIR)\Chatroom.obj"
	-@erase "$(INTDIR)\clrpref.obj"
	-@erase "$(INTDIR)\connect.obj"
	-@erase "$(INTDIR)\disconn.obj"
	-@erase "$(INTDIR)\getpass.obj"
	-@erase "$(INTDIR)\Joingame.obj"
	-@erase "$(INTDIR)\logon.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\Progress.obj"
	-@erase "$(INTDIR)\rasmgr.obj"
	-@erase "$(INTDIR)\scrllbar.obj"
	-@erase "$(INTDIR)\setpass.obj"
	-@erase "$(INTDIR)\Spi.obj"
	-@erase "$(INTDIR)\Srv.obj"
	-@erase "$(INTDIR)\Ui.obj"
	-@erase "$(OUTDIR)\battle.exp"
	-@erase "$(OUTDIR)\battle.lib"
	-@erase "$(OUTDIR)\battle.map"
	-@erase ".\bin\battle.snp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /G5Fry /c
CPP_PROJ=/nologo /MT /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/battle.pch" /YX /Fo"$(INTDIR)/" /G5Fry /c 
CPP_OBJS=.\battle\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "u:\diabloui" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Battle.res" /i "u:\diabloui" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/battle.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib wsock32.lib storm.lib /nologo /base:0x19000000 /subsystem:windows /dll /map /machine:I386 /out:"bin\battle.snp"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib\
 wsock32.lib storm.lib /nologo /base:0x19000000 /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/battle.pdb" /map:"$(INTDIR)/battle.map"\
 /machine:I386 /def:".\battle\Battle.def" /out:"bin\battle.snp"\
 /implib:"$(OUTDIR)/battle.lib" 
DEF_FILE= \
	".\battle\Battle.def"
LINK32_OBJS= \
	"$(INTDIR)\ad.obj" \
	"$(INTDIR)\Battle.obj" \
	"$(INTDIR)\Battle.res" \
	"$(INTDIR)\bnetlogo.obj" \
	"$(INTDIR)\Cache.obj" \
	"$(INTDIR)\cancel.obj" \
	"$(INTDIR)\chatchnl.obj" \
	"$(INTDIR)\Chatroom.obj" \
	"$(INTDIR)\clrpref.obj" \
	"$(INTDIR)\connect.obj" \
	"$(INTDIR)\disconn.obj" \
	"$(INTDIR)\getpass.obj" \
	"$(INTDIR)\Joingame.obj" \
	"$(INTDIR)\logon.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\Progress.obj" \
	"$(INTDIR)\rasmgr.obj" \
	"$(INTDIR)\scrllbar.obj" \
	"$(INTDIR)\setpass.obj" \
	"$(INTDIR)\Spi.obj" \
	"$(INTDIR)\Srv.obj" \
	"$(INTDIR)\Ui.obj" \
	".\STORM.LIB"

".\bin\battle.snp" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "battle - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "battle\battle__"
# PROP BASE Intermediate_Dir "battle\battle__"
# PROP BASE Target_Dir "battle"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "battle\debug"
# PROP Intermediate_Dir "battle\debug"
# PROP Target_Dir "battle"
OUTDIR=.\battle\debug
INTDIR=.\battle\debug

ALL : ".\bin\battle.snp"

CLEAN : 
	-@erase "$(INTDIR)\ad.obj"
	-@erase "$(INTDIR)\Battle.obj"
	-@erase "$(INTDIR)\Battle.res"
	-@erase "$(INTDIR)\bnetlogo.obj"
	-@erase "$(INTDIR)\Cache.obj"
	-@erase "$(INTDIR)\cancel.obj"
	-@erase "$(INTDIR)\chatchnl.obj"
	-@erase "$(INTDIR)\Chatroom.obj"
	-@erase "$(INTDIR)\clrpref.obj"
	-@erase "$(INTDIR)\connect.obj"
	-@erase "$(INTDIR)\disconn.obj"
	-@erase "$(INTDIR)\getpass.obj"
	-@erase "$(INTDIR)\Joingame.obj"
	-@erase "$(INTDIR)\logon.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\Progress.obj"
	-@erase "$(INTDIR)\rasmgr.obj"
	-@erase "$(INTDIR)\scrllbar.obj"
	-@erase "$(INTDIR)\setpass.obj"
	-@erase "$(INTDIR)\Spi.obj"
	-@erase "$(INTDIR)\Srv.obj"
	-@erase "$(INTDIR)\Ui.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\battle.exp"
	-@erase "$(OUTDIR)\battle.lib"
	-@erase "$(OUTDIR)\battle.map"
	-@erase "$(OUTDIR)\battle.pdb"
	-@erase ".\bin\battle.ilk"
	-@erase ".\bin\battle.snp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/battle.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\battle\debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "u:\diabloui" /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Battle.res" /i "u:\diabloui" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/battle.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib wsock32.lib storm.lib /nologo /base:0x19000000 /subsystem:windows /dll /map /debug /machine:I386 /out:"bin\battle.snp"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib\
 wsock32.lib storm.lib /nologo /base:0x19000000 /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)/battle.pdb" /map:"$(INTDIR)/battle.map" /debug\
 /machine:I386 /def:".\battle\Battle.def" /out:"bin\battle.snp"\
 /implib:"$(OUTDIR)/battle.lib" 
DEF_FILE= \
	".\battle\Battle.def"
LINK32_OBJS= \
	"$(INTDIR)\ad.obj" \
	"$(INTDIR)\Battle.obj" \
	"$(INTDIR)\Battle.res" \
	"$(INTDIR)\bnetlogo.obj" \
	"$(INTDIR)\Cache.obj" \
	"$(INTDIR)\cancel.obj" \
	"$(INTDIR)\chatchnl.obj" \
	"$(INTDIR)\Chatroom.obj" \
	"$(INTDIR)\clrpref.obj" \
	"$(INTDIR)\connect.obj" \
	"$(INTDIR)\disconn.obj" \
	"$(INTDIR)\getpass.obj" \
	"$(INTDIR)\Joingame.obj" \
	"$(INTDIR)\logon.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\Progress.obj" \
	"$(INTDIR)\rasmgr.obj" \
	"$(INTDIR)\scrllbar.obj" \
	"$(INTDIR)\setpass.obj" \
	"$(INTDIR)\Spi.obj" \
	"$(INTDIR)\Srv.obj" \
	"$(INTDIR)\Ui.obj" \
	".\STORM.LIB"

".\bin\battle.snp" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Standard - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Standard\Release"
# PROP BASE Intermediate_Dir "Standard\Release"
# PROP BASE Target_Dir "Standard"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Standard\Release"
# PROP Intermediate_Dir "Standard\Release"
# PROP Target_Dir "Standard"
OUTDIR=.\Standard\Release
INTDIR=.\Standard\Release

ALL : ".\bin\Standard.snp"

CLEAN : 
	-@erase "$(INTDIR)\IPX.OBJ"
	-@erase "$(INTDIR)\MODEM.OBJ"
	-@erase "$(INTDIR)\NULL.OBJ"
	-@erase "$(INTDIR)\PERF.OBJ"
	-@erase "$(INTDIR)\SERIAL.OBJ"
	-@erase "$(INTDIR)\STANDARD.OBJ"
	-@erase "$(INTDIR)\STANDARD.res"
	-@erase "$(INTDIR)\Trace.obj"
	-@erase "$(OUTDIR)\Standard.exp"
	-@erase "$(OUTDIR)\Standard.lib"
	-@erase "$(OUTDIR)\Standard.map"
	-@erase ".\bin\Standard.snp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /G5Fry /c
CPP_PROJ=/nologo /MT /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Standard.pch" /YX /Fo"$(INTDIR)/" /G5Fry /c 
CPP_OBJS=.\Standard\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/STANDARD.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Standard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 tapi32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib wsock32.lib storm.lib /nologo /base:0x18000000 /subsystem:windows /dll /map /machine:I386 /out:"bin\Standard.snp"
LINK32_FLAGS=tapi32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib\
 shell32.lib wsock32.lib storm.lib /nologo /base:0x18000000 /subsystem:windows\
 /dll /incremental:no /pdb:"$(OUTDIR)/Standard.pdb"\
 /map:"$(INTDIR)/Standard.map" /machine:I386 /def:".\Standard\STANDARD.DEF"\
 /out:"bin\Standard.snp" /implib:"$(OUTDIR)/Standard.lib" 
DEF_FILE= \
	".\Standard\STANDARD.DEF"
LINK32_OBJS= \
	"$(INTDIR)\IPX.OBJ" \
	"$(INTDIR)\MODEM.OBJ" \
	"$(INTDIR)\NULL.OBJ" \
	"$(INTDIR)\PERF.OBJ" \
	"$(INTDIR)\SERIAL.OBJ" \
	"$(INTDIR)\STANDARD.OBJ" \
	"$(INTDIR)\STANDARD.res" \
	"$(INTDIR)\Trace.obj" \
	".\STORM.LIB"

".\bin\Standard.snp" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Standard\Debug"
# PROP BASE Intermediate_Dir "Standard\Debug"
# PROP BASE Target_Dir "Standard"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Standard\Debug"
# PROP Intermediate_Dir "Standard\Debug"
# PROP Target_Dir "Standard"
OUTDIR=.\Standard\Debug
INTDIR=.\Standard\Debug

ALL : ".\bin\Standard.snp"

CLEAN : 
	-@erase "$(INTDIR)\IPX.OBJ"
	-@erase "$(INTDIR)\MODEM.OBJ"
	-@erase "$(INTDIR)\NULL.OBJ"
	-@erase "$(INTDIR)\PERF.OBJ"
	-@erase "$(INTDIR)\SERIAL.OBJ"
	-@erase "$(INTDIR)\STANDARD.OBJ"
	-@erase "$(INTDIR)\STANDARD.res"
	-@erase "$(INTDIR)\Trace.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Standard.exp"
	-@erase "$(OUTDIR)\Standard.lib"
	-@erase "$(OUTDIR)\Standard.map"
	-@erase "$(OUTDIR)\Standard.pdb"
	-@erase ".\bin\Standard.ilk"
	-@erase ".\bin\Standard.snp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Standard.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Standard\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/STANDARD.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Standard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 tapi32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib wsock32.lib storm.lib /nologo /base:0x18000000 /subsystem:windows /dll /map /debug /machine:I386 /out:"bin\Standard.snp"
LINK32_FLAGS=tapi32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib\
 shell32.lib wsock32.lib storm.lib /nologo /base:0x18000000 /subsystem:windows\
 /dll /incremental:yes /pdb:"$(OUTDIR)/Standard.pdb"\
 /map:"$(INTDIR)/Standard.map" /debug /machine:I386\
 /def:".\Standard\STANDARD.DEF" /out:"bin\Standard.snp"\
 /implib:"$(OUTDIR)/Standard.lib" 
DEF_FILE= \
	".\Standard\STANDARD.DEF"
LINK32_OBJS= \
	"$(INTDIR)\IPX.OBJ" \
	"$(INTDIR)\MODEM.OBJ" \
	"$(INTDIR)\NULL.OBJ" \
	"$(INTDIR)\PERF.OBJ" \
	"$(INTDIR)\SERIAL.OBJ" \
	"$(INTDIR)\STANDARD.OBJ" \
	"$(INTDIR)\STANDARD.res" \
	"$(INTDIR)\Trace.obj" \
	".\STORM.LIB"

".\bin\Standard.snp" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "DiabloUI - Win32 Release"
# Name "DiabloUI - Win32 Debug"

!IF  "$(CFG)" == "DiabloUI - Win32 Release"

!ELSEIF  "$(CFG)" == "DiabloUI - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "ui"

!IF  "$(CFG)" == "DiabloUI - Win32 Release"

"ui - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\Diabloui.mak" CFG="ui - Win32 Release" 

!ELSEIF  "$(CFG)" == "DiabloUI - Win32 Debug"

"ui - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\Diabloui.mak" CFG="ui - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\Testui.cpp
DEP_CPP_TESTU=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\Diabloui.h"\
	

"$(INTDIR)\Testui.obj" : $(SOURCE) $(DEP_CPP_TESTU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\STORM.LIB

!IF  "$(CFG)" == "DiabloUI - Win32 Release"

!ELSEIF  "$(CFG)" == "DiabloUI - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "battle"

!IF  "$(CFG)" == "DiabloUI - Win32 Release"

"battle - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\Diabloui.mak" CFG="battle - Win32 Release" 

!ELSEIF  "$(CFG)" == "DiabloUI - Win32 Debug"

"battle - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\Diabloui.mak" CFG="battle - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "Standard"

!IF  "$(CFG)" == "DiabloUI - Win32 Release"

"Standard - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\Diabloui.mak" CFG="Standard - Win32 Release" 

!ELSEIF  "$(CFG)" == "DiabloUI - Win32 Debug"

"Standard - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\Diabloui.mak" CFG="Standard - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "ui - Win32 Release"
# Name "ui - Win32 Debug"

!IF  "$(CFG)" == "ui - Win32 Release"

!ELSEIF  "$(CFG)" == "ui - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Ui\Title.cpp
DEP_CPP_TITLE=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\Title.obj" : $(SOURCE) $(DEP_CPP_TITLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\DiabloUI.rc
DEP_RSC_DIABL=\
	".\Ui\arrow.cur"\
	".\Ui\But_sml.raw"\
	".\Ui\Credits.txt"\
	".\Ui\Frank.rc"\
	".\Ui\ibeam.cur"\
	".\Ui\link.cur"\
	".\Ui\Mike.rc"\
	".\Ui\Srpopup.pal"\
	".\Ui\Srpopup.raw"\
	

!IF  "$(CFG)" == "ui - Win32 Release"


"$(INTDIR)\DiabloUI.res" : $(SOURCE) $(DEP_RSC_DIABL) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/DiabloUI.res" /i "Ui" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "ui - Win32 Debug"


"$(INTDIR)\DiabloUI.res" : $(SOURCE) $(DEP_RSC_DIABL) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/DiabloUI.res" /i "Ui" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\local.cpp
DEP_CPP_LOCAL=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\local.obj" : $(SOURCE) $(DEP_CPP_LOCAL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\Diabloui.cpp
DEP_CPP_DIABLO=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\Diabloui.obj" : $(SOURCE) $(DEP_CPP_DIABLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\Diabloui.def

!IF  "$(CFG)" == "ui - Win32 Release"

!ELSEIF  "$(CFG)" == "ui - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\MainMenu.cpp
DEP_CPP_MAINM=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\MainMenu.obj" : $(SOURCE) $(DEP_CPP_MAINM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\credits.cpp
DEP_CPP_CREDI=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\credits.obj" : $(SOURCE) $(DEP_CPP_CREDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\cr8game.cpp
DEP_CPP_CR8GA=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\cr8game.obj" : $(SOURCE) $(DEP_CPP_CR8GA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelHero.cpp
DEP_CPP_SELHE=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\SelHero.obj" : $(SOURCE) $(DEP_CPP_SELHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\OkCancel.cpp
DEP_CPP_OKCAN=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\OkCancel.obj" : $(SOURCE) $(DEP_CPP_OKCAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\Logon.cpp
DEP_CPP_LOGON=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\Logon.obj" : $(SOURCE) $(DEP_CPP_LOGON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\Progress.cpp
DEP_CPP_PROGR=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\Progress.obj" : $(SOURCE) $(DEP_CPP_PROGR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\connect.cpp
DEP_CPP_CONNE=\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\connect.obj" : $(SOURCE) $(DEP_CPP_CONNE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\disclaim.cpp
DEP_CPP_DISCL=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\disclaim.obj" : $(SOURCE) $(DEP_CPP_DISCL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\artfont.cpp
DEP_CPP_ARTFO=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\artfont.obj" : $(SOURCE) $(DEP_CPP_ARTFO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\Focus.cpp
DEP_CPP_FOCUS=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\Focus.obj" : $(SOURCE) $(DEP_CPP_FOCUS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\STORM.LIB

!IF  "$(CFG)" == "ui - Win32 Release"

!ELSEIF  "$(CFG)" == "ui - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelClass.cpp
DEP_CPP_SELCL=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\SelClass.obj" : $(SOURCE) $(DEP_CPP_SELCL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelList.cpp
DEP_CPP_SELLI=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\SelList.obj" : $(SOURCE) $(DEP_CPP_SELLI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\EntrName.cpp
DEP_CPP_ENTRN=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\EntrName.obj" : $(SOURCE) $(DEP_CPP_ENTRN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\UiSnd.cpp
DEP_CPP_UISND=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\UiSnd.obj" : $(SOURCE) $(DEP_CPP_UISND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelYesNo.cpp
DEP_CPP_SELYE=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\SelYesNo.obj" : $(SOURCE) $(DEP_CPP_SELYE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelConn.cpp
DEP_CPP_SELCO=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\SelConn.obj" : $(SOURCE) $(DEP_CPP_SELCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\CreaHero.cpp
DEP_CPP_CREAH=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\CreaHero.obj" : $(SOURCE) $(DEP_CPP_CREAH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\DiabEdit.cpp
DEP_CPP_DIABE=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\DiabEdit.obj" : $(SOURCE) $(DEP_CPP_DIABE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\Doom.cpp
DEP_CPP_DOOM_=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\Doom.obj" : $(SOURCE) $(DEP_CPP_DOOM_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\Fade.cpp
DEP_CPP_FADE_=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\Fade.obj" : $(SOURCE) $(DEP_CPP_FADE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelIPX.cpp
DEP_CPP_SELIP=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\SelIPX.obj" : $(SOURCE) $(DEP_CPP_SELIP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelGame.cpp
DEP_CPP_SELGA=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\SelGame.obj" : $(SOURCE) $(DEP_CPP_SELGA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\CreaDung.cpp
DEP_CPP_CREAD=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\CreaDung.obj" : $(SOURCE) $(DEP_CPP_CREAD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\DirLink.cpp
DEP_CPP_DIRLI=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\DirLink.obj" : $(SOURCE) $(DEP_CPP_DIRLI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\Sbar.cpp
DEP_CPP_SBAR_=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	

"$(INTDIR)\Sbar.obj" : $(SOURCE) $(DEP_CPP_SBAR_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelLoad.cpp
DEP_CPP_SELLO=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\SelLoad.obj" : $(SOURCE) $(DEP_CPP_SELLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\CopyProt.cpp
DEP_CPP_COPYP=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\CopyProt.obj" : $(SOURCE) $(DEP_CPP_COPYP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\Modem.cpp
DEP_CPP_MODEM=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\Modem.obj" : $(SOURCE) $(DEP_CPP_MODEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelModem.cpp
DEP_CPP_SELMO=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\SelModem.obj" : $(SOURCE) $(DEP_CPP_SELMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\EntrDial.cpp
DEP_CPP_ENTRD=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\EntrDial.obj" : $(SOURCE) $(DEP_CPP_ENTRD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\SelDial.cpp
DEP_CPP_SELDI=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\SelDial.obj" : $(SOURCE) $(DEP_CPP_SELDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ui\ModmStat.cpp
DEP_CPP_MODMS=\
	".\Sclass.h"\
	".\Ui\..\storm.h"\
	".\Ui\artfont.h"\
	".\Ui\DiabEdit.h"\
	".\Ui\Diabloui.h"\
	".\Ui\Doom.h"\
	".\Ui\Fade.h"\
	".\Ui\local.h"\
	".\Ui\Pch.h"\
	".\Ui\Sbar.h"\
	".\Ui\UiSnd.h"\
	

"$(INTDIR)\ModmStat.obj" : $(SOURCE) $(DEP_CPP_MODMS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "battle - Win32 Release"
# Name "battle - Win32 Debug"

!IF  "$(CFG)" == "battle - Win32 Release"

!ELSEIF  "$(CFG)" == "battle - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\battle\Ui.cpp
DEP_CPP_UI_CP=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\Ui.obj" : $(SOURCE) $(DEP_CPP_UI_CP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Battle.h

!IF  "$(CFG)" == "battle - Win32 Release"

!ELSEIF  "$(CFG)" == "battle - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Battle.rc
DEP_RSC_BATTL=\
	".\battle\Chathelp.txt"\
	

!IF  "$(CFG)" == "battle - Win32 Release"


"$(INTDIR)\Battle.res" : $(SOURCE) $(DEP_RSC_BATTL) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Battle.res" /i "u:\diabloui" /i "battle" /d\
 "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "battle - Win32 Debug"


"$(INTDIR)\Battle.res" : $(SOURCE) $(DEP_RSC_BATTL) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Battle.res" /i "u:\diabloui" /i "battle" /d\
 "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\bnetart.h

!IF  "$(CFG)" == "battle - Win32 Release"

!ELSEIF  "$(CFG)" == "battle - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Chatroom.cpp
DEP_CPP_CHATR=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\Chatroom.obj" : $(SOURCE) $(DEP_CPP_CHATR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Joingame.cpp
DEP_CPP_JOING=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\Joingame.obj" : $(SOURCE) $(DEP_CPP_JOING) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Pch.h

!IF  "$(CFG)" == "battle - Win32 Release"

!ELSEIF  "$(CFG)" == "battle - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Resource.h

!IF  "$(CFG)" == "battle - Win32 Release"

!ELSEIF  "$(CFG)" == "battle - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Spi.cpp
DEP_CPP_SPI_C=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\Spi.obj" : $(SOURCE) $(DEP_CPP_SPI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Srv.cpp
DEP_CPP_SRV_C=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\Srv.obj" : $(SOURCE) $(DEP_CPP_SRV_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Battle.cpp
DEP_CPP_BATTLE=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\Battle.obj" : $(SOURCE) $(DEP_CPP_BATTLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\STORM.LIB

!IF  "$(CFG)" == "battle - Win32 Release"

!ELSEIF  "$(CFG)" == "battle - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Battle.def

!IF  "$(CFG)" == "battle - Win32 Release"

!ELSEIF  "$(CFG)" == "battle - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\connect.cpp
DEP_CPP_CONNE=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\connect.obj" : $(SOURCE) $(DEP_CPP_CONNE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\logon.cpp
DEP_CPP_LOGON_=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\logon.obj" : $(SOURCE) $(DEP_CPP_LOGON_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Progress.cpp
DEP_CPP_PROGR=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\Progress.obj" : $(SOURCE) $(DEP_CPP_PROGR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\bnetlogo.cpp
DEP_CPP_BNETL=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\bnetlogo.obj" : $(SOURCE) $(DEP_CPP_BNETL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\cancel.cpp
DEP_CPP_CANCE=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\cancel.obj" : $(SOURCE) $(DEP_CPP_CANCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\Cache.cpp
DEP_CPP_CACHE=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\Cache.obj" : $(SOURCE) $(DEP_CPP_CACHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\clrpref.cpp
DEP_CPP_CLRPR=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\clrpref.obj" : $(SOURCE) $(DEP_CPP_CLRPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\ad.cpp
DEP_CPP_AD_CP=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\ad.obj" : $(SOURCE) $(DEP_CPP_AD_CP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\rasmgr.cpp
DEP_CPP_RASMG=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\rasmgr.obj" : $(SOURCE) $(DEP_CPP_RASMG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\disconn.cpp
DEP_CPP_DISCO=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\disconn.obj" : $(SOURCE) $(DEP_CPP_DISCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\chatchnl.cpp
DEP_CPP_CHATC=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\chatchnl.obj" : $(SOURCE) $(DEP_CPP_CHATC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\scrllbar.cpp
DEP_CPP_SCRLL=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\scrllbar.obj" : $(SOURCE) $(DEP_CPP_SCRLL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\message.cpp
DEP_CPP_MESSA=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Sclass.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\setpass.cpp
DEP_CPP_SETPA=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\setpass.obj" : $(SOURCE) $(DEP_CPP_SETPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle\getpass.cpp
DEP_CPP_GETPA=\
	".\battle\Battle.h"\
	".\battle\chat.h"\
	".\battle\Pch.h"\
	".\battle\rasmgr.h"\
	".\Ui\..\battle\bnetart.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\getpass.obj" : $(SOURCE) $(DEP_CPP_GETPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "Standard - Win32 Release"
# Name "Standard - Win32 Debug"

!IF  "$(CFG)" == "Standard - Win32 Release"

!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Standard\STANDARD.RC

!IF  "$(CFG)" == "Standard - Win32 Release"


"$(INTDIR)\STANDARD.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/STANDARD.res" /i "Standard" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"


"$(INTDIR)\STANDARD.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/STANDARD.res" /i "Standard" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Standard\MODEM.CPP

!IF  "$(CFG)" == "Standard - Win32 Release"

DEP_CPP_MODEM=\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	

"$(INTDIR)\MODEM.OBJ" : $(SOURCE) $(DEP_CPP_MODEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

DEP_CPP_MODEM=\
	".\Sclass.h"\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\MODEM.OBJ" : $(SOURCE) $(DEP_CPP_MODEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Standard\NULL.CPP

!IF  "$(CFG)" == "Standard - Win32 Release"

DEP_CPP_NULL_=\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	

"$(INTDIR)\NULL.OBJ" : $(SOURCE) $(DEP_CPP_NULL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

DEP_CPP_NULL_=\
	".\Sclass.h"\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\NULL.OBJ" : $(SOURCE) $(DEP_CPP_NULL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Standard\PERF.CPP

!IF  "$(CFG)" == "Standard - Win32 Release"

DEP_CPP_PERF_=\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	

"$(INTDIR)\PERF.OBJ" : $(SOURCE) $(DEP_CPP_PERF_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

DEP_CPP_PERF_=\
	".\Sclass.h"\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\PERF.OBJ" : $(SOURCE) $(DEP_CPP_PERF_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Standard\SERIAL.CPP

!IF  "$(CFG)" == "Standard - Win32 Release"

DEP_CPP_SERIA=\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	

"$(INTDIR)\SERIAL.OBJ" : $(SOURCE) $(DEP_CPP_SERIA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

DEP_CPP_SERIA=\
	".\Sclass.h"\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\SERIAL.OBJ" : $(SOURCE) $(DEP_CPP_SERIA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Standard\STANDARD.CPP

!IF  "$(CFG)" == "Standard - Win32 Release"

DEP_CPP_STAND=\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	

"$(INTDIR)\STANDARD.OBJ" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

DEP_CPP_STAND=\
	".\Sclass.h"\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\STANDARD.OBJ" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Standard\IPX.CPP

!IF  "$(CFG)" == "Standard - Win32 Release"

DEP_CPP_IPX_C=\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	

"$(INTDIR)\IPX.OBJ" : $(SOURCE) $(DEP_CPP_IPX_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

DEP_CPP_IPX_C=\
	".\Sclass.h"\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\IPX.OBJ" : $(SOURCE) $(DEP_CPP_IPX_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STORM.LIB

!IF  "$(CFG)" == "Standard - Win32 Release"

!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Standard\STANDARD.DEF

!IF  "$(CFG)" == "Standard - Win32 Release"

!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Standard\Trace.cpp
DEP_CPP_TRACE=\
	".\Standard\Pch.h"\
	".\Standard\Standard.h"\
	".\Standard\Trace.h"\
	".\Ui\..\storm.h"\
	

"$(INTDIR)\Trace.obj" : $(SOURCE) $(DEP_CPP_TRACE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
