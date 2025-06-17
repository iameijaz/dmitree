@echo off
setlocal EnableDelayedExpansion

rem ──────────────── 1. Find tools ────────────────
for /f "delims=" %%G in ('where gcc 2^>nul') do (
    set "GCC_PATH=%%G"
    goto :foundGcc
)
echo Error: gcc not found in PATH.& exit /b 1
:foundGcc

for /f "delims=" %%G in ('where mingw32-make 2^>nul') do (
    set "MAKE_PATH=%%G"
    goto :foundMake
)
echo Error: mingw32-make not found in PATH.& exit /b 1
:foundMake

rem ──────────────── 2. Derive MSYS2 prefix ────────────────
rem (strip off \mingw64\bin\gcc.exe)
set "MSYS_PREFIX=!GCC_PATH:\mingw64\bin\gcc.exe=!"
set "C_COMPILER=!GCC_PATH!"
set "MAKE_TOOL=!MAKE_PATH!"

echo Using compiler:  !C_COMPILER!
echo Using make:      !MAKE_TOOL!

rem ──────────────── 3. Prepare build dir ────────────────
if not exist build mkdir build
pushd build

rem ──────────────── 4. Run CMake configure ────────────────
cmake ^
  -G "MinGW Makefiles" ^
  -DCMAKE_C_COMPILER="!C_COMPILER!" ^
  -DCMAKE_MAKE_PROGRAM="!MAKE_TOOL!" ^
  ..

if errorlevel 1 (
  echo CMake configuration failed.& popd & exit /b 1
)

rem ──────────────── 5. Build ────────────────
"!MAKE_TOOL!"
if errorlevel 1 (
  echo Build failed.& popd & exit /b 1
)

echo.
echo ✅ Build succeeded!
echo Executable is at: %CD%\dmitree.exe

popd
endlocal
