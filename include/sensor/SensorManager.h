#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <string>
#include <vector>

// 传感器类型枚举
enum class SensorType {
    GPS,        // 全球定位系统
    IMU,        // 惯性测量单元
    CAMERA,     // 摄像头
    MICROPHONE, // 麦克风
    SPEAKER,    // 扬声器
    TEMPERATURE, // 温度传感器
    LIGHT       // 光线传感器
};

// 传感器数据结构体
typedef struct {
    SensorType type;         // 传感器类型
    std::string dataType;    // 数据类型
    double value;            // 数据值
    std::string timestamp;   // 时间戳
    std::string unit;        // 单位
    bool isValid;            // 数据是否有效
} SensorData;

// 传感器状态结构体
typedef struct {
    SensorType type;         // 传感器类型
    bool isAvailable;        // 传感器是否可用
    bool isActive;           // 传感器是否激活
    std::string model;       // 传感器型号
    float samplingRate;      // 采样率（Hz）
    std::string errorMessage;// 错误信息（如果有）
} SensorStatus;

class SensorManager {
public:
    SensorManager();
    ~SensorManager();
    
    // 初始化传感器管理器
    bool initialize();
    
    // 更新传感器数据
    void update();
    
    // 获取传感器数据
    std::vector<SensorData> getSensorData(SensorType type);
    
    // 获取所有传感器数据
    std::vector<SensorData> getAllSensorData();
    
    // 检查传感器是否可用
    bool isSensorAvailable(SensorType type);
    
    // 激活/停用传感器
    bool activateSensor(SensorType type);
    bool deactivateSensor(SensorType type);
    
    // 设置传感器采样率
    bool setSensorSamplingRate(SensorType type, float rate);
    
    // 获取传感器状态
    SensorStatus getSensorStatus(SensorType type);
    
    // 获取所有传感器状态
    std::vector<SensorStatus> getAllSensorStatus();
    
private:
    // 传感器状态列表
    std::vector<SensorStatus> sensors;
    
    // 传感器数据缓存
    std::vector<SensorData> sensorDataCache;
    
    // 初始化传感器
    bool initializeSensors();
    
    // 从传感器读取数据
    SensorData readSensorData(SensorType type);
    
    // 处理传感器数据
    void processSensorData(SensorData& data);
    
    // 获取当前时间戳
    std::string getCurrentTimestamp() const;
    
    // 校准传感器
    bool calibrateSensor(SensorType type);
};

#endif // SENSOR_MANAGER_H