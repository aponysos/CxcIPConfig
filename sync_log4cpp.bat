xcopy ..\log4cpp-msvc14\output\Debug\log4cpp.dll .\lib\Debug /D
xcopy ..\log4cpp-msvc14\output\Debug\log4cpp.pdb .\lib\Debug /D
xcopy ..\log4cpp-msvc14\output\Debug\log4cpp.lib .\lib\Debug /D

xcopy ..\log4cpp-msvc14\output\Release\log4cpp.dll .\lib\Release /D
xcopy ..\log4cpp-msvc14\output\Release\log4cpp.pdb .\lib\Release /D
xcopy ..\log4cpp-msvc14\output\Release\log4cpp.lib .\lib\Release /D

xcopy ..\log4cpp-msvc14\include .\include /D /S /E

pause
