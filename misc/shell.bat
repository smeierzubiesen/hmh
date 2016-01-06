@REM TODO(smzb): make script more robust for different environments
@Echo off
c:
cd "c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\"
call vcvarsall.bat x64
cd "C:\Users\smeierzubiesen\Documents\"
subst z: GitHub
z:
set path=%path%;z:\hmh\misc
