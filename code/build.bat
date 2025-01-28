@echo off

mkdir ..\builds
pushd ..\builds
cl -FC -Zi ..\code\win32_noengine.cpp user32.lib Gdi32.lib Dsound.lib
popd
