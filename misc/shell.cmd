@Echo off
@Echo Creating work environment in w:
subst w: %USERPROFILE%\source\repos\
for /f "skip=1 delims=" %%x in ('wmic cpu get addresswidth') do if not defined AddressWidth set AddressWidth=%%x
if %AddressWidth%==64 (
  @Echo Initializing Visual Studio environment for 64 bit OS
  call "E:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
  goto :continue
) else (
  @Echo Initializing Visual Studio environment for 32 bit OS
  call "E:\Program Files\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86
  goto :continue
)
:continue
@Echo Setting paths
set PATH=w:\hmh\misc;%path%
w:
pushd .\hmh\misc\
xcopy quit.batch quit.bat
@Echo Development environment ready. Type 'quit' to close down the environment