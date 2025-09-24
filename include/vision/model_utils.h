#ifndef MODEL_UTILS_H
#define MODEL_UTILS_H

#include <vector>

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
                         float nmsThreshold);

/**
 * @brief 计算两个矩形框的交并比(IoU)
 * @param box1 第一个矩形框
 * @param box2 第二个矩形框
 * @return IoU值
 */
float calculateIoU(const cv::Rect& box1, const cv::Rect& box2);

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
                      std::vector<int>& classIds);

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
                            const std::vector<std::string>& classNames);

#endif // MODEL_UTILS_H