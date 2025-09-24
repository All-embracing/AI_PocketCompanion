#ifndef CULTURAL_GUIDE_H
#define CULTURAL_GUIDE_H

#include <string>
#include <map>
#include <vector>

// 文化信息结构体
typedef struct {
    std::string title;       // 标题
    std::string description; // 详细描述
    std::string history;     // 历史背景
    std::string significance; // 文化意义
    std::vector<std::string> relatedTopics; // 相关主题
} CulturalInfo;

class CulturalGuide {
public:
    CulturalGuide();
    ~CulturalGuide();
    
    // 初始化文化知识库
    bool initialize();
    
    // 获取对象的文化讲解
    std::string getExplanation(const std::string& objectName);
    
    // 获取景点的文化信息
    std::vector<CulturalInfo> getLocationInfo(const std::string& locationName);
    
    // 搜索文化信息
    std::vector<CulturalInfo> searchCulturalInfo(const std::string& keyword);
    
    // 添加自定义文化信息
    bool addCulturalInfo(const std::string& objectName, const CulturalInfo& info);
    
    // 加载文化知识库
    bool loadKnowledgeBase(const std::string& filename);
    
    // 保存文化知识库
    bool saveKnowledgeBase(const std::string& filename);
    
private:
    // 文化知识库
    std::map<std::string, std::vector<CulturalInfo>> knowledgeBase;
    
    // 景点文化信息
    std::map<std::string, std::vector<CulturalInfo>> locationKnowledge;
    
    // 初始化默认文化知识库
    void initializeDefaultKnowledge();
    
    // 生成随机讲解内容
    std::string generateRandomExplanation(const std::string& objectName);
};

#endif // CULTURAL_GUIDE_H