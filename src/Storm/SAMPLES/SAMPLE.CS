if not %project%==sample goto start
halt
:start
#include <win32.cs>
#include <storm.cs>
!if exist *.bak del *.bak
