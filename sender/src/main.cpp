#include <esp_now.h>
#include <WiFi.h>

int sensor = 34; // พินที่ใช้สำหรับเซ็นเซอร์น้ำ
int val = 0;

// MAC address ของอุปกรณ์ที่รับ (เปลี่ยนตามจริง)
uint8_t receiverMAC[] = {0x88, 0x13, 0xBF, 0x0D, 0x8F, 0x3C}; // ตัวอย่าง

// Callback เมื่อส่งข้อมูลเสร็จ
void onSend(const uint8_t *macAddr, esp_now_send_status_t status)
{
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup()
{
  Serial.begin(115200);

  // ตั้งค่า Wi-Fi โหมด Station
  WiFi.mode(WIFI_STA);

  // เริ่มต้น ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW initialization failed!");
    return;
  }

  // เพิ่มข้อมูล peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverMAC, 6); // เพิ่ม MAC address ของตัวรับ
  peerInfo.channel = 0;                       // ใช้ channel เดิม
  peerInfo.encrypt = false;                   // ไม่เข้ารหัส

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("Peer added successfully");
}

void loop()
{
  // อ่านค่าจาก water sensor
  val = analogRead(sensor);

  // ส่งค่าไปยังตัวรับ
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&val, sizeof(val));

  if (result == ESP_OK)
  {
    Serial.print("Value sent: ");
    Serial.println(val);
  }
  else
  {
    Serial.println("Error sending value");
  }
  delay(2000); // ส่งทุก 2 วินาที
}
