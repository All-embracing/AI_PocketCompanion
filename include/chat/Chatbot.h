#ifndef CHATBOT_H
#define CHATBOT_H

#include <string>
#include <vector>
#include <map>

// 对话历史条目
typedef struct {
    std::string userQuery;    // 用户输入
    std::string botResponse;  // 机器人回复
    std::string timestamp;    // 时间戳
} ConversationEntry;

// 对话模式枚举
enum class ChatMode {
    NORMAL,     // 普通聊天模式
    CULTURAL,   // 文化讲解模式
    JOKE,       // 讲笑话模式
    STORY,      // 讲故事模式
    GUIDE       // 导游模式
};

class Chatbot {
public:
    Chatbot();
    ~Chatbot();
    
    // 初始化聊天机器人
    bool initialize();
    
    // 生成回复
    std::string generateResponse(const std::string& userQuery);
    
    // 设置对话模式
    void setChatMode(ChatMode mode);
    
    // 获取当前对话模式
    ChatMode getCurrentMode() const;
    
    // 清空对话历史
    void clearConversationHistory();
    
    // 获取对话历史
    std::vector<ConversationEntry> getConversationHistory() const;
    
    // 加载对话模型
    bool loadChatModel(const std::string& modelPath);
    
    // 保存对话历史
    bool saveConversationHistory(const std::string& filename);
    
    // 配置智谱AI GLM-Realtime API
    bool setupZhipuAIGLMAPI(const std::string& apiKey, const std::string& model = "glm-realtime");
    
    // 调用智谱AI GLM-Realtime API
    std::string callZhipuAIGLMAPI(const std::string& prompt);
    
private:
    // 对话模式
    ChatMode currentMode;
    
    // 对话历史
    std::vector<ConversationEntry> conversationHistory;
    
    // 回复模板库
    std::map<std::string, std::vector<std::string>> responseTemplates;
    
    // 初始化回复模板
    void initializeResponseTemplates();
    
    // 分析用户输入
    std::string analyzeUserInput(const std::string& userQuery);
    
    // 生成普通回复
    std::string generateNormalResponse(const std::string& userQuery);
    
    // 生成文化相关回复
    std::string generateCulturalResponse(const std::string& userQuery);
    
    // 讲笑话
    std::string tellJoke();
    
    // 讲故事
    std::string tellStory();
    
    // 获取当前时间戳
    std::string getCurrentTimestamp() const;
};

#endif // CHATBOT_H