项目中，GPS和IMU相关功能是集成在LocationTracker.cpp文件中的，而不是作为独立的文件存在。通过查看代码，我发现：

1. LocationTracker类实现了GPS和IMU的功能集成：
   
   - 包含了initializeGPS()方法用于初始化GPS模块
   - 包含了initializeIMU()方法用于初始化IMU模块
   - 包含了fuseSensorData()方法用于融合GPS和IMU数据
   - 使用gpsAvailable和imuAvailable变量标记这些传感器是否可用
2. 代码针对不同平台进行了条件编译：
   
   - 在ESP32环境下（通过#ifdef ESP32判断），预留了实际硬件初始化的位置
   - 在x86环境下，使用模拟数据来模拟GPS和IMU功能
3. 项目还包含了位置数据融合、距离计算、反向地理编码等功能，以支持完整的定位服务。
4. 景区电子围栏功能：
   - 实现了景区电子围栏功能，包括ScenicSpotFence结构体定义、5个景区（故宫博物院等）的围栏数据初始化
   - 实现了calculateDistance计算地理距离的方法（使用Haversine公式）
   - 实现了isPointInFence判断点是否在围栏内的方法
   - 实现了hasEnteredNewScenicSpot和getCurrentScenicSpot方法跟踪用户景区位置状态
   
这种设计将GPS和IMU功能集成在一个LocationTracker类中，而不是创建单独的GPS.cpp和IMU.cpp文件，这是一种常见的模块化设计方式，可以更好地封装和管理相关功能。