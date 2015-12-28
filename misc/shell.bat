@Echo off
c:
cd "c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\"
call vcvarsall.bat x64
cd "C:\Users\smzb-admin\"
subst z: Projects
z:
set path=%path%;z:\handmade\misc