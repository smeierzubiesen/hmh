@Echo off
:start_script
IF NOT EXIST c:\temp\%~n0.bat goto :clone_me
IF EXIST c:\temp\%~n0.bat goto :exit_me 
:clone_me
@Echo Cloning script ...
IF EXIST c:\temp\ goto :copy_script
IF NOT EXIST c:\temp\ goto :create_temp
:create_temp
@Echo Creating directory : C:\temp
mkdir c:\temp > nul
goto :copy_script
:copy_script
@Echo copying %~f0 -> c:\temp\%~n0.bat
copy "%~f0" "c:\temp\%~n0.bat"
goto :call_clone
:call_clone
@Echo Starting cloned script
call "c:\temp\%~n0.bat"
goto :done
:exit_me
@Echo Closing down environment
pushd c:\temp
subst w: /D
popd
for /f "skip=1 delims=" %%x in ('wmic cpu get addresswidth') do if not defined AddressWidth set AddressWidth=%%x
if %AddressWidth%==64 (
  @Echo Cleaning Visual Studio environment for 64 bit OS
  call "E:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" -clean_env
) else (
  @Echo Cleaning Visual Studio environment for 32 bit OS
  call "E:\Program Files\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" -clean_env
)
@Echo Deleting clone and exiting ...
del "%~f0"&exit
goto :done
:error
REM We should add a link here
echo [ERROR:~nx0] Error in script usage.
pause
goto :start_script
:done
@Echo We should NEVER reach this!!
pushd
exit /B
