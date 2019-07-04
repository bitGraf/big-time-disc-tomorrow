@echo off
echo Building...

cls

set LIB_VS="..\..\deps\Other\msvc"
set LIB_SDK_UC="..\..\deps\Other\ucrt"
set LIB_SDK_UM="..\..\deps\Other\um"

set files=gjk.c example.c

set opts=/MD /nologo /EHsc

set out_file="prog.exe"
cl %opts% %files% gdi32.lib kernel32.lib user32.lib shell32.lib /link /LIBPATH:%LIB% /LIBPATH:%LIB_VS% /LIBPATH:%LIB_SDK_UC% /LIBPATH:%LIB_SDK_Um% /out:%out_file%
del *.obj

echo Build Complete
echo.