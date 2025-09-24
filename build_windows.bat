@echo off
REM This script builds AICompanion for Windows
REM Enhanced version with compiler auto-detection

REM Enable delayed variable expansion and set UTF-8 encoding
setlocal enabledelayedexpansion
chcp 65001 >nul

REM Create output directory
if not exist "bin" mkdir "bin"

REM Function to find and use available C++ compiler
:find_compiler
    REM Try Visual Studio MSVC compiler (CL) from PATH
    echo Searching for C++ compilers...
    where cl >nul 2>nul
    if "%ERRORLEVEL%" == "0" (
        echo Found Visual Studio C++ compiler (cl.exe) in PATH
        set "COMPILER=cl"
        set "COMPILE_FLAGS=/EHsc /O2 /D_X86"
        set "OUTPUT_FLAG=/Fe:bin\AICompanion.exe"
        goto compile
    )

    REM Try to find Visual Studio in D:\Program Files (x86) directory
    echo Searching for Visual Studio installation in "D:\Program Files (x86)"...
    
    REM Check common Visual Studio versions in D:\Program Files (x86)
    set "VS_VERSIONS=2022 2019 2017 2015"
    for %%v in (%VS_VERSIONS%) do (
        set "VS_PATH=D:\Program Files (x86)\Microsoft Visual Studio\%%v"
        if exist "!VS_PATH!" (
            echo Found Visual Studio %%v at "!VS_PATH!"
            
            REM Check for Community, Professional, Enterprise editions
            set "VS_EDITIONS=Community Professional Enterprise"
            for %%e in (!VS_EDITIONS!) do (
                if exist "!VS_PATH!\%%e" (
                    echo Found Visual Studio %%v %%e edition
                    
                    REM Check for cl.exe in common locations
                    set "CL_PATH=!VS_PATH!\%%e\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe"
                    for /f "tokens=*" %%c in ('dir /b /s "!CL_PATH!" 2^>nul') do (
                        echo Found Visual Studio C++ compiler at "%%c"
                        set "COMPILER=%%c"
                        set "COMPILER_PATH=%%~dpc"
                        set "COMPILE_FLAGS=/EHsc /O2 /D_X86"
                        set "OUTPUT_FLAG=/Fe:bin\AICompanion.exe"
                        goto compile
                    )
                )
            )
        )
    )

    REM Check for Build Tools in D:\Program Files (x86)
    if exist "D:\Program Files (x86)\Microsoft Visual Studio\BuildTools" (
        echo Found Visual Studio Build Tools
        set "CL_PATH=D:\Program Files (x86)\Microsoft Visual Studio\BuildTools\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe"
        for /f "tokens=*" %%c in ('dir /b /s "%CL_PATH%" 2^>nul') do (
            echo Found Visual Studio C++ compiler at "%%c"
            set "COMPILER=%%c"
            set "COMPILER_PATH=%%~dpc"
            set "COMPILE_FLAGS=/EHsc /O2 /D_X86"
            set "OUTPUT_FLAG=/Fe:bin\AICompanion.exe"
            goto compile
        )
    )

    REM Try MinGW GCC compiler (g++), which is often used with VS Code
    where g++ >nul 2>nul
    if "%ERRORLEVEL%" == "0" (
        echo Found MinGW GCC compiler (g++).exe
        set "COMPILER=g++"
        set "COMPILE_FLAGS=-O2 -D_X86"
        set "OUTPUT_FLAG=-o bin\AICompanion.exe"
        goto compile
    )

    REM Try LLVM/Clang compiler (clang++)
    where clang++ >nul 2>nul
    if "%ERRORLEVEL%" == "0" (
        echo Found LLVM/Clang compiler (clang++).exe
        set "COMPILER=clang++"
        set "COMPILE_FLAGS=-O2 -D_X86"
        set "OUTPUT_FLAG=-o bin\AICompanion.exe"
        goto compile
    )

    REM If no compiler found, try to detect VS Code installation and suggest extensions
    echo.
    echo Error: No C++ compiler found on your system.
    echo.
    echo VS Code doesn't include a C++ compiler by default. You need to install one separately.
    echo.
    echo Recommended options:
    echo 1. Install Visual Studio with "Desktop development with C++" workload
    echo 2. Install MinGW (often used with VS Code):
    echo    - Download from https://sourceforge.net/projects/mingw/
    echo    - Install the g++ package
    echo    - Add MinGW\bin to your PATH environment variable
    echo 3. In VS Code, install C/C++ extension by Microsoft
    echo.
    echo After installation, restart this script or run it from a Visual Studio Command Prompt.
    pause
    exit /b 1

:compile
    REM Compile the project with the found compiler
    echo.
    echo Compiling AICompanion using %COMPILER%...
    
    REM If we found a compiler with full path (not in PATH), we may need to set environment variables
    if defined COMPILER_PATH (
        echo Setting up Visual Studio environment variables...
        
        REM Get the VC Tools directory (one level up from compiler bin directory)
        for %%d in ("!COMPILER_PATH!\..\..") do set "VCTOOLS_PATH=%%~fd"
        
        REM Add VC Tools bin and lib paths to PATH and LIB environment variables
        set "PATH=!COMPILER_PATH!;!VCTOOLS_PATH!\bin\Hostx64\x64;!PATH!"
        set "LIB=!VCTOOLS_PATH!\lib\x64;!LIB!"
        set "INCLUDE=!VCTOOLS_PATH!\include;!INCLUDE!"
    )
    
    echo Command: !COMPILER! !COMPILE_FLAGS! main.cpp AICompanion.cpp LocationTracker.cpp VisionProcessor.cpp CulturalGuide.cpp Chatbot.cpp SensorManager.cpp !OUTPUT_FLAG!
    
    !COMPILER! !COMPILE_FLAGS! main.cpp AICompanion.cpp LocationTracker.cpp VisionProcessor.cpp CulturalGuide.cpp Chatbot.cpp SensorManager.cpp !OUTPUT_FLAG!

    if "!ERRORLEVEL!" neq "0" (
        echo.
        echo Error: Compilation failed!
        echo.
        echo Solutions:
        
        REM Check if it's Visual Studio compiler (either cl.exe or full path)
        if "!COMPILER!"=="cl" (
            echo 1. Make sure Visual Studio with C++ tools is properly installed
            echo 2. Run this script from a Visual Studio Developer Command Prompt
        ) else (
            REM Check if it's a path containing cl.exe
            echo !COMPILER! | findstr /i "cl.exe" >nul
            if "!ERRORLEVEL!" == "0" (
                REM VS compiler found in D:\Program Files (x86) but environment setup might be incomplete
                echo 1. Make sure Visual Studio with C++ tools is properly installed
                echo 2. Try running this script from a Visual Studio Developer Command Prompt
                echo 3. Ensure all Visual Studio components are installed correctly
            ) else (
                REM Not a VS compiler
                echo 1. Make sure your !COMPILER! installation is correct
                echo 2. Check if all required source files exist in the project directory
            )
        )
        echo 4. Try using simple_build.bat for a more basic build process
        pause
        exit /b 1
    )

    REM Check if the executable was created
    if not exist "bin\AICompanion.exe" (
        echo.
        echo Error: Compilation completed but executable not found!
        echo Check if the output directory was created correctly.
        pause
        exit /b 1
    )

    REM Build successful
    echo.
    echo ====================================================
    echo Build successful!
    echo Executable: bin\AICompanion.exe
    echo ====================================================
    echo.
    echo Available commands:
    echo - help    : Show help information
    echo - status  : Show system status
    echo - location: Show current location
    echo - detect  : Start cultural relic detection
    echo - stop    : Stop detection
    echo - exit    : Exit program
    
    REM Ask user if they want to run the program
    set "runProgram=N"
    set /p "runProgram=Run program now? (Y/N): "
    if /i "!runProgram!"=="Y" (
        echo Starting AICompanion...
        start bin\AICompanion.exe
    )

:end
    pause
    endlocal