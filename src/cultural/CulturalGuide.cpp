#include "cultural/CulturalGuide.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

CulturalGuide::CulturalGuide() {
    // 初始化随机数生成器
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

CulturalGuide::~CulturalGuide() {
    // 清理资源
    knowledgeBase.clear();
    locationKnowledge.clear();
}

bool CulturalGuide::initialize() {
    std::cout << "初始化文化讲解系统..." << std::endl;
    
    // 初始化默认文化知识库
    initializeDefaultKnowledge();
    
    return true;
}

std::string CulturalGuide::getExplanation(const std::string& objectName) {
    // 检查对象是否在知识库中
    auto it = knowledgeBase.find(objectName);
    if (it != knowledgeBase.end() && !it->second.empty()) {
        // 随机选择一个相关的文化信息
        int index = std::rand() % it->second.size();
        const CulturalInfo& info = it->second[index];
        
        // 返回格式化的讲解内容
        std::string explanation = info.title + ": " + info.description;
        return explanation;
    } else {
        // 如果没有找到，生成随机讲解
        return generateRandomExplanation(objectName);
    }
}

std::vector<CulturalInfo> CulturalGuide::getLocationInfo(const std::string& locationName) {
    // 检查位置是否在知识库中
    auto it = locationKnowledge.find(locationName);
    if (it != locationKnowledge.end()) {
        return it->second;
    } else {
        return std::vector<CulturalInfo>();
    }
}

std::vector<CulturalInfo> CulturalGuide::searchCulturalInfo(const std::string& keyword) {
    std::vector<CulturalInfo> results;
    
    // 在对象知识库中搜索
    for (const auto& pair : knowledgeBase) {
        for (const auto& info : pair.second) {
            if (info.title.find(keyword) != std::string::npos ||
                info.description.find(keyword) != std::string::npos ||
                info.history.find(keyword) != std::string::npos) {
                results.push_back(info);
            }
        }
    }
    
    // 在位置知识库中搜索
    for (const auto& pair : locationKnowledge) {
        for (const auto& info : pair.second) {
            if (info.title.find(keyword) != std::string::npos ||
                info.description.find(keyword) != std::string::npos ||
                info.history.find(keyword) != std::string::npos) {
                results.push_back(info);
            }
        }
    }
    
    return results;
}

bool CulturalGuide::addCulturalInfo(const std::string& objectName, const CulturalInfo& info) {
    // 将文化信息添加到知识库
    knowledgeBase[objectName].push_back(info);
    return true;
}

bool CulturalGuide::loadKnowledgeBase(const std::string& filename) {
    std::cout << "加载文化知识库: " << filename << std::endl;
    
    // 在实际应用中，这里应该从文件加载知识库
    // 在这个简单实现中，我们只是返回true
    return true;
}

bool CulturalGuide::saveKnowledgeBase(const std::string& filename) {
    std::cout << "保存文化知识库: " << filename << std::endl;
    
    // 在实际应用中，这里应该将知识库保存到文件
    // 在这个简单实现中，我们只是返回true
    return true;
}

void CulturalGuide::initializeDefaultKnowledge() {
    std::cout << "初始化默认文化知识库..." << std::endl;
    
    // 添加一些常见文物的文化信息
    CulturalInfo info;
    
    // 古代雕像
    info.title = "古代雕像艺术";
    info.description = "古代雕像是人类文明的重要遗产，反映了当时的艺术水平和文化背景。";
    info.history = "雕像艺术在世界各地都有悠久的历史，从古希腊罗马的大理石雕像到中国的青铜像，各具特色。";
    info.significance = "雕像不仅是艺术作品，也是研究古代社会、宗教和文化的重要实物资料。";
    knowledgeBase["古代雕像"].push_back(info);
    knowledgeBase["雕塑"].push_back(info);
    
    // 文物展示
    info.title = "文物保护与展示";
    info.description = "文物展示是博物馆的核心功能，通过科学的陈列方式向观众传达历史文化信息。";
    info.history = "现代博物馆起源于17世纪的欧洲，经过几个世纪的发展，已成为文化传承的重要场所。";
    info.significance = "文物展示不仅让公众欣赏到珍贵的文化遗产，也促进了文化交流和历史研究。";
    knowledgeBase["文物展示"].push_back(info);
    
    // 历史建筑
    info.title = "历史建筑的价值";
    info.description = "历史建筑是人类文明的重要载体，见证了城市和社会的发展变迁。";
    info.history = "不同历史时期的建筑风格反映了当时的技术水平、审美观念和社会状况。";
    info.significance = "保护历史建筑对于维护文化多样性和城市特色具有重要意义。";
    knowledgeBase["历史建筑"].push_back(info);
    
    // 传统绘画
    info.title = "传统绘画艺术";
    info.description = "传统绘画是人类表达情感和记录生活的重要方式，不同文化形成了独特的绘画风格。";
    info.history = "从中国的水墨画到欧洲的油画，传统绘画艺术历经千年发展，形成了丰富多样的表现形式。";
    info.significance = "传统绘画不仅具有艺术价值，也是研究历史、文化和社会的重要资料。";
    knowledgeBase["传统绘画"].push_back(info);
    knowledgeBase["壁画"].push_back(info);
    knowledgeBase["书法作品"].push_back(info);
    
    // 碑文
    info.title = "碑文的历史价值";
    info.description = "碑文是刻在石头上的文字记录，是研究古代历史和文化的重要资料。";
    info.history = "碑文在世界各地都有发现，中国的甲骨文、埃及的象形文字碑刻都是著名的例子。";
    info.significance = "碑文为我们提供了珍贵的第一手历史资料，对于还原古代社会具有不可替代的作用。";
    knowledgeBase["碑文"].push_back(info);
    
    // 瓷器展品
    info.title = "瓷器的艺术魅力";
    info.description = "瓷器是中国古代的伟大发明之一，以其精湛的工艺和独特的艺术风格闻名于世。";
    info.history = "中国瓷器的制作始于商代，经过唐宋元明清等朝代的发展，达到了很高的艺术水平。";
    info.significance = "瓷器不仅是实用器皿，也是精美的艺术品，通过丝绸之路等贸易通道影响了世界文化。";
    knowledgeBase["瓷器展品"].push_back(info);
    
    // 青铜器
    info.title = "青铜器的历史地位";
    info.description = "青铜器是中国夏商周时期的重要文物，反映了当时高度发达的青铜冶炼技术和礼制文化。";
    info.history = "中国的青铜时代大约从公元前21世纪持续到公元前5世纪，创造了许多精美的青铜礼器和兵器。";
    info.significance = "青铜器对于研究中国古代社会结构、礼制文化和科技发展具有重要价值。";
    knowledgeBase["青铜器"].push_back(info);
    
    // 添加景点的文化信息
    // 故宫博物院
    info.title = "故宫博物院";
    info.description = "故宫博物院是中国明清两代的皇家宫殿，位于北京市中心，是世界上现存规模最大、保存最为完整的木质结构古建筑群之一。";
    info.history = "故宫始建于明朝永乐四年（1406年），建成于永乐十八年（1420年），历经明、清两代24位皇帝，是中国古代宫廷建筑的精华。";
    info.significance = "故宫博物院不仅是中国文化的重要象征，也是世界文化遗产，对于研究中国古代历史、艺术和建筑具有不可替代的价值。";
    locationKnowledge["故宫博物院"].push_back(info);
    
    // 天坛公园
    info.title = "天坛公园";
    info.description = "天坛是明清两代皇帝祭天、祈谷和祈雨的场所，位于北京市南部，是中国古代祭祀建筑的杰出代表。";
    info.history = "天坛始建于明朝永乐十八年（1420年），嘉靖年间进行了大规模改建，形成了现在的格局。";
    info.significance = "天坛以其独特的建筑设计和深厚的文化内涵，展现了中国古代哲学思想和建筑艺术的高度成就。";
    locationKnowledge["天坛公园"].push_back(info);
    
    // 兵马俑博物馆
    info.title = "兵马俑博物馆";
    info.description = "兵马俑是中国第一位皇帝秦始皇的陪葬坑，位于陕西省西安市临潼区，被誉为'世界第八大奇迹'。";
    info.history = "兵马俑始建于公元前246年至公元前208年，是秦始皇陵的重要组成部分，1974年被发现。";
    info.significance = "兵马俑生动地展现了秦朝军队的编制、武器装备和作战方式，对于研究中国古代军事、艺术和科技具有重要价值。";
    locationKnowledge["兵马俑博物馆"].push_back(info);
    
    // 杭州西湖
    info.title = "杭州西湖";
    info.description = "西湖位于浙江省杭州市中心，是中国著名的风景名胜区和文化遗产，以其秀丽的湖光山色和丰富的人文景观闻名于世。";
    info.history = "西湖的开发历史可以追溯到秦朝，经过历代的疏浚和建设，形成了现在的'西湖十景'等著名景观。";
    info.significance = "西湖不仅是自然美景的典范，也是中国传统文化的重要载体，体现了中国人'天人合一'的哲学思想。";
    locationKnowledge["杭州西湖"].push_back(info);
    
    // 深圳世界之窗
    info.title = "深圳世界之窗";
    info.description = "世界之窗是中国深圳的一座大型文化主题公园，汇集了世界各地的著名景观和建筑的微缩模型。";
    info.history = "世界之窗于1994年建成开放，占地48万平方米，分为世界广场、亚洲区、大洋洲区、欧洲区、非洲区、美洲区等八大区域。";
    info.significance = "世界之窗通过微缩景观的形式，让游客在短时间内领略世界各地的文化和建筑精华，促进了不同文化之间的交流和理解。";
    locationKnowledge["深圳世界之窗"].push_back(info);
}

std::string CulturalGuide::generateRandomExplanation(const std::string& objectName) {
    // 为未知对象生成随机讲解
    const std::string explanations[] = {
        objectName + "是一件很有特色的展品，展现了独特的艺术风格和文化内涵。",
        "这件" + objectName + "具有重要的历史价值，反映了当时的社会状况和审美观念。",
        "" + objectName + "是我们文化遗产的重要组成部分，值得我们珍视和保护。",
        "从这件" + objectName + "中，我们可以感受到古代工匠的精湛技艺和创造力。",
        objectName + "承载着丰富的历史信息，是研究古代文化的重要实物资料。"
    };
    
    int index = std::rand() % (sizeof(explanations) / sizeof(explanations[0]));
    return explanations[index];
}