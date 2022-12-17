#include <logic.h>

#ifdef touch1_debug
  const char *topic_pub = "/casting_publish";         /*向云平台定阅*/
  const char *topic_sub = "/casting_subscribe";      /*向云平台发送*/
  const char *topic_clean = "/clean_casting";
  float weight_chu = 120;                           //拟合直线
  long weight_cheng = 8800;                           //拟合直线
#endif

#ifdef touch2_debug
  const char *topic_pub = "/casting_publish_1";         /*向云平台定阅*/
  const char *topic_sub = "/casting_subscribe_1";      /*向云平台发送*/
  const char *topic_clean = "/clean_casting_1";
  float weight_chu = 1985;                           //拟合直线
  long weight_cheng = 139000;                           //拟合直线
#endif

/******数字量传输*******/
char Json_judge[] = "{\"castweight\":%ld}"; //信息模板
char datastausR [] = "{\"castweight\":%ld,\"caststaus\":\"ready\"}";
char datastausB [] = "{\"castweight\":%ld,\"caststaus\":\"busy\"}";
char datastausF [] = "{\"castweight\":%ld,\"caststaus\":\"finish\"}";
char cleanswitch1 [] = "{\"cleanswitch\":%s}";  
char CAST_NUM[] = "{\"castnum\":%d}";
/**********************/

//回调函数
int Reset_flag = 1;
char msgJson[75];                       //要发送的json格式的数据
//是否结束一次投喂_状态标志位
int Frist_flag ;
int return_flag = 0;
//是否开始清洁
long clean_flag = 0;
//投喂数量
int cast_num = 0;
//达成总投喂量_状态标志位
int list_flag = 0;
unsigned short json_len = 4;            // json长度
float  weight_Notyet;   //待投量
long weight_revise; //想修改的投喂量
long weight;          //传感器测试到的量
long weight_real;
long weight_now;
long fin = 70;
int thread_name = cast_close;
int connectNum = 0;

//实例对象
WiFiClient wifiClient;
PubSubClient ex_client(wifiClient);

//阀门打开，开始称重并发送busy
void feeding1()
{
   digitalWrite(valve,HIGH);
   //delay(3000);
   snprintf(msgJson, 80, datastausB, weight); //将称重数据套入dataWeight模板中, 生成的字符串传给msgJson5
   ex_client.publish(topic_situate,situate_busy);
   ex_client.publish(topic_sub,msgJson);
}

// 6 判断投喂是否最后一次
int whenisfinish ()
{
    if (weight_revise<=0)
    {
            Reset_flag = flag_close;
            weight = 0;   //数据清空
            Frist_flag = flag_open;
            list_flag = flag_open;
            thread_name = cast_close;  //off
            
            digitalWrite(valve,LOW);
            delay(1000);
      }
      else delay(50);

      return 0;
  }
  
// 7 投料和复位重量
int ResetWeight()
{
      ex_client.loop();
      if(return_flag == 1)
      {
        return_flag = 0;
        return 0;
      }
      Set_Pwm(-1000); //下滑推杆开始投料
      delay(7000);  //等待漏料
      Serial.printf("下料1");

      if(list_flag == 1)
      {
        ex_client.publish(topic_situate,situate_finish);
        delay(30);
        list_flag = 0;
        }

    //关闭
      Set_Pwm(1000);
      delay(7000);
      Serial.printf("下料2");
  

    if(Frist_flag == 0)
    {
      digitalWrite(valve,HIGH); 
      delay(5000);
      ex_client.loop();
    }
  
    return 0;
  }


// 获取投喂真实量
int tasktomotor(void)
{
     //打开
      //断开NC,关阀门
      ex_client.loop();
      if(return_flag == 1)
      {
        return_flag = 0;
        return 0;
      }
      digitalWrite(valve,LOW); 
      delay(5000);
      ex_client.loop();
      if(return_flag == flag_open)
      {
        return_flag = flag_close;
        return 0;
      }

      // //这时的count为真实量 //因为阀门关上并稳定了，不会有料的进来，设置的值一般在一半也不会挤压 
      weight_now = weight_real/weight_chu;
      // weight_revise = weight_revise - weight_now;  //总量-真实量=下次待投量

      weight_revise--;
      Serial.print("1_revise_weight:");
      Serial.println(weight_revise);
      Serial.print("weight_now:");
      Serial.println(weight_now);

      if(weight_now > 10*weight_real)
      {
          weight_now = 0;
          ex_client.publish(topic_sub, "error"); 
      }
        snprintf(msgJson, 80, datastausB, weight_now); //将称重数据套入dataWeight模板中, 生成的字符串传给msgJson
        json_len = strlen(msgJson);               // msgJson的长度
        ex_client.publish(topic_sub, msgJson); 
        
        cast_num++;
        snprintf(msgJson, 80, CAST_NUM, cast_num); //将称重数据套入dataWeight模板中, 生成的字符串传给msgJson
        json_len = strlen(msgJson);               // msgJson的长度
        ex_client.publish(topic_sub, msgJson); 

      //verify();
      ex_client.loop();
      if(return_flag == 1)
      {
        return_flag = 0;
        return 0;
      }
      whenisfinish();
      ResetWeight(); //投料和复位重量
      return 0;
  }