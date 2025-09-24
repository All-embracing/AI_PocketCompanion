#include "sensor/SensorManager.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <chrono>

SensorManager::SensorManager() {
    // 初始化随机数生成器
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

SensorManager::~SensorManager() {
    // 清理资源
    sensors.clear();
    sensorDataCache.clear();
}

bool SensorManager::initialize() {
    std::cout << "初始化传感器管理器..." << std::endl;
    
    // 初始化所有传感器
    if (!initializeSensors()) {
        std::cerr << "传感器初始化失败！" << std::endl;
        return false;
    }
    
    return true;
}

void SensorManager::update() {
    // 更新所有激活的传感器数据
    for (auto& sensor : sensors) {
        if (sensor.isActive && sensor.isAvailable) {
            // 读取传感器数据
            SensorData data = readSensorData(sensor.type);
            
            // 处理传感器数据
            processSensorData(data);
            
            // 添加到缓存
            sensorDataCache.push_back(data);
            
            // 限制缓存大小
            if (sensorDataCache.size() > 1000) {
                sensorDataCache.erase(sensorDataCache.begin());
            }
        }
    }
}

std::vector<SensorData> SensorManager::getSensorData(SensorType type) {
    std::vector<SensorData> result;
    
    // 从缓存中获取指定类型的传感器数据
    for (const auto& data : sensorDataCache) {
        if (data.type == type) {
            result.push_back(data);
        }
    }
    
    return result;
}

std::vector<SensorData> SensorManager::getAllSensorData() {
    return sensorDataCache;
}

bool SensorManager::isSensorAvailable(SensorType type) {
    // 检查传感器是否可用
    for (const auto& sensor : sensors) {
        if (sensor.type == type) {
            return sensor.isAvailable;
        }
    }
    
    return false;
}

bool SensorManager::activateSensor(SensorType type) {
    // 激活传感器
    for (auto& sensor : sensors) {
        if (sensor.type == type) {
            if (!sensor.isAvailable) {
                std::cerr << "传感器不可用，无法激活！" << std::endl;
                return false;
            }
            
            sensor.isActive = true;
            std::cout << "传感器已激活: " << static_cast<int>(type) << std::endl;
            return true;
        }
    }
    
    std::cerr << "未找到指定的传感器！" << std::endl;
    return false;
}

bool SensorManager::deactivateSensor(SensorType type) {
    // 停用传感器
    for (auto& sensor : sensors) {
        if (sensor.type == type) {
            sensor.isActive = false;
            std::cout << "传感器已停用: " << static_cast<int>(type) << std::endl;
            return true;
        }
    }
    
    std::cerr << "未找到指定的传感器！" << std::endl;
    return false;
}

bool SensorManager::setSensorSamplingRate(SensorType type, float rate) {
    // 设置传感器采样率
    for (auto& sensor : sensors) {
        if (sensor.type == type) {
            if (rate > 0) {
                sensor.samplingRate = rate;
                std::cout << "传感器采样率已设置为: " << rate << " Hz" << std::endl;
                return true;
            } else {
                std::cerr << "采样率必须大于0！" << std::endl;
                return false;
            }
        }
    }
    
    std::cerr << "未找到指定的传感器！" << std::endl;
    return false;
}

SensorStatus SensorManager::getSensorStatus(SensorType type) {
    // 获取传感器状态
    for (const auto& sensor : sensors) {
        if (sensor.type == type) {
            return sensor;
        }
    }
    
    // 如果未找到，返回默认状态
    SensorStatus defaultStatus;
    defaultStatus.type = type;
    defaultStatus.isAvailable = false;
    defaultStatus.isActive = false;
    defaultStatus.model = "未知";
    defaultStatus.samplingRate = 0.0f;
    defaultStatus.errorMessage = "未找到传感器";
    
    return defaultStatus;
}

std::vector<SensorStatus> SensorManager::getAllSensorStatus() {
    return sensors;
}

bool SensorManager::initializeSensors() {
    std::cout << "初始化所有传感器..." << std::endl;
    
    // 清空现有传感器列表
    sensors.clear();
    
    // 定义要初始化的传感器类型
    const SensorType sensorTypes[] = {
        SensorType::GPS,
        SensorType::IMU,
        SensorType::CAMERA,
        SensorType::MICROPHONE,
        SensorType::SPEAKER,
        SensorType::TEMPERATURE,
        SensorType::LIGHT
    };
    
    // 初始化每个传感器
    for (const auto& type : sensorTypes) {
        SensorStatus status;
        status.type = type;
        status.isActive = false;
        status.samplingRate = 10.0f; // 默认采样率10Hz
        
#ifdef ESP32
        // ESP32-S3上的传感器初始化
        switch (type) {
            case SensorType::GPS:
                status.isAvailable = true;
                status.model = "ESP32 GPS Module";
                break;
            case SensorType::IMU:
                status.isAvailable = true;
                status.model = "MPU6050";
                break;
            case SensorType::CAMERA:
                status.isAvailable = true;
                status.model = "OV2640 Camera";
                break;
            case SensorType::MICROPHONE:
                status.isAvailable = true;
                status.model = "INMP441";
                break;
            case SensorType::SPEAKER:
                status.isAvailable = true;
                status.model = "Generic Speaker";
                break;
            case SensorType::TEMPERATURE:
                status.isAvailable = true;
                status.model = "DHT22";
                break;
            case SensorType::LIGHT:
                status.isAvailable = true;
                status.model = "BH1750";
                break;
            default:
                status.isAvailable = false;
                status.errorMessage = "不支持的传感器类型";
                break;
        }
#else
        // x86环境中的传感器模拟
        switch (type) {
            case SensorType::GPS:
                status.isAvailable = true;
                status.model = "模拟GPS模块";
                break;
            case SensorType::IMU:
                status.isAvailable = true;
                status.model = "模拟IMU模块";
                break;
            case SensorType::CAMERA:
                status.isAvailable = true;
                status.model = "模拟摄像头";
                break;
            case SensorType::MICROPHONE:
                status.isAvailable = true;
                status.model = "模拟麦克风";
                break;
            case SensorType::SPEAKER:
                status.isAvailable = true;
                status.model = "模拟扬声器";
                break;
            case SensorType::TEMPERATURE:
                status.isAvailable = true;
                status.model = "模拟温度传感器";
                break;
            case SensorType::LIGHT:
                status.isAvailable = true;
                status.model = "模拟光线传感器";
                break;
            default:
                status.isAvailable = false;
                status.errorMessage = "不支持的传感器类型";
                break;
        }
#endif
        
        // 添加到传感器列表
        sensors.push_back(status);
        
        // 校准传感器（如果可用）
        if (status.isAvailable) {
            calibrateSensor(type);
        }
        
        // 输出传感器状态信息
        std::cout << "传感器类型: " << static_cast<int>(type)
                  << ", 可用: " << (status.isAvailable ? "是" : "否")
                  << ", 型号: " << status.model << std::endl;
    }
    
    return true;
}

SensorData SensorManager::readSensorData(SensorType type) {
    SensorData data;
    data.type = type;
    data.timestamp = getCurrentTimestamp();
    data.isValid = true;
    
    // 根据传感器类型生成模拟数据
    switch (type) {
        case SensorType::GPS:
            data.dataType = "location";
            // 生成随机的GPS坐标（模拟）
            data.value = 0.0;
            data.unit = "degrees";
            break;
        case SensorType::IMU:
            data.dataType = "orientation";
            // 生成随机的IMU数据（模拟）
            data.value = static_cast<double>(std::rand() % 360);
            data.unit = "degrees";
            break;
        case SensorType::CAMERA:
            data.dataType = "frame_count";
            data.value = static_cast<double>(std::rand() % 1000);
            data.unit = "frames";
            break;
        case SensorType::MICROPHONE:
            data.dataType = "audio_level";
            data.value = static_cast<double>(std::rand() % 100) / 100.0;
            data.unit = "dB";
            break;
        case SensorType::SPEAKER:
            data.dataType = "volume";
            data.value = static_cast<double>(std::rand() % 100);
            data.unit = "%";
            break;
        case SensorType::TEMPERATURE:
            data.dataType = "temperature";
            data.value = 20.0 + static_cast<double>(std::rand() % 20) - 5.0;
            data.unit = "°C";
            break;
        case SensorType::LIGHT:
            data.dataType = "light_level";
            data.value = static_cast<double>(std::rand() % 1000);
            data.unit = "lux";
            break;
        default:
            data.isValid = false;
            data.dataType = "unknown";
            data.value = 0.0;
            data.unit = "unknown";
            break;
    }
    
    return data;
}

void SensorManager::processSensorData(SensorData& data) {
    // 处理传感器数据
    // 在实际应用中，这里应该包含滤波、校准等处理
    
    // 简单示例：限制温度传感器的范围
    if (data.type == SensorType::TEMPERATURE) {
        if (data.value < -40.0) {
            data.value = -40.0;
        } else if (data.value > 85.0) {
            data.value = 85.0;
        }
    }
    
    // 简单示例：限制光线传感器的范围
    if (data.type == SensorType::LIGHT) {
        if (data.value < 0.0) {
            data.value = 0.0;
        } else if (data.value > 10000.0) {
            data.value = 10000.0;
        }
    }
}

std::string SensorManager::getCurrentTimestamp() const {
    // 获取当前时间戳
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::ctime(&now_c);
    
    // 移除换行符
    std::string timestamp = ss.str();
    if (!timestamp.empty() && timestamp[timestamp.length() - 1] == '\n') {
        timestamp.erase(timestamp.length() - 1);
    }
    
    return timestamp;
}

bool SensorManager::calibrateSensor(SensorType type) {
    std::cout << "校准传感器: " << static_cast<int>(type) << std::endl;
    
    // 在实际应用中，这里应该包含传感器的校准逻辑
    // 在这个简单实现中，我们只是返回true
    return true;
}