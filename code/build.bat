@Echo off
pushd ..\build
cl -doc -FC -Zi ..\code\win32_handmade.cpp user32.lib gdi32.lib
xdcmake win32_handmade.xdc
@REM // TODO(smzb): add SHFB build process for markdown and html documenation
popd
