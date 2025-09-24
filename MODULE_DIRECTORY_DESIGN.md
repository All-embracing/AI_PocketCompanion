# 模块功能目录结构设计方案

## 1. 当前项目结构分析

目前项目所有源文件和头文件都存放在根目录下，虽然通过文件命名体现了模块化设计，但随着项目规模扩大，这种扁平结构会导致文件管理困难，不利于后续维护和扩展。

## 2. 模块功能划分方案

根据项目的功能特点和现有代码结构，建议按照以下模块进行目录划分：

```
AICompanion/
├── src/                  # 源代码主目录
│   ├── core/             # 核心控制模块
│   ├── location/         # 位置追踪模块
│   ├── vision/           # 视觉处理模块
│   ├── cultural/         # 文化讲解模块
│   ├── chat/             # 聊天机器人模块
│   ├── sensor/           # 传感器管理模块
│   └── utils/            # 通用工具类和函数
├── include/              # 头文件目录
│   ├── core/             # 核心控制模块头文件
│   ├── location/         # 位置追踪模块头文件
│   ├── vision/           # 视觉处理模块头文件
│   ├── cultural/         # 文化讲解模块头文件
│   ├── chat/             # 聊天机器人模块头文件
│   ├── sensor/           # 传感器管理模块头文件
│   └── utils/            # 通用工具类头文件
├── build/                # 构建输出目录
├── scripts/              # 构建脚本目录
├── docs/                 # 文档目录
└── examples/             # 示例代码目录
```

## 3. 模块功能说明

### 3.1 core 模块
- **功能**：系统核心控制模块，负责协调各个子模块的工作
- **文件**：AICompanion.cpp, AICompanion.h

### 3.2 location 模块
- **功能**：位置追踪、GPS定位和导航、IMU传感器数据融合
- **文件**：LocationTracker.cpp, LocationTracker.h

### 3.3 vision 模块
- **功能**：视觉处理、目标检测和识别、文化文物识别
- **文件**：VisionProcessor.cpp, VisionProcessor.h

### 3.4 cultural 模块
- **功能**：文化讲解、基于位置和视觉识别的文化知识提供
- **文件**：CulturalGuide.cpp, CulturalGuide.h

### 3.5 chat 模块
- **功能**：聊天机器人、多模式对话、智能回复生成
- **文件**：Chatbot.cpp, Chatbot.h

### 3.6 sensor 模块
- **功能**：传感器管理、数据采集和处理
- **文件**：SensorManager.cpp, SensorManager.h

### 3.7 utils 模块
- **功能**：通用工具类和函数、跨平台适配

## 4. 实现步骤

### 4.1 创建目录结构
首先需要在项目根目录下创建上述的目录结构。

### 4.2 移动文件
将现有的源文件和头文件移动到对应的目录中：

- AICompanion.cpp → src/core/
- AICompanion.h → include/core/
- LocationTracker.cpp → src/location/
- LocationTracker.h → include/location/
- VisionProcessor.cpp → src/vision/
- VisionProcessor.h → include/vision/
- CulturalGuide.cpp → src/cultural/
- CulturalGuide.h → include/cultural/
- Chatbot.cpp → src/chat/
- Chatbot.h → include/chat/
- SensorManager.cpp → src/sensor/
- SensorManager.h → include/sensor/
- main.cpp → src/

### 4.3 更新文件中的包含路径
需要修改所有文件中的`#include`指令，以适应新的目录结构。例如：

```cpp
// 修改前
#include "AICompanion.h"

// 修改后
#include "core/AICompanion.h"
```

### 4.4 修改CMakeLists.txt文件
需要更新CMakeLists.txt文件以适应新的目录结构：

```cmake
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(AICompanion)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 检查是否为ESP32平台
if(ESP32)
    # ESP32平台特定设置
    message(STATUS "Building for ESP32 platform")
    
    # 添加ESP32相关的库
    # 注意：这里需要根据实际的ESP32开发环境进行调整
    
else()
    # 常规x86平台设置
    message(STATUS "Building for x86 platform")
    
    # 添加编译选项
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wall -Wextra -pedantic)
    elseif(MSVC)
        add_compile_options(/W4)
    endif()
endif()

# 添加头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 创建可执行文件
add_executable(AICompanion ${SOURCES})

# 设置输出目录
set_target_properties(AICompanion PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# 如果需要链接额外的库（如OpenCV、GPS库等），请在这里添加
# target_link_libraries(AICompanion <library_name>)

# 如果是ESP32平台，添加额外的配置
if(ESP32)
    # 这里可以添加ESP32特有的配置
    # 例如：target_link_libraries(AICompanion <esp32_specific_libraries>)
endif()
```

### 4.5 更新构建脚本
需要更新build_windows.bat和build_ubuntu.sh脚本，使其适应新的目录结构。

### 4.6 更新README.md文件
最后，需要更新README.md文件，以反映新的项目结构和构建方法。

## 5. 优势与注意事项

### 优势
- **模块化更清晰**：按照功能模块划分目录，使项目结构更加清晰
- **易于维护和扩展**：新功能可以在相应的模块中添加，不会影响其他模块
- **提升代码可读性**：开发者可以更容易找到和理解特定功能的代码
- **便于团队协作**：不同团队成员可以负责不同的模块，减少代码冲突

### 注意事项
- 确保所有文件的包含路径正确更新
- 测试构建过程，确保项目可以正常编译运行
- 考虑ESP32平台的特殊性，确保跨平台兼容性
- 文档要及时更新，反映项目结构的变化

## 6. 后续改进建议

1. **添加单元测试框架**：为每个模块添加单元测试，提高代码质量和稳定性
2. **引入依赖管理工具**：使用如vcpkg等工具管理第三方依赖库
3. **实施版本控制规范**：制定分支管理、代码提交等规范，提高团队协作效率
4. **添加日志系统**：为各个模块添加统一的日志功能，方便调试和问题排查