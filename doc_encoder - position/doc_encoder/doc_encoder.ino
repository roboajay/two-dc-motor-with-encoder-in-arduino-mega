#include "TimerOne.h"
#include<LiquidCrystal.h>
LiquidCrystal lcd(13,12,11,10,9,8);//thiet lap chan cho LCD 
 // LiquidCrystal Object(RS, E, D4, D5, D6, D7);
int ledPin = 3;
int vong;
float tocdo;
int dem;
void encoder()
{ dem++;
  if(dem==200)
 {digitalWrite(ledPin, HIGH);
  vong++;
  dem=0;}
}
void thuc()
{ tocdo=vong*2;
  vong=0;
 Serial.print("toc do dong co la: "); 
 Serial.print(tocdo);
 Serial.print(" vong/giay "); 
 Serial.print("\t"); 
}
void setup()
{  lcd.begin(16,2); //thiet lap loai LCD su dung la 16 cot va 2 dong 
   Serial.begin(9600);
   pinMode(ledPin, OUTPUT);
   pinMode(2, INPUT_PULLUP); // sử dụng điện trở kéo lên cho chân số 2, ngắt 0
   attachInterrupt(0, encoder, HIGH);
   ///////
  Timer1.initialize(500000); // khởi tạo timer 1 đến 1 giây
  Timer1.attachInterrupt(thuc); // khai báo ngắt timer 1
//// cau H
  pinMode(6,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(4,OUTPUT);
}

void loop()
{ digitalWrite(ledPin, LOW);
analogWrite(5,0);
digitalWrite(7,HIGH);
digitalWrite(4,LOW);
analogWrite(6,125);
delay(10000);
for(int i=0;i<100;i++)
{analogWrite(6,125+i);delay(50);}
analogWrite(6,225);
Serial.print("toc do dong co max la: "); 
Serial.print(tocdo);
delay(5000);
for(int i=0;i<255;i++)
{analogWrite(6,255-i);delay(50);}
delay(5000);
}
