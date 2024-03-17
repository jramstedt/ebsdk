
rem See if we're properly configured first...
if exist ..\sdkconf.bat goto dobuild

echo EBSDK is not yet configured; doing so now.
pushd ..
call configure.bat
popd

if exist ..\sdkconf.bat goto dobuild
echo EBSDK configuration failed, aborting!
goto bye

:dobuild

rem Configuration file found... use it!

call ../sdkconf.bat

rem Parameters to this script are an optional list of platforms
rem to build.  If none specified, we build all by default

if not "%1" == "" set EB_TARGETS=%1%
if not "%2" == "" set EB_TARGETS=%EB_TARGETS% %2%
if not "%3" == "" set EB_TARGETS=%EB_TARGETS% %3%
if not "%4" == "" set EB_TARGETS=%EB_TARGETS% %4%
if not "%5" == "" set EB_TARGETS=%EB_TARGETS% %5%
if not "%6" == "" set EB_TARGETS=%EB_TARGETS% %6%
if not "%7" == "" set EB_TARGETS=%EB_TARGETS% %7%
if not "%8" == "" set EB_TARGETS=%EB_TARGETS% %8%

set EB_SUBDIRS=h srom palcode lib stubs ether ladebug dbm demo dhry

rem if "%TASK%" == "" set TASKNAME=Building
rem if "%TASK%" == "clean" set TASKNAME=Cleaning
setlocal
set TASK=clean
set TASKNAME=Cleaning

echo EB_TARGETS is %EB_TARGETS%
echo EB_SUBDIRS is %EB_SUBDIRS%

for %%t in (%EB_TARGETS%) do for %%s in (%EB_SUBDIRS%) do call builddir.bat %%t %%s %TASK%

for %%t in (%EB_TARGETS%) do del /q/f obj\%%t\*.*
for %%t in (%EB_TARGETS%) do del /q/f ..\ebboot\%%t\*.*

endlocal
