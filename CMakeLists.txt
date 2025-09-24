# CMakeLists.txt for AI智能伴游项目
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
    
    # 查找OpenCV库
    find_package(OpenCV REQUIRED)
    if(OpenCV_FOUND)
        message(STATUS "OpenCV found: ${OpenCV_VERSION}")
        include_directories(${OpenCV_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "OpenCV not found. Please install OpenCV.")
    endif()
    
    # 查找curl库
    find_package(CURL REQUIRED)
    if(CURL_FOUND)
        message(STATUS "CURL found: ${CURL_VERSION_STRING}")
        include_directories(${CURL_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "CURL not found. Please install libcurl4-openssl-dev.")
    endif()
    
    # 查找nlohmann/json库
    find_package(nlohmann_json REQUIRED)
    if(nlohmann_json_FOUND)
        message(STATUS "nlohmann_json found")
        include_directories(${nlohmann_json_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "nlohmann_json not found. Please install nlohmann-json3-dev.")
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

# 链接库
if(NOT ESP32 AND OpenCV_FOUND)
    target_link_libraries(AICompanion 
        ${OpenCV_LIBS}
        ${CURL_LIBRARIES}
        nlohmann_json::nlohmann_json
    )
endif()

# 如果是ESP32平台，添加额外的配置
if(ESP32)
    # 这里可以添加ESP32特有的配置
    # 例如：target_link_libraries(AICompanion <esp32_specific_libraries>)
endif()