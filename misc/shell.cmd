@Echo off
@Echo Creating work environment in w:
subst w: D:\Projects
@Echo Initializing Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
@Echo Setting paths
set PATH=w:\hmh\misc;%path%
w:
@Echo Development environment ready. Type 'quit' to close down the environment
