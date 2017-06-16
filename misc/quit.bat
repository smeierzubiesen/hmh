@Echo off
:start_script
IF NOT EXIST c:\temp\%~n0.bat goto :clone_me
IF EXIST c:\temp\%~n0.bat goto :exit_me 
:clone_me
@Echo Cloning ...
IF EXIST c:\temp\ goto :copy_script
IF NOT EXIST c:\temp\ goto :create_temp
:create_temp
mkdir c:\temp > nul
goto :copy_script
:copy_script
@Echo copying "%~f0" -> "c:\temp\%~n0.bat"
copy "%~f0" "c:\temp\%~n0.bat"
goto :call_clone
:call_clone
call "c:\temp\%~n0.bat"
goto :done
:exit_me
pushd c:\temp
subst w: /D
popd
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -clean_env
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