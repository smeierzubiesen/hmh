@Echo off
IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
del /Q *.*
cl -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Zi ..\code\win32_handmade.cpp user32.lib gdi32.lib
popd
