
@echo off
if exist ..\..\sdkmake.conf goto dobuild

echo EBSDK has not yet been configured; doing so now
pushd ..\..
call configure.bat
popd

:dobuild

pushd ntalpha
call build
popd
pushd common
call build
popd

