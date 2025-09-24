#include "location/LocationTracker.h"
#include <iostream>
#include <cmath>

LocationTracker::LocationTracker() {
    gpsAvailable = false;
    imuAvailable = false;
    
    // 初始化位置信息
    currentLocation.latitude = 0.0;
    currentLocation.longitude = 0.0;
    currentLocation.altitude = 0.0;
    currentLocation.accuracy = 0.0;
    currentLocation.address = "未知地址";
    currentLocation.locationName = "未知位置";
    currentLocation.isValid = false;
    
    // 复制给上一次位置
    lastLocation = currentLocation;
}

LocationTracker::~LocationTracker() {
    // 清理资源
}

bool LocationTracker::initialize() {
    std::cout << "初始化位置追踪系统..." << std::endl;
    
    // 初始化GPS
    if (!initializeGPS()) {
        std::cerr << "GPS初始化失败，但将继续使用其他定位方式" << std::endl;
    }
    
    // 初始化IMU
    if (!initializeIMU()) {
        std::cerr << "IMU初始化失败，但将继续使用其他定位方式" << std::endl;
    }
    
    return true;
}

void LocationTracker::update() {
    // 模拟位置更新
    static int count = 0;
    count++;
    
    // 每10次更新模拟一个有效的位置
    if (count % 10 == 0) {
        // 模拟一些示例位置（例如博物馆）
        const double sampleLocations[5][2] = {
            {39.9042, 116.4074}, // 故宫博物院
            {39.9139, 116.3912}, // 天坛公园
            {34.2657, 108.9542}, // 兵马俑博物馆
            {30.2741, 120.1551}, // 杭州西湖
            {22.5431, 114.0579}  // 深圳世界之窗
        };
        
        int index = count / 10 % 5;
        currentLocation.latitude = sampleLocations[index][0];
        currentLocation.longitude = sampleLocations[index][1];
        currentLocation.altitude = 50.0 + (rand() % 100);
        currentLocation.accuracy = 5.0 + (rand() % 20) / 10.0;
        currentLocation.isValid = true;
        
        // 反向地理编码
        currentLocation.address = reverseGeocode(currentLocation.latitude, currentLocation.longitude);
        
        // 更新位置名称
        const std::string locationNames[5] = {
            "故宫博物院",
            "天坛公园",
            "兵马俑博物馆",
            "杭州西湖",
            "深圳世界之窗"
        };
        currentLocation.locationName = locationNames[index];
        
        // 融合传感器数据（如果可用）
        if (gpsAvailable || imuAvailable) {
            fuseSensorData();
        }
        
        // 保存为上一次位置
        lastLocation = currentLocation;
    }
}

LocationInfo LocationTracker::getCurrentLocation() {
    return currentLocation;
}

bool LocationTracker::isGPSAvailable() {
    return gpsAvailable;
}

float LocationTracker::getDistanceFromLastLocation() {
    // 使用Haversine公式计算两个经纬度之间的距离
    if (!currentLocation.isValid || !lastLocation.isValid) {
        return 0.0f;
    }
    
    const float R = 6371000.0f; // 地球半径（米）
    
    float lat1 = currentLocation.latitude * M_PI / 180.0f;
    float lon1 = currentLocation.longitude * M_PI / 180.0f;
    float lat2 = lastLocation.latitude * M_PI / 180.0f;
    float lon2 = lastLocation.longitude * M_PI / 180.0f;
    
    float dlat = lat2 - lat1;
    float dlon = lon2 - lon1;
    
    float a = std::sin(dlat / 2) * std::sin(dlat / 2) +
              std::cos(lat1) * std::cos(lat2) *
              std::sin(dlon / 2) * std::sin(dlon / 2);
    
    float c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return R * c;
}

bool LocationTracker::initializeGPS() {
#ifdef ESP32
    // ESP32-S3的GPS初始化代码
    std::cout << "初始化ESP32-S3的GPS模块..." << std::endl;
    // 这里可以添加ESP32的GPS初始化代码
    gpsAvailable = true;
#else
    // x86环境的GPS模拟
    std::cout << "在x86环境中模拟GPS模块..." << std::endl;
    // 在x86环境下，我们使用模拟数据
    gpsAvailable = true;
#endif
    return gpsAvailable;
}

bool LocationTracker::initializeIMU() {
#ifdef ESP32
    // ESP32-S3的IMU初始化代码
    std::cout << "初始化ESP32-S3的IMU模块..." << std::endl;
    // 这里可以添加ESP32的IMU初始化代码
    imuAvailable = true;
#else
    // x86环境的IMU模拟
    std::cout << "在x86环境中模拟IMU模块..." << std::endl;
    // 在x86环境下，我们使用模拟数据
    imuAvailable = true;
#endif
    return imuAvailable;
}

void LocationTracker::fuseSensorData() {
    // 这里可以添加GPS和IMU数据融合的算法
    // 例如使用卡尔曼滤波器等
    
    // 在这个简单实现中，我们只是模拟数据融合
    if (gpsAvailable && imuAvailable) {
        // 假设我们融合了GPS和IMU数据
        currentLocation.accuracy *= 0.9; // 模拟提高精度
    }
}

std::string LocationTracker::reverseGeocode(double lat, double lon) {
    // 在实际应用中，这里应该调用地理编码API
    // 但在这个简单实现中，我们只是返回一些示例地址
    
    // 基于示例位置返回地址
    if (std::abs(lat - 39.9042) < 0.01 && std::abs(lon - 116.4074) < 0.01) {
        return "北京市东城区景山前街4号";
    } else if (std::abs(lat - 39.9139) < 0.01 && std::abs(lon - 116.3912) < 0.01) {
        return "北京市东城区天坛东路甲1号";
    } else if (std::abs(lat - 34.2657) < 0.01 && std::abs(lon - 108.9542) < 0.01) {
        return "陕西省西安市临潼区秦始皇陵东侧";
    } else if (std::abs(lat - 30.2741) < 0.01 && std::abs(lon - 120.1551) < 0.01) {
        return "浙江省杭州市西湖区龙井路1号";
    } else if (std::abs(lat - 22.5431) < 0.01 && std::abs(lon - 114.0579) < 0.01) {
        return "广东省深圳市南山区深南大道9037号";
    } else {
        return "未知地址";
    }
}