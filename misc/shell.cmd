@Echo off
subst w: D:\Projects
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
set PATH=w:\hmh\misc;%path%
w: