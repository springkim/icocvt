@echo off
cd prj
call CMakeBuild.bat
cd ..
call :SafeRMDIR ico
md ico
call :SafeRMDIR img
md img
SETLOCAL EnableDelayedExpansion
FOR /R %%E IN (png/*.png) DO (
	set file=%%~nxE
	echo !file!
	prj\cvtico.exe png\!file!
)
ENDLOCAL
move png\*.ico ico\
move png\*_readme.png img\


echo. > Readme.md
SETLOCAL EnableDelayedExpansion
FOR /R %%E IN (img/*.png) DO (
	set full_path=%%E
	set file=%%~nxE
	echo|set /p = "^![](img/%%~nxE)" >> Readme.md
)
ENDLOCAL
pause
exit /b
:SafeRMDIR
IF EXIST "%~1" (
	RMDIR /S /Q "%~1"
)
exit /b