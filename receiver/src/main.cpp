#include <esp_now.h>
#include <WiFi.h>

// โครงสร้างข้อมูลที่รับ
struct SensorData {
  int waterLevel;
  uint16_t lightIntensity;
};

// Callback เมื่อได้รับข้อมูล
void onReceive(const uint8_t *macAddr, const uint8_t *incomingData, int len) {
  SensorData receivedData;
  memcpy(&receivedData, incomingData, sizeof(receivedData)); // ดึงข้อมูลจาก incomingData

  // แสดงค่าที่ได้รับใน Serial Monitor
  Serial.print("Received Water Level: ");
  Serial.println(receivedData.waterLevel);
  Serial.print("Received Light Intensity: ");
  Serial.println(receivedData.lightIntensity);
}

void setup() {
  Serial.begin(115200);

  // ตั้งค่า Wi-Fi โหมด Station
  WiFi.mode(WIFI_STA);

  // เริ่มต้น ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed!");
    return;
  }

  // ลงทะเบียน callback เมื่อได้รับข้อมูล
  esp_now_register_recv_cb(onReceive);
  Serial.println("Receiver ready");
}

void loop() {
  // รอรับข้อมูล
}
