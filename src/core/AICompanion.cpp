#include "core/AICompanion.h"
#include <iostream>

AICompanion::AICompanion() {
    locationTracker = nullptr;
    visionProcessor = nullptr;
    culturalGuide = nullptr;
    chatbot = nullptr;
    sensorManager = nullptr;
    isInitialized = false;
    isDetecting = false;
}

AICompanion::~AICompanion() {
    shutdown();
}

bool AICompanion::initialize() {
    std::cout << "正在初始化AI智能伴游系统..." << std::endl;
    
    // 检测设备类型
    if (!detectDeviceType()) {
        std::cerr << "无法识别设备类型！" << std::endl;
        return false;
    }
    
    // 设置硬件
    if (!setupHardware()) {
        std::cerr << "硬件设置失败！" << std::endl;
        return false;
    }
    
    // 初始化子系统
    locationTracker = new LocationTracker();
    if (!locationTracker->initialize()) {
        std::cerr << "位置追踪器初始化失败！" << std::endl;
        return false;
    }
    
    visionProcessor = new VisionProcessor();
    if (!visionProcessor->initialize()) {
        std::cerr << "视觉处理器初始化失败！" << std::endl;
        return false;
    }
    
    culturalGuide = new CulturalGuide();
    if (!culturalGuide->initialize()) {
        std::cerr << "文化讲解系统初始化失败！" << std::endl;
        return false;
    }
    
    chatbot = new Chatbot();
    if (!chatbot->initialize()) {
        std::cerr << "聊天机器人初始化失败！" << std::endl;
        return false;
    }
    
    sensorManager = new SensorManager();
    if (!sensorManager->initialize()) {
        std::cerr << "传感器管理器初始化失败！" << std::endl;
        return false;
    }
    
    isInitialized = true;
    std::cout << "系统初始化完成！" << std::endl;
    return true;
}

void AICompanion::shutdown() {
    if (isInitialized) {
        std::cout << "正在关闭AI智能伴游系统..." << std::endl;
        
        if (isDetecting) {
            stopDetection();
        }
        
        delete locationTracker;
        delete visionProcessor;
        delete culturalGuide;
        delete chatbot;
        delete sensorManager;
        
        locationTracker = nullptr;
        visionProcessor = nullptr;
        culturalGuide = nullptr;
        chatbot = nullptr;
        sensorManager = nullptr;
        
        isInitialized = false;
        std::cout << "系统关闭完成！" << std::endl;
    }
}

void AICompanion::update() {
    if (!isInitialized) return;
    
    // 更新传感器数据
    sensorManager->update();
    
    // 更新位置信息
    locationTracker->update();
    
    // 如果正在检测，更新视觉处理
    if (isDetecting) {
        visionProcessor->update();
        
        // 获取识别结果
        std::vector<std::string> detectedObjects = visionProcessor->getDetectedObjects();
        
        // 对于检测到的对象，提供文化讲解
        for (const auto& object : detectedObjects) {
            std::string explanation = culturalGuide->getExplanation(object);
            if (!explanation.empty()) {
                std::cout << "文化讲解: " << explanation << std::endl;
            }
        }
    }
}

void AICompanion::getCurrentLocation() {
    if (!isInitialized) return;
    
    LocationInfo location = locationTracker->getCurrentLocation();
    std::cout << "当前位置: " << location.latitude << ", " << location.longitude << std::endl;
    std::cout << "精度: " << location.accuracy << " 米" << std::endl;
    std::cout << "地址: " << location.address << std::endl;
}

void AICompanion::startDetection() {
    if (!isInitialized) return;
    
    visionProcessor->start();
    isDetecting = true;
    std::cout << "开始视觉检测和识别..." << std::endl;
}

void AICompanion::stopDetection() {
    if (!isInitialized) return;
    
    visionProcessor->stop();
    isDetecting = false;
    std::cout << "停止视觉检测和识别..." << std::endl;
}

void AICompanion::processUserQuery(const std::string& query) {
    if (!isInitialized) return;
    
    std::string response = chatbot->generateResponse(query);
    std::cout << "AI伴游: " << response << std::endl;
}

void AICompanion::setChatMode(ChatMode mode) {
    if (!isInitialized || chatbot == nullptr) return;
    
    chatbot->setChatMode(mode);
}

bool AICompanion::setupZhipuAIGLMAPI(const std::string& apiKey, const std::string& model) {
    if (!isInitialized || chatbot == nullptr) return false;
    
    return chatbot->setupZhipuAIGLMAPI(apiKey, model);
}

bool AICompanion::setupAmapAPI(const std::string& apiKey) {
    if (!isInitialized || locationTracker == nullptr) return false;
    
    return locationTracker->setupAmapAPI(apiKey);
}

void AICompanion::showHelp() {
    std::cout << "可用命令:\n";
    std::cout << "  help        - 显示帮助信息\n";
    std::cout << "  status      - 显示系统状态\n";
    std::cout << "  location    - 获取当前位置\n";
    std::cout << "  detect      - 开始视觉检测\n";
    std::cout << "  stop        - 停止视觉检测\n";
    std::cout << "  chatmode 模式 - 切换聊天模式（模式: normal/普通, cultural/文化, joke/笑话/解闷, story/故事, guide/导游/伴游）\n";
    std::cout << "  setapikey 密钥 - 设置智谱AI API密钥\n";
    std::cout << "  setamapkey 密钥 - 设置高德地图API密钥\n";
    std::cout << "  exit/quit   - 退出系统\n";
    std::cout << "  其他输入    - 与AI伴游对话\n";
}

void AICompanion::showStatus() {
    if (!isInitialized) {
        std::cout << "系统未初始化！" << std::endl;
        return;
    }
    
    std::cout << "系统状态:\n";
    std::cout << "  初始化状态: 已初始化\n";
    std::cout << "  检测状态: " << (isDetecting ? "正在检测" : "未检测") << std::endl;
    
    // 显示GPS状态
    bool gpsAvailable = locationTracker->isGPSAvailable();
    std::cout << "  GPS状态: " << (gpsAvailable ? "可用" : "不可用") << std::endl;
    
    // 显示摄像头状态
    bool cameraAvailable = visionProcessor->isCameraAvailable();
    std::cout << "  摄像头状态: " << (cameraAvailable ? "可用" : "不可用") << std::endl;
}

bool AICompanion::detectDeviceType() {
    // 检测当前运行设备类型 (ESP32-S3 或 x86)
#ifdef ESP32
    std::cout << "检测到ESP32-S3设备" << std::endl;
    return true;
#else
    std::cout << "检测到x86设备" << std::endl;
    return true;
#endif
}

bool AICompanion::setupHardware() {
    // 设置硬件配置
#ifdef ESP32
    std::cout << "配置ESP32-S3硬件..." << std::endl;
    // ESP32-S3特定的硬件配置
#else
    std::cout << "配置x86硬件..." << std::endl;
    // x86特定的硬件配置
#endif
    return true;
}