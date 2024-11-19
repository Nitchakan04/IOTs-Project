#include <Arduino.h>
int sensor = 34; // พินที่ใช้สำหรับเซ็นเซอร์
int val = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  val = analogRead(sensor); // อ่านค่าจากเซ็นเซอร์
  Serial.println(val);      // แสดงค่าที่อ่านได้ทาง Serial Monitor
  delay(1000);               // รอ 100 มิลลิวินาที
}