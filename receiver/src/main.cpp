#include <esp_now.h>
#include <WiFi.h>

// Callback when data is received
void onReceive(const uint8_t *macAddr, const uint8_t *incomingData, int len) {
  int receivedVal;
  memcpy(&receivedVal, incomingData, sizeof(receivedVal)); // ดึงค่าจากข้อมูลที่ได้รับ
  Serial.print("Received Value: ");
  Serial.println(receivedVal);  // แสดงค่าที่ได้รับใน Serial Monitor
}

void setup() {
  Serial.begin(115200);

  // Initialize Wi-Fi in station mode
  WiFi.mode(WIFI_STA);
  Serial.println("Wi-Fi initialized in station mode");

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed!");
    return;
  }
  Serial.println("ESP-NOW initialized");

  // Register the receive callback
  esp_now_register_recv_cb(onReceive);
}

void loop() {
  // Do nothing. The callback handles all the work.
}
