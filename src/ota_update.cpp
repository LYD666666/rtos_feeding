#include <ota_update.h>
#include <config.h>

const int LED_Pin = 2;              //单片机LED引脚值，D2是NodeMcu引脚命名方式，其他esp8266型号将D2改为自己的引脚
String otaclient = "http://api.rslly.top/api/v2/micro/";        //bin文件存放地址
#ifdef touch1_debug
String binclient = "2ba525b9-6665-41b2-bd09-786306fdefb1.bin";  //bin文件名
#endif

#ifdef touch2_debug
String binclient = "e25caf50-819c-404e-bb77-21fecd33a393.bin";  //bin文件名
#endif

//打开灯泡
void turnOnLed(){
  Serial.println("Turn ON");
  digitalWrite(LED_Pin,LOW);
}
//关闭灯泡
void turnOffLed(){
  Serial.println("Turn OFF");
    digitalWrite(LED_Pin,HIGH);
}


//当升级开始时，打印日志
void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

//当升级结束时，打印日志
void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

//当升级中，打印日志
void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

//当升级失败时，打印日志
void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

/*
  *初始化和服务器建立连接 :style="value.online?'订阅设备在线':'无订阅设备'" 
*/
void startMQTTClient(){
  // connecting to a mqtt broker
  WiFiClient OtaClient;
  PubSubClient MQTTclient(OtaClient);
  MQTTclient.setServer(mqtt_broker, mqtt_port);   //MQTT服务器连接函数（服务器IP，端口号）
  MQTTclient.setKeepAlive(600);
  MQTTclient.setCallback(callback);               //设定回调方式，当ESP32收到订阅消息时会调用此方法
  while (!MQTTclient.connected())
  {
    #ifdef touch1_debug
    String client_id = "motor_feeding";
    #endif

    #ifdef touch2_debug
    String client_id = "motor_feeding_1";
    #endif
    //client_id += String(WiFi.macAddress());

    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (MQTTclient.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("Failed connected to server:");
      Serial.print(MQTTclient.state());
      vTaskDelay(100/portTICK_PERIOD_MS); //需要更改，最后用RTOS
    }
  }
}

/**
 * 固件升级函数
 * 在需要升级的地方，加上这个函数即可，例如setup中加的updateBin(); 
 * 原理：通过http请求获取远程固件，实现升级
 */
void updateBin(){
  Serial.println("start update");    
  WiFiClient UpdateClient;
  String ota_url = otaclient + binclient;

  httpUpdate.onStart(update_started);//当升级开始时
  httpUpdate.onEnd(update_finished);//当升级结束时
  httpUpdate.onProgress(update_progress);//当升级中
  httpUpdate.onError(update_error);//当升级失败时
  Serial.println(ota_url);

  t_httpUpdate_return ret = httpUpdate.update(UpdateClient, ota_url);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.println("[update] Update failed.");
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[update] Update no Update.");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("[update] Update ok.");
      break;

    default: 
      Serial.println("[update] Update .....");
      break;
  }
}

