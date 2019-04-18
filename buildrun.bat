@echo off

call build.bat
rem pushd run_tree
call run_tree/Release/run.exe
rem popd
