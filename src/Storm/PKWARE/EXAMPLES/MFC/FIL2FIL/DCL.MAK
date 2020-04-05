# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "DCL.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/DCL.exe $(OUTDIR)/DCL.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32  
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /YX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp$(OUTDIR)/"DCL.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"DCL.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"DCL.bsc" 

$(OUTDIR)/DCL.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 oldnames.lib implode.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /MACHINE:IX86
# SUBTRACT LINK32 /INCREMENTAL:yes
LINK32_FLAGS=oldnames.lib implode.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"DCL.pdb" /MACHINE:IX86 /OUT:$(OUTDIR)/"DCL.exe"\
 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/DCL.res \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/DCL.OBJ \
	$(INTDIR)/MAINFRM.OBJ \
	$(INTDIR)/DCLDOC.OBJ \
	$(INTDIR)/DCLVIEW.OBJ \
	$(INTDIR)/COMPDLG.OBJ

$(OUTDIR)/DCL.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/DCL.exe $(OUTDIR)/DCL.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32  
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp$(OUTDIR)/"DCL.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"DCL.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"DCL.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"DCL.bsc" 

$(OUTDIR)/DCL.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 oldnames.lib implode.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /INCREMENTAL:no /DEBUG /MACHINE:IX86
LINK32_FLAGS=oldnames.lib implode.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"DCL.pdb" /DEBUG /MACHINE:IX86\
 /OUT:$(OUTDIR)/"DCL.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/DCL.res \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/DCL.OBJ \
	$(INTDIR)/MAINFRM.OBJ \
	$(INTDIR)/DCLDOC.OBJ \
	$(INTDIR)/DCLVIEW.OBJ \
	$(INTDIR)/COMPDLG.OBJ

$(OUTDIR)/DCL.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\DCL.RC
DEP_DCL_R=\
	.\RES\DCL.ICO\
	.\RES\TOOLBAR.BMP\
	.\RESOURCE.H\
	.\RES\DCL.RC2

$(INTDIR)/DCL.res :  $(SOURCE)  $(DEP_DCL_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_STDAF=\
	.\STDAFX.H

$(INTDIR)/STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DCL.CPP
DEP_DCL_C=\
	.\STDAFX.H\
	.\DCL.H\
	.\MAINFRM.H\
	.\DCLDOC.H\
	.\DCLVIEW.H\
	.\RESOURCE.H

$(INTDIR)/DCL.OBJ :  $(SOURCE)  $(DEP_DCL_C) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MAINFRM.CPP
DEP_MAINF=\
	.\STDAFX.H\
	.\DCL.H\
	.\MAINFRM.H\
	.\RESOURCE.H

$(INTDIR)/MAINFRM.OBJ :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DCLDOC.CPP
DEP_DCLDO=\
	.\STDAFX.H\
	.\DCL.H\
	.\DCLDOC.H\
	.\RESOURCE.H

$(INTDIR)/DCLDOC.OBJ :  $(SOURCE)  $(DEP_DCLDO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DCLVIEW.CPP
DEP_DCLVI=\
	.\STDAFX.H\
	.\DCL.H\
	.\DCLDOC.H\
	.\DCLVIEW.H\
	.\COMPDLG.H\
	.\RESOURCE.H\
	.\PKSTRUCT.H

$(INTDIR)/DCLVIEW.OBJ :  $(SOURCE)  $(DEP_DCLVI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\COMPDLG.CPP
DEP_COMPD=\
	.\STDAFX.H\
	.\DCL.H\
	.\implode.h\
	.\PKSTRUCT.H\
	.\COMPDLG.H\
	.\RESOURCE.H

$(INTDIR)/COMPDLG.OBJ :  $(SOURCE)  $(DEP_COMPD) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
