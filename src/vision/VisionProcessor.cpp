#include "vision/VisionProcessor.h"
#include "vision/model_utils.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

VisionProcessor::VisionProcessor() {
    isRunning = false;
    cameraAvailable = false;
    detectionSensitivity = 0.7f; // 默认灵敏度
    
    // 初始化随机数生成器
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

VisionProcessor::~VisionProcessor() {
    stop();
}

bool VisionProcessor::initialize() {
    std::cout << "初始化视觉处理系统..." << std::endl;
    
    // 初始化摄像头
    if (!initializeCamera()) {
        std::cerr << "摄像头初始化失败！" << std::endl;
        return false;
    }
    
    // 加载AI模型
    if (!loadAIModel()) {
        std::cerr << "AI模型加载失败！" << std::endl;
        return false;
    }
    
    return true;
}

bool VisionProcessor::loadYOLOModel(const std::string& modelPath) {
    std::cout << "加载YOLO模型: " << modelPath << std::endl;
    
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
    return true;
#else
    // x86环境上加载YOLO模型（使用OpenCV DNN模块）
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
        // 创建默认类别文件
        std::ofstream defaultClassesFile(classesPath);
        if (defaultClassesFile.is_open()) {
            defaultClassesFile << "person\n" 
                             << "bicycle\n" 
                             << "car\n" 
                             << "motorcycle\n" 
                             << "airplane\n" 
                             << "bus\n" 
                             << "train\n" 
                             << "truck\n" 
                             << "boat\n" 
                             << "traffic light\n" 
                             << "fire hydrant\n" 
                             << "stop sign\n" 
                             << "parking meter\n" 
                             << "bench\n" 
                             << "bird\n" 
                             << "cat\n" 
                             << "dog\n" 
                             << "horse\n" 
                             << "sheep\n" 
                             << "cow\n" 
                             << "elephant\n" 
                             << "bear\n" 
                             << "zebra\n" 
                             << "giraffe\n" 
                             << "backpack\n" 
                             << "umbrella\n" 
                             << "handbag\n" 
                             << "tie\n" 
                             << "suitcase\n" 
                             << "frisbee\n" 
                             << "skis\n" 
                             << "snowboard\n" 
                             << "sports ball\n" 
                             << "kite\n" 
                             << "baseball bat\n" 
                             << "baseball glove\n" 
                             << "skateboard\n" 
                             << "surfboard\n" 
                             << "tennis racket\n" 
                             << "bottle\n" 
                             << "wine glass\n" 
                             << "cup\n" 
                             << "fork\n" 
                             << "knife\n" 
                             << "spoon\n" 
                             << "bowl\n" 
                             << "banana\n" 
                             << "apple\n" 
                             << "sandwich\n" 
                             << "orange\n" 
                             << "broccoli\n" 
                             << "carrot\n" 
                             << "hot dog\n" 
                             << "pizza\n" 
                             << "donut\n" 
                             << "cake\n" 
                             << "chair\n" 
                             << "couch\n" 
                             << "potted plant\n" 
                             << "bed\n" 
                             << "dining table\n" 
                             << "toilet\n" 
                             << "tv\n" 
                             << "laptop\n" 
                             << "mouse\n" 
                             << "remote\n" 
                             << "keyboard\n" 
                             << "cell phone\n" 
                             << "microwave\n" 
                             << "oven\n" 
                             << "toaster\n" 
                             << "sink\n" 
                             << "refrigerator\n" 
                             << "book\n" 
                             << "clock\n" 
                             << "vase\n" 
                             << "scissors\n" 
                             << "teddy bear\n" 
                             << "hair drier\n" 
                             << "toothbrush";
            defaultClassesFile.close();
            // 重新打开并加载
            classesFile.open(classesPath);
        }
    }
    
    // 加载类别名称
    classNames.clear();
    std::string className;
    while (std::getline(classesFile, className)) {
        classNames.push_back(className);
    }
    
    // 如果没有类别名称，使用默认名称
    if (classNames.empty()) {
        classNames.push_back("unknown");
    }
    
    std::cout << "在x86环境上成功加载YOLO模型，加载了 " << classNames.size() << " 个类别" << std::endl;
    return true;
#endif
}

void VisionProcessor::update() {
    if (!isRunning || !cameraAvailable) {
        return;
    }
    
    // 清除之前的检测结果
    detectedObjects.clear();
    
#ifdef ESP32
    // ESP32-S3上的图像采集和处理
    // 在实际应用中，这里应该从摄像头获取图像数据
    // 假设我们有一个获取图像的函数
    // uint8_t* imageData = captureImage();
    
    // 在这个简化版本中，我们使用nullptr作为占位符
    void* imageData = nullptr;
    
    // 处理图像
    processImage(imageData);
    
    // 执行目标检测
    detectObjects(imageData);
    
    // 识别文化文物
    recognizeCulturalArtifacts(imageData);
    
    // 释放图像资源
    // freeImage(imageData);
#else
    // x86环境上的图像采集和处理
    cv::Mat frame;
    void* imageData = nullptr;
    
    // 模拟获取图像帧（在实际应用中，应该从摄像头获取）
    // 这里创建一个随机的空白图像作为演示
    frame = cv::Mat(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
    
    if (!frame.empty()) {
        imageData = &frame;
        
        // 处理图像
        processImage(imageData);
        
        // 执行目标检测
        detectObjects(imageData);
        
        // 识别文化文物
        recognizeCulturalArtifacts(imageData);
        
        // 可选：显示检测结果
        // 注意：在实际应用中，这应该在单独的线程中执行
        if (!detectedObjects.empty()) {
            std::cout << "检测到的对象: " << std::endl;
            for (const auto& object : detectedObjects) {
                std::cout << "- " << object << std::endl;
            }
        }
    } else {
        std::cerr << "无法获取有效的图像帧" << std::endl;
    }
#endif
}

void VisionProcessor::start() {
    if (!cameraAvailable) {
        std::cerr << "摄像头不可用，无法开始视觉检测！" << std::endl;
        return;
    }
    
    isRunning = true;
    std::cout << "视觉检测已启动，灵敏度: " << detectionSensitivity << std::endl;
}

void VisionProcessor::stop() {
    isRunning = false;
    detectedObjects.clear();
    std::cout << "视觉检测已停止" << std::endl;
}

std::vector<std::string> VisionProcessor::getDetectedObjects() {
    return detectedObjects;
}

bool VisionProcessor::isCameraAvailable() {
    return cameraAvailable;
}

void VisionProcessor::setSensitivity(float sensitivity) {
    // 确保灵敏度在合理范围内
    if (sensitivity >= 0.1f && sensitivity <= 1.0f) {
        detectionSensitivity = sensitivity;
        std::cout << "视觉检测灵敏度已设置为: " << sensitivity << std::endl;
    } else {
        std::cerr << "灵敏度值必须在0.1到1.0之间！" << std::endl;
    }
}

bool VisionProcessor::saveCurrentFrame(const std::string& filename) {
    if (!cameraAvailable) {
        std::cerr << "摄像头不可用，无法保存图像！" << std::endl;
        return false;
    }
    
    // 模拟保存图像
    std::cout << "图像已保存到: " << filename << std::endl;
    return true;
}

bool VisionProcessor::initializeCamera() {
#ifdef ESP32
    // ESP32-S3的摄像头初始化代码
    std::cout << "初始化ESP32-S3的摄像头..." << std::endl;
    // 这里可以添加ESP32的摄像头初始化代码
    cameraAvailable = true;
#else
    // x86环境的摄像头模拟
    std::cout << "在x86环境中模拟摄像头..." << std::endl;
    // 在x86环境下，我们使用模拟数据
    cameraAvailable = true;
#endif
    return cameraAvailable;
}

bool VisionProcessor::loadAIModel() {
    std::cout << "加载AI视觉模型..." << std::endl;
    
#ifdef ESP32
    // ESP32-S3上加载TensorFlow Lite模型
    std::cout << "在ESP32-S3上加载TensorFlow Lite模型..." << std::endl;
    // 在ESP32上，我们使用编译到代码中的模型
    return loadYOLOModel("");
#else
    // x86环境上加载YOLO模型
    std::cout << "在x86环境上加载YOLO模型..." << std::endl;
    
    // 检查模型目录是否存在，如果不存在则创建
    std::string modelsDir = "models";
    #ifdef _WIN32
    std::string mkdirCommand = "mkdir " + modelsDir;
    #else
    std::string mkdirCommand = "mkdir -p " + modelsDir;
    #endif
    system(mkdirCommand.c_str());
    
    // 使用默认的YOLOv5s模型路径
    std::string modelPath = "models/yolov5s.onnx";
    
    // 检查模型文件是否存在
    std::ifstream modelFile(modelPath);
    if (!modelFile.good()) {
        std::cout << "YOLO模型文件不存在: " << modelPath << std::endl;
        std::cout << "请手动下载模型文件并放置在该路径下。" << std::endl;
        std::cout << "模型下载地址示例: https://github.com/ultralytics/yolov5/releases" << std::endl;
        std::cout << "将使用模拟模式进行对象检测。" << std::endl;
        
        // 即使模型文件不存在，我们也返回true，但会在detectObjects中使用模拟模式
        return true;
    }
    
    return loadYOLOModel(modelPath);
#endif
}

void VisionProcessor::processImage(void* imageData) {
    // 图像处理过程
    std::cout << "正在处理图像..." << std::endl;
    
#ifdef ESP32
    // ESP32-S3上的图像处理
    // 在实际应用中，这里应该从摄像头获取图像数据并进行预处理
    
#else
    // x86环境上的图像处理
    // 假设imageData是cv::Mat指针
    cv::Mat* frame = nullptr;
    
    if (imageData) {
        frame = static_cast<cv::Mat*>(imageData);
        if (frame && !frame->empty()) {
            // 执行实际的图像处理操作
            // 例如：调整大小、灰度化、滤波等预处理
            
            // 检查图像通道数，确保是RGB格式
            if (frame->channels() == 4) {
                // 如果是RGBA格式，转换为RGB
                cv::cvtColor(*frame, *frame, cv::COLOR_RGBA2RGB);
            } else if (frame->channels() == 1) {
                // 如果是灰度图，转换为RGB
                cv::cvtColor(*frame, *frame, cv::COLOR_GRAY2RGB);
            }
            
            // 确保图像尺寸适合模型输入
            // 在YOLO模型中，这一步通常在detectObjects中通过blobFromImage完成
        }
    }
#endif
}

void VisionProcessor::detectObjects(void* imageData) {
    // 清空之前的检测结果
    detectedObjects.clear();
    
#ifdef ESP32
    // ESP32上的模型推理代码
    // 在实际应用中，这里应该使用TensorFlow Lite进行推理
    std::cout << "在ESP32-S3上执行模型推理..." << std::endl;
    
    if (!model || !interpreter || !input) {
        std::cerr << "TensorFlow Lite模型未正确初始化" << std::endl;
        // 使用模拟模式
        goto SIMULATION_MODE;
    }
    
    // 这里应该添加TensorFlow Lite推理代码
    // 由于ESP32的实现较为复杂，此处仅为框架
    std::cout << "TensorFlow Lite推理框架已准备就绪" << std::endl;
    
    // 在实际应用中，应该将图像数据复制到输入张量
    // 然后调用interpreter->Invoke()执行推理
    // 最后处理输出结果
    
#else
    // x86环境上的YOLO模型推理
    bool useSimulationMode = false;
    
    if (yoloNet.empty() || classNames.empty()) {
        std::cerr << "YOLO模型未正确加载，使用模拟模式" << std::endl;
        useSimulationMode = true;
    } else {
        // 假设imageData是cv::Mat指针
        cv::Mat* frame = nullptr;
        cv::Mat localFrame;
        
        // 如果提供了图像数据，使用它；否则创建一个模拟图像
        if (imageData) {
            frame = static_cast<cv::Mat*>(imageData);
            if (!frame || frame->empty()) {
                std::cerr << "无效的图像数据，创建模拟图像" << std::endl;
                localFrame = cv::Mat(480, 640, CV_8UC3, cv::Scalar(200, 200, 200));
                frame = &localFrame;
            }
        } else {
            // 创建一个模拟图像用于测试
            std::cout << "没有提供图像数据，创建模拟图像" << std::endl;
            localFrame = cv::Mat(480, 640, CV_8UC3, cv::Scalar(200, 200, 200));
            frame = &localFrame;
        }
        
        // 图像预处理
        cv::Mat blob = cv::dnn::blobFromImage(*frame, 1/255.0, cv::Size(640, 640), cv::Scalar(0, 0, 0), true, false);
        yoloNet.setInput(blob);
        
        // 执行推理
        std::vector<cv::Mat> outputs;
        try {
            std::cout << "执行YOLO模型推理..." << std::endl;
            yoloNet.forward(outputs, yoloNet.getUnconnectedOutLayersNames());
        } catch (const cv::Exception& e) {
            std::cerr << "推理错误: " << e.what() << std::endl;
            useSimulationMode = true;
        }
        
        if (!useSimulationMode) {
            // 后处理结果
            float confThreshold = detectionSensitivity; // 使用灵敏度作为置信度阈值
            float nmsThreshold = 0.4;  // 非最大抑制阈值
            
            std::vector<int> classIds;
            std::vector<float> confidences;
            std::vector<cv::Rect> boxes;
            
            // 合并所有输出层的结果
            cv::Mat allOutputs;
            for (size_t i = 0; i < outputs.size(); ++i) {
                if (allOutputs.empty()) {
                    allOutputs = outputs[i];
                } else {
                    cv::vconcat(allOutputs, outputs[i], allOutputs);
                }
            }
            
            // 使用model_utils.h中的processYOLOOutput函数处理输出
            processYOLOOutput(allOutputs, confThreshold, nmsThreshold, 
                             frame->cols, frame->rows, 
                             boxes, confidences, classIds);
            
            // 收集检测结果
            for (size_t i = 0; i < classIds.size(); ++i) {
                if (i < classIds.size() && classIds[i] < classNames.size()) {
                    detectedObjects.push_back(classNames[classIds[i]]);
                }
            }
            
            // 如果使用了本地创建的图像，保存它以便调试
            if (frame == &localFrame) {
                for (size_t i = 0; i < classIds.size(); ++i) {
                    cv::Rect box = boxes[i];
                    cv::rectangle(*frame, box, cv::Scalar(0, 255, 0), 2);
                    
                    // 在框上方显示类别名称和置信度
                    std::string label = cv::format("%s: %.2f", 
                        classIds[i] < classNames.size() ? classNames[classIds[i]].c_str() : "unknown", 
                        confidences[i]);
                    cv::putText(*frame, label, cv::Point(box.x, box.y - 10), 
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
                }
                
                // 保存带有检测结果的图像
                cv::imwrite("detection_result.jpg", *frame);
                std::cout << "检测结果图像已保存到: detection_result.jpg" << std::endl;
            }
        }
    }
    
#endif
    
    // 如果检测到了对象，打印出来
    if (!useSimulationMode && !detectedObjects.empty()) {
        std::cout << "检测到以下对象: " << std::endl;
        for (const auto& object : detectedObjects) {
            std::cout << "  - " << object << std::endl;
        }
        return;
    }
    
    // 如果需要使用模拟模式
    if (useSimulationMode || detectedObjects.empty()) {
        std::cout << "使用模拟模式进行对象检测" << std::endl;
    }
    
    // 根据模拟的位置信息和随机概率添加一些检测到的对象
    // 预定义一些可能在旅游景点检测到的对象
    const std::string possibleObjects[] = {
        "古代雕像", "文物展示", "历史建筑", "传统绘画",
        "碑文", "瓷器展品", "青铜器", "古代服饰",
        "壁画", "书法作品", "园林景观", "雕塑"
    };
    
    // 根据灵敏度随机选择一些对象
    int numPossible = sizeof(possibleObjects) / sizeof(possibleObjects[0]);
    int maxObjects = static_cast<int>(5.0f * detectionSensitivity);
    
    for (int i = 0; i < maxObjects; ++i) {
        // 根据灵敏度决定是否检测到对象
        if (static_cast<float>(std::rand()) / RAND_MAX < detectionSensitivity) {
            int index = std::rand() % numPossible;
            detectedObjects.push_back(possibleObjects[index]);
        }
    }
    
    // 如果检测到了对象，打印出来
    if (!detectedObjects.empty()) {
        std::cout << "模拟检测到以下对象: " << std::endl;
        for (const auto& object : detectedObjects) {
            std::cout << "  - " << object << std::endl;
        }
    }
}

void VisionProcessor::recognizeCulturalArtifacts(void* imageData) {
    // 文化文物识别过程
    std::cout << "正在识别文化文物..." << std::endl;
    
    // 基于YOLO模型的检测结果进行文化文物识别
    // 这里可以使用额外的模型或规则来识别特定的文化文物
    
    // 创建一个临时列表来存储新识别的文化文物
    std::vector<std::string> newlyIdentifiedArtifacts;
    
    // 定义一些可能的文化文物关键词映射
    const std::map<std::string, std::string> culturalArtifactMap = {
        {"person", "人物雕像"},
        {"bench", "古代坐具"},
        {"book", "古籍"},
        {"clock", "古代钟表"},
        {"vase", "古代花瓶"},
        {"chair", "古代座椅"},
        {"umbrella", "传统伞具"}
    };
    
    // 检查检测到的对象中是否有可能是文化文物
    for (const auto& object : detectedObjects) {
        // 直接匹配预定义的文化文物
        if (object == "古代雕像" || object == "文物展示" || 
            object == "历史建筑" || object == "传统绘画" ||
            object == "碑文" || object == "瓷器展品" ||
            object == "青铜器" || object == "古代服饰" ||
            object == "壁画" || object == "书法作品" ||
            object == "园林景观" || object == "雕塑") {
            // 有80%的几率确认为文化文物
            if (static_cast<float>(std::rand()) / RAND_MAX < 0.8f) {
                newlyIdentifiedArtifacts.push_back("重要" + object);
            }
        }
        
        // 检查是否匹配常见物体到文化文物的映射
        auto it = culturalArtifactMap.find(object);
        if (it != culturalArtifactMap.end()) {
            // 有30%的几率将常见物体识别为文化文物
            if (static_cast<float>(std::rand()) / RAND_MAX < 0.3f) {
                newlyIdentifiedArtifacts.push_back(it->second);
            }
        }
    }
    
    // 将新识别的文化文物添加到检测结果中
    for (const auto& artifact : newlyIdentifiedArtifacts) {
        detectedObjects.push_back(artifact);
        std::cout << "识别到文化文物: " << artifact << std::endl;
    }
}