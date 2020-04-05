#include <win32.cs>
if %debug% set clopt=-DSTATICLIB -D_DEBUG -Zi
if not %debug% set clopt=-DSTATICLIB -DNDEBUG -O1 -GBFry
set extralib=implode.lib
set .def=
set .obj=.lib
!copy %project%.lib ..\lib > NUL:
!copy storm.res ..\lib\stormst.res > NUL:
!if exist *.bak del *.bak
!if exist *.res del *.res
!if exist %project%.lib del %project%.lib