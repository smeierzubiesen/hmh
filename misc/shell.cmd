@Echo off
subst w: D:\Projects
for /f "skip=1 delims=" %%x in ('wmic cpu get addresswidth') do if not defined AddressWidth set AddressWidth=%%x

if %AddressWidth%==64 (
  @Echo 64 bit OS
  call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
  goto :continue
) else (
  @Echo 32 bit OS
  call "C:\Program Files\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
  goto :continue
)
:continue
set PATH=w:\hmh\misc;%path%
w:
