@echo off
echo Building...

cls

set INC_GL="..\deps\OpenGL\headers"

set LIB="..\deps\OpenGL\libs"
set LIB_VS="..\deps\Other\msvc"
set LIB_SDK_UC="..\deps\Other\ucrt"
set LIB_SDK_UM="..\deps\Other\um"

set files=Main.cpp Game.cpp glad.c ShaderProgram.cpp Player.cpp
set files=%files% VMQMath.cpp ModelLoader.cpp EntityManager.cpp
set files=%files% Platform.cpp Input.cpp Texture.cpp Level.cpp
set files=%files% stb_implementation.c DynamicFont.cpp
set files=%files% ResourceManager.cpp Crawler.cpp Lights.cpp
set files=%files% Physics.cpp Missile.cpp AI.cpp CollisionDetection.cpp
set files=%files% EPA.cpp GJK.cpp

set opts=/MD /nologo /EHsc

pushd src
set out_file="..\run_tree\Release\run.exe"
cl %opts% /I %INC_GL% %files% gdi32.lib kernel32.lib user32.lib shell32.lib glew32.lib glew32s.lib glfw3.lib glfw3dll.lib OpenGL32.lib /link /LIBPATH:%LIB% /LIBPATH:%LIB_VS% /LIBPATH:%LIB_SDK_UC% /LIBPATH:%LIB_SDK_Um% /out:%out_file%
del *.obj

popd
echo Build Complete
echo.