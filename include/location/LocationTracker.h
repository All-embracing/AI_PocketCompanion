#ifndef LOCATION_TRACKER_H
#define LOCATION_TRACKER_H

#include <string>

// 位置信息结构体
typedef struct {
    double latitude;      // 纬度
    double longitude;     // 经度
    float altitude;       // 海拔高度
    float accuracy;       // 定位精度
    std::string address;  // 地址信息
    std::string locationName; // 位置名称
    bool isValid;         // 位置是否有效
} LocationInfo;

class LocationTracker {
public:
    LocationTracker();
    ~LocationTracker();
    
    // 初始化定位系统
    bool initialize();
    
    // 更新位置信息
    void update();
    
    // 获取当前位置
    LocationInfo getCurrentLocation();
    
    // 检查GPS是否可用
    bool isGPSAvailable();
    
    // 获取位置变化
    float getDistanceFromLastLocation();
    
private:
    // 当前位置信息
    LocationInfo currentLocation;
    LocationInfo lastLocation;
    
    // 系统状态
    bool gpsAvailable;
    bool imuAvailable;
    
    // 初始化GPS模块
    bool initializeGPS();
    
    // 初始化IMU模块
    bool initializeIMU();
    
    // 融合GPS和IMU数据
    void fuseSensorData();
    
    // 地址解析（反向地理编码）
    std::string reverseGeocode(double lat, double lon);
};

#endif // LOCATION_TRACKER_H