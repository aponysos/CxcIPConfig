xcopy .\lib\Debug\x64\*.dll .\output\Debug\x64\ /D /Y
xcopy .\lib\Debug\x64\*.pdb .\output\Debug\x64\ /D /Y
xcopy .\lib\Release\x64\*.dll .\output\Release\x64\ /D /Y
xcopy .\lib\Release\x64\*.pdb .\output\Release\x64\ /D /Y

xcopy .\lib\Debug\Win32\*.dll .\output\Debug\Win32\ /D /Y
xcopy .\lib\Debug\Win32\*.pdb .\output\Debug\Win32\ /D /Y
xcopy .\lib\Release\Win32\*.dll .\output\Release\Win32\ /D /Y
xcopy .\lib\Release\Win32\*.pdb .\output\Release\Win32\ /D /Y

pause
