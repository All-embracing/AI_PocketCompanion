#ifndef VISION_PROCESSOR_H
#define VISION_PROCESSOR_H

#include <string>
#include <vector>

#ifdef ESP32
#include <tensorflow/lite/micro/kernels/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/system_setup.h>
#include <tensorflow/lite/schema/schema_generated.h>
#else
#include <opencv2/opencv.hpp>
#include <fstream>
#endif

class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // 初始化视觉处理系统
    bool initialize();
    
    // 更新视觉处理
    void update();
    
    // 开始视觉检测
    void start();
    
    // 停止视觉检测
    void stop();
    
    // 获取检测到的对象列表
    std::vector<std::string> getDetectedObjects();
    
    // 检查摄像头是否可用
    bool isCameraAvailable();
    
    // 设置检测灵敏度
    void setSensitivity(float sensitivity);
    
    // 保存当前图像
    bool saveCurrentFrame(const std::string& filename);
    
    // 加载YOLO模型（新增方法）
    bool loadYOLOModel(const std::string& modelPath);
    
    // 重置视觉处理系统
    void reset();
    
private:
    // 系统状态
    bool isRunning;
    bool cameraAvailable;
    float detectionSensitivity;
    
    // 检测到的对象列表
    std::vector<std::string> detectedObjects;
    
    // YOLO模型相关变量（ESP32平台）
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
    // YOLO模型相关变量（x86平台）
    cv::dnn::Net yoloNet;
    std::vector<std::string> classNames;
#endif
    
    // 初始化摄像头
    bool initializeCamera();
    
    // 加载AI模型
    bool loadAIModel();
    
    // 处理图像
    void processImage(void* imageData);
    
    // 执行目标检测
    void detectObjects(void* imageData);
    
    // 识别文化文物
    void recognizeCulturalArtifacts(void* imageData);
};

#endif // VISION_PROCESSOR_H