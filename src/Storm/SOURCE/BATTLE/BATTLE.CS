#include <win32.cs>
#include <storm.cs>
set deffile=battle.def
set extralib=wsock32.lib shell32.lib
set linkopt=%linkopt% -base:0x19000000

// DETERMINE THE PROJECT NAME
if %debug% set project=%project%d
set outfile=%project%.snp

// ADD THE CAPS SIGNATURE TO THE END OF THE FILE
!copy /b %project%.dll+caps.mpq > NUL:

// RENAME IT TO .SNP AND COPY IT TO THE OUTPUT DIRECTORY
!if exist %outfile% del %outfile%
!rename %project%.dll %outfile%
!copy %outfile% ..\..\bin > NUL:
!if exist *.bak del *.bak
