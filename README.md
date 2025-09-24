# AI智能伴游系统

一款基于嵌入式芯片（如ESP32-S3）兼容x86（Windows/Ubuntu）环境的AI智能伴游产品，通过GPS/IMU及视觉定位识别分析实现位置识别、文化讲解和解闷陪伴的功能。

## 项目结构

```
AICompanion/
├── main.cpp              # 主程序入口
├── AICompanion.h/cpp     # 核心控制类
├── LocationTracker.h/cpp # 位置追踪模块（GPS/IMU）
├── VisionProcessor.h/cpp # 视觉处理模块
├── CulturalGuide.h/cpp   # 文化讲解模块
├── Chatbot.h/cpp         # 聊天机器人模块（解闷陪伴）
├── SensorManager.h/cpp   # 传感器管理模块
├── CMakeLists.txt        # CMake构建文件
└── README.md             # 项目说明文档
```

## 功能特点

1. **位置识别**：
   - GPS定位和导航
   - IMU传感器数据融合
   - 反向地理编码（获取地址信息）

2. **视觉识别**：
   - 目标检测和识别
   - 文化文物识别
   - 图像保存和处理

3. **文化讲解**：
   - 基于位置的文化知识讲解
   - 基于视觉识别的文物讲解
   - 可扩展的文化知识库

4. **解闷陪伴**：
   - 多模式聊天（普通、文化、笑话、故事等）
   - 对话历史记录
   - 智能回复生成

5. **跨平台兼容**：
   - 支持ESP32-S3嵌入式平台
   - 支持x86桌面平台（Windows/Ubuntu）

## 环境设置与构建指南

### 前提条件

- CMake 3.10或更高版本
- C++11兼容的编译器（如GCC、MSVC等）
- （可选）ESP32-S3开发环境（如需要在ESP32上运行）

### 安装必要的工具

#### Windows系统

1. **安装Visual Studio**
   - 下载并安装[Visual Studio Community](https://visualstudio.microsoft.com/zh-hans/vs/community/)
   - 在安装过程中选择"使用C++的桌面开发"工作负载

2. **安装CMake**
   - 下载并安装[CMake](https://cmake.org/download/)
   - 在安装过程中选择"Add CMake to the system PATH for all users"

3. **安装Git（可选）**
   - 下载并安装[Git for Windows](https://git-scm.com/download/win)

#### Ubuntu系统

```bash
# 更新软件包列表
sudo apt update

# 安装C++编译器
sudo apt install build-essential

# 安装CMake
sudo apt install cmake

# 安装Git（可选）
sudo apt install git
```

### 在x86平台构建

#### Windows系统（使用Visual Studio）

您可以选择以下方法之一：

方法一：增强版构建脚本（推荐）
1. 确保您的系统已安装Visual Studio（包含C++开发工具）
2. 双击运行`new_simple_build.bat`脚本
3. 脚本会自动检测Visual Studio环境、设置构建参数并编译代码
4. 编译成功后，脚本会自动启动程序
5. 构建成功后，可执行文件位于`bin\AICompanion.exe`

这个脚本提供了更强大的错误处理和自动环境检测功能，适合大多数Windows用户。

方法二：使用CMake
```bash
# 创建构建目录
mkdir build && cd build

# 配置项目（使用Visual Studio生成器）
cmake -G "Visual Studio 17 2022" -A x64 ..

# 编译项目
cmake --build . --config Release
```

#### 方法三：使用修复版批处理脚本（推荐）
```bash
# 直接运行修复版批处理脚本
fixed_build_windows.bat
```
这个修复版脚本解决了原始脚本中的几个问题，包括VS安装路径检测、MSBuild定位和平台兼容性问题。它会自动检测Visual Studio环境，创建必要的项目文件，并使用MSBuild构建项目。
构建成功后，可执行文件将位于x64\Release\AICompanion.exe

#### 方法四：使用原始批处理脚本
```bash
# 直接运行原始批处理脚本
build_windows.bat
```
如果修复版脚本仍然有问题，可以尝试使用原始版本。构建成功后，可执行文件将位于x64\Release\AICompanion.exe

方法四：使用简单构建脚本（适用于没有CMake的环境）
```bash
# 直接运行简单构建脚本
simple_build.bat
```

此脚本直接使用Visual Studio的cl编译器编译所有源文件，适用于没有安装CMake的环境。使用前需确保已打开Visual Studio的"Developer Command Prompt"或已运行"vcvarsall.bat x64"设置编译器环境。构建成功后，可执行文件将位于bin\AICompanion.exe

#### Ubuntu系统（使用GCC）

您可以选择以下两种方法之一：

方法一：使用CMake
```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译项目
make -j$(nproc)
```

方法二：使用提供的shell脚本（更简单）
```bash
# 给脚本添加执行权限
chmod +x build_ubuntu.sh

# 运行脚本
./build_ubuntu.sh
```

shell脚本会自动检查g++编译器和C++11支持，创建构建目录，并直接使用g++编译所有源文件，无需手动配置CMake环境。构建成功后，可执行文件将位于build_linux/AICompanion

### 在ESP32平台构建

1. **安装ESP-IDF**
   - 按照[ESP-IDF官方文档](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s3/get-started/index.html)安装ESP-IDF

2. **配置项目**
   ```bash
   # 设置ESP-IDF环境变量
   . $HOME/esp/esp-idf/export.sh
   
   # 进入项目目录
   cd /path/to/AICompanion
   
   # 设置目标芯片
   idf.py set-target esp32s3
   
   # 配置项目（可选）
   idf.py menuconfig
   ```

3. **编译和烧录**
   ```bash
   # 编译项目
   idf.py build
   
   # 烧录到ESP32-S3
   idf.py -p /dev/ttyUSB0 flash
   
   # 查看串口输出
   idf.py -p /dev/ttyUSB0 monitor
   ```

## 使用说明

1. 运行编译生成的可执行文件：
   ```bash
   ./AICompanion
   ```

2. 系统启动后，可以使用以下命令与系统交互：
   - `help` - 显示帮助信息
   - `status` - 显示系统状态
   - `location` - 获取当前位置
   - `detect` - 开始视觉检测
   - `stop` - 停止视觉检测
   - `exit/quit` - 退出系统
   - 其他输入 - 与AI伴游对话

## 平台兼容性

系统通过条件编译（`#ifdef ESP32`）来区分不同平台的实现：

- **ESP32-S3平台**：使用轻量级模型和ESP32特定的硬件接口
- **x86平台**：使用完整功能的模型和模拟传感器数据

## 扩展和定制

1. **添加新的文化知识**：
   - 在`CulturalGuide.cpp`的`initializeDefaultKnowledge`方法中添加新的文化信息

2. **自定义聊天回复**：
   - 在`Chatbot.cpp`的`initializeResponseTemplates`方法中添加新的回复模板

3. **集成真实传感器**：
   - 在各传感器相关类中替换模拟数据为实际传感器数据读取代码

## 注意事项

1. 目前的实现中，传感器数据和视觉识别结果都是模拟的
2. 在实际应用中，需要集成真实的GPS模块、IMU传感器和摄像头
3. 对于ESP32平台，可能需要根据实际硬件配置调整代码
4. 文化知识库可以通过外部文件加载，实现更灵活的内容更新

## License

[MIT License](https://opensource.org/licenses/MIT)