#ifndef __WIFI_SERVER_H_
#define __WIFI_SERVER_H_

#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "PubSubClient.h"

#define WIFI_ERR  -1
#define WIFI_OK    0
#define SERVER_ERR  -1
#define SERVER_OK    0

#define WIFI_SSID           "TP-LINK_FF67"        
#define WIFI_PWDWORD        "abc123456"         
#define SERVER_HOST         "114.132.163.111"   //mqtt服务器
#define SERVER_POST         1883              
#define MQTTSERVER_HOST     "114.132.163.111"   
#define MQTTSERVER_PORT     6002             


int8_t connectWifi(const char ssid[], const char password[]);
int8_t connectServer(String device_log_in_head);

#endif /*__WIFI_SERVER_H_*/