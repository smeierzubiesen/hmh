@Echo off
c:
pushd c:\
subst w: /D
@Echo You can now exit the console with either "exit" or by simply 
@Echo closing the tabs in your console manager (or GitBash!)
exit /B
popd