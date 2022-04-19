#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <Servo.h>
#include "OneButton.h"
#include "MyFont.h"
#include "Water.h"
#include "Fire.h"
#include "Snow.h"
#include "Windy.h"
#include "Bailan.h"
#include "QR.h"
#include "saoma2.h"
#include "Null.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.aliyun.com");
TFT_eSPI tft = TFT_eSPI();
DynamicJsonDocument jsonBuffer(256);
#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
static const int servoPin = 13;
Servo servo1;
int posDegrees = 0;
const int httpPort = 80;
const char *ssid = "CMCC-3hza";
const char *password = "yky74f6q";
const char* host = "api.seniverse.com";
const char *host2 = "192.168.1.5";
String cpu_frequence;
String cpu_usage;
String mem_usage;
String cpu_temperature;
String gpu_temperature;
String brightness;
String volume;
String C_usage;
uint32_t updateTimeA = 0;
uint32_t updateTimeB = 0;
uint32_t updateTimeC = 0;
uint32_t updateTimeD = 0;
uint32_t updateTimeQR = 0;
bool PC_flag = true;
bool Page1_flag = true;
bool Page2_flag = false;
bool Page3_flag = false;
bool Page4_flag = false;
bool FillScreen_flag = false;
bool REzz_flag = false;
String now_address = "", now_weather = "", now_temperature = "";
String url = "/v3/weather/now.json?key=SlKKew7DsEr9oL14U&location=jilin&language=en&unit=c";
OneButton RST(17, true);
OneButton SET(16, true);
OneButton MID(4, true);
OneButton R(12, true);
OneButton L(14, true);
OneButton D(22, true);
OneButton U(21, true);

void setup()
{
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  dht.begin();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  servo1.attach(servoPin);
  timeClient.begin();
  timeClient.setTimeOffset(28800);
  client.connect(host2, httpPort);
  updateTimeA = millis(); //1s延时
  updateTimeB = millis(); //20s延时
  updateTimeC = millis(); //20s延时
  updateTimeD = millis(); //1.1s延时
  updateTimeQR = millis();
  MID.attachClick(click1);
  SET.attachClick(click2);
  RST.attachClick(click3);
  U.attachClick(clickU);
  D.attachClick(clickD);
  U.attachDuringLongPress(longPressU);
  D.attachDuringLongPress(longPressD);
  tft.setSwapBytes(true);
  tft.pushImage(10, 100, 16, 16, Fire);
  tft.pushImage(140, 100, 16, 16, Water);
  tft.pushImage(10, 135, 16, 16, Snow);
  tft.pushImage(140, 135, 16, 16, Windy);
  tft.pushImage(12, 175, 73, 52, Bailan);
  showMyFont(110, 177, "刷", 0x4FD6);
  showMyFont(240, 177, "存", 0x4FD6);
  showMyFont(110, 199, "火", 0xEE29);
  showMyFont(206, 199, "火", 0xEE29);
  showMyFont(110, 199 + 21, "文", 0xA7FF);
  showMyFont(206, 199 + 22, "光", 0x061F);
  showMyFont(250 + 16, 199 + 22, "声", 0x061F);
  tft.setTextSize(1);
  tft.setTextColor(0x4FD6, TFT_BLACK);
  tft.drawString("C", 110 + 7, 177 + 5, 1);
  tft.drawString("C", 240 + 5, 177 + 5, 1);
  tft.setTextColor(0xEE29, TFT_BLACK);
  tft.drawString("C", 110 + 6, 199 + 4, 1);
  tft.drawString("G", 206 + 6, 199 + 4, 1);
  tft.setTextColor(0xA7FF, TFT_BLACK);
  tft.drawString("C", 110 + 6, 199 + 22 + 4, 1);
  tft.setTextColor(0x061F, TFT_BLACK);
  tft.drawString("L", 206 + 6, 199 + 22 + 4, 1);
  //tft.pushImage(135, 177, 150, 52, Menu);
  //showMyFonts(116, 177, "右", TFT_WHITE);
}

void loop()
{
  RST.tick();
  SET.tick();
  MID.tick();
  R.tick();
  L.tick();
  D.tick();
  U.tick();
  if (Page2_flag == true) //2页
  {
    if (FillScreen_flag == false)
    {
      tft.fillScreen(TFT_BLACK);
      FillScreen_flag = true;
    }
    if (updateTimeQR <= millis())
    {
      updateTimeQR = millis() + 5000;
      tft.pushImage(0, 0, 320, 240, QR);
      if (REzz_flag == true) {
        tft.pushImage(0, 190, 322, 50, saoma2);
        REzz_flag = false;
      }
      else REzz_flag = true;
    }
  }
  if (Page1_flag == true) { //1页
    if (updateTimeA <= millis())
    {
      //时钟
      updateTimeA = millis() + 1000;
      timeClient.update();
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(4);
      tft.drawCentreString(timeClient.getFormattedTime(), 160, 10, 1);
      unsigned long epochTime = timeClient.getEpochTime();
      struct tm *ptm = gmtime ((time_t *)&epochTime);
      tft.setTextSize(3);
      tft.drawCentreString("2022 " + String(ptm->tm_mon + 1) + "/" + String(ptm->tm_mday) + " " + String(timeClient.getDay()), 160, 55, 1);
    }
    if (updateTimeB <= millis())
    {
      updateTimeB = millis() + 20000;
      //天气
      if (!client.connect(host, httpPort))
      {
        Serial.println("connection failed"); return;
      }
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
      String answer;
      while (!client.available()) {};
      while (client.available())
      {
        String line = client.readStringUntil('\r');
        answer += line;
      }
      client.stop();
      String jsonAnswer;
      int jsonIndex;
      for (int i = 0; i < answer.length(); i++) {
        if (answer[i] == '{') {
          jsonIndex = i;
          break;
        }
      }
      jsonAnswer = answer.substring(jsonIndex);
      Serial.println();
      Serial.println("JSON answer: ");
      Serial.println(jsonAnswer);

      // JSON解析
      StaticJsonDocument<512> doc;
      deserializeJson(doc, jsonAnswer);
      JsonObject results_0 = doc["results"][0];
      JsonObject results_0_location = results_0["location"];
      const char* results_0_location_name = results_0_location["name"]; // "吉林"
      JsonObject results_0_now = results_0["now"];
      const char* results_0_now_text = results_0_now["text"]; // "晴"
      const char* results_0_now_temperature = results_0_now["temperature"]; // "-12"
      now_address = results_0_location_name;
      now_weather = results_0_now_text;
      now_temperature = results_0_now_temperature;
      tft.setTextColor(0xA7FF, TFT_BLACK);
      tft.drawString(now_temperature + "  ", 37, 135, 1);
      tft.setTextColor(0x4FD6, TFT_BLACK);
      tft.drawString(now_weather + "      ", 167, 135, 1);
      Serial.println(now_address);

      if (PC_flag == true) {
        if (!client.connect(host2, httpPort))
        {
          Serial.println("PC connection failed");
          PC_flag = false;
          tft.setTextColor(0xEE29, TFT_BLACK);
          //tft.drawString("PC power off", 110, 180, 1);
          tft.pushImage(110, 180, 126, 126, Null);
        }
      }
    }
    if (updateTimeC <= millis())
    {
      updateTimeC = millis() + 2500;
      //温湿度计
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.drawString(String(t, 1), 37, 100, 1);
      tft.setTextColor(0x061F, TFT_BLACK);
      tft.drawString(String(h, 1) + "%", 167, 100, 1);
    }
    if (updateTimeD <= millis())
    {
      updateTimeD = millis() + 1100;
      //电脑检测
      if (PC_flag == true)
      {
        String getUrl = "/sse";
        client.print(String("GET ") + getUrl +
                     " HTTP/1.1\r\n" + "Content-Type=application/json;charset=utf-8\r\n" +
                     "Host: " + host2 + "\r\n" + "User-Agent=ESP32\r\n" +
                     "Connection: close\r\n\r\n");
        Serial.println("Get send");
        delay(10);
        char endOfHeaders[] = "\n\n";
        bool ok = client.find(endOfHeaders);
        if (!ok)
        {
          Serial.println("No response or invalid response!");
        }
        Serial.println("Skip headers");
        String line = "";
        line += client.readStringUntil('\n');
        Serial.println("Content:");
        Serial.println(line);
        int16_t dataStart = 0;
        int16_t dataEnd = 0;
        String dataStr;
        tft.setTextSize(2);
        char cpuFreq[] = "CPU fequence";//CPU频率
        dataStart = line.indexOf(cpuFreq) + strlen(cpuFreq);
        dataEnd = line.indexOf(" MHz", dataStart);
        cpu_frequence = line.substring(dataStart, dataEnd);
        tft.setTextColor(0x4FD6, TFT_BLACK);
        tft.drawString(cpu_frequence + "MHz", 126, 177, 1);

        char cpuUsage[] = "CPU usage";//CPU用量
        dataStart = line.indexOf(cpuUsage) + strlen(cpuUsage);
        dataEnd = line.indexOf("%", dataStart);
        cpu_usage = line.substring(dataStart, dataEnd);
        tft.drawString(cpu_usage + "% ", 258, 161, 1);

        char memUsage[] = "Memory Utilization";//内存用量
        dataStart = line.indexOf(memUsage) + strlen(memUsage);
        dataEnd = line.indexOf("%", dataStart);
        mem_usage = line.substring(dataStart, dataEnd);
        tft.setTextColor(0x4FD6, TFT_BLACK);
        tft.drawString(mem_usage + "%", 258, 177, 1);

        char cpuTem[] = "CPU Tem";//CPU温度
        dataStart = line.indexOf(cpuTem) + strlen(cpuTem);
        dataEnd = line.indexOf("*C", dataStart);
        cpu_temperature = line.substring(dataStart, dataEnd);
        tft.setTextColor(0xEE29, TFT_BLACK);
        tft.drawString(cpu_temperature + "`C", 127, 200, 1);

        char gpuTem[] = "GPU Hotspot";//GPU温度
        dataStart = line.indexOf(gpuTem) + strlen(gpuTem);
        dataEnd = line.indexOf("*C", dataStart);
        gpu_temperature = line.substring(dataStart, dataEnd);
        tft.setTextColor(0xEE29, TFT_BLACK);
        tft.drawString(gpu_temperature + "`C", 222, 200, 1);

        char Cfs[] = "Drive C: Free Space";//C盘余量
        dataStart = line.indexOf(Cfs) + strlen(Cfs);
        dataEnd = line.indexOf(" GB", dataStart);
        C_usage = line.substring(dataStart, dataEnd);
        tft.setTextColor(0xA7FF, TFT_BLACK);
        tft.drawString(C_usage + "G", 126, 221, 1);

        char Brg[] = "Display Brightness Level";//亮度
        dataStart = line.indexOf(Brg) + strlen(Brg);
        dataEnd = line.indexOf("%", dataStart);
        brightness = line.substring(dataStart, dataEnd);
        tft.setTextColor(0x061F, TFT_BLACK);
        if (brightness.length() == 2) tft.drawString(brightness + " ", 222, 221, 1);
        else tft.drawString(brightness, 222, 221, 1);

        char Vol[] = "Master Volume";//音量
        dataStart = line.indexOf(Vol) + strlen(Vol);
        dataEnd = line.indexOf("%", dataStart);
        volume = line.substring(dataStart, dataEnd);
        tft.setTextColor(0x061F, TFT_BLACK);
        if (volume.length() == 2) tft.drawString(volume + " ", 282, 221, 1);
        else tft.drawString(volume, 282, 221, 1);

        tft.setTextSize(3);
      }
    }
  }
  if (Page3_flag == true) //3页
  {
    if (FillScreen_flag == false)
    {
      tft.fillScreen(TFT_BLACK);
      FillScreen_flag = true;
    }
    tft.drawString(String(posDegrees) + "` ", 150, 100, 1);
    servo1.write(posDegrees);
    delay(10);
  }
}

void click1() {
  Page1_flag = true;
  Page2_flag = false;
  Page3_flag = false;
  Page4_flag = false;
  Serial.println("MID click.");
}
void click2() {
  Page1_flag = false;
  Page2_flag = true;
  Page3_flag = false;
  Page4_flag = false;
  Serial.println("SET click.");
}
void click3() {
  Page1_flag = false;
  Page2_flag = false;
  Page3_flag = true;
  Page4_flag = false;
  Serial.println("RST click.");
}
void clickU() {
  if (posDegrees < 180) posDegrees ++;
  Serial.println("U click.");
}
void clickD() {
  if (posDegrees > 0) posDegrees --;
  Serial.println("D click.");
}
void longPressU() {
  if (posDegrees < 180) posDegrees ++;
  Serial.println("U longPress...");
}
void longPressD() {
  if (posDegrees > 0) posDegrees --;
  Serial.println("D longPress...");
}

/*******************单个汉字显示****************/
void showMyFont(int32_t x, int32_t y, const char c[3], uint32_t color) {
  for (int k = 0; k < 7; k++)// 根据字库的字数调节循环的次数
    if (hanzi[k].Index[0] == c[0] && hanzi[k].Index[1] == c[1] && hanzi[k].Index[2] == c[2])
    {
      tft.drawBitmap(x, y, hanzi[k].hz_Id, hanzi[k].hz_width, 16, color);
    }
}
/*******************整句汉字显示****************/
void showMyFonts(int32_t x, int32_t y, const char str[], uint32_t color) {
  int x0 = x;
  for (int i = 0; i < strlen(str); i += 3) {
    showMyFont(x0, y, str + i, color);
    x0 += 17;
  }
}
