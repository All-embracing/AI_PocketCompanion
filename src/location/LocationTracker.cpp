#include "location/LocationTracker.h"
#include <iostream>
#include <cmath>
#include "location/AmapAPI.h"

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
    
    // 初始化电子围栏相关
    currentScenicSpot = "";
    lastScenicSpot = "";
    
    // 初始化景区电子围栏
    initializeScenicSpotFences();
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

// 初始化景区电子围栏
void LocationTracker::initializeScenicSpotFences() {
    // 添加一些示例景区的电子围栏
    ScenicSpotFence fence;
    
    // 故宫博物院
    fence.scenicSpotName = "故宫博物院";
    fence.centerLatitude = 39.9042;
    fence.centerLongitude = 116.4074;
    fence.radius = 500.0; // 500米半径
    scenicSpotFences.push_back(fence);
    
    // 天坛公园
    fence.scenicSpotName = "天坛公园";
    fence.centerLatitude = 39.9139;
    fence.centerLongitude = 116.3912;
    fence.radius = 800.0; // 800米半径
    scenicSpotFences.push_back(fence);
    
    // 兵马俑博物馆
    fence.scenicSpotName = "兵马俑博物馆";
    fence.centerLatitude = 34.2657;
    fence.centerLongitude = 108.9542;
    fence.radius = 1000.0; // 1000米半径
    scenicSpotFences.push_back(fence);
    
    // 杭州西湖
    fence.scenicSpotName = "杭州西湖";
    fence.centerLatitude = 30.2741;
    fence.centerLongitude = 120.1551;
    fence.radius = 2000.0; // 2000米半径
    scenicSpotFences.push_back(fence);
    
    // 深圳世界之窗
    fence.scenicSpotName = "深圳世界之窗";
    fence.centerLatitude = 22.5431;
    fence.centerLongitude = 114.0579;
    fence.radius = 800.0; // 800米半径
    scenicSpotFences.push_back(fence);
}

// 重置位置追踪系统
void LocationTracker::reset() {
    // 重置位置信息
    currentLocation.latitude = 0.0;
    currentLocation.longitude = 0.0;
    currentLocation.altitude = 0.0;
    currentLocation.accuracy = 0.0;
    currentLocation.address = "未知地址";
    currentLocation.locationName = "未知位置";
    currentLocation.isValid = false;
    
    // 复制给上一次位置
    lastLocation = currentLocation;
    
    // 重置电子围栏相关状态
    currentScenicSpot = "";
    lastScenicSpot = "";
    
    std::cout << "位置追踪系统已重置" << std::endl;
}

// 计算两个经纬度之间的距离（米）
float LocationTracker::calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const float R = 6371000.0f; // 地球半径（米）
    
    float radLat1 = static_cast<float>(lat1 * M_PI / 180.0);
    float radLon1 = static_cast<float>(lon1 * M_PI / 180.0);
    float radLat2 = static_cast<float>(lat2 * M_PI / 180.0);
    float radLon2 = static_cast<float>(lon2 * M_PI / 180.0);
    
    float dlat = radLat2 - radLat1;
    float dlon = radLon2 - radLon1;
    
    float a = std::sin(dlat / 2) * std::sin(dlat / 2) +
              std::cos(radLat1) * std::cos(radLat2) *
              std::sin(dlon / 2) * std::sin(dlon / 2);
    
    float c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return R * c;
}

// 检查点是否在电子围栏内
bool LocationTracker::isPointInFence(double lat, double lon, const ScenicSpotFence& fence) {
    float distance = calculateDistance(lat, lon, fence.centerLatitude, fence.centerLongitude);
    return distance <= fence.radius;
}

void LocationTracker::update() {
    // 保存当前景区作为上一次景区
    lastScenicSpot = currentScenicSpot;
    
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
        
        // 检查用户是否进入景区
        checkScenicSpotEntry();
    }
}

// 检查用户是否进入景区
void LocationTracker::checkScenicSpotEntry() {
    if (!currentLocation.isValid) {
        currentScenicSpot = "";
        return;
    }
    
    // 重置当前景区
    currentScenicSpot = "";
    
    // 检查用户是否在任何景区电子围栏内
    for (const auto& fence : scenicSpotFences) {
        if (isPointInFence(currentLocation.latitude, currentLocation.longitude, fence)) {
            currentScenicSpot = fence.scenicSpotName;
            break; // 找到一个匹配的景区就可以了
        }
    }
}

// 获取当前所在景区
std::string LocationTracker::getCurrentScenicSpot() {
    return currentScenicSpot;
}

// 检查用户是否进入新景区
bool LocationTracker::hasEnteredNewScenicSpot() {
    // 如果当前在景区，且上一次不在同一景区，则表示进入了新景区
    return !currentScenicSpot.empty() && (currentScenicSpot != lastScenicSpot);
}

// 获取上一次所在景区
std::string LocationTracker::getLastScenicSpot() {
    return lastScenicSpot;
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

// 设置高德地图API密钥
bool LocationTracker::setupAmapAPI(const std::string& apiKey) {
    if (apiKey.empty()) {
        std::cerr << "高德地图API密钥不能为空" << std::endl;
        return false;
    }
    
    AmapAPI::getInstance().setApiKey(apiKey);
    std::cout << "高德地图API已配置" << std::endl;
    return true;
}

std::string LocationTracker::reverseGeocode(double lat, double lon) {
    // 首先尝试使用高德地图API进行反向地理编码
    std::string address = AmapAPI::getInstance().reverseGeocode(lat, lon);
    
    // 如果API返回了有效的地址，使用它
    if (address != "未知地址" && address != "模拟地址：未知位置") {
        return address;
    }
    
    // 如果API调用失败或没有返回有效地址，使用内置的模拟地址（作为备选方案）
    std::cout << "使用内置模拟地址作为备选方案" << std::endl;
    
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