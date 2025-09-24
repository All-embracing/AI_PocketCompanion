# AICompanion Windows 构建指南

## 增强版构建脚本

已更新的 `build_windows.bat` 脚本具有**自动编译器检测功能**，可以识别并使用系统中已安装的各种C++编译器，包括VS Code环境中常用的编译器。

## 支持的编译器

脚本会自动按以下顺序查找并使用系统中的编译器：

1. **Visual Studio MSVC (cl.exe)** - 微软官方C++编译器
   - 首先检查系统PATH中的cl.exe
   - 然后专门搜索D:\Program Files (x86)目录下的Visual Studio安装
   - 支持Visual Studio 2015/2017/2019/2022版本
   - 支持Community/Professional/Enterprise版本
   - 支持Visual Studio Build Tools
2. **MinGW GCC (g++.exe)** - VS Code中常用的开源编译器
3. **LLVM/Clang (clang++.exe)** - 现代化的开源编译器

## 快速开始

### 方法一：直接运行增强版构建脚本

1. 双击运行 `build_windows.bat`
2. 脚本会自动检测系统中可用的编译器，包括专门搜索D:\Program Files (x86)目录下的Visual Studio
3. 如检测到编译器，将自动设置必要的环境变量并开始编译过程
4. 编译成功后，可执行文件将位于 `bin\AICompanion.exe`

### 方法二：从命令提示符运行

1. 打开命令提示符（Win+R → cmd → 确定）
2. 导航到项目目录：
   ```
   cd d:\test_lab\ai_companion
   ```
3. 运行构建脚本：
   ```
   build_windows.bat
   ```

## VS Code环境配置

### VS Code + MinGW 配置指南

1. **安装VS Code**
   - 从 [VS Code官网](https://code.visualstudio.com/) 下载并安装

2. **安装MinGW编译器**
   - 从 [MinGW官网](https://sourceforge.net/projects/mingw/)
   - 安装时选择"mingw32-gcc-g++"包
   - 将MinGW的bin目录（通常是 `C:\MinGW\bin`）添加到系统PATH环境变量

3. **在VS Code中安装C/C++扩展**
   - 打开VS Code
   - 在扩展面板中搜索并安装"C/C++"扩展（由Microsoft提供）

4. **配置VS Code的tasks.json**（可选，但推荐）
   - 按 `Ctrl+Shift+P` 打开命令面板
   - 输入"Tasks: Configure Task"并选择
   - 选择"Create tasks.json file from template"
   - 选择"Others"
   - 替换tasks.json内容为：
   ```json
   {
     "version": "2.0.0",
     "tasks": [
       {
         "label": "Build AICompanion",
         "type": "shell",
         "command": "build_windows.bat",
         "group": {
           "kind": "build",
           "isDefault": true
         },
         "problemMatcher": ["$gcc"]
       }
     ]
   }
   ```

5. **使用VS Code构建**
   - 按 `Ctrl+Shift+B` 运行构建任务
   - 或直接在VS Code的终端中运行 `build_windows.bat`

## 常见问题解决

### 错误：未找到C++编译器

- **原因**：系统中没有安装C++编译器，或编译器未添加到PATH环境变量
- **解决方案**：
  1. 按照脚本中的提示安装Visual Studio或MinGW
  2. 确保Visual Studio已安装"使用C++的桌面开发"工作负载
  3. 如果编译器已安装在D:\Program Files (x86)目录但仍未检测到，请检查安装是否完整
  4. 安装完成后重启命令提示符或VS Code

### 错误：Visual Studio编译器已找到但编译失败

- **原因**：即使找到了编译器可执行文件，也可能缺少必要的库文件或环境设置
- **解决方案**：
  1. 尝试从Visual Studio的"Developer Command Prompt"运行脚本
  2. 确保Visual Studio安装了完整的C++开发工具
  3. 检查项目目录中的所有源文件是否都存在且没有损坏

### 错误：编译失败

- **对于MSVC编译器**：
  1. 确保Visual Studio已安装"使用C++的桌面开发"工作负载
  2. 尝试从Visual Studio的"Developer Command Prompt"运行脚本

- **对于MinGW/GCC编译器**：
  1. 确保MinGW安装正确且已添加到PATH
  2. 检查所有源文件是否存在于项目目录中

### 错误：编译成功但找不到可执行文件

- **原因**：可能是输出目录创建失败或权限问题
- **解决方案**：
  1. 检查`bin`目录是否存在
  2. 尝试以管理员身份运行构建脚本

## 构建命令详解

脚本会根据检测到的编译器自动选择合适的编译命令：

- **MSVC**：`cl /EHsc /O2 /D_X86 *.cpp /Fe:bin\AICompanion.exe`
- **GCC/Clang**：`g++/clang++ -O2 -D_X86 *.cpp -o bin\AICompanion.exe`

其中：
- `/O2/-O2`：启用优化
- `/D_X86/-D_X86`：定义_X86预处理宏
- `/EHsc`：启用C++异常处理

## 程序运行

构建成功后：

1. 可以直接在脚本提示时输入`Y`运行程序
2. 或手动导航到`bin`目录并运行`AICompanion.exe`
3. 在程序中使用以下命令：
   - `help`    : 显示帮助信息
   - `status`  : 显示系统状态
   - `location`: 显示当前位置
   - `detect`  : 开始文化文物检测
   - `stop`    : 停止检测
   - `exit`    : 退出程序