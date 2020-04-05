# Microsoft Developer Studio Generated NMAKE File, Based on Diablo.dsp
!IF "$(CFG)" == ""
CFG=Diablo - Win32 Release
!MESSAGE No configuration specified. Defaulting to Diablo - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Diablo - Win32 Release" && "$(CFG)" != "Diablo - Win32 Debug"\
 && "$(CFG)" != "Diablo - Win32 FinalFinal" && "$(CFG)" !=\
 "Diablo - Win32 Shareware FinalFinal" && "$(CFG)" !=\
 "Diablo - Win32 Shareware Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Diablo - Win32 Release"

OUTDIR=.\WinRel
INTDIR=.\WinRel
# Begin Custom Macros
OutDir=.\.\WinRel
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Diablo.exe" "$(OUTDIR)\Diablo.bsc"

!ELSE 

ALL : "$(OUTDIR)\Diablo.exe" "$(OUTDIR)\Diablo.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\appfat.obj"
	-@erase "$(INTDIR)\appfat.sbr"
	-@erase "$(INTDIR)\automap.obj"
	-@erase "$(INTDIR)\automap.sbr"
	-@erase "$(INTDIR)\capture.obj"
	-@erase "$(INTDIR)\capture.sbr"
	-@erase "$(INTDIR)\CODEC.OBJ"
	-@erase "$(INTDIR)\CODEC.SBR"
	-@erase "$(INTDIR)\CONTROL.OBJ"
	-@erase "$(INTDIR)\CONTROL.SBR"
	-@erase "$(INTDIR)\CURSOR.OBJ"
	-@erase "$(INTDIR)\CURSOR.SBR"
	-@erase "$(INTDIR)\DEAD.OBJ"
	-@erase "$(INTDIR)\DEAD.SBR"
	-@erase "$(INTDIR)\DEBUG.OBJ"
	-@erase "$(INTDIR)\DEBUG.SBR"
	-@erase "$(INTDIR)\DIABLO.OBJ"
	-@erase "$(INTDIR)\diablo.res"
	-@erase "$(INTDIR)\DIABLO.SBR"
	-@erase "$(INTDIR)\doom.obj"
	-@erase "$(INTDIR)\doom.sbr"
	-@erase "$(INTDIR)\DRLG_L1.OBJ"
	-@erase "$(INTDIR)\DRLG_L1.SBR"
	-@erase "$(INTDIR)\DRLG_L2.OBJ"
	-@erase "$(INTDIR)\DRLG_L2.SBR"
	-@erase "$(INTDIR)\DRLG_L3.OBJ"
	-@erase "$(INTDIR)\DRLG_L3.SBR"
	-@erase "$(INTDIR)\drlg_l4.obj"
	-@erase "$(INTDIR)\drlg_l4.sbr"
	-@erase "$(INTDIR)\dthread.obj"
	-@erase "$(INTDIR)\dthread.sbr"
	-@erase "$(INTDIR)\dx.obj"
	-@erase "$(INTDIR)\dx.sbr"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\effects.sbr"
	-@erase "$(INTDIR)\encrypt.obj"
	-@erase "$(INTDIR)\encrypt.sbr"
	-@erase "$(INTDIR)\ENGINE.OBJ"
	-@erase "$(INTDIR)\ENGINE.SBR"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\error.sbr"
	-@erase "$(INTDIR)\except.obj"
	-@erase "$(INTDIR)\except.sbr"
	-@erase "$(INTDIR)\GameMenu.obj"
	-@erase "$(INTDIR)\GameMenu.sbr"
	-@erase "$(INTDIR)\GENDUNG.OBJ"
	-@erase "$(INTDIR)\GENDUNG.SBR"
	-@erase "$(INTDIR)\gmenu.obj"
	-@erase "$(INTDIR)\gmenu.sbr"
	-@erase "$(INTDIR)\help.obj"
	-@erase "$(INTDIR)\help.sbr"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\init.sbr"
	-@erase "$(INTDIR)\Interfac.obj"
	-@erase "$(INTDIR)\Interfac.sbr"
	-@erase "$(INTDIR)\inv.obj"
	-@erase "$(INTDIR)\inv.sbr"
	-@erase "$(INTDIR)\itemdat.obj"
	-@erase "$(INTDIR)\itemdat.sbr"
	-@erase "$(INTDIR)\ITEMS.OBJ"
	-@erase "$(INTDIR)\ITEMS.SBR"
	-@erase "$(INTDIR)\LIGHTING.OBJ"
	-@erase "$(INTDIR)\LIGHTING.SBR"
	-@erase "$(INTDIR)\loadsave.obj"
	-@erase "$(INTDIR)\loadsave.sbr"
	-@erase "$(INTDIR)\mainmenu.obj"
	-@erase "$(INTDIR)\mainmenu.sbr"
	-@erase "$(INTDIR)\minitext.obj"
	-@erase "$(INTDIR)\minitext.sbr"
	-@erase "$(INTDIR)\misdat.obj"
	-@erase "$(INTDIR)\misdat.sbr"
	-@erase "$(INTDIR)\MISSILES.OBJ"
	-@erase "$(INTDIR)\MISSILES.SBR"
	-@erase "$(INTDIR)\MONSTDAT.OBJ"
	-@erase "$(INTDIR)\MONSTDAT.SBR"
	-@erase "$(INTDIR)\MONSTER.OBJ"
	-@erase "$(INTDIR)\MONSTER.SBR"
	-@erase "$(INTDIR)\movie.obj"
	-@erase "$(INTDIR)\movie.sbr"
	-@erase "$(INTDIR)\mpqapi.obj"
	-@erase "$(INTDIR)\mpqapi.sbr"
	-@erase "$(INTDIR)\msg.obj"
	-@erase "$(INTDIR)\msg.sbr"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\multi.sbr"
	-@erase "$(INTDIR)\nthread.obj"
	-@erase "$(INTDIR)\nthread.sbr"
	-@erase "$(INTDIR)\objdat.obj"
	-@erase "$(INTDIR)\objdat.sbr"
	-@erase "$(INTDIR)\OBJECTS.OBJ"
	-@erase "$(INTDIR)\OBJECTS.SBR"
	-@erase "$(INTDIR)\packplr.obj"
	-@erase "$(INTDIR)\packplr.sbr"
	-@erase "$(INTDIR)\PALETTE.OBJ"
	-@erase "$(INTDIR)\PALETTE.SBR"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\path.sbr"
	-@erase "$(INTDIR)\pfile.obj"
	-@erase "$(INTDIR)\pfile.sbr"
	-@erase "$(INTDIR)\PLAYER.OBJ"
	-@erase "$(INTDIR)\PLAYER.SBR"
	-@erase "$(INTDIR)\plrmsg.obj"
	-@erase "$(INTDIR)\plrmsg.sbr"
	-@erase "$(INTDIR)\portal.obj"
	-@erase "$(INTDIR)\portal.sbr"
	-@erase "$(INTDIR)\Quests.obj"
	-@erase "$(INTDIR)\Quests.sbr"
	-@erase "$(INTDIR)\SCROLLRT.OBJ"
	-@erase "$(INTDIR)\SCROLLRT.SBR"
	-@erase "$(INTDIR)\SetMaps.obj"
	-@erase "$(INTDIR)\SetMaps.sbr"
	-@erase "$(INTDIR)\SHA.OBJ"
	-@erase "$(INTDIR)\SHA.SBR"
	-@erase "$(INTDIR)\SOUND.OBJ"
	-@erase "$(INTDIR)\SOUND.SBR"
	-@erase "$(INTDIR)\Spelldat.obj"
	-@erase "$(INTDIR)\Spelldat.sbr"
	-@erase "$(INTDIR)\SPELLS.OBJ"
	-@erase "$(INTDIR)\SPELLS.SBR"
	-@erase "$(INTDIR)\stores.obj"
	-@erase "$(INTDIR)\stores.sbr"
	-@erase "$(INTDIR)\sync.obj"
	-@erase "$(INTDIR)\sync.sbr"
	-@erase "$(INTDIR)\Textdat.obj"
	-@erase "$(INTDIR)\Textdat.sbr"
	-@erase "$(INTDIR)\themes.obj"
	-@erase "$(INTDIR)\themes.sbr"
	-@erase "$(INTDIR)\tmsg.obj"
	-@erase "$(INTDIR)\tmsg.sbr"
	-@erase "$(INTDIR)\TOWN.OBJ"
	-@erase "$(INTDIR)\TOWN.SBR"
	-@erase "$(INTDIR)\towners.obj"
	-@erase "$(INTDIR)\towners.sbr"
	-@erase "$(INTDIR)\track.obj"
	-@erase "$(INTDIR)\track.sbr"
	-@erase "$(INTDIR)\TRIGS.OBJ"
	-@erase "$(INTDIR)\TRIGS.SBR"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(INTDIR)\wave.sbr"
	-@erase "$(OUTDIR)\Diablo.bsc"
	-@erase "$(OUTDIR)\Diablo.exe"
	-@erase "$(OUTDIR)\Diablo.map"
	-@erase "$(OUTDIR)\Diablo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "_DEBUG"\
 /D PROGRAM_VERSION=RETAIL /FAcs /Fa"$(INTDIR)\\" /Fr"$(INTDIR)\\"\
 /Fp"$(INTDIR)\Diablo.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\WinRel/

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\diablo.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Diablo.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\appfat.sbr" \
	"$(INTDIR)\automap.sbr" \
	"$(INTDIR)\capture.sbr" \
	"$(INTDIR)\CODEC.SBR" \
	"$(INTDIR)\CONTROL.SBR" \
	"$(INTDIR)\CURSOR.SBR" \
	"$(INTDIR)\DEAD.SBR" \
	"$(INTDIR)\DEBUG.SBR" \
	"$(INTDIR)\DIABLO.SBR" \
	"$(INTDIR)\doom.sbr" \
	"$(INTDIR)\DRLG_L1.SBR" \
	"$(INTDIR)\DRLG_L2.SBR" \
	"$(INTDIR)\DRLG_L3.SBR" \
	"$(INTDIR)\drlg_l4.sbr" \
	"$(INTDIR)\dthread.sbr" \
	"$(INTDIR)\dx.sbr" \
	"$(INTDIR)\effects.sbr" \
	"$(INTDIR)\encrypt.sbr" \
	"$(INTDIR)\ENGINE.SBR" \
	"$(INTDIR)\error.sbr" \
	"$(INTDIR)\except.sbr" \
	"$(INTDIR)\GameMenu.sbr" \
	"$(INTDIR)\GENDUNG.SBR" \
	"$(INTDIR)\gmenu.sbr" \
	"$(INTDIR)\help.sbr" \
	"$(INTDIR)\init.sbr" \
	"$(INTDIR)\Interfac.sbr" \
	"$(INTDIR)\inv.sbr" \
	"$(INTDIR)\itemdat.sbr" \
	"$(INTDIR)\ITEMS.SBR" \
	"$(INTDIR)\LIGHTING.SBR" \
	"$(INTDIR)\loadsave.sbr" \
	"$(INTDIR)\mainmenu.sbr" \
	"$(INTDIR)\minitext.sbr" \
	"$(INTDIR)\misdat.sbr" \
	"$(INTDIR)\MISSILES.SBR" \
	"$(INTDIR)\MONSTDAT.SBR" \
	"$(INTDIR)\MONSTER.SBR" \
	"$(INTDIR)\movie.sbr" \
	"$(INTDIR)\mpqapi.sbr" \
	"$(INTDIR)\msg.sbr" \
	"$(INTDIR)\multi.sbr" \
	"$(INTDIR)\nthread.sbr" \
	"$(INTDIR)\objdat.sbr" \
	"$(INTDIR)\OBJECTS.SBR" \
	"$(INTDIR)\packplr.sbr" \
	"$(INTDIR)\PALETTE.SBR" \
	"$(INTDIR)\path.sbr" \
	"$(INTDIR)\pfile.sbr" \
	"$(INTDIR)\PLAYER.SBR" \
	"$(INTDIR)\plrmsg.sbr" \
	"$(INTDIR)\portal.sbr" \
	"$(INTDIR)\Quests.sbr" \
	"$(INTDIR)\SCROLLRT.SBR" \
	"$(INTDIR)\SetMaps.sbr" \
	"$(INTDIR)\SHA.SBR" \
	"$(INTDIR)\SOUND.SBR" \
	"$(INTDIR)\Spelldat.sbr" \
	"$(INTDIR)\SPELLS.SBR" \
	"$(INTDIR)\stores.sbr" \
	"$(INTDIR)\sync.sbr" \
	"$(INTDIR)\Textdat.sbr" \
	"$(INTDIR)\themes.sbr" \
	"$(INTDIR)\tmsg.sbr" \
	"$(INTDIR)\TOWN.SBR" \
	"$(INTDIR)\towners.sbr" \
	"$(INTDIR)\track.sbr" \
	"$(INTDIR)\TRIGS.SBR" \
	"$(INTDIR)\wave.sbr"

"$(OUTDIR)\Diablo.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\Diablo.pdb" /map:"$(INTDIR)\Diablo.map" /debug\
 /machine:I386 /nodefaultlib /out:"$(OUTDIR)\Diablo.exe" 
LINK32_OBJS= \
	"$(INTDIR)\appfat.obj" \
	"$(INTDIR)\automap.obj" \
	"$(INTDIR)\capture.obj" \
	"$(INTDIR)\CODEC.OBJ" \
	"$(INTDIR)\CONTROL.OBJ" \
	"$(INTDIR)\CURSOR.OBJ" \
	"$(INTDIR)\DEAD.OBJ" \
	"$(INTDIR)\DEBUG.OBJ" \
	"$(INTDIR)\DIABLO.OBJ" \
	"$(INTDIR)\diablo.res" \
	"$(INTDIR)\doom.obj" \
	"$(INTDIR)\DRLG_L1.OBJ" \
	"$(INTDIR)\DRLG_L2.OBJ" \
	"$(INTDIR)\DRLG_L3.OBJ" \
	"$(INTDIR)\drlg_l4.obj" \
	"$(INTDIR)\dthread.obj" \
	"$(INTDIR)\dx.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\encrypt.obj" \
	"$(INTDIR)\ENGINE.OBJ" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\except.obj" \
	"$(INTDIR)\GameMenu.obj" \
	"$(INTDIR)\GENDUNG.OBJ" \
	"$(INTDIR)\gmenu.obj" \
	"$(INTDIR)\help.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\Interfac.obj" \
	"$(INTDIR)\inv.obj" \
	"$(INTDIR)\itemdat.obj" \
	"$(INTDIR)\ITEMS.OBJ" \
	"$(INTDIR)\LIGHTING.OBJ" \
	"$(INTDIR)\loadsave.obj" \
	"$(INTDIR)\mainmenu.obj" \
	"$(INTDIR)\minitext.obj" \
	"$(INTDIR)\misdat.obj" \
	"$(INTDIR)\MISSILES.OBJ" \
	"$(INTDIR)\MONSTDAT.OBJ" \
	"$(INTDIR)\MONSTER.OBJ" \
	"$(INTDIR)\movie.obj" \
	"$(INTDIR)\mpqapi.obj" \
	"$(INTDIR)\msg.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\nthread.obj" \
	"$(INTDIR)\objdat.obj" \
	"$(INTDIR)\OBJECTS.OBJ" \
	"$(INTDIR)\packplr.obj" \
	"$(INTDIR)\PALETTE.OBJ" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\pfile.obj" \
	"$(INTDIR)\PLAYER.OBJ" \
	"$(INTDIR)\plrmsg.obj" \
	"$(INTDIR)\portal.obj" \
	"$(INTDIR)\Quests.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\SCROLLRT.OBJ" \
	"$(INTDIR)\SetMaps.obj" \
	"$(INTDIR)\SHA.OBJ" \
	"$(INTDIR)\SOUND.OBJ" \
	"$(INTDIR)\Spelldat.obj" \
	"$(INTDIR)\SPELLS.OBJ" \
	"$(INTDIR)\stores.obj" \
	"$(INTDIR)\sync.obj" \
	"$(INTDIR)\Textdat.obj" \
	"$(INTDIR)\themes.obj" \
	"$(INTDIR)\tmsg.obj" \
	"$(INTDIR)\TOWN.OBJ" \
	"$(INTDIR)\towners.obj" \
	"$(INTDIR)\track.obj" \
	"$(INTDIR)\TRIGS.OBJ" \
	"$(INTDIR)\wave.obj" \
	".\ddraw.lib" \
	".\dsound.lib" \
	".\Hellfrui.lib" \
	".\implode.lib" \
	".\Storm.lib"

"$(OUTDIR)\Diablo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

OUTDIR=.\WinDebug
INTDIR=.\WinDebug
# Begin Custom Macros
OutDir=.\.\WinDebug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Diablo.exe"

!ELSE 

ALL : "$(OUTDIR)\Diablo.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\appfat.obj"
	-@erase "$(INTDIR)\automap.obj"
	-@erase "$(INTDIR)\capture.obj"
	-@erase "$(INTDIR)\CODEC.OBJ"
	-@erase "$(INTDIR)\CONTROL.OBJ"
	-@erase "$(INTDIR)\CURSOR.OBJ"
	-@erase "$(INTDIR)\DEAD.OBJ"
	-@erase "$(INTDIR)\DEBUG.OBJ"
	-@erase "$(INTDIR)\DIABLO.OBJ"
	-@erase "$(INTDIR)\diablo.res"
	-@erase "$(INTDIR)\doom.obj"
	-@erase "$(INTDIR)\DRLG_L1.OBJ"
	-@erase "$(INTDIR)\DRLG_L2.OBJ"
	-@erase "$(INTDIR)\DRLG_L3.OBJ"
	-@erase "$(INTDIR)\drlg_l4.obj"
	-@erase "$(INTDIR)\dthread.obj"
	-@erase "$(INTDIR)\dx.obj"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\encrypt.obj"
	-@erase "$(INTDIR)\ENGINE.OBJ"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\except.obj"
	-@erase "$(INTDIR)\GameMenu.obj"
	-@erase "$(INTDIR)\GENDUNG.OBJ"
	-@erase "$(INTDIR)\gmenu.obj"
	-@erase "$(INTDIR)\help.obj"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\Interfac.obj"
	-@erase "$(INTDIR)\inv.obj"
	-@erase "$(INTDIR)\itemdat.obj"
	-@erase "$(INTDIR)\ITEMS.OBJ"
	-@erase "$(INTDIR)\LIGHTING.OBJ"
	-@erase "$(INTDIR)\loadsave.obj"
	-@erase "$(INTDIR)\mainmenu.obj"
	-@erase "$(INTDIR)\minitext.obj"
	-@erase "$(INTDIR)\misdat.obj"
	-@erase "$(INTDIR)\MISSILES.OBJ"
	-@erase "$(INTDIR)\MONSTDAT.OBJ"
	-@erase "$(INTDIR)\MONSTER.OBJ"
	-@erase "$(INTDIR)\movie.obj"
	-@erase "$(INTDIR)\mpqapi.obj"
	-@erase "$(INTDIR)\msg.obj"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\nthread.obj"
	-@erase "$(INTDIR)\objdat.obj"
	-@erase "$(INTDIR)\OBJECTS.OBJ"
	-@erase "$(INTDIR)\packplr.obj"
	-@erase "$(INTDIR)\PALETTE.OBJ"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\pfile.obj"
	-@erase "$(INTDIR)\PLAYER.OBJ"
	-@erase "$(INTDIR)\plrmsg.obj"
	-@erase "$(INTDIR)\portal.obj"
	-@erase "$(INTDIR)\Quests.obj"
	-@erase "$(INTDIR)\SCROLLRT.OBJ"
	-@erase "$(INTDIR)\SetMaps.obj"
	-@erase "$(INTDIR)\SHA.OBJ"
	-@erase "$(INTDIR)\SOUND.OBJ"
	-@erase "$(INTDIR)\Spelldat.obj"
	-@erase "$(INTDIR)\SPELLS.OBJ"
	-@erase "$(INTDIR)\stores.obj"
	-@erase "$(INTDIR)\sync.obj"
	-@erase "$(INTDIR)\Textdat.obj"
	-@erase "$(INTDIR)\themes.obj"
	-@erase "$(INTDIR)\tmsg.obj"
	-@erase "$(INTDIR)\TOWN.OBJ"
	-@erase "$(INTDIR)\towners.obj"
	-@erase "$(INTDIR)\track.obj"
	-@erase "$(INTDIR)\TRIGS.OBJ"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\Diablo.exe"
	-@erase "$(OUTDIR)\Diablo.map"
	-@erase "$(OUTDIR)\Diablo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Gr /MT /W3 /Gm /Zi /Od /D "WIN32" /D "_WINDOWS" /D\
 "_DEBUG" /D PROGRAM_VERSION=RETAIL /FAcs /Fa"$(INTDIR)\\"\
 /Fp"$(INTDIR)\Diablo.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\diablo.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Diablo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\Diablo.pdb" /map:"$(INTDIR)\Diablo.map" /debug\
 /machine:I386 /nodefaultlib /out:"$(OUTDIR)\Diablo.exe" 
LINK32_OBJS= \
	"$(INTDIR)\appfat.obj" \
	"$(INTDIR)\automap.obj" \
	"$(INTDIR)\capture.obj" \
	"$(INTDIR)\CODEC.OBJ" \
	"$(INTDIR)\CONTROL.OBJ" \
	"$(INTDIR)\CURSOR.OBJ" \
	"$(INTDIR)\DEAD.OBJ" \
	"$(INTDIR)\DEBUG.OBJ" \
	"$(INTDIR)\DIABLO.OBJ" \
	"$(INTDIR)\diablo.res" \
	"$(INTDIR)\doom.obj" \
	"$(INTDIR)\DRLG_L1.OBJ" \
	"$(INTDIR)\DRLG_L2.OBJ" \
	"$(INTDIR)\DRLG_L3.OBJ" \
	"$(INTDIR)\drlg_l4.obj" \
	"$(INTDIR)\dthread.obj" \
	"$(INTDIR)\dx.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\encrypt.obj" \
	"$(INTDIR)\ENGINE.OBJ" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\except.obj" \
	"$(INTDIR)\GameMenu.obj" \
	"$(INTDIR)\GENDUNG.OBJ" \
	"$(INTDIR)\gmenu.obj" \
	"$(INTDIR)\help.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\Interfac.obj" \
	"$(INTDIR)\inv.obj" \
	"$(INTDIR)\itemdat.obj" \
	"$(INTDIR)\ITEMS.OBJ" \
	"$(INTDIR)\LIGHTING.OBJ" \
	"$(INTDIR)\loadsave.obj" \
	"$(INTDIR)\mainmenu.obj" \
	"$(INTDIR)\minitext.obj" \
	"$(INTDIR)\misdat.obj" \
	"$(INTDIR)\MISSILES.OBJ" \
	"$(INTDIR)\MONSTDAT.OBJ" \
	"$(INTDIR)\MONSTER.OBJ" \
	"$(INTDIR)\movie.obj" \
	"$(INTDIR)\mpqapi.obj" \
	"$(INTDIR)\msg.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\nthread.obj" \
	"$(INTDIR)\objdat.obj" \
	"$(INTDIR)\OBJECTS.OBJ" \
	"$(INTDIR)\packplr.obj" \
	"$(INTDIR)\PALETTE.OBJ" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\pfile.obj" \
	"$(INTDIR)\PLAYER.OBJ" \
	"$(INTDIR)\plrmsg.obj" \
	"$(INTDIR)\portal.obj" \
	"$(INTDIR)\Quests.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\SCROLLRT.OBJ" \
	"$(INTDIR)\SetMaps.obj" \
	"$(INTDIR)\SHA.OBJ" \
	"$(INTDIR)\SOUND.OBJ" \
	"$(INTDIR)\Spelldat.obj" \
	"$(INTDIR)\SPELLS.OBJ" \
	"$(INTDIR)\stores.obj" \
	"$(INTDIR)\sync.obj" \
	"$(INTDIR)\Textdat.obj" \
	"$(INTDIR)\themes.obj" \
	"$(INTDIR)\tmsg.obj" \
	"$(INTDIR)\TOWN.OBJ" \
	"$(INTDIR)\towners.obj" \
	"$(INTDIR)\track.obj" \
	"$(INTDIR)\TRIGS.OBJ" \
	"$(INTDIR)\wave.obj" \
	".\ddraw.lib" \
	".\dsound.lib" \
	".\Hellfrui.lib" \
	".\implode.lib" \
	".\Storm.lib"

"$(OUTDIR)\Diablo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

OUTDIR=.\WinFinal
INTDIR=.\WinFinal
# Begin Custom Macros
OutDir=.\.\WinFinal
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Diablo.exe"

!ELSE 

ALL : "$(OUTDIR)\Diablo.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\appfat.obj"
	-@erase "$(INTDIR)\automap.obj"
	-@erase "$(INTDIR)\capture.obj"
	-@erase "$(INTDIR)\CODEC.OBJ"
	-@erase "$(INTDIR)\CONTROL.OBJ"
	-@erase "$(INTDIR)\CURSOR.OBJ"
	-@erase "$(INTDIR)\DEAD.OBJ"
	-@erase "$(INTDIR)\DEBUG.OBJ"
	-@erase "$(INTDIR)\DIABLO.OBJ"
	-@erase "$(INTDIR)\diablo.res"
	-@erase "$(INTDIR)\doom.obj"
	-@erase "$(INTDIR)\DRLG_L1.OBJ"
	-@erase "$(INTDIR)\DRLG_L2.OBJ"
	-@erase "$(INTDIR)\DRLG_L3.OBJ"
	-@erase "$(INTDIR)\drlg_l4.obj"
	-@erase "$(INTDIR)\dthread.obj"
	-@erase "$(INTDIR)\dx.obj"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\encrypt.obj"
	-@erase "$(INTDIR)\ENGINE.OBJ"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\except.obj"
	-@erase "$(INTDIR)\GameMenu.obj"
	-@erase "$(INTDIR)\GENDUNG.OBJ"
	-@erase "$(INTDIR)\gmenu.obj"
	-@erase "$(INTDIR)\help.obj"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\Interfac.obj"
	-@erase "$(INTDIR)\inv.obj"
	-@erase "$(INTDIR)\itemdat.obj"
	-@erase "$(INTDIR)\ITEMS.OBJ"
	-@erase "$(INTDIR)\LIGHTING.OBJ"
	-@erase "$(INTDIR)\loadsave.obj"
	-@erase "$(INTDIR)\mainmenu.obj"
	-@erase "$(INTDIR)\minitext.obj"
	-@erase "$(INTDIR)\misdat.obj"
	-@erase "$(INTDIR)\MISSILES.OBJ"
	-@erase "$(INTDIR)\MONSTDAT.OBJ"
	-@erase "$(INTDIR)\MONSTER.OBJ"
	-@erase "$(INTDIR)\movie.obj"
	-@erase "$(INTDIR)\mpqapi.obj"
	-@erase "$(INTDIR)\msg.obj"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\nthread.obj"
	-@erase "$(INTDIR)\objdat.obj"
	-@erase "$(INTDIR)\OBJECTS.OBJ"
	-@erase "$(INTDIR)\packplr.obj"
	-@erase "$(INTDIR)\PALETTE.OBJ"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\pfile.obj"
	-@erase "$(INTDIR)\PLAYER.OBJ"
	-@erase "$(INTDIR)\plrmsg.obj"
	-@erase "$(INTDIR)\portal.obj"
	-@erase "$(INTDIR)\Quests.obj"
	-@erase "$(INTDIR)\SCROLLRT.OBJ"
	-@erase "$(INTDIR)\SetMaps.obj"
	-@erase "$(INTDIR)\SHA.OBJ"
	-@erase "$(INTDIR)\SOUND.OBJ"
	-@erase "$(INTDIR)\Spelldat.obj"
	-@erase "$(INTDIR)\SPELLS.OBJ"
	-@erase "$(INTDIR)\stores.obj"
	-@erase "$(INTDIR)\sync.obj"
	-@erase "$(INTDIR)\Textdat.obj"
	-@erase "$(INTDIR)\themes.obj"
	-@erase "$(INTDIR)\tmsg.obj"
	-@erase "$(INTDIR)\TOWN.OBJ"
	-@erase "$(INTDIR)\towners.obj"
	-@erase "$(INTDIR)\track.obj"
	-@erase "$(INTDIR)\TRIGS.OBJ"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\Diablo.exe"
	-@erase "$(OUTDIR)\Diablo.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG"\
 /D PROGRAM_VERSION=RETAIL /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\Diablo.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\WinFinal/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\diablo.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Diablo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\Diablo.pdb" /map:"$(INTDIR)\Diablo.map"\
 /machine:I386 /nodefaultlib /out:"$(OUTDIR)\Diablo.exe" 
LINK32_OBJS= \
	"$(INTDIR)\appfat.obj" \
	"$(INTDIR)\automap.obj" \
	"$(INTDIR)\capture.obj" \
	"$(INTDIR)\CODEC.OBJ" \
	"$(INTDIR)\CONTROL.OBJ" \
	"$(INTDIR)\CURSOR.OBJ" \
	"$(INTDIR)\DEAD.OBJ" \
	"$(INTDIR)\DEBUG.OBJ" \
	"$(INTDIR)\DIABLO.OBJ" \
	"$(INTDIR)\diablo.res" \
	"$(INTDIR)\doom.obj" \
	"$(INTDIR)\DRLG_L1.OBJ" \
	"$(INTDIR)\DRLG_L2.OBJ" \
	"$(INTDIR)\DRLG_L3.OBJ" \
	"$(INTDIR)\drlg_l4.obj" \
	"$(INTDIR)\dthread.obj" \
	"$(INTDIR)\dx.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\encrypt.obj" \
	"$(INTDIR)\ENGINE.OBJ" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\except.obj" \
	"$(INTDIR)\GameMenu.obj" \
	"$(INTDIR)\GENDUNG.OBJ" \
	"$(INTDIR)\gmenu.obj" \
	"$(INTDIR)\help.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\Interfac.obj" \
	"$(INTDIR)\inv.obj" \
	"$(INTDIR)\itemdat.obj" \
	"$(INTDIR)\ITEMS.OBJ" \
	"$(INTDIR)\LIGHTING.OBJ" \
	"$(INTDIR)\loadsave.obj" \
	"$(INTDIR)\mainmenu.obj" \
	"$(INTDIR)\minitext.obj" \
	"$(INTDIR)\misdat.obj" \
	"$(INTDIR)\MISSILES.OBJ" \
	"$(INTDIR)\MONSTDAT.OBJ" \
	"$(INTDIR)\MONSTER.OBJ" \
	"$(INTDIR)\movie.obj" \
	"$(INTDIR)\mpqapi.obj" \
	"$(INTDIR)\msg.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\nthread.obj" \
	"$(INTDIR)\objdat.obj" \
	"$(INTDIR)\OBJECTS.OBJ" \
	"$(INTDIR)\packplr.obj" \
	"$(INTDIR)\PALETTE.OBJ" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\pfile.obj" \
	"$(INTDIR)\PLAYER.OBJ" \
	"$(INTDIR)\plrmsg.obj" \
	"$(INTDIR)\portal.obj" \
	"$(INTDIR)\Quests.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\SCROLLRT.OBJ" \
	"$(INTDIR)\SetMaps.obj" \
	"$(INTDIR)\SHA.OBJ" \
	"$(INTDIR)\SOUND.OBJ" \
	"$(INTDIR)\Spelldat.obj" \
	"$(INTDIR)\SPELLS.OBJ" \
	"$(INTDIR)\stores.obj" \
	"$(INTDIR)\sync.obj" \
	"$(INTDIR)\Textdat.obj" \
	"$(INTDIR)\themes.obj" \
	"$(INTDIR)\tmsg.obj" \
	"$(INTDIR)\TOWN.OBJ" \
	"$(INTDIR)\towners.obj" \
	"$(INTDIR)\track.obj" \
	"$(INTDIR)\TRIGS.OBJ" \
	"$(INTDIR)\wave.obj" \
	".\ddraw.lib" \
	".\dsound.lib" \
	".\Hellfrui.lib" \
	".\implode.lib" \
	".\Storm.lib"

"$(OUTDIR)\Diablo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

OUTDIR=.\SFinal
INTDIR=.\SFinal
# Begin Custom Macros
OutDir=.\.\SFinal
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Diablo.exe"

!ELSE 

ALL : "$(OUTDIR)\Diablo.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\appfat.obj"
	-@erase "$(INTDIR)\automap.obj"
	-@erase "$(INTDIR)\capture.obj"
	-@erase "$(INTDIR)\CODEC.OBJ"
	-@erase "$(INTDIR)\CONTROL.OBJ"
	-@erase "$(INTDIR)\CURSOR.OBJ"
	-@erase "$(INTDIR)\DEAD.OBJ"
	-@erase "$(INTDIR)\DEBUG.OBJ"
	-@erase "$(INTDIR)\DIABLO.OBJ"
	-@erase "$(INTDIR)\diablo.res"
	-@erase "$(INTDIR)\doom.obj"
	-@erase "$(INTDIR)\DRLG_L1.OBJ"
	-@erase "$(INTDIR)\DRLG_L2.OBJ"
	-@erase "$(INTDIR)\DRLG_L3.OBJ"
	-@erase "$(INTDIR)\drlg_l4.obj"
	-@erase "$(INTDIR)\dthread.obj"
	-@erase "$(INTDIR)\dx.obj"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\encrypt.obj"
	-@erase "$(INTDIR)\ENGINE.OBJ"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\except.obj"
	-@erase "$(INTDIR)\GameMenu.obj"
	-@erase "$(INTDIR)\GENDUNG.OBJ"
	-@erase "$(INTDIR)\gmenu.obj"
	-@erase "$(INTDIR)\help.obj"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\Interfac.obj"
	-@erase "$(INTDIR)\inv.obj"
	-@erase "$(INTDIR)\itemdat.obj"
	-@erase "$(INTDIR)\ITEMS.OBJ"
	-@erase "$(INTDIR)\LIGHTING.OBJ"
	-@erase "$(INTDIR)\loadsave.obj"
	-@erase "$(INTDIR)\mainmenu.obj"
	-@erase "$(INTDIR)\minitext.obj"
	-@erase "$(INTDIR)\misdat.obj"
	-@erase "$(INTDIR)\MISSILES.OBJ"
	-@erase "$(INTDIR)\MONSTDAT.OBJ"
	-@erase "$(INTDIR)\MONSTER.OBJ"
	-@erase "$(INTDIR)\movie.obj"
	-@erase "$(INTDIR)\mpqapi.obj"
	-@erase "$(INTDIR)\msg.obj"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\nthread.obj"
	-@erase "$(INTDIR)\objdat.obj"
	-@erase "$(INTDIR)\OBJECTS.OBJ"
	-@erase "$(INTDIR)\packplr.obj"
	-@erase "$(INTDIR)\PALETTE.OBJ"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\pfile.obj"
	-@erase "$(INTDIR)\PLAYER.OBJ"
	-@erase "$(INTDIR)\plrmsg.obj"
	-@erase "$(INTDIR)\portal.obj"
	-@erase "$(INTDIR)\Quests.obj"
	-@erase "$(INTDIR)\SCROLLRT.OBJ"
	-@erase "$(INTDIR)\SetMaps.obj"
	-@erase "$(INTDIR)\SHA.OBJ"
	-@erase "$(INTDIR)\SOUND.OBJ"
	-@erase "$(INTDIR)\Spelldat.obj"
	-@erase "$(INTDIR)\SPELLS.OBJ"
	-@erase "$(INTDIR)\stores.obj"
	-@erase "$(INTDIR)\sync.obj"
	-@erase "$(INTDIR)\Textdat.obj"
	-@erase "$(INTDIR)\themes.obj"
	-@erase "$(INTDIR)\tmsg.obj"
	-@erase "$(INTDIR)\TOWN.OBJ"
	-@erase "$(INTDIR)\towners.obj"
	-@erase "$(INTDIR)\track.obj"
	-@erase "$(INTDIR)\TRIGS.OBJ"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\Diablo.exe"
	-@erase "$(OUTDIR)\Diablo.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG"\
 /D PROGRAM_VERSION=SHAREWARE /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\Diablo.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\SFinal/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\diablo.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Diablo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\Diablo.pdb" /map:"$(INTDIR)\Diablo.map"\
 /machine:I386 /nodefaultlib /out:"$(OUTDIR)\Diablo.exe" 
LINK32_OBJS= \
	"$(INTDIR)\appfat.obj" \
	"$(INTDIR)\automap.obj" \
	"$(INTDIR)\capture.obj" \
	"$(INTDIR)\CODEC.OBJ" \
	"$(INTDIR)\CONTROL.OBJ" \
	"$(INTDIR)\CURSOR.OBJ" \
	"$(INTDIR)\DEAD.OBJ" \
	"$(INTDIR)\DEBUG.OBJ" \
	"$(INTDIR)\DIABLO.OBJ" \
	"$(INTDIR)\diablo.res" \
	"$(INTDIR)\doom.obj" \
	"$(INTDIR)\DRLG_L1.OBJ" \
	"$(INTDIR)\DRLG_L2.OBJ" \
	"$(INTDIR)\DRLG_L3.OBJ" \
	"$(INTDIR)\drlg_l4.obj" \
	"$(INTDIR)\dthread.obj" \
	"$(INTDIR)\dx.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\encrypt.obj" \
	"$(INTDIR)\ENGINE.OBJ" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\except.obj" \
	"$(INTDIR)\GameMenu.obj" \
	"$(INTDIR)\GENDUNG.OBJ" \
	"$(INTDIR)\gmenu.obj" \
	"$(INTDIR)\help.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\Interfac.obj" \
	"$(INTDIR)\inv.obj" \
	"$(INTDIR)\itemdat.obj" \
	"$(INTDIR)\ITEMS.OBJ" \
	"$(INTDIR)\LIGHTING.OBJ" \
	"$(INTDIR)\loadsave.obj" \
	"$(INTDIR)\mainmenu.obj" \
	"$(INTDIR)\minitext.obj" \
	"$(INTDIR)\misdat.obj" \
	"$(INTDIR)\MISSILES.OBJ" \
	"$(INTDIR)\MONSTDAT.OBJ" \
	"$(INTDIR)\MONSTER.OBJ" \
	"$(INTDIR)\movie.obj" \
	"$(INTDIR)\mpqapi.obj" \
	"$(INTDIR)\msg.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\nthread.obj" \
	"$(INTDIR)\objdat.obj" \
	"$(INTDIR)\OBJECTS.OBJ" \
	"$(INTDIR)\packplr.obj" \
	"$(INTDIR)\PALETTE.OBJ" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\pfile.obj" \
	"$(INTDIR)\PLAYER.OBJ" \
	"$(INTDIR)\plrmsg.obj" \
	"$(INTDIR)\portal.obj" \
	"$(INTDIR)\Quests.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\SCROLLRT.OBJ" \
	"$(INTDIR)\SetMaps.obj" \
	"$(INTDIR)\SHA.OBJ" \
	"$(INTDIR)\SOUND.OBJ" \
	"$(INTDIR)\Spelldat.obj" \
	"$(INTDIR)\SPELLS.OBJ" \
	"$(INTDIR)\stores.obj" \
	"$(INTDIR)\sync.obj" \
	"$(INTDIR)\Textdat.obj" \
	"$(INTDIR)\themes.obj" \
	"$(INTDIR)\tmsg.obj" \
	"$(INTDIR)\TOWN.OBJ" \
	"$(INTDIR)\towners.obj" \
	"$(INTDIR)\track.obj" \
	"$(INTDIR)\TRIGS.OBJ" \
	"$(INTDIR)\wave.obj" \
	".\ddraw.lib" \
	".\dsound.lib" \
	".\Hellfrui.lib" \
	".\implode.lib" \
	".\Storm.lib"

"$(OUTDIR)\Diablo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

OUTDIR=.\SRel
INTDIR=.\SRel
# Begin Custom Macros
OutDir=.\.\SRel
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Diablo.exe"

!ELSE 

ALL : "$(OUTDIR)\Diablo.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\appfat.obj"
	-@erase "$(INTDIR)\automap.obj"
	-@erase "$(INTDIR)\capture.obj"
	-@erase "$(INTDIR)\CODEC.OBJ"
	-@erase "$(INTDIR)\CONTROL.OBJ"
	-@erase "$(INTDIR)\CURSOR.OBJ"
	-@erase "$(INTDIR)\DEAD.OBJ"
	-@erase "$(INTDIR)\DEBUG.OBJ"
	-@erase "$(INTDIR)\DIABLO.OBJ"
	-@erase "$(INTDIR)\diablo.res"
	-@erase "$(INTDIR)\doom.obj"
	-@erase "$(INTDIR)\DRLG_L1.OBJ"
	-@erase "$(INTDIR)\DRLG_L2.OBJ"
	-@erase "$(INTDIR)\DRLG_L3.OBJ"
	-@erase "$(INTDIR)\drlg_l4.obj"
	-@erase "$(INTDIR)\dthread.obj"
	-@erase "$(INTDIR)\dx.obj"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\encrypt.obj"
	-@erase "$(INTDIR)\ENGINE.OBJ"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\except.obj"
	-@erase "$(INTDIR)\GameMenu.obj"
	-@erase "$(INTDIR)\GENDUNG.OBJ"
	-@erase "$(INTDIR)\gmenu.obj"
	-@erase "$(INTDIR)\help.obj"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\Interfac.obj"
	-@erase "$(INTDIR)\inv.obj"
	-@erase "$(INTDIR)\itemdat.obj"
	-@erase "$(INTDIR)\ITEMS.OBJ"
	-@erase "$(INTDIR)\LIGHTING.OBJ"
	-@erase "$(INTDIR)\loadsave.obj"
	-@erase "$(INTDIR)\mainmenu.obj"
	-@erase "$(INTDIR)\minitext.obj"
	-@erase "$(INTDIR)\misdat.obj"
	-@erase "$(INTDIR)\MISSILES.OBJ"
	-@erase "$(INTDIR)\MONSTDAT.OBJ"
	-@erase "$(INTDIR)\MONSTER.OBJ"
	-@erase "$(INTDIR)\movie.obj"
	-@erase "$(INTDIR)\mpqapi.obj"
	-@erase "$(INTDIR)\msg.obj"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\nthread.obj"
	-@erase "$(INTDIR)\objdat.obj"
	-@erase "$(INTDIR)\OBJECTS.OBJ"
	-@erase "$(INTDIR)\packplr.obj"
	-@erase "$(INTDIR)\PALETTE.OBJ"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\pfile.obj"
	-@erase "$(INTDIR)\PLAYER.OBJ"
	-@erase "$(INTDIR)\plrmsg.obj"
	-@erase "$(INTDIR)\portal.obj"
	-@erase "$(INTDIR)\Quests.obj"
	-@erase "$(INTDIR)\SCROLLRT.OBJ"
	-@erase "$(INTDIR)\SetMaps.obj"
	-@erase "$(INTDIR)\SHA.OBJ"
	-@erase "$(INTDIR)\SOUND.OBJ"
	-@erase "$(INTDIR)\Spelldat.obj"
	-@erase "$(INTDIR)\SPELLS.OBJ"
	-@erase "$(INTDIR)\stores.obj"
	-@erase "$(INTDIR)\sync.obj"
	-@erase "$(INTDIR)\Textdat.obj"
	-@erase "$(INTDIR)\themes.obj"
	-@erase "$(INTDIR)\tmsg.obj"
	-@erase "$(INTDIR)\TOWN.OBJ"
	-@erase "$(INTDIR)\towners.obj"
	-@erase "$(INTDIR)\track.obj"
	-@erase "$(INTDIR)\TRIGS.OBJ"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\Diablo.exe"
	-@erase "$(OUTDIR)\Diablo.map"
	-@erase "$(OUTDIR)\Diablo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Gr /MT /W3 /Zi /O1 /D "WIN32" /D "_WINDOWS" /D "_DEBUG"\
 /D PROGRAM_VERSION=SHAREWARE /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\Diablo.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\SRel/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\diablo.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Diablo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib winmm.lib kernel32.lib user32.lib gdi32.lib\
 comdlg32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\Diablo.pdb" /map:"$(INTDIR)\Diablo.map" /debug\
 /machine:I386 /nodefaultlib /out:"$(OUTDIR)\Diablo.exe" 
LINK32_OBJS= \
	"$(INTDIR)\appfat.obj" \
	"$(INTDIR)\automap.obj" \
	"$(INTDIR)\capture.obj" \
	"$(INTDIR)\CODEC.OBJ" \
	"$(INTDIR)\CONTROL.OBJ" \
	"$(INTDIR)\CURSOR.OBJ" \
	"$(INTDIR)\DEAD.OBJ" \
	"$(INTDIR)\DEBUG.OBJ" \
	"$(INTDIR)\DIABLO.OBJ" \
	"$(INTDIR)\diablo.res" \
	"$(INTDIR)\doom.obj" \
	"$(INTDIR)\DRLG_L1.OBJ" \
	"$(INTDIR)\DRLG_L2.OBJ" \
	"$(INTDIR)\DRLG_L3.OBJ" \
	"$(INTDIR)\drlg_l4.obj" \
	"$(INTDIR)\dthread.obj" \
	"$(INTDIR)\dx.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\encrypt.obj" \
	"$(INTDIR)\ENGINE.OBJ" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\except.obj" \
	"$(INTDIR)\GameMenu.obj" \
	"$(INTDIR)\GENDUNG.OBJ" \
	"$(INTDIR)\gmenu.obj" \
	"$(INTDIR)\help.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\Interfac.obj" \
	"$(INTDIR)\inv.obj" \
	"$(INTDIR)\itemdat.obj" \
	"$(INTDIR)\ITEMS.OBJ" \
	"$(INTDIR)\LIGHTING.OBJ" \
	"$(INTDIR)\loadsave.obj" \
	"$(INTDIR)\mainmenu.obj" \
	"$(INTDIR)\minitext.obj" \
	"$(INTDIR)\misdat.obj" \
	"$(INTDIR)\MISSILES.OBJ" \
	"$(INTDIR)\MONSTDAT.OBJ" \
	"$(INTDIR)\MONSTER.OBJ" \
	"$(INTDIR)\movie.obj" \
	"$(INTDIR)\mpqapi.obj" \
	"$(INTDIR)\msg.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\nthread.obj" \
	"$(INTDIR)\objdat.obj" \
	"$(INTDIR)\OBJECTS.OBJ" \
	"$(INTDIR)\packplr.obj" \
	"$(INTDIR)\PALETTE.OBJ" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\pfile.obj" \
	"$(INTDIR)\PLAYER.OBJ" \
	"$(INTDIR)\plrmsg.obj" \
	"$(INTDIR)\portal.obj" \
	"$(INTDIR)\Quests.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\SCROLLRT.OBJ" \
	"$(INTDIR)\SetMaps.obj" \
	"$(INTDIR)\SHA.OBJ" \
	"$(INTDIR)\SOUND.OBJ" \
	"$(INTDIR)\Spelldat.obj" \
	"$(INTDIR)\SPELLS.OBJ" \
	"$(INTDIR)\stores.obj" \
	"$(INTDIR)\sync.obj" \
	"$(INTDIR)\Textdat.obj" \
	"$(INTDIR)\themes.obj" \
	"$(INTDIR)\tmsg.obj" \
	"$(INTDIR)\TOWN.OBJ" \
	"$(INTDIR)\towners.obj" \
	"$(INTDIR)\track.obj" \
	"$(INTDIR)\TRIGS.OBJ" \
	"$(INTDIR)\wave.obj" \
	".\ddraw.lib" \
	".\dsound.lib" \
	".\Hellfrui.lib" \
	".\implode.lib" \
	".\Storm.lib"

"$(OUTDIR)\Diablo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "Diablo - Win32 Release" || "$(CFG)" == "Diablo - Win32 Debug"\
 || "$(CFG)" == "Diablo - Win32 FinalFinal" || "$(CFG)" ==\
 "Diablo - Win32 Shareware FinalFinal" || "$(CFG)" ==\
 "Diablo - Win32 Shareware Release"
SOURCE=.\appfat.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_APPFA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Storm.h"\
	

"$(INTDIR)\appfat.obj"	"$(INTDIR)\appfat.sbr" : $(SOURCE) $(DEP_CPP_APPFA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_APPFA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Storm.h"\
	

"$(INTDIR)\appfat.obj" : $(SOURCE) $(DEP_CPP_APPFA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_APPFA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Storm.h"\
	

"$(INTDIR)\appfat.obj" : $(SOURCE) $(DEP_CPP_APPFA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_APPFA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Storm.h"\
	

"$(INTDIR)\appfat.obj" : $(SOURCE) $(DEP_CPP_APPFA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_APPFA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Storm.h"\
	

"$(INTDIR)\appfat.obj" : $(SOURCE) $(DEP_CPP_APPFA) "$(INTDIR)"


!ENDIF 

SOURCE=.\automap.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_AUTOM=\
	".\automap.h"\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	

"$(INTDIR)\automap.obj"	"$(INTDIR)\automap.sbr" : $(SOURCE) $(DEP_CPP_AUTOM)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_AUTOM=\
	".\automap.h"\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	

"$(INTDIR)\automap.obj" : $(SOURCE) $(DEP_CPP_AUTOM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_AUTOM=\
	".\automap.h"\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	

"$(INTDIR)\automap.obj" : $(SOURCE) $(DEP_CPP_AUTOM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_AUTOM=\
	".\automap.h"\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	

"$(INTDIR)\automap.obj" : $(SOURCE) $(DEP_CPP_AUTOM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_AUTOM=\
	".\automap.h"\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	

"$(INTDIR)\automap.obj" : $(SOURCE) $(DEP_CPP_AUTOM) "$(INTDIR)"


!ENDIF 

SOURCE=.\capture.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_CAPTU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\capture.obj"	"$(INTDIR)\capture.sbr" : $(SOURCE) $(DEP_CPP_CAPTU)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_CAPTU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\capture.obj" : $(SOURCE) $(DEP_CPP_CAPTU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_CAPTU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\capture.obj" : $(SOURCE) $(DEP_CPP_CAPTU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_CAPTU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\capture.obj" : $(SOURCE) $(DEP_CPP_CAPTU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_CAPTU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\capture.obj" : $(SOURCE) $(DEP_CPP_CAPTU) "$(INTDIR)"


!ENDIF 

SOURCE=.\CODEC.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_CODEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\CODEC.OBJ"	"$(INTDIR)\CODEC.SBR" : $(SOURCE) $(DEP_CPP_CODEC)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_CODEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\CODEC.OBJ" : $(SOURCE) $(DEP_CPP_CODEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_CODEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\CODEC.OBJ" : $(SOURCE) $(DEP_CPP_CODEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_CODEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\CODEC.OBJ" : $(SOURCE) $(DEP_CPP_CODEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_CODEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\CODEC.OBJ" : $(SOURCE) $(DEP_CPP_CODEC) "$(INTDIR)"


!ENDIF 

SOURCE=.\CONTROL.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_CONTR=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CONTROL.OBJ"	"$(INTDIR)\CONTROL.SBR" : $(SOURCE) $(DEP_CPP_CONTR)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_CONTR=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CONTROL.OBJ" : $(SOURCE) $(DEP_CPP_CONTR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_CONTR=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CONTROL.OBJ" : $(SOURCE) $(DEP_CPP_CONTR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_CONTR=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CONTROL.OBJ" : $(SOURCE) $(DEP_CPP_CONTR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_CONTR=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CONTROL.OBJ" : $(SOURCE) $(DEP_CPP_CONTR) "$(INTDIR)"


!ENDIF 

SOURCE=.\CURSOR.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_CURSO=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CURSOR.OBJ"	"$(INTDIR)\CURSOR.SBR" : $(SOURCE) $(DEP_CPP_CURSO)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_CURSO=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CURSOR.OBJ" : $(SOURCE) $(DEP_CPP_CURSO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_CURSO=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CURSOR.OBJ" : $(SOURCE) $(DEP_CPP_CURSO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_CURSO=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CURSOR.OBJ" : $(SOURCE) $(DEP_CPP_CURSO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_CURSO=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\CURSOR.OBJ" : $(SOURCE) $(DEP_CPP_CURSO) "$(INTDIR)"


!ENDIF 

SOURCE=.\DEAD.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DEAD_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Sound.h"\
	

"$(INTDIR)\DEAD.OBJ"	"$(INTDIR)\DEAD.SBR" : $(SOURCE) $(DEP_CPP_DEAD_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DEAD_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Sound.h"\
	

"$(INTDIR)\DEAD.OBJ" : $(SOURCE) $(DEP_CPP_DEAD_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DEAD_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Sound.h"\
	

"$(INTDIR)\DEAD.OBJ" : $(SOURCE) $(DEP_CPP_DEAD_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DEAD_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Sound.h"\
	

"$(INTDIR)\DEAD.OBJ" : $(SOURCE) $(DEP_CPP_DEAD_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DEAD_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Sound.h"\
	

"$(INTDIR)\DEAD.OBJ" : $(SOURCE) $(DEP_CPP_DEAD_) "$(INTDIR)"


!ENDIF 

SOURCE=.\DEBUG.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DEBUG=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	

"$(INTDIR)\DEBUG.OBJ"	"$(INTDIR)\DEBUG.SBR" : $(SOURCE) $(DEP_CPP_DEBUG)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DEBUG=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	

"$(INTDIR)\DEBUG.OBJ" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DEBUG=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	

"$(INTDIR)\DEBUG.OBJ" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DEBUG=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	

"$(INTDIR)\DEBUG.OBJ" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DEBUG=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	

"$(INTDIR)\DEBUG.OBJ" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


!ENDIF 

SOURCE=.\DIABLO.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DIABL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\mainmenu.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\mpqapi.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DIABLO.OBJ"	"$(INTDIR)\DIABLO.SBR" : $(SOURCE) $(DEP_CPP_DIABL)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DIABL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\mainmenu.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\mpqapi.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DIABLO.OBJ" : $(SOURCE) $(DEP_CPP_DIABL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DIABL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\mainmenu.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\mpqapi.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DIABLO.OBJ" : $(SOURCE) $(DEP_CPP_DIABL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DIABL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\mainmenu.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\mpqapi.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DIABLO.OBJ" : $(SOURCE) $(DEP_CPP_DIABL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DIABL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\mainmenu.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\mpqapi.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DIABLO.OBJ" : $(SOURCE) $(DEP_CPP_DIABL) "$(INTDIR)"


!ENDIF 

SOURCE=.\diablo.rc
DEP_RSC_DIABLO=\
	".\icon1.ico"\
	

"$(INTDIR)\diablo.res" : $(SOURCE) $(DEP_RSC_DIABLO) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\doom.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DOOM_=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\doom.obj"	"$(INTDIR)\doom.sbr" : $(SOURCE) $(DEP_CPP_DOOM_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DOOM_=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\doom.obj" : $(SOURCE) $(DEP_CPP_DOOM_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DOOM_=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\doom.obj" : $(SOURCE) $(DEP_CPP_DOOM_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DOOM_=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\doom.obj" : $(SOURCE) $(DEP_CPP_DOOM_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DOOM_=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\doom.obj" : $(SOURCE) $(DEP_CPP_DOOM_) "$(INTDIR)"


!ENDIF 

SOURCE=.\DRLG_L1.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DRLG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L1.OBJ"	"$(INTDIR)\DRLG_L1.SBR" : $(SOURCE) $(DEP_CPP_DRLG_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DRLG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L1.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DRLG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L1.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DRLG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L1.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DRLG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L1.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_) "$(INTDIR)"


!ENDIF 

SOURCE=.\DRLG_L2.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DRLG_L=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l2.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L2.OBJ"	"$(INTDIR)\DRLG_L2.SBR" : $(SOURCE) $(DEP_CPP_DRLG_L)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DRLG_L=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l2.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L2.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_L) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DRLG_L=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l2.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L2.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_L) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DRLG_L=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l2.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L2.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_L) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DRLG_L=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l2.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L2.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_L) "$(INTDIR)"


!ENDIF 

SOURCE=.\DRLG_L3.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DRLG_L3=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l3.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L3.OBJ"	"$(INTDIR)\DRLG_L3.SBR" : $(SOURCE) $(DEP_CPP_DRLG_L3)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DRLG_L3=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l3.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L3.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_L3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DRLG_L3=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l3.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L3.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_L3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DRLG_L3=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l3.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L3.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_L3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DRLG_L3=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l3.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\DRLG_L3.OBJ" : $(SOURCE) $(DEP_CPP_DRLG_L3) "$(INTDIR)"


!ENDIF 

SOURCE=.\drlg_l4.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DRLG_L4=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\drlg_l4.obj"	"$(INTDIR)\drlg_l4.sbr" : $(SOURCE) $(DEP_CPP_DRLG_L4)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DRLG_L4=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\drlg_l4.obj" : $(SOURCE) $(DEP_CPP_DRLG_L4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DRLG_L4=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\drlg_l4.obj" : $(SOURCE) $(DEP_CPP_DRLG_L4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DRLG_L4=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\drlg_l4.obj" : $(SOURCE) $(DEP_CPP_DRLG_L4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DRLG_L4=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Trigs.h"\
	

"$(INTDIR)\drlg_l4.obj" : $(SOURCE) $(DEP_CPP_DRLG_L4) "$(INTDIR)"


!ENDIF 

SOURCE=.\dthread.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\dthread.obj"	"$(INTDIR)\dthread.sbr" : $(SOURCE) $(DEP_CPP_DTHRE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\dthread.obj" : $(SOURCE) $(DEP_CPP_DTHRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\dthread.obj" : $(SOURCE) $(DEP_CPP_DTHRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\dthread.obj" : $(SOURCE) $(DEP_CPP_DTHRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\dthread.obj" : $(SOURCE) $(DEP_CPP_DTHRE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dx.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_DX_CP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\dx.obj"	"$(INTDIR)\dx.sbr" : $(SOURCE) $(DEP_CPP_DX_CP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_DX_CP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\dx.obj" : $(SOURCE) $(DEP_CPP_DX_CP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_DX_CP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\dx.obj" : $(SOURCE) $(DEP_CPP_DX_CP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_DX_CP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\dx.obj" : $(SOURCE) $(DEP_CPP_DX_CP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_DX_CP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\dx.obj" : $(SOURCE) $(DEP_CPP_DX_CP) "$(INTDIR)"


!ENDIF 

SOURCE=.\effects.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_EFFEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\effects.obj"	"$(INTDIR)\effects.sbr" : $(SOURCE) $(DEP_CPP_EFFEC)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_EFFEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\effects.obj" : $(SOURCE) $(DEP_CPP_EFFEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_EFFEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\effects.obj" : $(SOURCE) $(DEP_CPP_EFFEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_EFFEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\effects.obj" : $(SOURCE) $(DEP_CPP_EFFEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_EFFEC=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\effects.obj" : $(SOURCE) $(DEP_CPP_EFFEC) "$(INTDIR)"


!ENDIF 

SOURCE=.\encrypt.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_ENCRY=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\implode.h"\
	".\mpqapi.h"\
	

"$(INTDIR)\encrypt.obj"	"$(INTDIR)\encrypt.sbr" : $(SOURCE) $(DEP_CPP_ENCRY)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_ENCRY=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\implode.h"\
	".\mpqapi.h"\
	

"$(INTDIR)\encrypt.obj" : $(SOURCE) $(DEP_CPP_ENCRY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_ENCRY=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\implode.h"\
	".\mpqapi.h"\
	

"$(INTDIR)\encrypt.obj" : $(SOURCE) $(DEP_CPP_ENCRY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_ENCRY=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\implode.h"\
	".\mpqapi.h"\
	

"$(INTDIR)\encrypt.obj" : $(SOURCE) $(DEP_CPP_ENCRY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_ENCRY=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\implode.h"\
	".\mpqapi.h"\
	

"$(INTDIR)\encrypt.obj" : $(SOURCE) $(DEP_CPP_ENCRY) "$(INTDIR)"


!ENDIF 

SOURCE=.\ENGINE.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_ENGIN=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Palette.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Storm.h"\
	

"$(INTDIR)\ENGINE.OBJ"	"$(INTDIR)\ENGINE.SBR" : $(SOURCE) $(DEP_CPP_ENGIN)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_ENGIN=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Palette.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Storm.h"\
	

"$(INTDIR)\ENGINE.OBJ" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_ENGIN=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Palette.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Storm.h"\
	

"$(INTDIR)\ENGINE.OBJ" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_ENGIN=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Palette.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Storm.h"\
	

"$(INTDIR)\ENGINE.OBJ" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_ENGIN=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Palette.h"\
	".\regconst.h"\
	".\Scrollrt.h"\
	".\Storm.h"\
	

"$(INTDIR)\ENGINE.OBJ" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"


!ENDIF 

SOURCE=.\error.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_ERROR=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Items.h"\
	".\Scrollrt.h"\
	".\stores.h"\
	

"$(INTDIR)\error.obj"	"$(INTDIR)\error.sbr" : $(SOURCE) $(DEP_CPP_ERROR)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_ERROR=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Items.h"\
	".\Scrollrt.h"\
	".\stores.h"\
	

"$(INTDIR)\error.obj" : $(SOURCE) $(DEP_CPP_ERROR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_ERROR=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Items.h"\
	".\Scrollrt.h"\
	".\stores.h"\
	

"$(INTDIR)\error.obj" : $(SOURCE) $(DEP_CPP_ERROR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_ERROR=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Items.h"\
	".\Scrollrt.h"\
	".\stores.h"\
	

"$(INTDIR)\error.obj" : $(SOURCE) $(DEP_CPP_ERROR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_ERROR=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Items.h"\
	".\Scrollrt.h"\
	".\stores.h"\
	

"$(INTDIR)\error.obj" : $(SOURCE) $(DEP_CPP_ERROR) "$(INTDIR)"


!ENDIF 

SOURCE=.\except.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_EXCEP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\except.obj"	"$(INTDIR)\except.sbr" : $(SOURCE) $(DEP_CPP_EXCEP)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_EXCEP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_EXCEP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_EXCEP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_EXCEP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"


!ENDIF 

SOURCE=.\GameMenu.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_GAMEM=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\GameMenu.obj"	"$(INTDIR)\GameMenu.sbr" : $(SOURCE) $(DEP_CPP_GAMEM)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_GAMEM=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\GameMenu.obj" : $(SOURCE) $(DEP_CPP_GAMEM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_GAMEM=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\GameMenu.obj" : $(SOURCE) $(DEP_CPP_GAMEM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_GAMEM=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\GameMenu.obj" : $(SOURCE) $(DEP_CPP_GAMEM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_GAMEM=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\GameMenu.obj" : $(SOURCE) $(DEP_CPP_GAMEM) "$(INTDIR)"


!ENDIF 

SOURCE=.\GENDUNG.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_GENDU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Spells.h"\
	

"$(INTDIR)\GENDUNG.OBJ"	"$(INTDIR)\GENDUNG.SBR" : $(SOURCE) $(DEP_CPP_GENDU)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_GENDU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Spells.h"\
	

"$(INTDIR)\GENDUNG.OBJ" : $(SOURCE) $(DEP_CPP_GENDU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_GENDU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Spells.h"\
	

"$(INTDIR)\GENDUNG.OBJ" : $(SOURCE) $(DEP_CPP_GENDU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_GENDU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Spells.h"\
	

"$(INTDIR)\GENDUNG.OBJ" : $(SOURCE) $(DEP_CPP_GENDU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_GENDU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Spells.h"\
	

"$(INTDIR)\GENDUNG.OBJ" : $(SOURCE) $(DEP_CPP_GENDU) "$(INTDIR)"


!ENDIF 

SOURCE=.\gmenu.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_GMENU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	

"$(INTDIR)\gmenu.obj"	"$(INTDIR)\gmenu.sbr" : $(SOURCE) $(DEP_CPP_GMENU)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_GMENU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	

"$(INTDIR)\gmenu.obj" : $(SOURCE) $(DEP_CPP_GMENU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_GMENU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	

"$(INTDIR)\gmenu.obj" : $(SOURCE) $(DEP_CPP_GMENU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_GMENU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	

"$(INTDIR)\gmenu.obj" : $(SOURCE) $(DEP_CPP_GMENU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_GMENU=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	

"$(INTDIR)\gmenu.obj" : $(SOURCE) $(DEP_CPP_GMENU) "$(INTDIR)"


!ENDIF 

SOURCE=.\help.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_HELP_=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	

"$(INTDIR)\help.obj"	"$(INTDIR)\help.sbr" : $(SOURCE) $(DEP_CPP_HELP_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_HELP_=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	

"$(INTDIR)\help.obj" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_HELP_=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	

"$(INTDIR)\help.obj" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_HELP_=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	

"$(INTDIR)\help.obj" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_HELP_=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Objects.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	

"$(INTDIR)\help.obj" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"


!ENDIF 

SOURCE=.\init.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_INIT_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\init.obj"	"$(INTDIR)\init.sbr" : $(SOURCE) $(DEP_CPP_INIT_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_INIT_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_INIT_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_INIT_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_INIT_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Lighting.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Interfac.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_INTER=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Storm.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Interfac.obj"	"$(INTDIR)\Interfac.sbr" : $(SOURCE) $(DEP_CPP_INTER)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_INTER=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Storm.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Interfac.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_INTER=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Storm.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Interfac.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_INTER=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Storm.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Interfac.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_INTER=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Storm.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Interfac.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


!ENDIF 

SOURCE=.\inv.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_INV_C=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	

"$(INTDIR)\inv.obj"	"$(INTDIR)\inv.sbr" : $(SOURCE) $(DEP_CPP_INV_C)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_INV_C=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	

"$(INTDIR)\inv.obj" : $(SOURCE) $(DEP_CPP_INV_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_INV_C=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	

"$(INTDIR)\inv.obj" : $(SOURCE) $(DEP_CPP_INV_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_INV_C=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	

"$(INTDIR)\inv.obj" : $(SOURCE) $(DEP_CPP_INV_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_INV_C=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	

"$(INTDIR)\inv.obj" : $(SOURCE) $(DEP_CPP_INV_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\itemdat.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_ITEMD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Spells.h"\
	

"$(INTDIR)\itemdat.obj"	"$(INTDIR)\itemdat.sbr" : $(SOURCE) $(DEP_CPP_ITEMD)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_ITEMD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Spells.h"\
	

"$(INTDIR)\itemdat.obj" : $(SOURCE) $(DEP_CPP_ITEMD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_ITEMD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Spells.h"\
	

"$(INTDIR)\itemdat.obj" : $(SOURCE) $(DEP_CPP_ITEMD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_ITEMD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Spells.h"\
	

"$(INTDIR)\itemdat.obj" : $(SOURCE) $(DEP_CPP_ITEMD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_ITEMD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Spells.h"\
	

"$(INTDIR)\itemdat.obj" : $(SOURCE) $(DEP_CPP_ITEMD) "$(INTDIR)"


!ENDIF 

SOURCE=.\ITEMS.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_ITEMS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\ITEMS.OBJ"	"$(INTDIR)\ITEMS.SBR" : $(SOURCE) $(DEP_CPP_ITEMS)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_ITEMS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\ITEMS.OBJ" : $(SOURCE) $(DEP_CPP_ITEMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_ITEMS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\ITEMS.OBJ" : $(SOURCE) $(DEP_CPP_ITEMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_ITEMS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\ITEMS.OBJ" : $(SOURCE) $(DEP_CPP_ITEMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_ITEMS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\ITEMS.OBJ" : $(SOURCE) $(DEP_CPP_ITEMS) "$(INTDIR)"


!ENDIF 

SOURCE=.\LIGHTING.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_LIGHT=\
	".\automap.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\LIGHTING.OBJ"	"$(INTDIR)\LIGHTING.SBR" : $(SOURCE) $(DEP_CPP_LIGHT)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_LIGHT=\
	".\automap.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\LIGHTING.OBJ" : $(SOURCE) $(DEP_CPP_LIGHT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_LIGHT=\
	".\automap.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\LIGHTING.OBJ" : $(SOURCE) $(DEP_CPP_LIGHT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_LIGHT=\
	".\automap.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\LIGHTING.OBJ" : $(SOURCE) $(DEP_CPP_LIGHT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_LIGHT=\
	".\automap.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\LIGHTING.OBJ" : $(SOURCE) $(DEP_CPP_LIGHT) "$(INTDIR)"


!ENDIF 

SOURCE=.\loadsave.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_LOADS=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\Trigs.h"\
	

"$(INTDIR)\loadsave.obj"	"$(INTDIR)\loadsave.sbr" : $(SOURCE) $(DEP_CPP_LOADS)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_LOADS=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\Trigs.h"\
	

"$(INTDIR)\loadsave.obj" : $(SOURCE) $(DEP_CPP_LOADS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_LOADS=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\Trigs.h"\
	

"$(INTDIR)\loadsave.obj" : $(SOURCE) $(DEP_CPP_LOADS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_LOADS=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\Trigs.h"\
	

"$(INTDIR)\loadsave.obj" : $(SOURCE) $(DEP_CPP_LOADS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_LOADS=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Interfac.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Town.h"\
	".\Trigs.h"\
	

"$(INTDIR)\loadsave.obj" : $(SOURCE) $(DEP_CPP_LOADS) "$(INTDIR)"


!ENDIF 

SOURCE=.\mainmenu.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MAINM=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\mainmenu.obj"	"$(INTDIR)\mainmenu.sbr" : $(SOURCE) $(DEP_CPP_MAINM)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MAINM=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\mainmenu.obj" : $(SOURCE) $(DEP_CPP_MAINM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MAINM=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\mainmenu.obj" : $(SOURCE) $(DEP_CPP_MAINM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MAINM=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\mainmenu.obj" : $(SOURCE) $(DEP_CPP_MAINM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MAINM=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\mainmenu.obj" : $(SOURCE) $(DEP_CPP_MAINM) "$(INTDIR)"


!ENDIF 

SOURCE=.\minitext.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MINIT=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\textdat.h"\
	

"$(INTDIR)\minitext.obj"	"$(INTDIR)\minitext.sbr" : $(SOURCE) $(DEP_CPP_MINIT)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MINIT=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\textdat.h"\
	

"$(INTDIR)\minitext.obj" : $(SOURCE) $(DEP_CPP_MINIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MINIT=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\textdat.h"\
	

"$(INTDIR)\minitext.obj" : $(SOURCE) $(DEP_CPP_MINIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MINIT=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\textdat.h"\
	

"$(INTDIR)\minitext.obj" : $(SOURCE) $(DEP_CPP_MINIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MINIT=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\textdat.h"\
	

"$(INTDIR)\minitext.obj" : $(SOURCE) $(DEP_CPP_MINIT) "$(INTDIR)"


!ENDIF 

SOURCE=.\misdat.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MISDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\misdat.h"\
	".\Missiles.h"\
	

"$(INTDIR)\misdat.obj"	"$(INTDIR)\misdat.sbr" : $(SOURCE) $(DEP_CPP_MISDA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MISDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\misdat.h"\
	".\Missiles.h"\
	

"$(INTDIR)\misdat.obj" : $(SOURCE) $(DEP_CPP_MISDA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MISDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\misdat.h"\
	".\Missiles.h"\
	

"$(INTDIR)\misdat.obj" : $(SOURCE) $(DEP_CPP_MISDA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MISDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\misdat.h"\
	".\Missiles.h"\
	

"$(INTDIR)\misdat.obj" : $(SOURCE) $(DEP_CPP_MISDA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MISDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\misdat.h"\
	".\Missiles.h"\
	

"$(INTDIR)\misdat.obj" : $(SOURCE) $(DEP_CPP_MISDA) "$(INTDIR)"


!ENDIF 

SOURCE=.\MISSILES.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MISSI=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\mono.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MISSILES.OBJ"	"$(INTDIR)\MISSILES.SBR" : $(SOURCE) $(DEP_CPP_MISSI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MISSI=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\mono.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MISSILES.OBJ" : $(SOURCE) $(DEP_CPP_MISSI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MISSI=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\mono.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MISSILES.OBJ" : $(SOURCE) $(DEP_CPP_MISSI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MISSI=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\mono.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MISSILES.OBJ" : $(SOURCE) $(DEP_CPP_MISSI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MISSI=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\mono.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MISSILES.OBJ" : $(SOURCE) $(DEP_CPP_MISSI) "$(INTDIR)"


!ENDIF 

SOURCE=.\MONSTDAT.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MONST=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Sound.h"\
	".\textdat.h"\
	

"$(INTDIR)\MONSTDAT.OBJ"	"$(INTDIR)\MONSTDAT.SBR" : $(SOURCE) $(DEP_CPP_MONST)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MONST=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Sound.h"\
	".\textdat.h"\
	

"$(INTDIR)\MONSTDAT.OBJ" : $(SOURCE) $(DEP_CPP_MONST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MONST=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Sound.h"\
	".\textdat.h"\
	

"$(INTDIR)\MONSTDAT.OBJ" : $(SOURCE) $(DEP_CPP_MONST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MONST=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Sound.h"\
	".\textdat.h"\
	

"$(INTDIR)\MONSTDAT.OBJ" : $(SOURCE) $(DEP_CPP_MONST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MONST=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\Sound.h"\
	".\textdat.h"\
	

"$(INTDIR)\MONSTDAT.OBJ" : $(SOURCE) $(DEP_CPP_MONST) "$(INTDIR)"


!ENDIF 

SOURCE=.\MONSTER.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MONSTE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MONSTER.OBJ"	"$(INTDIR)\MONSTER.SBR" : $(SOURCE) $(DEP_CPP_MONSTE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MONSTE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MONSTER.OBJ" : $(SOURCE) $(DEP_CPP_MONSTE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MONSTE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MONSTER.OBJ" : $(SOURCE) $(DEP_CPP_MONSTE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MONSTE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MONSTER.OBJ" : $(SOURCE) $(DEP_CPP_MONSTE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MONSTE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\MONSTER.OBJ" : $(SOURCE) $(DEP_CPP_MONSTE) "$(INTDIR)"


!ENDIF 

SOURCE=.\movie.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MOVIE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\movie.obj"	"$(INTDIR)\movie.sbr" : $(SOURCE) $(DEP_CPP_MOVIE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MOVIE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\movie.obj" : $(SOURCE) $(DEP_CPP_MOVIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MOVIE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\movie.obj" : $(SOURCE) $(DEP_CPP_MOVIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MOVIE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\movie.obj" : $(SOURCE) $(DEP_CPP_MOVIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MOVIE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Palette.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\movie.obj" : $(SOURCE) $(DEP_CPP_MOVIE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mpqapi.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MPQAP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\mpqapi.h"\
	".\Storm.h"\
	

"$(INTDIR)\mpqapi.obj"	"$(INTDIR)\mpqapi.sbr" : $(SOURCE) $(DEP_CPP_MPQAP)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MPQAP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\mpqapi.h"\
	".\Storm.h"\
	

"$(INTDIR)\mpqapi.obj" : $(SOURCE) $(DEP_CPP_MPQAP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MPQAP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\mpqapi.h"\
	".\Storm.h"\
	

"$(INTDIR)\mpqapi.obj" : $(SOURCE) $(DEP_CPP_MPQAP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MPQAP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\mpqapi.h"\
	".\Storm.h"\
	

"$(INTDIR)\mpqapi.obj" : $(SOURCE) $(DEP_CPP_MPQAP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MPQAP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\mpqapi.h"\
	".\Storm.h"\
	

"$(INTDIR)\mpqapi.obj" : $(SOURCE) $(DEP_CPP_MPQAP) "$(INTDIR)"


!ENDIF 

SOURCE=.\msg.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MSG_C=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\msg.obj"	"$(INTDIR)\msg.sbr" : $(SOURCE) $(DEP_CPP_MSG_C)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MSG_C=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MSG_C=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MSG_C=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MSG_C=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\multi.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_MULTI=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\portal.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\multi.obj"	"$(INTDIR)\multi.sbr" : $(SOURCE) $(DEP_CPP_MULTI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_MULTI=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\portal.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\multi.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_MULTI=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\portal.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\multi.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_MULTI=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\portal.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\multi.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_MULTI=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\portal.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\multi.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"


!ENDIF 

SOURCE=.\nthread.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_NTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\nthread.obj"	"$(INTDIR)\nthread.sbr" : $(SOURCE) $(DEP_CPP_NTHRE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_NTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\nthread.obj" : $(SOURCE) $(DEP_CPP_NTHRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_NTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\nthread.obj" : $(SOURCE) $(DEP_CPP_NTHRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_NTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\nthread.obj" : $(SOURCE) $(DEP_CPP_NTHRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_NTHRE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\nthread.obj" : $(SOURCE) $(DEP_CPP_NTHRE) "$(INTDIR)"


!ENDIF 

SOURCE=.\objdat.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_OBJDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	

"$(INTDIR)\objdat.obj"	"$(INTDIR)\objdat.sbr" : $(SOURCE) $(DEP_CPP_OBJDA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_OBJDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	

"$(INTDIR)\objdat.obj" : $(SOURCE) $(DEP_CPP_OBJDA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_OBJDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	

"$(INTDIR)\objdat.obj" : $(SOURCE) $(DEP_CPP_OBJDA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_OBJDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	

"$(INTDIR)\objdat.obj" : $(SOURCE) $(DEP_CPP_OBJDA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_OBJDA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	

"$(INTDIR)\objdat.obj" : $(SOURCE) $(DEP_CPP_OBJDA) "$(INTDIR)"


!ENDIF 

SOURCE=.\OBJECTS.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_OBJEC=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	

"$(INTDIR)\OBJECTS.OBJ"	"$(INTDIR)\OBJECTS.SBR" : $(SOURCE) $(DEP_CPP_OBJEC)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_OBJEC=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	

"$(INTDIR)\OBJECTS.OBJ" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_OBJEC=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	

"$(INTDIR)\OBJECTS.OBJ" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_OBJEC=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	

"$(INTDIR)\OBJECTS.OBJ" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_OBJEC=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\stores.h"\
	".\textdat.h"\
	".\themes.h"\
	".\towners.h"\
	

"$(INTDIR)\OBJECTS.OBJ" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


!ENDIF 

SOURCE=.\packplr.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_PACKP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\packplr.obj"	"$(INTDIR)\packplr.sbr" : $(SOURCE) $(DEP_CPP_PACKP)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_PACKP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\packplr.obj" : $(SOURCE) $(DEP_CPP_PACKP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_PACKP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\packplr.obj" : $(SOURCE) $(DEP_CPP_PACKP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_PACKP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\packplr.obj" : $(SOURCE) $(DEP_CPP_PACKP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_PACKP=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	

"$(INTDIR)\packplr.obj" : $(SOURCE) $(DEP_CPP_PACKP) "$(INTDIR)"


!ENDIF 

SOURCE=.\PALETTE.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_PALET=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\PALETTE.OBJ"	"$(INTDIR)\PALETTE.SBR" : $(SOURCE) $(DEP_CPP_PALET)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_PALET=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\PALETTE.OBJ" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_PALET=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\PALETTE.OBJ" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_PALET=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\PALETTE.OBJ" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_PALET=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Palette.h"\
	".\Storm.h"\
	

"$(INTDIR)\PALETTE.OBJ" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"


!ENDIF 

SOURCE=.\path.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_PATH_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\path.h"\
	".\Player.h"\
	

"$(INTDIR)\path.obj"	"$(INTDIR)\path.sbr" : $(SOURCE) $(DEP_CPP_PATH_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_PATH_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\path.h"\
	".\Player.h"\
	

"$(INTDIR)\path.obj" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_PATH_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\path.h"\
	".\Player.h"\
	

"$(INTDIR)\path.obj" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_PATH_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\path.h"\
	".\Player.h"\
	

"$(INTDIR)\path.obj" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_PATH_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\path.h"\
	".\Player.h"\
	

"$(INTDIR)\path.obj" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"


!ENDIF 

SOURCE=.\pfile.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_PFILE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\mpqapi.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\pfile.obj"	"$(INTDIR)\pfile.sbr" : $(SOURCE) $(DEP_CPP_PFILE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_PFILE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\mpqapi.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\pfile.obj" : $(SOURCE) $(DEP_CPP_PFILE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_PFILE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\mpqapi.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\pfile.obj" : $(SOURCE) $(DEP_CPP_PFILE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_PFILE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\mpqapi.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\pfile.obj" : $(SOURCE) $(DEP_CPP_PFILE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_PFILE=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Diabloui.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\mpqapi.h"\
	".\Multi.h"\
	".\packplr.h"\
	".\Player.h"\
	".\Spells.h"\
	".\Storm.h"\
	

"$(INTDIR)\pfile.obj" : $(SOURCE) $(DEP_CPP_PFILE) "$(INTDIR)"


!ENDIF 

SOURCE=.\PLAYER.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_PLAYE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\PLAYER.OBJ"	"$(INTDIR)\PLAYER.SBR" : $(SOURCE) $(DEP_CPP_PLAYE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_PLAYE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\PLAYER.OBJ" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_PLAYE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\PLAYER.OBJ" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_PLAYE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\PLAYER.OBJ" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_PLAYE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\path.h"\
	".\Player.h"\
	".\portal.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	".\stores.h"\
	".\Storm.h"\
	".\textdat.h"\
	".\themes.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\PLAYER.OBJ" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plrmsg.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_PLRMS=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	

"$(INTDIR)\plrmsg.obj"	"$(INTDIR)\plrmsg.sbr" : $(SOURCE) $(DEP_CPP_PLRMS)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_PLRMS=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	

"$(INTDIR)\plrmsg.obj" : $(SOURCE) $(DEP_CPP_PLRMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_PLRMS=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	

"$(INTDIR)\plrmsg.obj" : $(SOURCE) $(DEP_CPP_PLRMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_PLRMS=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	

"$(INTDIR)\plrmsg.obj" : $(SOURCE) $(DEP_CPP_PLRMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_PLRMS=\
	".\Control.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	

"$(INTDIR)\plrmsg.obj" : $(SOURCE) $(DEP_CPP_PLRMS) "$(INTDIR)"


!ENDIF 

SOURCE=.\portal.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_PORTA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\msg.h"\
	".\Player.h"\
	".\portal.h"\
	

"$(INTDIR)\portal.obj"	"$(INTDIR)\portal.sbr" : $(SOURCE) $(DEP_CPP_PORTA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_PORTA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\msg.h"\
	".\Player.h"\
	".\portal.h"\
	

"$(INTDIR)\portal.obj" : $(SOURCE) $(DEP_CPP_PORTA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_PORTA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\msg.h"\
	".\Player.h"\
	".\portal.h"\
	

"$(INTDIR)\portal.obj" : $(SOURCE) $(DEP_CPP_PORTA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_PORTA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\msg.h"\
	".\Player.h"\
	".\portal.h"\
	

"$(INTDIR)\portal.obj" : $(SOURCE) $(DEP_CPP_PORTA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_PORTA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\msg.h"\
	".\Player.h"\
	".\portal.h"\
	

"$(INTDIR)\portal.obj" : $(SOURCE) $(DEP_CPP_PORTA) "$(INTDIR)"


!ENDIF 

SOURCE=.\Quests.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_QUEST=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Quests.obj"	"$(INTDIR)\Quests.sbr" : $(SOURCE) $(DEP_CPP_QUEST)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_QUEST=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Quests.obj" : $(SOURCE) $(DEP_CPP_QUEST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_QUEST=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Quests.obj" : $(SOURCE) $(DEP_CPP_QUEST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_QUEST=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Quests.obj" : $(SOURCE) $(DEP_CPP_QUEST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_QUEST=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Drlg_l1.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Missiles.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\Setmaps.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\Quests.obj" : $(SOURCE) $(DEP_CPP_QUEST) "$(INTDIR)"


!ENDIF 

SOURCE=.\Scroll.asm

!IF  "$(CFG)" == "Diablo - Win32 Release"

IntDir=.\.\WinRel
InputPath=.\Scroll.asm
InputName=Scroll

"$(IntDir)\scroll.obj"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	tasm /t /m5 /ml /w2 /zn $(InputName) $(IntDir)\scroll.obj

!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

IntDir=.\.\WinDebug
InputPath=.\Scroll.asm
InputName=Scroll

"$(IntDir)\scroll.obj"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	tasm /t /m5 /ml /w2 /zn $(InputName) $(IntDir)\scroll.obj

!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

IntDir=.\.\WinFinal
InputPath=.\Scroll.asm
InputName=Scroll

"$(IntDir)\scroll.obj"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	tasm /t /m5 /ml /w2 /zn $(InputName) $(IntDir)\scroll.obj

!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

IntDir=.\.\SFinal
InputPath=.\Scroll.asm
InputName=Scroll

"$(IntDir)\scroll.obj"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	tasm /t /m5 /ml /w2 /zn $(InputName) $(IntDir)\scroll.obj

!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

IntDir=.\.\SRel
InputPath=.\Scroll.asm
InputName=Scroll

"$(IntDir)\scroll.obj"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	tasm /t /m5 /ml /w2 /zn $(InputName) $(IntDir)\scroll.obj

!ENDIF 

SOURCE=.\SCROLLRT.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_SCROL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	".\Town.h"\
	

"$(INTDIR)\SCROLLRT.OBJ"	"$(INTDIR)\SCROLLRT.SBR" : $(SOURCE) $(DEP_CPP_SCROL)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_SCROL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	".\Town.h"\
	

"$(INTDIR)\SCROLLRT.OBJ" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_SCROL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	".\Town.h"\
	

"$(INTDIR)\SCROLLRT.OBJ" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_SCROL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	".\Town.h"\
	

"$(INTDIR)\SCROLLRT.OBJ" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_SCROL=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Dead.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Lighting.h"\
	".\MiniText.h"\
	".\misdat.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\Multi.h"\
	".\Objects.h"\
	".\Player.h"\
	".\Quests.h"\
	".\regconst.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\Spells.h"\
	".\Storm.h"\
	".\Town.h"\
	

"$(INTDIR)\SCROLLRT.OBJ" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"


!ENDIF 

SOURCE=.\SetMaps.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_SETMA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\SetMaps.obj"	"$(INTDIR)\SetMaps.sbr" : $(SOURCE) $(DEP_CPP_SETMA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_SETMA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\SetMaps.obj" : $(SOURCE) $(DEP_CPP_SETMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_SETMA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\SetMaps.obj" : $(SOURCE) $(DEP_CPP_SETMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_SETMA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\SetMaps.obj" : $(SOURCE) $(DEP_CPP_SETMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_SETMA=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Drlg_l1.h"\
	".\Drlg_l2.h"\
	".\Drlg_l3.h"\
	".\Drlg_l4.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Objects.h"\
	".\Palette.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\SetMaps.obj" : $(SOURCE) $(DEP_CPP_SETMA) "$(INTDIR)"


!ENDIF 

SOURCE=.\SHA.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_SHA_C=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\SHA.OBJ"	"$(INTDIR)\SHA.SBR" : $(SOURCE) $(DEP_CPP_SHA_C)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_SHA_C=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\SHA.OBJ" : $(SOURCE) $(DEP_CPP_SHA_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_SHA_C=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\SHA.OBJ" : $(SOURCE) $(DEP_CPP_SHA_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_SHA_C=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\SHA.OBJ" : $(SOURCE) $(DEP_CPP_SHA_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_SHA_C=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	

"$(INTDIR)\SHA.OBJ" : $(SOURCE) $(DEP_CPP_SHA_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\SOUND.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_SOUND=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\SOUND.OBJ"	"$(INTDIR)\SOUND.SBR" : $(SOURCE) $(DEP_CPP_SOUND)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_SOUND=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\SOUND.OBJ" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_SOUND=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\SOUND.OBJ" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_SOUND=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\SOUND.OBJ" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_SOUND=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\SOUND.OBJ" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"


!ENDIF 

SOURCE=.\Spelldat.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_SPELL=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Missiles.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\Spelldat.obj"	"$(INTDIR)\Spelldat.sbr" : $(SOURCE) $(DEP_CPP_SPELL)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_SPELL=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Missiles.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\Spelldat.obj" : $(SOURCE) $(DEP_CPP_SPELL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_SPELL=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Missiles.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\Spelldat.obj" : $(SOURCE) $(DEP_CPP_SPELL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_SPELL=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Missiles.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\Spelldat.obj" : $(SOURCE) $(DEP_CPP_SPELL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_SPELL=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Missiles.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\Spelldat.obj" : $(SOURCE) $(DEP_CPP_SPELL) "$(INTDIR)"


!ENDIF 

SOURCE=.\SPELLS.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_SPELLS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\SPELLS.OBJ"	"$(INTDIR)\SPELLS.SBR" : $(SOURCE) $(DEP_CPP_SPELLS)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_SPELLS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\SPELLS.OBJ" : $(SOURCE) $(DEP_CPP_SPELLS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_SPELLS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\SPELLS.OBJ" : $(SOURCE) $(DEP_CPP_SPELLS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_SPELLS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\SPELLS.OBJ" : $(SOURCE) $(DEP_CPP_SPELLS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_SPELLS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Missiles.h"\
	".\Monster.h"\
	".\Player.h"\
	".\Sound.h"\
	".\spelldat.h"\
	".\Spells.h"\
	

"$(INTDIR)\SPELLS.OBJ" : $(SOURCE) $(DEP_CPP_SPELLS) "$(INTDIR)"


!ENDIF 

SOURCE=.\stores.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_STORE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\spelldat.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	

"$(INTDIR)\stores.obj"	"$(INTDIR)\stores.sbr" : $(SOURCE) $(DEP_CPP_STORE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_STORE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\spelldat.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	

"$(INTDIR)\stores.obj" : $(SOURCE) $(DEP_CPP_STORE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_STORE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\spelldat.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	

"$(INTDIR)\stores.obj" : $(SOURCE) $(DEP_CPP_STORE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_STORE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\spelldat.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	

"$(INTDIR)\stores.obj" : $(SOURCE) $(DEP_CPP_STORE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_STORE=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Scrollrt.h"\
	".\spelldat.h"\
	".\stores.h"\
	".\textdat.h"\
	".\towners.h"\
	

"$(INTDIR)\stores.obj" : $(SOURCE) $(DEP_CPP_STORE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sync.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_SYNC_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\sync.obj"	"$(INTDIR)\sync.sbr" : $(SOURCE) $(DEP_CPP_SYNC_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_SYNC_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\sync.obj" : $(SOURCE) $(DEP_CPP_SYNC_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_SYNC_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\sync.obj" : $(SOURCE) $(DEP_CPP_SYNC_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_SYNC_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\sync.obj" : $(SOURCE) $(DEP_CPP_SYNC_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_SYNC_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Debug.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\Monster.h"\
	".\monstint.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Sound.h"\
	

"$(INTDIR)\sync.obj" : $(SOURCE) $(DEP_CPP_SYNC_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Textdat.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_TEXTD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\MiniText.h"\
	".\textdat.h"\
	

"$(INTDIR)\Textdat.obj"	"$(INTDIR)\Textdat.sbr" : $(SOURCE) $(DEP_CPP_TEXTD)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_TEXTD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\MiniText.h"\
	".\textdat.h"\
	

"$(INTDIR)\Textdat.obj" : $(SOURCE) $(DEP_CPP_TEXTD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_TEXTD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\MiniText.h"\
	".\textdat.h"\
	

"$(INTDIR)\Textdat.obj" : $(SOURCE) $(DEP_CPP_TEXTD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_TEXTD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\MiniText.h"\
	".\textdat.h"\
	

"$(INTDIR)\Textdat.obj" : $(SOURCE) $(DEP_CPP_TEXTD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_TEXTD=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\MiniText.h"\
	".\textdat.h"\
	

"$(INTDIR)\Textdat.obj" : $(SOURCE) $(DEP_CPP_TEXTD) "$(INTDIR)"


!ENDIF 

SOURCE=.\themes.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_THEME=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\themes.obj"	"$(INTDIR)\themes.sbr" : $(SOURCE) $(DEP_CPP_THEME)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_THEME=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\themes.obj" : $(SOURCE) $(DEP_CPP_THEME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_THEME=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\themes.obj" : $(SOURCE) $(DEP_CPP_THEME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_THEME=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\themes.obj" : $(SOURCE) $(DEP_CPP_THEME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_THEME=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\Monstdat.h"\
	".\Monster.h"\
	".\objdat.h"\
	".\Objects.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\themes.h"\
	".\Trigs.h"\
	

"$(INTDIR)\themes.obj" : $(SOURCE) $(DEP_CPP_THEME) "$(INTDIR)"


!ENDIF 

SOURCE=.\tmsg.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_TMSG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\tmsg.obj"	"$(INTDIR)\tmsg.sbr" : $(SOURCE) $(DEP_CPP_TMSG_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_TMSG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\tmsg.obj" : $(SOURCE) $(DEP_CPP_TMSG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_TMSG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\tmsg.obj" : $(SOURCE) $(DEP_CPP_TMSG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_TMSG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\tmsg.obj" : $(SOURCE) $(DEP_CPP_TMSG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_TMSG_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	

"$(INTDIR)\tmsg.obj" : $(SOURCE) $(DEP_CPP_TMSG_) "$(INTDIR)"


!ENDIF 

SOURCE=.\TOWN.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_TOWN_=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TOWN.OBJ"	"$(INTDIR)\TOWN.SBR" : $(SOURCE) $(DEP_CPP_TOWN_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_TOWN_=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TOWN.OBJ" : $(SOURCE) $(DEP_CPP_TOWN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_TOWN_=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TOWN.OBJ" : $(SOURCE) $(DEP_CPP_TOWN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_TOWN_=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TOWN.OBJ" : $(SOURCE) $(DEP_CPP_TOWN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_TOWN_=\
	".\automap.h"\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\doom.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\error.h"\
	".\Gamemenu.h"\
	".\Gendung.h"\
	".\help.h"\
	".\Inv.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\Multi.h"\
	".\Player.h"\
	".\Quests.h"\
	".\scrlasm.h"\
	".\Scrollrt.h"\
	".\Sound.h"\
	".\stores.h"\
	".\Town.h"\
	".\towners.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TOWN.OBJ" : $(SOURCE) $(DEP_CPP_TOWN_) "$(INTDIR)"


!ENDIF 

SOURCE=.\towners.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_TOWNE=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\towners.obj"	"$(INTDIR)\towners.sbr" : $(SOURCE) $(DEP_CPP_TOWNE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_TOWNE=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\towners.obj" : $(SOURCE) $(DEP_CPP_TOWNE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_TOWNE=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\towners.obj" : $(SOURCE) $(DEP_CPP_TOWNE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_TOWNE=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\towners.obj" : $(SOURCE) $(DEP_CPP_TOWNE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_TOWNE=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\Engine.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\itemdat.h"\
	".\Items.h"\
	".\MiniText.h"\
	".\Monster.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Sound.h"\
	".\stores.h"\
	".\textdat.h"\
	".\Town.h"\
	".\towners.h"\
	

"$(INTDIR)\towners.obj" : $(SOURCE) $(DEP_CPP_TOWNE) "$(INTDIR)"


!ENDIF 

SOURCE=.\track.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_TRACK=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Storm.h"\
	

"$(INTDIR)\track.obj"	"$(INTDIR)\track.sbr" : $(SOURCE) $(DEP_CPP_TRACK)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_TRACK=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Storm.h"\
	

"$(INTDIR)\track.obj" : $(SOURCE) $(DEP_CPP_TRACK) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_TRACK=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Storm.h"\
	

"$(INTDIR)\track.obj" : $(SOURCE) $(DEP_CPP_TRACK) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_TRACK=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Storm.h"\
	

"$(INTDIR)\track.obj" : $(SOURCE) $(DEP_CPP_TRACK) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_TRACK=\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Gendung.h"\
	".\Items.h"\
	".\msg.h"\
	".\Player.h"\
	".\Storm.h"\
	

"$(INTDIR)\track.obj" : $(SOURCE) $(DEP_CPP_TRACK) "$(INTDIR)"


!ENDIF 

SOURCE=.\TRIGS.CPP

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_TRIGS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TRIGS.OBJ"	"$(INTDIR)\TRIGS.SBR" : $(SOURCE) $(DEP_CPP_TRIGS)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_TRIGS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TRIGS.OBJ" : $(SOURCE) $(DEP_CPP_TRIGS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_TRIGS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TRIGS.OBJ" : $(SOURCE) $(DEP_CPP_TRIGS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_TRIGS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TRIGS.OBJ" : $(SOURCE) $(DEP_CPP_TRIGS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_TRIGS=\
	".\Control.h"\
	".\Cursor.h"\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Effects.h"\
	".\error.h"\
	".\Gendung.h"\
	".\Inv.h"\
	".\Items.h"\
	".\msg.h"\
	".\Multi.h"\
	".\Palette.h"\
	".\Player.h"\
	".\Quests.h"\
	".\Setmaps.h"\
	".\Trigs.h"\
	

"$(INTDIR)\TRIGS.OBJ" : $(SOURCE) $(DEP_CPP_TRIGS) "$(INTDIR)"


!ENDIF 

SOURCE=.\wave.cpp

!IF  "$(CFG)" == "Diablo - Win32 Release"

DEP_CPP_WAVE_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\wave.obj"	"$(INTDIR)\wave.sbr" : $(SOURCE) $(DEP_CPP_WAVE_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

DEP_CPP_WAVE_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\wave.obj" : $(SOURCE) $(DEP_CPP_WAVE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 FinalFinal"

DEP_CPP_WAVE_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\wave.obj" : $(SOURCE) $(DEP_CPP_WAVE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware FinalFinal"

DEP_CPP_WAVE_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\wave.obj" : $(SOURCE) $(DEP_CPP_WAVE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Diablo - Win32 Shareware Release"

DEP_CPP_WAVE_=\
	".\d3dtypes.h"\
	".\Ddraw.h"\
	".\Diablo.h"\
	".\Dsound.h"\
	".\Engine.h"\
	".\Sound.h"\
	".\Storm.h"\
	

"$(INTDIR)\wave.obj" : $(SOURCE) $(DEP_CPP_WAVE_) "$(INTDIR)"


!ENDIF 


!ENDIF 

