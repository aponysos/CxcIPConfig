xcopy .\lib\Debug\x64\*.pdb .\outdir\Debug\x64\ /D /Y
xcopy .\lib\Release\x64\*.pdb .\outdir\Release\x64\ /D /Y

xcopy .\lib\Debug\Win32\*.pdb .\outdir\Debug\Win32\ /D /Y
xcopy .\lib\Release\Win32\*.pdb .\outdir\Release\Win32\ /D /Y

pause
