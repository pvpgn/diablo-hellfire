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
!MESSAGE NMAKE /f "MEM2MEM.MAK" CFG="Win32 Debug"
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

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/MEM2MEM.exe $(OUTDIR)/MEM2MEM.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"STDAFX.H" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp$(OUTDIR)/"MEM2MEM.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"MEM2MEM.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"MEM2MEM.bsc" 

$(OUTDIR)/MEM2MEM.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib pkwdcl.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 oldnames.lib implodei.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /MACHINE:IX86
# SUBTRACT LINK32 /INCREMENTAL:yes
LINK32_FLAGS=oldnames.lib implodei.lib /NOLOGO /STACK:0x10240\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"MEM2MEM.pdb" /MACHINE:IX86\
 /OUT:$(OUTDIR)/"MEM2MEM.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/MEM2MEM.res \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/MEM2MEM.OBJ \
	$(INTDIR)/MAINFRM.OBJ \
	$(INTDIR)/MEM2MDOC.OBJ \
	$(INTDIR)/MEM2MVW.OBJ \
	$(INTDIR)/DCL.OBJ

$(OUTDIR)/MEM2MEM.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/MEM2MEM.exe $(OUTDIR)/MEM2MEM.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32  
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"STDAFX.H" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp$(OUTDIR)/"MEM2MEM.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"MEM2MEM.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"MEM2MEM.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"MEM2MEM.bsc" 

$(OUTDIR)/MEM2MEM.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib pkwdcl.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 oldnames.lib implodei.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /INCREMENTAL:no /DEBUG /MACHINE:IX86
LINK32_FLAGS=oldnames.lib implodei.lib /NOLOGO /STACK:0x10240\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"MEM2MEM.pdb" /DEBUG\
 /MACHINE:IX86 /OUT:$(OUTDIR)/"MEM2MEM.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/MEM2MEM.res \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/MEM2MEM.OBJ \
	$(INTDIR)/MAINFRM.OBJ \
	$(INTDIR)/MEM2MDOC.OBJ \
	$(INTDIR)/MEM2MVW.OBJ \
	$(INTDIR)/DCL.OBJ

$(OUTDIR)/MEM2MEM.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\MEM2MEM.RC
DEP_MEM2M=\
	.\RES\MEM2MEM.ICO\
	.\RES\TOOLBAR.BMP\
	.\RESOURCE.H\
	.\RES\MEM2MEM.RC2

$(INTDIR)/MEM2MEM.res :  $(SOURCE)  $(DEP_MEM2M) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_STDAF=\
	.\STDAFX.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp$(OUTDIR)/"MEM2MEM.pch" /Yc"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp$(OUTDIR)/"MEM2MEM.pch" /Yc"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"MEM2MEM.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MEM2MEM.CPP
DEP_MEM2ME=\
	.\STDAFX.H\
	.\MEM2MEM.H\
	.\MAINFRM.H\
	.\MEM2MDOC.H\
	.\MEM2MVW.H\
	.\RESOURCE.H

$(INTDIR)/MEM2MEM.OBJ :  $(SOURCE)  $(DEP_MEM2ME) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MAINFRM.CPP
DEP_MAINF=\
	.\STDAFX.H\
	.\MEM2MEM.H\
	.\MAINFRM.H\
	.\DCL.H\
	.\RESOURCE.H

$(INTDIR)/MAINFRM.OBJ :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MEM2MDOC.CPP
DEP_MEM2MD=\
	.\STDAFX.H\
	.\MEM2MEM.H\
	.\MEM2MDOC.H\
	.\RESOURCE.H

$(INTDIR)/MEM2MDOC.OBJ :  $(SOURCE)  $(DEP_MEM2MD) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MEM2MVW.CPP
DEP_MEM2MV=\
	.\STDAFX.H\
	.\MEM2MEM.H\
	.\MEM2MDOC.H\
	.\MEM2MVW.H\
	.\RESOURCE.H

$(INTDIR)/MEM2MVW.OBJ :  $(SOURCE)  $(DEP_MEM2MV) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DCL.CPP
DEP_DCL_C=\
	.\STDAFX.H\
	.\PKWDCL.H

$(INTDIR)/DCL.OBJ :  $(SOURCE)  $(DEP_DCL_C) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
# End Group
# End Project
################################################################################
