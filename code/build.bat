@Echo off
@Echo Building win32_handmade.exe
IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
del /Q *.*
cl -MT -nologo -Gm- -GR- -EHa- -Od -Oi -wd4456 -wd4189 -wd4100 -wd4201  -WX -W4 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -Fmwin32_handmade.map -FC -Z7 ..\code\win32_handmade.cpp /link -opt:ref -subsystem:windows,5.1 user32.lib gdi32.lib
popd
