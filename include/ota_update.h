#ifndef  __ota__
#define  __ota__
#include <WiFi.h>
#include <HTTPUpdate.h>
#include <PubSubClient.h>

//连接WIFI
void doWiFiTick();
void startSTA();

//MQTT初始化连接
void updateBin();
void startMQTTClient();
void callback(char *topic_pub, byte *payload, unsigned int length);

//led控制函数，具体函数内容见下方
void turnOnLed();
void turnOffLed();

#endif