#include <win32.cs>
set extralib=storm.lib wsock32.lib
set linkopt=%linkopt% -base:0x17000000
!if exist %project%.snp del %project%.snp
!rename %project%.dll %project%.snp
!copy %project%.snp ..\..\bin > NUL:
!if %debug% copy %project%.snp ..\..\bin\debug > NUL:
!if exist *.bak del *.bak
