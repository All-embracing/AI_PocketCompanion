@echo off

REM AI智能伴游系统 - 简单Windows构建脚本
REM 此脚本适用于没有CMake和bash的Windows环境

echo AI智能伴游系统 - 简单Windows构建脚本

echo 正在检查Visual Studio环境...

REM 检查是否存在cl编译器（Visual Studio C++编译器）
cl /? >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo 未找到Visual Studio C++编译器。
    echo 请先打开Visual Studio的"Developer Command Prompt"
    echo 或运行"vcvarsall.bat x64"来设置编译器环境
    echo.
    echo 或者，您可以安装MinGW并将其添加到系统PATH中
    pause
    exit /b 1
)

echo 找到C++编译器，开始编译...

REM 创建输出目录
if not exist "bin" mkdir "bin"

REM 编译所有源文件
cl /EHsc /O2 /D_X86 /Fe:bin\AICompanion.exe main.cpp AICompanion.cpp LocationTracker.cpp VisionProcessor.cpp CulturalGuide.cpp Chatbot.cpp SensorManager.cpp

if %ERRORLEVEL% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo.
echo 编译成功！
echo 可执行文件位于：bin\AICompanion.exe

echo.
echo 使用说明：
echo 1. 运行 bin\AICompanion.exe 启动程序

echo 2. 系统支持以下命令：
echo    help   - 显示帮助信息
echo    status - 显示系统状态
echo    location - 获取当前位置
echo    detect - 开始视觉检测
echo    stop   - 停止视觉检测
echo    exit/quit - 退出系统
echo    其他输入 - 与AI伴游对话

echo.
pause