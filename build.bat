@ECHO OFF

SET CFLAGS=-Zi -nologo -Oi -FC -Od
REM -WX To make Warnings errors

IF NOT EXIST "build" MD "build"
PUSHD  "build"
cl %CFLAGS% "../main.cpp" /link User32.lib

POPD
