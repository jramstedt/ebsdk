setlocal
set tgt=%1%
set subdir=%2%
set task=%3%

if exist %subdir%/Makefile set mf=Makefile
if exist %subdir%/makefile set mf=makefile

pushd %subdir%
%EB_MAKE% MAKE=%EB_MAKE% MAKEFILE=%mf% TARGET=%tgt% %task%
if not errorlevel 1 goto done
echo An error occurred during build
pause
:done
popd
endlocal

