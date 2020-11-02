@echo off
cmake . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=RELEASE
cmake --build . --config Release
del cmake_install.cmake
del Makefile
del CMakeCache.txt
call :SafeRMDIR CMakeFiles

exit /b


:SafeRMDIR
IF EXIST "%~1" (
	RMDIR /S /Q "%~1"
)
exit /b