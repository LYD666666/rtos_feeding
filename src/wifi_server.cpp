#include "wifi_server.h"
#include <ArduinoJson.h>

WiFiClient feedbox_client;  //创建一个WIFI连接客户端

/*
 * 连接WiFi
 * 错误返回WIFI_ERR，成功返回WIFI_OK
 */
int8_t connectWifi(const char ssid[], const char password[])
{
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  //查找WiFi
  Serial.printf("Scan wifi %s ...\r\n", ssid);
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n + 1; i++)
  {
    Serial.println(WiFi.SSID(i));
    //查找要连接的WiFi，退出查找
    if (WiFi.SSID(i) == (String)ssid)
    {
      break;
    }
    if (i == n)
    {
      Serial.println("Couldn't find your wifi!");
      return WIFI_ERR;
    }
  }
  //连接WiFi
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  unsigned long current_time = micros();
  while (!WiFi.isConnected())
  {
    //10s连接超时
    if ((micros() - current_time) >= 10 * 1000 * 1000)
    {
      Serial.println("WiFi connect error!");
      return WIFI_ERR;
    }
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //设置断电自动重连
  WiFi.setAutoReconnect(true);

  return WIFI_OK;
}

/*
 * 连接服务器
 * 错误返回SERVER_ERR，成功返回SERVER_OK
 */
int8_t connectServer(String device_log_in_head)
{
  unsigned long current_time = micros();
  while (!feedbox_client.connect(SERVER_HOST, SERVER_POST)) //客户端连接到指定的产品
  {
    delay(1000);
    //10s连接超时
    if ((micros() - current_time) >= 10 * 1000)
    {
      Serial.println("Connect Remote Server Error!");
      return SERVER_ERR;
    }
  }
  //连上服务器，发送登陆信息
  feedbox_client.print(device_log_in_head);
  //等待应答信息
  unsigned long timeout = millis();
  while (feedbox_client.available() == 0)
  {
    if (millis() - timeout > 5000)
    {
      Serial.println("Client Timeout!");
      feedbox_client.stop();
      return SERVER_ERR;
    }
  }
  //收到received表示登陆服务器，可能需要更改"received"
  if (feedbox_client.find("received"))
  {
    Serial.println("Connect to Server!");
    //清空TCP接收缓冲区
    while (feedbox_client.read() >= 0)
      ;
    return SERVER_OK;
  }
  else
  {
    Serial.println("Couldn't Connect to Server!");
    return SERVER_ERR;
  }
}
