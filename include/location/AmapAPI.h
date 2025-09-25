#ifndef AMAP_API_H
#define AMAP_API_H

#include <string>
#include <memory>
#include <map>

// 高德地图API类，用于实现地理编码、反向地理编码等功能
class AmapAPI {
public:
    AmapAPI();
    ~AmapAPI();
    
    // 单例模式获取实例
    static AmapAPI& getInstance();
    
    // 配置API密钥
    void setApiKey(const std::string& apiKey);
    
    // 检查API密钥是否已设置
    bool hasApiKey() const;
    
    // 反向地理编码：经纬度 -> 地址
    std::string reverseGeocode(double lat, double lon);
    
    // 获取附近的兴趣点
    std::string getNearbyPOI(double lat, double lon, double radius = 1000.0, const std::string& keywords = "");
    
private:
    // API密钥
    std::string apiKey;
    
    // 发送HTTP请求的内部方法
    std::string sendHttpRequest(const std::string& url);
    
    // 缓存结果，提高性能
    void cacheResult(const std::string& key, const std::string& result);
    bool getCachedResult(const std::string& key, std::string& result);
    
    // 缓存数据结构
    struct CacheEntry {
        std::string result;
        long timestamp;
    };
    
    std::map<std::string, CacheEntry> cache;
    
    // 获取当前时间戳（毫秒）
    long getCurrentTimestamp() const;
};

#endif // AMAP_API_H