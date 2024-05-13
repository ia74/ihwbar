@ECHO OFF

color 0a
cls

mkdir build
mkdir bin
cd build

:PACK_RESOURCES
echo Packing resources...
rc /nologo /r /fo bar.res ../src/bar.rc
if errorlevel 1 goto :ERROR
goto :COMPILE

:ERROR
echo Error: %ERRORLEVEL%
pause
exit /b %ERRORLEVEL%

:COMPILE
echo Compiling...
cl /nologo /EHsc User32.lib Gdi32.lib ^
	../src/main.cpp ^
	../src/widgets/WidgetText.cpp ^
	../src/Boot.cpp ^
	../src/BarConfiguration.cpp ^
	bar.res
if errorlevel 1 goto :ERROR
goto :RENAME

:RENAME
echo Renaming...
move main.exe IHWBar.exe
if errorlevel 1 goto :ERROR
goto :MOVE_TO_BIN

:MOVE_TO_BIN
echo Moving to bin...
move IHWBar.exe ../bin
if errorlevel 1 goto :ERROR
goto :RUN

:RUN
color 07
echo Running...
cd ..
start bin/IHWBar.exe
if errorlevel 1 goto :ERROR
goto :EOF

:EOF
