#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "vision/model_utils.h"

#ifdef ESP32
// ESP32环境不需要OpenCV
#else
#include <opencv2/opencv.hpp>
#endif

/**
 * @brief 非最大抑制算法，用于过滤重叠的检测框
 * @param boxes 检测框列表
 * @param scores 检测框的置信度分数
 * @param scoreThreshold 分数阈值
 * @param nmsThreshold NMS阈值
 * @return 过滤后的检测框索引
 */
std::vector<int> applyNMS(const std::vector<cv::Rect>& boxes, 
                         const std::vector<float>& scores, 
                         float scoreThreshold, 
                         float nmsThreshold) {
    std::vector<int> indices;
    
    // 首先过滤掉低于分数阈值的检测框
    std::vector<int> candidateIndices;
    for (size_t i = 0; i < scores.size(); ++i) {
        if (scores[i] >= scoreThreshold) {
            candidateIndices.push_back(static_cast<int>(i));
        }
    }
    
    // 应用NMS
    while (!candidateIndices.empty()) {
        // 找到置信度最高的检测框
        int bestIdx = 0;
        float bestScore = scores[candidateIndices[bestIdx]];
        for (size_t i = 1; i < candidateIndices.size(); ++i) {
            if (scores[candidateIndices[i]] > bestScore) {
                bestScore = scores[candidateIndices[i]];
                bestIdx = static_cast<int>(i);
            }
        }
        
        // 将最佳检测框添加到结果中
        int bestCandidateIdx = candidateIndices[bestIdx];
        indices.push_back(bestCandidateIdx);
        
        // 从候选列表中移除最佳检测框
        candidateIndices.erase(candidateIndices.begin() + bestIdx);
        
        // 计算与剩余检测框的IoU，并移除重叠度高的检测框
        std::vector<int> newCandidateIndices;
        for (int idx : candidateIndices) {
            float iou = calculateIoU(boxes[bestCandidateIdx], boxes[idx]);
            if (iou < nmsThreshold) {
                newCandidateIndices.push_back(idx);
            }
        }
        candidateIndices = newCandidateIndices;
    }
    
    return indices;
}

/**
 * @brief 计算两个矩形框的交并比(IoU)
 * @param box1 第一个矩形框
 * @param box2 第二个矩形框
 * @return IoU值
 */
float calculateIoU(const cv::Rect& box1, const cv::Rect& box2) {
    // 计算交集区域
    int x1 = std::max(box1.x, box2.x);
    int y1 = std::max(box1.y, box2.y);
    int x2 = std::min(box1.x + box1.width, box2.x + box2.width);
    int y2 = std::min(box1.y + box1.height, box2.y + box2.height);
    
    // 如果没有交集，IoU为0
    if (x1 >= x2 || y1 >= y2) {
        return 0.0f;
    }
    
    // 计算交集面积
    float intersectionArea = static_cast<float>((x2 - x1) * (y2 - y1));
    
    // 计算并集面积
    float unionArea = static_cast<float>(box1.area() + box2.area() - intersectionArea);
    
    // 计算IoU
    return intersectionArea / unionArea;
}

/**
 * @brief 对YOLO模型的输出进行后处理，获取检测框和类别
 * @param outputs 模型输出
 * @param confidenceThreshold 置信度阈值
 * @param nmsThreshold NMS阈值
 * @param imageWidth 图像宽度
 * @param imageHeight 图像高度
 * @param boxes 输出的检测框
 * @param confidences 输出的置信度
 * @param classIds 输出的类别ID
 */
void processYOLOOutput(const cv::Mat& outputs, 
                      float confidenceThreshold, 
                      float nmsThreshold, 
                      int imageWidth, 
                      int imageHeight, 
                      std::vector<cv::Rect>& boxes, 
                      std::vector<float>& confidences, 
                      std::vector<int>& classIds) {
    // YOLO输出格式：每行包含 [center_x, center_y, width, height, confidence, class1_score, class2_score, ...]
    const int dimensions = outputs.size[1];
    const int numBoxes = outputs.size[0];
    
    boxes.clear();
    confidences.clear();
    classIds.clear();
    
    for (int i = 0; i < numBoxes; ++i) {
        const float* data = outputs.ptr<float>(i);
        
        // 提取边界框坐标
        float centerX = data[0] * imageWidth;
        float centerY = data[1] * imageHeight;
        float width = data[2] * imageWidth;
        float height = data[3] * imageHeight;
        
        // 转换为左上角坐标和宽高
        int x = static_cast<int>(centerX - width / 2);
        int y = static_cast<int>(centerY - height / 2);
        
        // 确保边界框在图像范围内
        x = std::max(0, x);
        y = std::max(0, y);
        width = std::min(static_cast<float>(imageWidth - x), width);
        height = std::min(static_cast<float>(imageHeight - y), height);
        
        // 创建检测框
        cv::Rect box(x, y, static_cast<int>(width), static_cast<int>(height));
        
        // 提取置信度
        float confidence = data[4];
        
        // 如果置信度高于阈值，继续处理
        if (confidence >= confidenceThreshold) {
            // 查找最高得分的类别
            float highestScore = 0.0f;
            int classId = -1;
            
            for (int j = 5; j < dimensions; ++j) {
                if (data[j] > highestScore) {
                    highestScore = data[j];
                    classId = j - 5;
                }
            }
            
            // 如果类别得分高于阈值，添加到结果
            if (highestScore >= confidenceThreshold) {
                boxes.push_back(box);
                confidences.push_back(confidence * highestScore);
                classIds.push_back(classId);
            }
        }
    }
    
    // 应用NMS
    std::vector<int> indices = applyNMS(boxes, confidences, confidenceThreshold, nmsThreshold);
    
    // 根据NMS结果过滤检测框
    std::vector<cv::Rect> filteredBoxes;
    std::vector<float> filteredConfidences;
    std::vector<int> filteredClassIds;
    
    for (int idx : indices) {
        filteredBoxes.push_back(boxes[idx]);
        filteredConfidences.push_back(confidences[idx]);
        filteredClassIds.push_back(classIds[idx]);
    }
    
    boxes = filteredBoxes;
    confidences = filteredConfidences;
    classIds = filteredClassIds;
}

/**
 * @brief 在图像上绘制检测结果
 * @param image 输入图像
 * @param boxes 检测框
 * @param confidences 置信度
 * @param classIds 类别ID
 * @param classNames 类别名称列表
 * @return 绘制后的图像
 */
cv::Mat drawDetectionResults(cv::Mat image, 
                            const std::vector<cv::Rect>& boxes, 
                            const std::vector<float>& confidences, 
                            const std::vector<int>& classIds, 
                            const std::vector<std::string>& classNames) {
    cv::Mat result = image.clone();
    
    // 定义颜色列表（用于不同类别）
    std::vector<cv::Scalar> colors;
    for (size_t i = 0; i < 80; ++i) {  // COCO数据集有80个类别
        int hue = static_cast<int>(i * 360.0 / 80) % 360;
        colors.push_back(cv::Scalar(hue, 255, 255));
    }
    
    // 绘制每个检测框
    for (size_t i = 0; i < boxes.size(); ++i) {
        const cv::Rect& box = boxes[i];
        float confidence = confidences[i];
        int classId = classIds[i];
        
        // 获取类别名称
        std::string className = "Unknown";
        if (classId >= 0 && classId < static_cast<int>(classNames.size())) {
            className = classNames[classId];
        }
        
        // 创建标签文本
        std::string label = className + " " + cv::format("%.2f", confidence);
        
        // 绘制检测框
        cv::rectangle(result, box, cv::Scalar(0, 255, 0), 2);
        
        // 绘制标签背景
        int baseLine;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        int top = std::max(box.y, labelSize.height);
        cv::rectangle(result, 
                     cv::Point(box.x, top - labelSize.height),
                     cv::Point(box.x + labelSize.width, top + baseLine),
                     cv::Scalar(0, 255, 0), -1);
        
        // 绘制标签文本
        cv::putText(result, label, cv::Point(box.x, top),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
    
    return result;
}