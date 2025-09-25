#ifndef LOCATION_TRACKER_H
#define LOCATION_TRACKER_H

#include <string>
#include <vector>
#include <map>
#include <cmath>

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

// 景区电子围栏结构体
typedef struct {
    std::string scenicSpotName; // 景区名称
    double centerLatitude;      // 中心点纬度
    double centerLongitude;     // 中心点经度
    float radius;               // 围栏半径（米）
} ScenicSpotFence;

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
    
    // 设置高德地图API密钥
    bool setupAmapAPI(const std::string& apiKey);
    
    // 获取当前所在景区
    std::string getCurrentScenicSpot();
    
    // 检查用户是否进入新景区
    bool hasEnteredNewScenicSpot();
    
    // 获取上一次所在景区
    std::string getLastScenicSpot();
    
    // 重置位置追踪系统
    void reset();
    
private:
    // 当前位置信息
    LocationInfo currentLocation;
    LocationInfo lastLocation;
    
    // 系统状态
    bool gpsAvailable;
    bool imuAvailable;
    
    // 电子围栏相关
    std::vector<ScenicSpotFence> scenicSpotFences; // 景区电子围栏列表
    std::string currentScenicSpot;                  // 当前所在景区
    std::string lastScenicSpot;                     // 上一次所在景区
    
    // 初始化景区电子围栏
    void initializeScenicSpotFences();
    
    // 检查点是否在电子围栏内
    bool isPointInFence(double lat, double lon, const ScenicSpotFence& fence);
    
    // 计算两个经纬度之间的距离（米）
    float calculateDistance(double lat1, double lon1, double lat2, double lon2);
    
    // 初始化GPS模块
    bool initializeGPS();
    
    // 初始化IMU模块
    bool initializeIMU();
    
    // 融合GPS和IMU数据
    void fuseSensorData();
    
    // 地址解析（反向地理编码）
    std::string reverseGeocode(double lat, double lon);
    
    // 检查用户是否进入景区
    void checkScenicSpotEntry();
};

#endif // LOCATION_TRACKER_H