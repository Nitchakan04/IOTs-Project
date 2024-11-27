#define BLYNK_TEMPLATE_ID "TMPL6KGtHgx8M"
#define BLYNK_TEMPLATE_NAME "luxlvl"
#define BLYNK_AUTH_TOKEN "1lKex0q-RPGf0IED_l-tBtd62KX7VnNO"

#include <WiFi.h>
#include <esp_now.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>

// Wi-Fi Credentials
// const char* ssid = "S22";
// const char* password = "nawakorn";
  const char* ssid = "PP";
  const char* password = "ppaaoo48";

// Blynk Auth Token
char auth[] = "1lKex0q-RPGf0IED_l-tBtd62KX7VnNO";  // Replace with your Blynk Auth Token
const char* GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/AKfycbwfGB9ultFj1AX1i6fI6URAjYX9Msc42i7QqhGeek7BausrYUuZOjSI7S-RaFu8hOiCvg/exec";

// Structure to hold sensor data
struct SensorData {
  int waterLevel;
  uint16_t lightIntensity;
};

static SensorData receivedData;

// ESP-NOW Callback Function
void onReceive(const uint8_t* macAddr, const uint8_t* incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData)); // Extract sensor data
  Serial.printf("Received Water Level: %d, Light Intensity: %d\n", receivedData.waterLevel, receivedData.lightIntensity);

  // Send data to Blynk
  Blynk.virtualWrite(V0, receivedData.waterLevel);       // Send Water Level to Virtual Pin V1
  Blynk.virtualWrite(V1, receivedData.lightIntensity);   // Send Light Intensity to Virtual Pin V2

}

void sendToGoogleSheets(int waterLevel, int lightIntensity) {
  HTTPClient http;

  // Create the URL with parameters
  String url = String(GOOGLE_SCRIPT_URL) + "?value1=" + String(waterLevel) + "&value2=" + String(lightIntensity);

  http.begin(url);  // Initialize HTTP request
  int httpCode = http.GET();  // Send GET request

  if (httpCode > 0) {
    Serial.printf("Data sent to Google Sheets. HTTP Response: %d\n", httpCode);
  } else {
    Serial.printf("Error sending to Google Sheets: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();  // Close connection
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");
  Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());

  // Initialize Blynk
  Blynk.begin(auth, ssid, password);
  Serial.println("Blynk initialized");

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed!");
    return;
  }
  esp_now_register_recv_cb(onReceive);
  Serial.println("Receiver ready");

  Serial.println("Ready to send data to Google Sheets.");
}

void loop() {
  Blynk.run();  // Run Blynk

  // Send data to Google Sheets
  sendToGoogleSheets(receivedData.waterLevel, receivedData.lightIntensity);

  delay(1000);

}
