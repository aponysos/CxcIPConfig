xcopy ..\log4cpp-msvc14\output\Debug\log4cpp.dll .\output\Win32\Debug /D
xcopy ..\log4cpp-msvc14\output\Debug\log4cpp.pdb .\output\Win32\Debug /D
xcopy ..\log4cpp-msvc14\output\Debug\log4cpp.lib .\lib\Debug /D

xcopy ..\log4cpp-msvc14\output\Release\log4cpp.dll .\output\Win32\Release /D
xcopy ..\log4cpp-msvc14\output\Release\log4cpp.pdb .\output\Win32\Release /D
xcopy ..\log4cpp-msvc14\output\Release\log4cpp.lib .\lib\Release /D

xcopy ..\log4cpp-msvc14\include .\include /D /S /E

pause
