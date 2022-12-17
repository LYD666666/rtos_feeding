#ifndef _CONFIG_H_
#define _CONFIG_H_

//投食
#define touch1_debug 
//#define touch2_debug  

//投喂开关
#define cast_open  1
#define cast_close 2

//标志位开关
#define flag_close 0
#define flag_open  1

#define stoptransport  "{\"motorswitch\":\"off\"}" 
#define stopfeeding  "{\"planid\":\"1\"}" 
#define rst  "{\"planid\":\"1\",\"feedstate\":\"ready\"}" 


//主题
#define topic_ota        "/ota/123"
#define topic_situate    "/situate"               /*发送设备状态*/
#define topic_check      "/check"                 /*送料复位*/

#define mqtt_username  "cwl"
#define mqtt_password  "19260817"
#define mqtt_broker    "114.132.163.111"
#define mqtt_port  1883


#endif // !_CONFIG_H_