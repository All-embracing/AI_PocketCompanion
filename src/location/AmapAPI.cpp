#include "location/AmapAPI.h"
#include <iostream>
#include <sstream>
#include <chrono>

// 根据平台选择不同的HTTP客户端库
#ifdef ESP32
#include <WiFiClient.h>
#include <HTTPClient.h>
#else
#include <curl/curl.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// 写入回调函数
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize);
    return totalSize;
}
#endif

AmapAPI::AmapAPI() {
#ifdef ESP32
    // ESP32平台不需要初始化CURL
#else
    // 初始化CURL库（全局只需要初始化一次）
    static bool curlInitialized = false;
    if (!curlInitialized) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curlInitialized = true;
    }
#endif
}

AmapAPI::~AmapAPI() {
    // 清理缓存
    cache.clear();
    
    // 注意：CURL全局资源只需要在程序结束时释放一次，不应该在对象析构时释放
}

AmapAPI& AmapAPI::getInstance() {
    static AmapAPI instance;
    return instance;
}

void AmapAPI::setApiKey(const std::string& key) {
    apiKey = key;
    std::cout << "高德地图API密钥已设置" << std::endl;
}

bool AmapAPI::hasApiKey() const {
    return !apiKey.empty();
}

std::string AmapAPI::reverseGeocode(double lat, double lon) {
    // 如果没有设置API密钥，返回模拟地址
    if (!hasApiKey()) {
        std::cout << "警告：未设置高德地图API密钥，使用模拟地址" << std::endl;
        return "模拟地址：未知位置";
    }
    
    // 创建缓存键
    std::stringstream keyStream;
    keyStream << "rev_" << lat << "_" << lon;
    std::string cacheKey = keyStream.str();
    
    // 检查缓存
    std::string cachedResult;
    if (getCachedResult(cacheKey, cachedResult)) {
        return cachedResult;
    }
    
    // 构建API请求URL
    std::stringstream urlStream;
    urlStream << "https://restapi.amap.com/v3/geocode/regeo?location=" 
              << lon << "," << lat 
              << "&key=" << apiKey 
              << "&radius=1000&extensions=all";
    
    std::string url = urlStream.str();
    std::string response = sendHttpRequest(url);
    
    // 解析JSON响应
    try {
        json j = json::parse(response);
        
        // 检查状态码
        if (j["status"] == "1" && !j["regeocode"].empty()) {
            std::string formattedAddress = j["regeocode"]["formatted_address"];
            
            // 缓存结果
            cacheResult(cacheKey, formattedAddress);
            
            return formattedAddress;
        } else {
            std::cerr << "高德地图API反向地理编码失败: " << response << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "高德地图API响应解析错误: " << e.what() << std::endl;
    }
    
    return "未知地址";
}

std::string AmapAPI::getNearbyPOI(double lat, double lon, double radius, const std::string& keywords) {
    // 如果没有设置API密钥，返回模拟数据
    if (!hasApiKey()) {
        std::cout << "警告：未设置高德地图API密钥，使用模拟POI数据" << std::endl;
        return "[""附近的兴趣点：博物馆、公园、餐厅""]";
    }
    
    // 创建缓存键
    std::stringstream keyStream;
    keyStream << "poi_" << lat << "_" << lon << "_" << radius << "_" << keywords;
    std::string cacheKey = keyStream.str();
    
    // 检查缓存
    std::string cachedResult;
    if (getCachedResult(cacheKey, cachedResult)) {
        return cachedResult;
    }
    
    // 构建API请求URL
    std::stringstream urlStream;
    urlStream << "https://restapi.amap.com/v3/place/around?location=" 
              << lon << "," << lat 
              << "&radius=" << radius 
              << "&key=" << apiKey;
    
    if (!keywords.empty()) {
        urlStream << "&keywords=" << keywords;
    }
    
    std::string url = urlStream.str();
    std::string response = sendHttpRequest(url);
    
    // 缓存结果
    cacheResult(cacheKey, response);
    
    return response;
}

std::string AmapAPI::sendHttpRequest(const std::string& url) {
#ifdef ESP32
    // ESP32平台使用HTTPClient
    WiFiClient client;
    HTTPClient http;
    
    // 连接到服务器
    std::cout << "正在发送HTTP请求到: " << url << std::endl;
    if (!http.begin(client, url.c_str())) {
        std::cerr << "HTTP请求初始化失败" << std::endl;
        return "";
    }
    
    // 设置超时时间
    http.setTimeout(10000); // 10秒
    
    // 发送GET请求
    int httpCode = http.GET();
    std::string responseString;
    
    if (httpCode > 0) {
        // 请求成功
        if (httpCode == HTTP_CODE_OK) {
            responseString = http.getString().c_str();
        } else {
            std::cerr << "HTTP请求失败，错误码: " << httpCode << " URL: " << url << std::endl;
        }
    } else {
        std::cerr << "HTTP请求失败，错误: " << http.errorToString(httpCode).c_str() << " URL: " << url << std::endl;
    }
    
    // 关闭连接
    http.end();
    
    return responseString;
#else
    // x86平台使用CURL
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "CURL初始化失败" << std::endl;
        return "";
    }
    
    std::string responseString;
    
    // 设置CURL选项
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 设置超时时间为10秒
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // 跟随重定向
    
    // 执行请求
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        std::cerr << "HTTP请求失败: " << curl_easy_strerror(res) << " URL: " << url << std::endl;
        responseString = "";
    }
    
    // 清理
    curl_easy_cleanup(curl);
    
    return responseString;
#endif
}

void AmapAPI::cacheResult(const std::string& key, const std::string& result) {
    CacheEntry entry;
    entry.result = result;
    entry.timestamp = getCurrentTimestamp();
    
    // 简单的缓存管理：限制缓存大小为100条
    if (cache.size() >= 100) {
        // 删除最早的缓存条目
        auto oldest = cache.begin();
        cache.erase(oldest);
    }
    
    cache[key] = entry;
}

bool AmapAPI::getCachedResult(const std::string& key, std::string& result) {
    auto it = cache.find(key);
    if (it != cache.end()) {
        // 检查缓存是否过期（10分钟）
        long now = getCurrentTimestamp();
        if (now - it->second.timestamp < 10 * 60 * 1000) {
            result = it->second.result;
            return true;
        } else {
            // 缓存过期，删除
            cache.erase(it);
        }
    }
    
    return false;
}

long AmapAPI::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}