
@echo off
if exist ..\..\sdkmake.conf goto doclean

echo EBSDK has not yet been configured; doing so now
pushd ..\..
call configure.bat
popd

:doclean

pushd ntalpha
call build clean
popd
pushd common
call build clean
popd

