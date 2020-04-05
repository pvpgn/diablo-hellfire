@echo off
if "%1"=="" goto usage
if "%2"=="" goto usage
goto start
:usage
echo Usage: ARCHIVE filename password
goto done
:start
cmd /c clean
pkzip32 -add -dir -max %1.zip * -excl=*.smk -excl=*.cod -excl=*.dsp -excl=*.dsw -excl=*.mak -excl=*.map
rename %1.zip %1.zip.enc
encrypt %1.zip.enc %2
:done
