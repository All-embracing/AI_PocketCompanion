#ifndef AI_COMPANION_H
#define AI_COMPANION_H

#include <string>
#include <vector>
#include "location/LocationTracker.h"
#include "vision/VisionProcessor.h"
#include "cultural/CulturalGuide.h"
#include "chat/Chatbot.h"
#include "sensor/SensorManager.h"

class AICompanion {
public:
    // 构造函数和析构函数
    AICompanion();
    ~AICompanion();
    
    // 系统初始化和关闭
    bool initialize();
    void shutdown();
    
    // 主循环更新
    void update();
    
    // 核心功能接口
    void getCurrentLocation();
    void startDetection();
    void stopDetection();
    void processUserQuery(const std::string& query);
    
    // 聊天模式控制
    void setChatMode(ChatMode mode);
    
    // AI模型配置
    bool setupZhipuAIGLMAPI(const std::string& apiKey, const std::string& model = "glm-realtime");
    
    // 地图API配置
    bool setupAmapAPI(const std::string& apiKey);
    
    // 辅助功能
    void showHelp();
    void showStatus();
    
private:
    // 子系统组件
    LocationTracker* locationTracker;
    VisionProcessor* visionProcessor;
    CulturalGuide* culturalGuide;
    Chatbot* chatbot;
    SensorManager* sensorManager;
    
    // 系统状态
    bool isInitialized;
    bool isDetecting;
    
    // 设备兼容性检测
    bool detectDeviceType();
    bool setupHardware();
};

#endif // AI_COMPANION_H