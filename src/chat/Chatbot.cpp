#include "chat/Chatbot.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

// 使用nlohmann/json库处理JSON
using json = nlohmann::json;

// 智谱AI GLM-Realtime API相关变量
std::string zhipuAIKey;
std::string zhipuAIModel;

Chatbot::Chatbot() {
    currentMode = ChatMode::NORMAL;
    
    // 初始化随机数生成器
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Chatbot::~Chatbot() {
    // 清理资源
    conversationHistory.clear();
    responseTemplates.clear();
}

bool Chatbot::initialize() {
    std::cout << "初始化聊天机器人系统..." << std::endl;
    
    // 初始化回复模板
    initializeResponseTemplates();
    
    return true;
}

std::string Chatbot::generateResponse(const std::string& userQuery) {
    std::string response;
    
    // 如果配置了智谱AI GLM-Realtime API，则使用API生成回复
    if (!zhipuAIKey.empty()) {
        std::cout << "使用智谱AI GLM-Realtime API生成回复..." << std::endl;
        response = callZhipuAIGLMAPI(userQuery);
    } else {
        // 分析用户输入
        std::string analysisResult = analyzeUserInput(userQuery);
        
        // 根据当前对话模式生成回复
        if (userQuery.find("笑话") != std::string::npos || 
            userQuery.find("幽默") != std::string::npos ||
            userQuery.find("搞笑") != std::string::npos) {
            // 用户请求讲笑话
            response = tellJoke();
        } else if (userQuery.find("故事") != std::string::npos ||
                   userQuery.find("传说") != std::string::npos ||
                   userQuery.find("历史") != std::string::npos) {
            // 用户请求讲故事
            response = tellStory();
        } else {
            // 根据当前模式生成回复
            switch (currentMode) {
                case ChatMode::NORMAL:
                    response = generateNormalResponse(userQuery);
                    break;
                case ChatMode::CULTURAL:
                    response = generateCulturalResponse(userQuery);
                    break;
                case ChatMode::JOKE:
                    response = tellJoke();
                    break;
                case ChatMode::STORY:
                    response = tellStory();
                    break;
                case ChatMode::GUIDE:
                    response = generateCulturalResponse(userQuery);
                    break;
                default:
                    response = generateNormalResponse(userQuery);
                    break;
            }
        }
    }
    
    // 保存对话历史
    ConversationEntry entry;
    entry.userQuery = userQuery;
    entry.botResponse = response;
    entry.timestamp = getCurrentTimestamp();
    conversationHistory.push_back(entry);
    
    return response;
}

void Chatbot::setChatMode(ChatMode mode) {
    currentMode = mode;
    
    // 根据新的对话模式显示提示信息
    switch (mode) {
        case ChatMode::NORMAL:
            std::cout << "聊天模式已切换为：普通聊天" << std::endl;
            break;
        case ChatMode::CULTURAL:
            std::cout << "聊天模式已切换为：文化讲解" << std::endl;
            break;
        case ChatMode::JOKE:
            std::cout << "聊天模式已切换为：讲笑话模式" << std::endl;
            break;
        case ChatMode::STORY:
            std::cout << "聊天模式已切换为：讲故事模式" << std::endl;
            break;
        case ChatMode::GUIDE:
            std::cout << "聊天模式已切换为：导游模式" << std::endl;
            break;
    }
}

ChatMode Chatbot::getCurrentMode() const {
    return currentMode;
}

void Chatbot::clearConversationHistory() {
    conversationHistory.clear();
    std::cout << "对话历史已清空" << std::endl;
}

std::vector<ConversationEntry> Chatbot::getConversationHistory() const {
    return conversationHistory;
}

bool Chatbot::loadChatModel(const std::string& modelPath) {
    std::cout << "加载聊天模型: " << modelPath << std::endl;
    
    // 在实际应用中，这里应该加载预训练的聊天模型
    // 在这个简单实现中，我们只是返回true
    return true;
}

bool Chatbot::saveConversationHistory(const std::string& filename) {
    std::cout << "保存对话历史到: " << filename << std::endl;
    
    // 在实际应用中，这里应该将对话历史保存到文件
    // 在这个简单实现中，我们只是返回true
    return true;
}

// 配置智谱AI GLM-Realtime API
bool Chatbot::setupZhipuAIGLMAPI(const std::string& apiKey, const std::string& model) {
    zhipuAIKey = apiKey;
    zhipuAIModel = model;
    std::cout << "智谱AI GLM-Realtime API已配置，模型: " << model << std::endl;
    return true;
}

void Chatbot::initializeResponseTemplates() {
    // 初始化普通聊天的回复模板
    responseTemplates["greeting"].push_back("你好！很高兴见到你！");
    responseTemplates["greeting"].push_back("嗨！有什么我可以帮你的吗？");
    responseTemplates["greeting"].push_back("你好啊！今天过得怎么样？");
    
    responseTemplates["thanks"].push_back("不客气！很高兴能帮到你。");
    responseTemplates["thanks"].push_back("不用谢，这是我应该做的。");
    responseTemplates["thanks"].push_back("能为你服务是我的荣幸！");
    
    responseTemplates["goodbye"].push_back("再见！祝你有美好的一天！");
    responseTemplates["goodbye"].push_back("下次见！期待与你再次交流。");
    responseTemplates["goodbye"].push_back("拜拜！有需要随时找我。");
    
    responseTemplates["weather"].push_back("今天天气不错，适合外出游玩！");
    responseTemplates["weather"].push_back("外面阳光明媚，记得做好防晒哦！");
    responseTemplates["weather"].push_back("今天可能会下雨，建议带上雨具。");
    
    responseTemplates["unknown"].push_back("抱歉，我不太明白你的意思。能换个方式表达吗？");
    responseTemplates["unknown"].push_back("这个问题有点难，我还在学习中。");
    responseTemplates["unknown"].push_back("你能再详细解释一下吗？我想更好地理解你的问题。");
    
    responseTemplates["default"].push_back("真的吗？说来听听。");
    responseTemplates["default"].push_back("很有趣！还有什么其他想法吗？");
    responseTemplates["default"].push_back("我明白了，你觉得呢？");
    
    // 初始化文化相关的回复模板
    responseTemplates["culture"].push_back("这是一个很有趣的文化话题！");
    responseTemplates["culture"].push_back("文化是人类文明的瑰宝，值得我们深入了解。");
    responseTemplates["culture"].push_back("不同的文化背景造就了丰富多彩的世界。");
}

std::string Chatbot::analyzeUserInput(const std::string& userQuery) {
    // 简单的用户输入分析
    // 在实际应用中，这里应该使用自然语言处理技术
    
    if (userQuery.find("你好") != std::string::npos ||
        userQuery.find("嗨") != std::string::npos ||
        userQuery.find("早上好") != std::string::npos ||
        userQuery.find("晚上好") != std::string::npos) {
        return "greeting";
    } else if (userQuery.find("谢谢") != std::string::npos ||
               userQuery.find("感谢") != std::string::npos) {
        return "thanks";
    } else if (userQuery.find("再见") != std::string::npos ||
               userQuery.find("拜拜") != std::string::npos ||
               userQuery.find("下次见") != std::string::npos) {
        return "goodbye";
    } else if (userQuery.find("天气") != std::string::npos ||
               userQuery.find("下雨") != std::string::npos ||
               userQuery.find("晴天") != std::string::npos) {
        return "weather";
    } else if (userQuery.find("文化") != std::string::npos ||
               userQuery.find("历史") != std::string::npos ||
               userQuery.find("传统") != std::string::npos) {
        return "culture";
    } else {
        return "unknown";
    }
}

std::string Chatbot::generateNormalResponse(const std::string& userQuery) {
    // 根据用户输入分析结果选择回复模板
    std::string analysisResult = analyzeUserInput(userQuery);
    
    // 检查是否有对应的回复模板
    auto it = responseTemplates.find(analysisResult);
    if (it != responseTemplates.end() && !it->second.empty()) {
        // 随机选择一个回复
        int index = std::rand() % it->second.size();
        return it->second[index];
    } else {
        // 使用默认回复
        auto defaultIt = responseTemplates.find("default");
        if (defaultIt != responseTemplates.end() && !defaultIt->second.empty()) {
            int index = std::rand() % defaultIt->second.size();
            return defaultIt->second[index];
        } else {
            // 如果没有默认回复，使用通用回复
            return "我在听呢，请继续。";
        }
    }
}

std::string Chatbot::generateCulturalResponse(const std::string& userQuery) {
    // 生成与文化相关的回复
    // 在实际应用中，这里应该结合文化知识库
    
    const std::string culturalResponses[] = {
        "这是一个非常有深度的文化话题！",
        "文化遗产是我们共同的财富，需要我们共同保护和传承。",
        "了解不同的文化可以拓宽我们的视野，增进我们的理解和包容。",
        "传统文化中蕴含着丰富的智慧，值得我们深入学习和借鉴。",
        "文化交流是促进世界和平与发展的重要途径。"
    };
    
    int index = std::rand() % (sizeof(culturalResponses) / sizeof(culturalResponses[0]));
    return culturalResponses[index];
}

std::string Chatbot::tellJoke() {
    // 讲笑话
    const std::string jokes[] = {
        "为什么程序员总是分不清万圣节和圣诞节？因为 Oct 31 = Dec 25。",
        "什么是AI？就是让计算机做人类不想做的事情。",
        "为什么古代的建筑师总是很快乐？因为他们喜欢构建未来！",
        "博物馆里最忙的是谁？当然是讲解员，因为他们总是在'说古论今'！",
        "为什么历史书总是那么重？因为它们承载了太多的过去！"
    };
    
    int index = std::rand() % (sizeof(jokes) / sizeof(jokes[0]));
    return jokes[index];
}

std::string Chatbot::tellStory() {
    // 讲故事
    const std::string stories[] = {
        "从前有一位旅行家，他游历了世界各地，收集了许多有趣的文化故事。每到一个地方，他都会记录当地的风俗习惯和历史传说，这些故事后来被整理成了一本厚厚的旅行日记，成为了珍贵的文化遗产。",
        "在古老的东方，有一座神秘的宫殿，里面收藏着无数的珍宝和艺术品。这些文物见证了朝代的更替和历史的变迁，每一件都有着独特的故事。如今，这座宫殿已经成为了博物馆，向人们展示着过去的辉煌。",
        "有一位年轻的艺术家，他热爱传统文化，致力于将古老的技艺与现代艺术相结合。通过他的努力，许多濒临失传的传统工艺得到了传承和创新，焕发出新的生命力。",
        "在一个小镇上，有一位百岁老人，他见证了小镇的发展和变化。每当有游客来到这里，老人都会热情地向他们讲述小镇的历史和传说，让更多的人了解这里的文化和故事。"
    };
    
    int index = std::rand() % (sizeof(stories) / sizeof(stories[0]));
    return stories[index];
}

// CURL写入回调函数
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize);
    return totalSize;
}

// 调用智谱AI GLM-Realtime API
std::string Chatbot::callZhipuAIGLMAPI(const std::string& prompt) {
    // 构建请求JSON
    json requestBody;
    requestBody["model"] = zhipuAIModel;
    requestBody["messages"] = json::array();
    
    // 添加历史对话
    for (const auto& entry : conversationHistory) {
        requestBody["messages"].push_back({{"role", "user"}, {"content", entry.userQuery}});
        requestBody["messages"].push_back({{"role", "assistant"}, {"content", entry.botResponse}});
    }
    
    // 添加当前查询
    requestBody["messages"].push_back({{"role", "user"}, {"content", prompt}});
    
    // 设置CURL请求
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "CURL初始化失败" << std::endl;
        return "抱歉，我暂时无法回答这个问题。";
    }
    
    // 设置请求头和URL
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string authHeader = "Authorization: Bearer " + zhipuAIKey;
    headers = curl_slist_append(headers, authHeader.c_str());
    
    // 智谱AI GLM-Realtime API的URL
    curl_easy_setopt(curl, CURLOPT_URL, "https://open.bigmodel.cn/api/paas/v4/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    // 设置POST数据
    std::string jsonStr = requestBody.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
    
    // 用于存储响应的缓冲区
    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
    
    // 执行请求
    CURLcode res = curl_easy_perform(curl);
    
    // 清理
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    
    // 解析响应
    if (res != CURLE_OK) {
        std::cerr << "API请求失败: " << curl_easy_strerror(res) << std::endl;
        return "抱歉，我暂时无法回答这个问题。";
    }
    
    try {
        json response = json::parse(responseString);
        if (response.contains("choices") && !response["choices"].empty() && response["choices"][0].contains("message")) {
            return response["choices"][0]["message"]["content"].get<std::string>();
        }
        // 如果响应结构不符合预期，打印出来以便调试
        std::cerr << "响应结构不符合预期: " << responseString << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "响应解析失败: " << e.what() << std::endl;
        std::cerr << "原始响应: " << responseString << std::endl;
    }
    
    return "抱歉，我暂时无法回答这个问题。";
}

std::string Chatbot::getCurrentTimestamp() const {
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