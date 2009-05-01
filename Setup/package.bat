@echo off

if "%1"=="" goto usage

del %1\ItemAssistant.zip
"C:\Program Files\7-Zip\7z.exe" a "%1\ItemAssistant.zip" "%1\ItemAssistant.exe" "%1\README.txt" "%1\*.dll" "%1\*.manifest"
goto:end

:usage
@echo Incorrect syntax.
@echo    package.bat "path-to-out-dir"

:end
REM exit
