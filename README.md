![](/Pic/Pic1.jpg)
# LittleTV——基于ESP32的多功能信息显示屏
## 0. 关于本项目
电子立牌，用以显示日期时间、室内温湿度、室外天气和电脑状态
## 1. 使用说明
**联网模块**   
<img src="/Pic/WiFi.png">  
在该处将WiFi替换为自己的即可进行联网  
**时钟和日期显示模块**  
<img src="/Pic/Time.png">  
依赖NTPClient.h实现，显示了时间、日期与星期

**温湿度模块**  
<img src="/Pic/Hot.png">  
依赖DHT.h和外接的DHT22温湿度传感器实现  

**天气显示模块**  
<img src="/Pic/W.png">  
使用了api.seniverse.com的天气信息  

**电脑信息显示模块**  
<img src="/Pic/Co.png">  
该部分显示了CPU频率、CPU&GPU内存使用率、CPU&GPU温度、C盘余量和电脑音量与亮度、使用了AIDA64 Extreme中的信息  
该模块具体使用方法详见https://zhuanlan.zhihu.com/p/409765418
