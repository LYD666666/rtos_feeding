#include <Arduino.h>
#include <ArduinoJson.h>
#include "ota_update.h"
#include "motor.h"
#include "wifi_server.h"
#include "digital.h"
#include "config.h"
#include "math.h"
#include "logic.h"
#include <stdio.h>


/******数字量传输*******/
extern char Json_judge[]; //信息模板
extern char datastausR [];
extern char datastausB [];
extern char datastausF [];
extern char cleanswitch1 [];  
extern char CAST_NUM[];
/**********************/

//回调函数
extern int Reset_flag;
extern char msgJson[75];                       //要发送的json格式的数据
//是否结束一次投喂_状态标志位
extern int Frist_flag ;
extern int return_flag;
//是否开始清洁
extern long clean_flag;
//达成总投喂量_状态标志位
extern int list_flag;
extern unsigned short json_len;            // json长度
extern float  weight_Notyet;   //待投量
extern long weight_revise; //想修改的投喂量
extern long weight;          //传感器测试到的量
extern long weight_real;
extern long weight_now;
extern long fin;
extern int thread_name;
extern int connectNum;

extern const char *topic_pub;
extern const char *topic_sub;
extern const char *topic_clean;
extern float weight_chu;
extern long weight_cheng;

//实例对象
WiFiClient espClient;
PubSubClient client(espClient);

//  1   硬件初始化
void HarewardInit(void)
{
  // Set software serial baud to 115200;
  Serial.begin(115200); //串口
  myserial(9600); //软串口

  // connecting to  WiFi network
  if(connectWifi(WIFI_SSID, WIFI_PWDWORD) == WIFI_ERR)  ESP.restart();

  // connecting to  mqtt broker
  startMQTTClient();

  // 推杆、阀门复位，都是关闭状态,发送ready，准备称重
  motor_init();
  digitalWrite(valve,LOW);
  Set_Pwm(1000);
  delay(4000);

  client.subscribe(topic_pub);                 //连接MQTT服务器后订阅主题
  client.subscribe(topic_clean);                 //连接MQTT服务器后订阅主题
  client.subscribe(topic_check);                 //连接MQTT服务器后订阅主题
  client.subscribe(topic_ota);
  client.publish(topic_situate,situate_ready);
}

//回调函数
void callback(char *topic_pub, byte *payload, unsigned int length)
{
  char json[200];
  char *parameter = (char *)malloc(length); 
  const char *cleanswitch = (char *)malloc(length);
  const char *OTA = (char *)malloc(length);

  /* 解析JSON */
  StaticJsonDocument<200> jsonBuffer2; //声明一个JsonDocument对象，长度200
  
  Serial.print("Message arrived in topic_pub: ");
  Serial.println(topic_pub);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    parameter[i] = (char)payload[i];
    Serial.print(parameter[i]);
  } 
  
  // 反序列化JSON
  DeserializationError error = deserializeJson(jsonBuffer2, parameter,length);
     if (error) 
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    JsonObject Json_judge = jsonBuffer2.as<JsonObject>();

    //复位指令判断  主题：/check
      if(strncmp(parameter,"inspection",strlen("inspection"))==0)
    {
       if(Json_judge[String("inspection")] == "on")
      {
        Reset_flag=1;
      }
    }

    // 投料重量赋值   主题：/casting_publish
   if(strncmp(parameter,"castweight",strlen("castweight"))==0)
  {
    if(weight == 0){
       weight_revise = Json_judge[String("castweight")];              // 读取整形数据,总重量
       Frist_flag = 1;
      }
      if(Json_judge[String("castweight")] != 0)
      {
        thread_name = cast_open;
      }
      if(Json_judge[String("castweight")] == "0")
      {
       thread_name = cast_close;
       clean_flag = 0;
       return_flag = 1;
      }
  }

  //投喂关闭指令判断  主题：/casting_publish
  if(strncmp(parameter,"castbegin",strlen("castbegin"))== flag_close)
  {
   if(Json_judge[String("castbegin")] == "0")
      {
       thread_name = cast_close;
       clean_flag = 0;
       return_flag = 1;
      }
  }

  //清洁指令赋值（外部判断：防止关闭投喂进程指令和赋值冲突） 主题：/clean_casting
    if(strncmp(parameter,"cleanswitch",strlen("cleanswitch"))==0)
      {
        cleanswitch = Json_judge["cleanswitch"];
      }
  
  //投喂进程关闭下清洁指令判断   主题：/clean_casting
  if(thread_name == cast_close)  
  {
    if(strncmp(parameter,"cleanswitch",strlen("cleanswitch"))==0)
    {
      Serial.printf("cleanswitch:");
      Serial.printf(cleanswitch);
      if(strncmp(cleanswitch,"on",strlen(cleanswitch))== 0)
      {
        clean_flag = 1;
        client.publish(topic_clean,clean_busy);
        }
      else if(strncmp(cleanswitch,"off",strlen(cleanswitch))== 0)
      {
        clean_flag = 0;
        }
    }
  }

  //OTA升级json判断  主题：/ota/123
  if(strncmp(parameter,"fileName",strlen("fileName"))==0){
    OTA = Json_judge["fileName"];
    
    if(strstr(OTA,"bin") != 0)
    {
      updateBin();//执行升级函数
      //client.publish(topic_ota,situate_busy);
      }
  }

  Serial.print("now:\r\n");
  Serial.println(weight);
  Serial.println(Json_judge);
  free(parameter);
  Serial.println("--------------------------");
}



/*----------------------------程序主体-------------------------------*/

//初始化
void setup()
{
  //联网和硬件设备初始化
  HarewardInit();
  Serial.print("init OK\r\n");
}

//循环
void loop()
{
  client.loop();
  //复位反馈ready
  if(Reset_flag == flag_open)
    {
      client.publish(topic_situate,situate_ready);
      Reset_flag = flag_close;
    }

  //
  if(thread_name == cast_open)
  {
    if(Frist_flag == flag_open) //第一次进入循环要打开阀门
    {
      client.publish(topic_situate,situate_busy);
      feeding1();
      Frist_flag = flag_close;
    }
       weight_real= Readweight();
       delay(500);
      Serial.printf("ti\r\n");
      if(weight_real > weight_cheng)   //如果达到阈值就进入投喂程序接口
      {
           tasktomotor();
           delay(500);
       }
    }

  else if(thread_name == cast_close) //如果云平台发送off就会开（在关闭状态无法运行，避免冲突）
  {
    digitalWrite(valve,LOW);
    if(clean_flag == flag_open)
    {
      digitalWrite(valve,HIGH);
      Set_Pwm(-1000); //下滑推杆开始清料
      delay(8000);  //等待漏料
      Serial.printf("漏料结束");
      Set_Pwm(1000);
      delay(8000);
      Serial.printf("清洁结束");
      clean_flag = flag_close;
    }
  }
}

/*----------------------------程序EXIT-------------------------------*/