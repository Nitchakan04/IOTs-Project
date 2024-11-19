#include "../lib/BH1750FVI/src/BH1750FVI.h" // ระบุเส้นทางที่ถูกต้อง
#include <Arduino.h>

BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);
int ledPin = 23;
void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  LightSensor.begin();
}
void loop()
{
  uint16_t lux = LightSensor.GetLightIntensity();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lux");
  if (lux < 400)
  {                             // สามารถกำหนดค่าความสว่างตามต้องการได้
    digitalWrite(ledPin, HIGH); // สั่งให้ LED ติดสว่าง
    Serial.println("LED ON");
    Serial.println();
  }
  if (lux > 400)
  {                            // สามารถกำหนดค่าความสว่างตามต้องการได้
    digitalWrite(ledPin, LOW); // สั่งให้ LED ดับ
    Serial.println("LED OFF");
    Serial.println();
  }
  delay(1000);
}