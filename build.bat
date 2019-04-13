@echo off
echo Building...

cls

set INC_GL="G:\deps\OpenGL\headers"
set INC_VS="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\include"
set INC_SDK_SH="C:\Program Files (x86)\Windows Kits\10\Include\10.0.16299.0\shared"
set INC_SDK_UM="C:\Program Files (x86)\Windows Kits\10\Include\10.0.16299.0\um"
REM set INC_SDK_UC="C:\Program Files (x86)\Windows Kits\10\Include\10.0.16299.0\ucrt"

set LIB="G:\deps\OpenGL\libs"
set LIB_VS="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\lib\x64"
set LIB_SDK_UC="C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\ucrt\x64"
set LIB_SDK_UM="C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64"

set files=Main.cpp Game.cpp glad.c ShaderProgram.cpp Player.cpp
set files=%files% VMQMath.cpp ModelLoader.cpp EntityManager.cpp
set files=%files% Platform.cpp
set files=%files% stb_implementation.c DynamicFont.cpp

set opts=/MD /nologo /EHsc

pushd src
set out_file="..\run_tree\run.exe"
cl %opts% /I %INC_GL% %files% gdi32.lib kernel32.lib user32.lib shell32.lib glew32.lib glew32s.lib glfw3.lib glfw3dll.lib OpenGL32.lib /link /LIBPATH:%LIB% /LIBPATH:%LIB_VS% /LIBPATH:%LIB_SDK_UC% /LIBPATH:%LIB_SDK_Um% /out:%out_file%
del *.obj

popd
echo Build Complete
echo.