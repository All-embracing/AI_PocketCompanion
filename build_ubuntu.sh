#!/bin/bash

# AI智能伴游系统Ubuntu构建脚本
# 此脚本自动检查依赖并使用g++直接构建项目

# 设置颜色变量
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # 无颜色

# 打印标题
clear
echo -e "${GREEN}AI智能伴游系统Ubuntu构建脚本${NC}"

# 检查g++编译器
if ! command -v g++ &> /dev/null
then
    echo -e "${RED}未找到g++编译器。请先安装：${NC}"
    echo "sudo apt update && sudo apt install build-essential"
    exit 1
fi

# 获取g++版本
GCC_VERSION="$(g++ -dumpversion)"
echo -e "找到g++版本：${GREEN}${GCC_VERSION}${NC}"

# 检查是否支持C++11
echo -e "检查C++11支持..."

# 检查是否安装了bc工具
if ! command -v bc &> /dev/null
then
    echo -e "${RED}未找到bc工具，无法检查C++11支持。请安装：${NC}"
    echo "sudo apt install bc"
    exit 1
fi

if [ "$(echo "$GCC_VERSION >= 4.8" | bc -l)" -eq 1 ]
then
    echo -e "${GREEN}✓ C++11支持已确认${NC}"
else
    echo -e "${RED}✗ g++版本过低，不支持C++11。请升级g++。${NC}"
    exit 1
fi

# 检查curl库是否安装
echo -e "检查curl库支持..."
if ! pkg-config --exists libcurl

then
    echo -e "${RED}未找到libcurl库。请安装：${NC}"
    echo "sudo apt update && sudo apt install libcurl4-openssl-dev"
    exit 1
else
    CURL_CFLAGS="$(pkg-config --cflags libcurl)"
    CURL_LIBS="$(pkg-config --libs libcurl)"
    echo -e "${GREEN}✓ 找到libcurl库${NC}"
    echo -e "curl编译标志: ${CURL_CFLAGS}"
    echo -e "curl链接标志: ${CURL_LIBS}"
fi

# 检查nlohmann/json库是否安装
echo -e "检查nlohmann/json库支持..."
#if ! pkg-config --exists nlohmann_json
if ! dpkg -l | grep -qE '^ii\s+nlohmann-json[0-9]*-dev'

then
    echo -e "${RED}未找到nlohmann/json库。请安装：${NC}"
    echo "sudo apt update && sudo apt install nlohmann-json3-dev"
    exit 1
else
    JSON_CFLAGS="$(pkg-config --cflags nlohmann_json)"
    JSON_LIBS="$(pkg-config --libs nlohmann_json)"
    echo -e "${GREEN}✓ 找到nlohmann/json库${NC}"
    echo -e "json编译标志: ${JSON_CFLAGS}"
    echo -e "json链接标志: ${JSON_LIBS}"
fi

# 检查OpenCV是否安装
echo -e "检查OpenCV支持..."
if ! command -v pkg-config &> /dev/null

then
    echo -e "${RED}未找到pkg-config工具，无法检查OpenCV支持。请安装：${NC}"
    echo "sudo apt install pkg-config"
    exit 1
fi

if pkg-config --exists opencv4 || pkg-config --exists opencv
then
    # 获取OpenCV编译和链接标志
    if pkg-config --exists opencv4
    then
        OPENCV_CFLAGS="$(pkg-config --cflags opencv4)"
        OPENCV_LIBS="$(pkg-config --libs opencv4)"
        echo -e "${GREEN}✓ 找到OpenCV 4版本${NC}"
    else
        OPENCV_CFLAGS="$(pkg-config --cflags opencv)"
        OPENCV_LIBS="$(pkg-config --libs opencv)"
        echo -e "${GREEN}✓ 找到OpenCV版本${NC}"
    fi
    echo -e "OpenCV编译标志: ${OPENCV_CFLAGS}"
    echo -e "OpenCV链接标志: ${OPENCV_LIBS}"
else
    echo -e "${RED}✗ 未找到OpenCV库。请安装：${NC}"
    echo "sudo apt update && sudo apt install libopencv-dev"
    exit 1
fi

# 创建构建目录
BUILD_DIR="build_linux"
if [ ! -d "$BUILD_DIR" ]
then
    echo -e "创建构建目录：${GREEN}${BUILD_DIR}${NC}"
    mkdir -p "$BUILD_DIR"
else
    echo -e "使用现有构建目录：${GREEN}${BUILD_DIR}${NC}"
fi

# 收集所有源文件
SOURCE_FILES=(src/main.cpp src/core/AICompanion.cpp src/location/LocationTracker.cpp src/vision/VisionProcessor.cpp src/vision/model_utils.cpp src/cultural/CulturalGuide.cpp src/chat/Chatbot.cpp src/sensor/SensorManager.cpp)

# 检查源文件是否存在
for file in "${SOURCE_FILES[@]}"
do
    if [ ! -f "$file" ]
    then
        echo -e "${RED}错误：未找到源文件 $file${NC}"
        exit 1
    fi
done

# 编译选项 - 设置包含路径以确保编译器能找到所有头文件
CXXFLAGS="-std=c++11 -Wall -O2 -D_X86 -Iinclude -I../include -I/usr/include $OPENCV_CFLAGS $CURL_CFLAGS $JSON_CFLAGS"

# 开始编译
cd "$BUILD_DIR"
echo -e "开始编译项目..."

g++ $CXXFLAGS ../src/main.cpp ../src/core/AICompanion.cpp ../src/location/LocationTracker.cpp ../src/vision/VisionProcessor.cpp ../src/vision/model_utils.cpp ../src/cultural/CulturalGuide.cpp ../src/chat/Chatbot.cpp ../src/sensor/SensorManager.cpp -o AICompanion $OPENCV_LIBS $CURL_LIBS $JSON_LIBS

# 检查编译是否成功
if [ $? -eq 0 ]
then
    echo -e "${GREEN}✓ 构建成功！${NC}"
    echo -e "可执行文件位于：${GREEN}$(pwd)/AICompanion${NC}"
    echo -e "\n运行方法：./AICompanion"
    echo -e "\n命令列表："
    echo -e "  help   - 显示帮助信息"
    echo -e "  status - 显示系统状态"
    echo -e "  location - 获取当前位置"
    echo -e "  detect - 开始视觉检测"
    echo -e "  stop   - 停止视觉检测"
    echo -e "  chatmode 模式 - 切换聊天模式（模式: normal/普通, cultural/文化, joke/笑话/解闷, story/故事, guide/导游/伴游）"
    echo -e "  setapikey 密钥 - 设置智谱AI API密钥"
    echo -e "  其他输入    - 与AI伴游对话"
    echo -e "  exit/quit - 退出系统"
else
    echo -e "${RED}✗ 构建失败！${NC}"
    exit 1
fi