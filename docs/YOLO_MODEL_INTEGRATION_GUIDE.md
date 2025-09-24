# YOLO模型集成指南

本指南介绍了如何在AI Companion项目中集成和使用本地YOLO模型进行图像检测。

## 支持的模型格式

项目支持以下平台上的YOLO模型:

1. **ESP32-S3平台**
   - TensorFlow Lite格式模型 (.tflite)
   
2. **x86平台**
   - ONNX格式模型 (.onnx)
   - 推荐使用YOLOv5、YOLOv6、YOLOv7或YOLOv8模型

## 模型下载和配置

### 1. 下载YOLO模型

可以从以下来源下载预训练的YOLO模型:

- [Ultralytics YOLO官方仓库](https://github.com/ultralytics/yolov5/releases)
- [YOLOv8官方仓库](https://github.com/ultralytics/ultralytics)
- [ONNX Model Zoo](https://github.com/onnx/models)

对于x86平台，我们推荐使用YOLOv5s.onnx作为默认模型，因为它在性能和准确度之间提供了良好的平衡。

### 2. 放置模型文件

下载模型后，请将其放置在项目的`models`目录中:

```
d:\test_lab\ai_companion\models\
```

默认情况下，系统会查找`models/yolov5s.onnx`文件。如果需要使用其他模型，请修改`VisionProcessor.cpp`文件中的`loadAIModel`方法。

### 3. 类别文件配置

项目已经默认包含了`models/coco.names`文件，其中包含了COCO数据集的80个类别名称。如果您使用的是自定义训练的模型，请确保相应地更新类别文件。

## 使用方法

一旦模型正确配置，系统会自动在启动时加载模型并进行图像检测。您可以通过以下步骤使用图像检测功能:

1. 初始化VisionProcessor:
   ```cpp
   VisionProcessor visionProcessor;
   visionProcessor.initialize();
   ```

2. 启动检测:
   ```cpp
   visionProcessor.start();
   ```

3. 定期更新以处理图像和获取检测结果:
   ```cpp
   visionProcessor.update();
   std::vector<std::string> detectedObjects = visionProcessor.getDetectedObjects();
   ```

4. 停止检测:
   ```cpp
   visionProcessor.stop();
   ```

## 配置参数

您可以调整以下参数以优化检测性能:

- **检测灵敏度**: 设置目标检测的置信度阈值
  ```cpp
  visionProcessor.setSensitivity(0.7f); // 值范围: 0.1-1.0，默认为0.7
  ```

## 模拟模式

如果系统无法加载YOLO模型（例如，模型文件不存在），它会自动切换到模拟模式。在模拟模式下，系统会根据预定义的对象列表和随机概率生成检测结果。

这对于在没有实际模型的情况下测试应用程序的其他功能非常有用。

## 常见问题和故障排除

### 模型加载失败

- 确保模型文件路径正确
- 验证模型格式是否兼容（ESP32-S3: .tflite，x86: .onnx）
- 检查模型文件是否损坏
- 确保项目有足够的内存加载模型

### 检测性能问题

- 对于x86平台，可以尝试使用更小的模型（如yolov5n.onnx）来提高性能
- 降低检测灵敏度可以减少误报
- 确保图像分辨率适合模型输入（默认: 640x640）

### 检测结果不准确

- 尝试调整检测灵敏度
- 确保使用的模型经过适当的训练，能够识别您想要检测的对象
- 对于特定领域的应用，考虑使用自定义训练的模型

## 扩展和定制

如果您想扩展或定制图像检测功能，可以修改以下文件:

- `src/vision/VisionProcessor.cpp`: 主要的视觉处理实现
- `src/vision/model_utils.cpp`: 模型辅助函数（如NMS、后处理等）
- `include/vision/model_utils.h`: 模型辅助函数的头文件

## 注意事项

- 在ESP32-S3平台上，模型应该编译到固件中以获得最佳性能
- 对于生产环境，建议使用经过适当训练的自定义模型以提高特定场景下的检测精度
- 较大的模型可能会显著增加内存使用和推理时间

## 参考链接

- [YOLOv5官方文档](https://github.com/ultralytics/yolov5)
- [TensorFlow Lite官方文档](https://www.tensorflow.org/lite)
- [OpenCV DNN模块文档](https://docs.opencv.org/4.x/d2/d58/tutorial_table_of_content_dnn.html)