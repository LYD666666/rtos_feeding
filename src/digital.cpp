#include "digital.h"

//定义软串口
unsigned char ass[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};//发送信息
unsigned char vss[13] = {0x01, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x01,0xF3,0xAF};//校正报文
SoftwareSerial mySerial(RxD, TxD);
byte data[21];

//软串口
void myserial(int baud)
{
  mySerial.begin(baud);
}

//称重任务
float Readweight(void)
{
  mySerial.write(ass, 8);//发送信息
  delay(100);
  int i=0;
  int data_len=mySerial.available();
  while(mySerial.available())
  {
    data[i]=mySerial.read();//读原始数据
    i++;
    delay(1);
  }
 Serial.write(data,data_len);//原始数据

  if(data[3]==0xFF&&data[4]==0xFF&&data[5]==0xFF)
  {
        data[3]=data[4]=data[5]=data[6]=0x00;
    }

  float weight=data[3]*256*256+data[4]*256*256+data[5]*256+data[6];
  Serial.println("重量:");
  Serial.println(weight);
  return weight;
}

//校正函数
void verify(void)
{
  mySerial.write(vss, 13);//发送信息
  Serial.println("开始校验");
  int i=0;
  int data_len=mySerial.available();
  while(mySerial.available())
  {
    data[i]=mySerial.read();//读原始数据
    i++;
    delay(1);
    
  }
 Serial.write(data,data_len);//原始数据

  delay(500);//发送时间设定
}