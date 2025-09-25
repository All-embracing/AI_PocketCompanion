#include <iostream>
#include <string>
#include <vector>
#include "core/AICompanion.h"
#include "chat/Chatbot.h"

int main() {
    // 创建AI智能伴游实例
    AICompanion companion;
    
    // 初始化系统
    if (!companion.initialize()) {
        std::cerr << "Failed to initialize AI Companion system!" << std::endl;
        return -1;
    }
    
    std::cout << "AI智能伴游系统启动成功！" << std::endl;
    
    // 主循环
    bool running = true;
    while (running) {
        // 更新系统状态
        companion.update();
        
        // 检查用户输入
        std::string command;
        std::cout << "请输入命令 (help for commands): ";
        std::getline(std::cin, command);
        
        if (command == "exit" || command == "quit") {
            running = false;
        } else if (command == "help") {
            companion.showHelp();
        } else if (command == "status") {
            companion.showStatus();
        } else if (command == "location") {
            companion.getCurrentLocation();
        } else if (command == "detect") {
            companion.startDetection();
        } else if (command == "stop") {
            companion.stopDetection();
        } else if (command.substr(0, 9) == "chatmode ") {
            // 切换聊天模式
            std::string modeStr = command.substr(9);
            if (modeStr == "normal" || modeStr == "普通") {
                companion.setChatMode(ChatMode::NORMAL);
            } else if (modeStr == "cultural" || modeStr == "文化") {
                companion.setChatMode(ChatMode::CULTURAL);
            } else if (modeStr == "joke" || modeStr == "笑话" || modeStr == "解闷") {
                companion.setChatMode(ChatMode::JOKE);
                std::cout << "已进入伴游解闷模式 - 笑话模式！" << std::endl;
            } else if (modeStr == "story" || modeStr == "故事") {
                companion.setChatMode(ChatMode::STORY);
                std::cout << "已进入伴游解闷模式 - 故事模式！" << std::endl;
            } else if (modeStr == "guide" || modeStr == "导游" || modeStr == "伴游") {
                companion.setChatMode(ChatMode::GUIDE);
                std::cout << "已进入伴游解闷模式 - 导游模式！" << std::endl;
            } else {
                std::cout << "未知的聊天模式。可用模式: normal, cultural, joke, story, guide" << std::endl;
            }
        } else if (command.substr(0, 10) == "setapikey ") {
            // 设置智谱AI API密钥
            std::string apiKey = command.substr(10);
            if (!apiKey.empty()) {
                bool success = companion.setupZhipuAIGLMAPI(apiKey);
                if (success) {
                    std::cout << "智谱AI API密钥设置成功！现在可以直接和智谱AI开聊了。" << std::endl;
                } else {
                    std::cout << "智谱AI API密钥设置失败！" << std::endl;
                }
            } else {
                std::cout << "请输入有效的API密钥。用法: setapikey [your_api_key]" << std::endl;
            }
        } else if (command.substr(0, 11) == "setamapkey ") {
            // 设置高德地图API密钥
            std::string apiKey = command.substr(11);
            if (!apiKey.empty()) {
                bool success = companion.setupAmapAPI(apiKey);
                if (success) {
                    std::cout << "高德地图API密钥设置成功！现在可以获取真实的地理位置信息了。" << std::endl;
                } else {
                    std::cout << "高德地图API密钥设置失败！" << std::endl;
                }
            } else {
                std::cout << "请输入有效的API密钥。用法: setamapkey [your_api_key]" << std::endl;
            }
        } else if (command != "") {
            companion.processUserQuery(command);
        }
    }
    
    // 关闭系统
    companion.shutdown();
    std::cout << "AI智能伴游系统已关闭。" << std::endl;
    
    return 0;
}