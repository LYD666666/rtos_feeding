#ifndef DIGITAL_H_
#define DIGITAL_H_
#include <Arduino.h>
#include <SoftwareSerial.h>

#define RxD 18
#define TxD 19    

float Readweight(void); //称重函数
void verify(void);      //校正函数
void myserial(int baud);//软串口

#endif /*DIGITAL_H_*/