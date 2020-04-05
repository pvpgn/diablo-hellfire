@echo off
if not exist bin\storm.dll goto done
locate /d     bin\*.bak
locate /d    help\*.fts
locate /d samples\*.aps *.bak *.ilk *.exp *.obj *.pch *.pdb *.res *.tmp a.* dump.*
locate /d  source\*.aps *.bak *.ilk *.exp *.obj *.pch *.pdb *.res *.tmp a.* dump.* *.lib *.dll *.snp
if exist *.bak del *.bak
:done
