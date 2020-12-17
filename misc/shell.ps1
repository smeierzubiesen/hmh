$workingDrive="W"
$vsToolsDrive="E"
$vsToolsVersion="2019"
Write-Host "Mounting environment (${env:USERPROFILE}\source\repos\) in ${workingDrive}:"
subst w: ${env:USERPROFILE}\source\repos\
if ([System.Environment]::Is64BitOperatingSystem) 
{
  & "${vsToolsDrive}:\Program Files (x86)\Microsoft Visual Studio\${vsToolsVersion}\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
} else {
  & "${vsToolsDrive}:\Program Files\Microsoft Visual Studio\${vsToolsVersion}\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86
}
Function Add-PathVariable {
  param (
      [string]$addPath
  )
  if (Test-Path $addPath){
      $regexAddPath = [regex]::Escape($addPath)
      $arrPath = $env:Path -split ';' | Where-Object {$_ -notMatch 
"^$regexAddPath\\?"}
      $env:Path = ($arrPath + $addPath) -join ';'
  } else {
      Throw "'$addPath' is not a valid path."
  }
}
Write-Host "Setting paths"
Add-PathVariable("w:\hmh\misc\")
Set-Location -Path "${workingDrive}:"
Push-Location -Path $PSScriptRoot
Copy-Item quit.powershell quit.ps1
Pop-Location
Write-Host "Development environment ready. Type 'quit' to close down the environment"