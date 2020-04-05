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
!MESSAGE NMAKE /f "SPAN.MAK" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/SPAN.exe $(OUTDIR)/SPAN.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"STDAFX.H" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp$(OUTDIR)/"SPAN.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SPAN.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"SPAN.bsc" 

$(OUTDIR)/SPAN.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib pkwdcl.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 oldnames.lib implodei.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /MACHINE:IX86
# SUBTRACT LINK32 /INCREMENTAL:yes
LINK32_FLAGS=oldnames.lib implodei.lib /NOLOGO /STACK:0x10240\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"SPAN.pdb" /MACHINE:IX86\
 /OUT:$(OUTDIR)/"SPAN.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/SPAN.res \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/SPAN.OBJ \
	$(INTDIR)/MAINFRM.OBJ \
	$(INTDIR)/SPANDOC.OBJ \
	$(INTDIR)/SPANVIEW.OBJ \
	$(INTDIR)/DCL.OBJ \
	$(INTDIR)/MULTFDLG.OBJ

$(OUTDIR)/SPAN.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/SPAN.exe $(OUTDIR)/SPAN.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"STDAFX.H" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp$(OUTDIR)/"SPAN.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"SPAN.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"SPAN.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"SPAN.bsc" 

$(OUTDIR)/SPAN.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib pkwdcl.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 oldnames.lib implodei.lib /NOLOGO /STACK:0x10240 /SUBSYSTEM:windows /INCREMENTAL:no /DEBUG /MACHINE:IX86
LINK32_FLAGS=oldnames.lib implodei.lib /NOLOGO /STACK:0x10240\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"SPAN.pdb" /DEBUG\
 /MACHINE:IX86 /OUT:$(OUTDIR)/"SPAN.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/SPAN.res \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/SPAN.OBJ \
	$(INTDIR)/MAINFRM.OBJ \
	$(INTDIR)/SPANDOC.OBJ \
	$(INTDIR)/SPANVIEW.OBJ \
	$(INTDIR)/DCL.OBJ \
	$(INTDIR)/MULTFDLG.OBJ

$(OUTDIR)/SPAN.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\SPAN.RC
DEP_SPAN_=\
	.\RES\SPAN.ICO\
	.\RES\TOOLBAR.BMP\
	.\resource.h\
	.\res\span.rc2

$(INTDIR)/SPAN.res :  $(SOURCE)  $(DEP_SPAN_) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_STDAF=\
	.\stdafx.h

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp$(OUTDIR)/"SPAN.pch" /Yc"STDAFX.H" /Fo$(INTDIR)/ /c\
  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp$(OUTDIR)/"SPAN.pch" /Yc"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"SPAN.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SPAN.CPP
DEP_SPAN_C=\
	.\stdafx.h\
	.\span.h\
	.\mainfrm.h\
	.\spandoc.h\
	.\spanview.h\
	.\resource.h

$(INTDIR)/SPAN.OBJ :  $(SOURCE)  $(DEP_SPAN_C) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MAINFRM.CPP
DEP_MAINF=\
	.\stdafx.h\
	.\span.h\
	.\mainfrm.h\
	.\implode.h\
	.\dcl.h\
	.\multfdlg.h\
	.\resource.h

$(INTDIR)/MAINFRM.OBJ :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SPANDOC.CPP
DEP_SPAND=\
	.\stdafx.h\
	.\span.h\
	.\spandoc.h\
	.\resource.h

$(INTDIR)/SPANDOC.OBJ :  $(SOURCE)  $(DEP_SPAND) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SPANVIEW.CPP
DEP_SPANV=\
	.\stdafx.h\
	.\span.h\
	.\spandoc.h\
	.\spanview.h\
	.\resource.h

$(INTDIR)/SPANVIEW.OBJ :  $(SOURCE)  $(DEP_SPANV) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DCL.CPP
DEP_DCL_C=\
	.\stdafx.h\
	.\implode.h

$(INTDIR)/DCL.OBJ :  $(SOURCE)  $(DEP_DCL_C) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MULTFDLG.CPP
DEP_MULTF=\
	.\stdafx.h\
	.\multfdlg.h

$(INTDIR)/MULTFDLG.OBJ :  $(SOURCE)  $(DEP_MULTF) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ

# End Source File
# End Group
# End Project
################################################################################
