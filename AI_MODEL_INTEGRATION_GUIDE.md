# AI模型接入指南

本指南提供了如何在AI智能伴游系统中接入真实AI模型的详细方案。目前项目中的AI功能主要是模拟实现，通过本指南的方法可以接入真实的AI能力。

## 目录
- [整体架构](#整体架构)
- [第三方API接入方案](#第三方api接入方案)
- [本地模型部署方案](#本地模型部署方案)
- [代码修改示例](#代码修改示例)
- [平台兼容性考虑](#平台兼容性考虑)
- [API密钥管理](#api密钥管理)

## 整体架构

AI智能伴游系统采用模块化设计，主要包含以下与AI相关的模块：

1. **Chatbot模块**：负责对话功能，目前使用基于模板的简单实现
2. **VisionProcessor模块**：负责视觉识别，目前是模拟实现
3. **CulturalGuide模块**：负责文化知识讲解，目前使用预定义知识库

AI模型接入可以针对这些模块分别进行，也可以构建统一的AI服务层进行管理。

## 第三方API接入方案

### 聊天模型API接入

可以接入的第三方聊天API包括：
- OpenAI GPT系列 API
- 百度文心一言 API
- 阿里云通义千问 API
- 腾讯混元大模型 API
- Google Gemini API

#### 接入步骤：

1. 在Chatbot类中添加HTTP请求库依赖，如curl或Boost.Asio
2. 创建API调用封装函数
3. 修改generateResponse方法调用API而非使用模板

### 视觉识别API接入

可以接入的第三方视觉API包括：
- 百度AI开放平台 图像识别
- Google Cloud Vision API
- 阿里云视觉智能开放平台
- 腾讯云AI视觉

#### 接入步骤：

1. 在VisionProcessor类中添加HTTP和图像处理库依赖
2. 实现图像编码和上传功能
3. 修改detectObjects和recognizeCulturalArtifacts方法调用API

## 本地模型部署方案

### 聊天模型本地部署

适合本地部署的轻量级模型：
- LLaMA系列模型（如LLaMA-2-7B-Chat）
- Mistral-7B模型
- 百度ERNIE-Bot Tiny
- 阿里巴巴Qwen-1.8B

#### 接入步骤：

1. 选择合适的推理框架，如 llama.cpp、ONNX Runtime 或 TensorFlow Lite
2. 添加模型加载和推理功能
3. 修改Chatbot类的generateResponse方法调用本地模型

### 视觉模型本地部署

适合本地部署的视觉模型：
- YOLOv5/YOLOv8 (目标检测)
- MobileNet SSD (轻量级目标检测)
- TensorFlow Lite模型（ESP32平台）

#### 接入步骤：

1. 准备适合目标平台的模型文件
2. 在VisionProcessor类中集成推理框架
3. 实现图像预处理和后处理功能
4. 修改detectObjects和recognizeCulturalArtifacts方法

## 代码修改示例

### Chatbot类接入OpenAI API示例

```cpp
// Chatbot.h中添加新方法声明
bool setupOpenAIAPI(const std::string& apiKey, const std::string& model = "gpt-3.5-turbo");
std::string callOpenAIAPI(const std::string& prompt);

// Chatbot.cpp中实现API调用
#include <curl/curl.h>
#include <nlohmann/json.hpp> // 使用nlohmann/json库处理JSON

using json = nlohmann::json;

std::string apiKey;
std::string openAIModel;

bool Chatbot::setupOpenAIAPI(const std::string& key, const std::string& model) {
    apiKey = key;
    openAIModel = model;
    std::cout << "OpenAI API已配置" << std::endl;
    return true;
}

std::string Chatbot::callOpenAIAPI(const std::string& prompt) {
    // 构建请求JSON
    json requestBody;
    requestBody["model"] = openAIModel;
    requestBody["messages"] = json::array();
    
    // 添加历史对话
    for (const auto& entry : conversationHistory) {
        requestBody["messages"].push_back({{"role", "user"}, {"content", entry.userQuery}});
        requestBody["messages"].push_back({{"role", "assistant"}, {"content", entry.botResponse}});
    }
    
    // 添加当前查询
    requestBody["messages"].push_back({{"role", "user"}, {"content", prompt}});
    
    // 设置CURL请求
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "CURL初始化失败" << std::endl;
        return "抱歉，我暂时无法回答这个问题。";
    }
    
    // 设置请求头和URL
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string authHeader = "Authorization: Bearer " + apiKey;
    headers = curl_slist_append(headers, authHeader.c_str());
    
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    // 设置POST数据
    std::string jsonStr = requestBody.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
    
    // 用于存储响应的缓冲区
    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void *ptr, size_t size, size_t nmemb, std::string *data) {
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
    
    // 执行请求
    CURLcode res = curl_easy_perform(curl);
    
    // 清理
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    
    // 解析响应
    if (res != CURLE_OK) {
        std::cerr << "API请求失败: " << curl_easy_strerror(res) << std::endl;
        return "抱歉，我暂时无法回答这个问题。";
    }
    
    try {
        json response = json::parse(responseString);
        if (response.contains("choices") && !response["choices"].empty()) {
            return response["choices"][0]["message"]["content"].get<std::string>();
        }
    } catch (const std::exception& e) {
        std::cerr << "响应解析失败: " << e.what() << std::endl;
    }
    
    return "抱歉，我暂时无法回答这个问题。";
}

// 修改generateResponse方法使用API
std::string Chatbot::generateResponse(const std::string& userQuery) {
    std::string response;
    
    // 如果配置了OpenAI API，则使用API生成回复
    if (!apiKey.empty()) {
        std::cout << "使用OpenAI API生成回复..." << std::endl;
        response = callOpenAIAPI(userQuery);
    } else {
        // 原有的模板回复逻辑
        std::string analysisResult = analyzeUserInput(userQuery);
        // ... 原有的回复生成代码 ...
    }
    
    // 保存对话历史
    ConversationEntry entry;
    entry.userQuery = userQuery;
    entry.botResponse = response;
    entry.timestamp = getCurrentTimestamp();
    conversationHistory.push_back(entry);
    
    return response;
}
```

### VisionProcessor类接入本地YOLO模型示例

```cpp
// VisionProcessor.h中添加新方法声明
bool loadYOLOModel(const std::string& modelPath);

// VisionProcessor.cpp中实现YOLO模型加载和推理
#ifdef ESP32
#include <tensorflow/lite/micro/kernels/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/system_setup.h>
#include <tensorflow/lite/schema/schema_generated.h>
#else
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <string>
#endif

// YOLO模型相关变量
#ifdef ESP32
namespace { 
    tflite::ErrorReporter* error_reporter = nullptr;
    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;
    
    // 模型内存分配
    constexpr int kTensorArenaSize = 8 * 1024 * 1024; // 8MB
    uint8_t tensor_arena[kTensorArenaSize];
}  // namespace
#else
cv::dnn::Net yoloNet;
std::vector<std::string> classNames;
#endif

bool VisionProcessor::loadAIModel() {
    std::cout << "加载AI视觉模型..." << std::endl;
    
#ifdef ESP32
    // ESP32-S3上加载TensorFlow Lite模型
    tflite::InitializeTarget();
    
    // 设置错误报告器
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;
    
    // 加载模型（假设模型已编译到二进制中）
    extern const unsigned char yolo_model_tflite[];
    extern const int yolo_model_tflite_len;
    
    model = tflite::GetModel(yolo_model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter, 
            "模型版本不匹配: 期望 %d, 得到 %d\n",
            TFLITE_SCHEMA_VERSION, model->version());
        return false;
    }
    
    // 设置操作解析器
    static tflite::MicroMutableOpResolver<5> micro_op_resolver;
    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddMaxPool2D();
    micro_op_resolver.AddFullyConnected();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddSoftmax();
    
    // 构建解释器
    static tflite::MicroInterpreter static_interpreter(
        model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;
    
    // 分配张量
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "分配张量失败");
        return false;
    }
    
    // 获取输入张量
    input = interpreter->input(0);
    
    std::cout << "在ESP32-S3上成功加载TensorFlow Lite模型" << std::endl;
#else
    // x86环境上加载YOLO模型（使用OpenCV DNN模块）
    std::string modelPath = "models/yolov5s.onnx";
    std::string classesPath = "models/coco.names";
    
    // 加载模型
    yoloNet = cv::dnn::readNetFromONNX(modelPath);
    if (yoloNet.empty()) {
        std::cerr << "无法加载YOLO模型: " << modelPath << std::endl;
        return false;
    }
    
    // 设置计算后端（可选）
    yoloNet.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    yoloNet.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    
    // 加载类别名称
    std::ifstream classesFile(classesPath);
    if (!classesFile.is_open()) {
        std::cerr << "无法加载类别文件: " << classesPath << std::endl;
        return false;
    }
    
    std::string className;
    while (std::getline(classesFile, className)) {
        classNames.push_back(className);
    }
    
    std::cout << "在x86环境上成功加载YOLO模型" << std::endl;
#endif
    
    return true;
}

// 修改detectObjects方法使用真实模型
void VisionProcessor::detectObjects(void* imageData) {
    // 清空之前的检测结果
    detectedObjects.clear();
    
#ifdef ESP32
    // ESP32上的模型推理代码
    // ...
    // 在实际应用中，这里应该使用TensorFlow Lite进行推理
#else
    // x86环境上的YOLO模型推理
    if (yoloNet.empty() || classNames.empty()) {
        std::cerr << "YOLO模型未正确加载" << std::endl;
        return;
    }
    
    // 假设imageData是cv::Mat指针
    cv::Mat* frame = static_cast<cv::Mat*>(imageData);
    if (!frame || frame->empty()) {
        std::cerr << "无效的图像数据" << std::endl;
        return;
    }
    
    // 图像预处理
    cv::Mat blob = cv::dnn::blobFromImage(*frame, 1/255.0, cv::Size(640, 640), cv::Scalar(0, 0, 0), true, false);
    yoloNet.setInput(blob);
    
    // 执行推理
    std::vector<cv::Mat> outputs;
    yoloNet.forward(outputs, yoloNet.getUnconnectedOutLayersNames());
    
    // 后处理结果
    float confThreshold = 0.5; // 置信度阈值
    float nmsThreshold = 0.4;  // 非最大抑制阈值
    
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    
    // 解析输出
    for (size_t i = 0; i < outputs.size(); ++i) {
        float* data = (float*)outputs[i].data;
        for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
            cv::Mat scores = outputs[i].row(j).colRange(5, outputs[i].cols);
            cv::Point classIdPoint;
            double confidence;
            cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            
            if (confidence > confThreshold) {
                int centerX = (int)(data[0] * frame->cols);
                int centerY = (int)(data[1] * frame->rows);
                int width = (int)(data[2] * frame->cols);
                int height = (int)(data[3] * frame->rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                
                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }
    
    // 应用非最大抑制
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    
    // 收集检测结果
    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i];
        if (idx >= 0 && idx < classIds.size() && classIds[idx] < classNames.size()) {
            detectedObjects.push_back(classNames[classIds[idx]]);
        }
    }
#endif
    
    // 如果检测到了对象，打印出来
    if (!detectedObjects.empty()) {
        std::cout << "检测到以下对象: " << std::endl;
        for (const auto& object : detectedObjects) {
            std::cout << "  - " << object << std::endl;
        }
    }
}
```

## 平台兼容性考虑

项目支持ESP32-S3和x86两个平台，在接入AI模型时需要考虑平台差异：

1. **ESP32-S3平台**：
   - 资源受限，适合使用轻量级模型如TensorFlow Lite模型
   - 模型大小应控制在几MB以内
   - 计算能力有限，复杂模型可能导致性能问题

2. **x86平台**：
   - 资源丰富，可以运行更复杂的模型
   - 可以使用完整版本的深度学习框架
   - 可以考虑GPU加速

代码中已使用条件编译（`#ifdef ESP32`）区分不同平台，在实现AI模型接入时应继续使用这种模式。

## API密钥管理

接入第三方API时，密钥管理非常重要：

1. **不要硬编码密钥**：在代码中直接写入API密钥是不安全的
2. **使用环境变量或配置文件**：将密钥存储在环境变量或配置文件中
3. **配置文件加密**：如果使用配置文件存储密钥，考虑加密敏感信息
4. **权限控制**：限制密钥的访问权限

### 安全的API密钥使用示例

```cpp
// 从环境变量读取API密钥
std::string getAPIKeyFromEnvironment(const std::string& envVarName) {
    const char* key = std::getenv(envVarName.c_str());
    if (key) {
        return std::string(key);
    }
    
    // 如果环境变量不存在，尝试从配置文件读取
    std::ifstream configFile("config.json");
    if (configFile.is_open()) {
        try {
            json config = json::parse(configFile);
            if (config.contains(envVarName)) {
                return config[envVarName].get<std::string>();
            }
        } catch (const std::exception& e) {
            std::cerr << "配置文件解析错误: " << e.what() << std::endl;
        }
        configFile.close();
    }
    
    return "";
}

// 在AICompanion::initialize()中调用
void AICompanion::initialize() {
    // ... 其他初始化代码 ...
    
    // 配置Chatbot的OpenAI API
    std::string openAIKey = getAPIKeyFromEnvironment("OPENAI_API_KEY");
    if (!openAIKey.empty()) {
        chatbot->setupOpenAIAPI(openAIKey);
    }
    
    // ... 其他初始化代码 ...
}
```

## 依赖库安装指南

### 第三方API接入依赖

#### CURL库（HTTP请求）
```bash
# Ubuntu
sudo apt-get install libcurl4-openssl-dev

# Windows (使用vcpkg)
vcpkg install curl
```

#### JSON库（处理API响应）
```bash
# 使用nlohmann/json库
# Ubuntu
sudo apt-get install nlohmann-json3-dev

# Windows (使用vcpkg)
vcpkg install nlohmann-json
```

### 本地模型部署依赖

#### OpenCV（图像处理和YOLO推理）
```bash
# Ubuntu
sudo apt-get install libopencv-dev

# Windows (使用vcpkg)
vcpkg install opencv
```

#### TensorFlow Lite（ESP32平台）
需要在ESP32 Arduino或ESP-IDF环境中配置TensorFlow Lite库。

## 总结

本指南提供了在AI智能伴游系统中接入真实AI模型的完整方案，包括第三方API接入和本地模型部署两种方式。通过这些方法，可以显著提升系统的AI能力，实现更智能的对话和视觉识别功能。

在实际实现时，请根据项目需求和目标平台选择合适的接入方式，并确保遵循安全最佳实践，特别是在处理API密钥等敏感信息时。