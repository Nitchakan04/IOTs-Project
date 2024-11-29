#define BLYNK_TEMPLATE_ID "TMPL6y5pk9mCa"
#define BLYNK_TEMPLATE_NAME "Sensor Data"
#define BLYNK_AUTH_TOKEN "nPlod4N5_OwUKZmhRstb_9BvLH6OIsJQ"

#include <WiFi.h>
#include <esp_now.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>

// Wi-Fi Credentials
const char* ssid = "pp";
const char* password = "ppaaoo48";

// Blynk Auth Token
char auth[] = BLYNK_AUTH_TOKEN;  // Replace with your Blynk Auth Token
const char* GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/AKfycbytpXNTXvF9Jun-6WfARvI6Ex7ADbducSU5DFHu-zc2f3pkPs-ljn6uXCCSnb4zUUlvxg/exec";
const char* LINE_NOTIFY_TOKEN = "kZbwzGeFqTgkXFt4GDtiMR4PkxdTvTYQ3WIcdBjwX6I";

// Thermistor Constants
#define RT0 10000       // Ω
#define B 3977          // K
#define VCC 3.3         // Supply voltage
#define R 10000         // Fixed resistor value (10KΩ)
#define SENSOR_PIN 34   // Thermistor ADC pin

// TDS Sensor Constants
#define TdsSensorPin 33 // TDS ADC pin
#define VREF 3.3        // Analog reference voltage (ESP32 = 3.3V)
#define SCOUNT 10       // Number of samples for TDS

// Thermistor Variables
float RT, VR, ln, Temp, T0_temp, Read;

// TDS Sensor Variables
int analogBuffer[SCOUNT];       // Buffer for TDS ADC readings
int analogBufferTemp[SCOUNT];   // Temporary buffer for sorting
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0;

// Timing Variable
unsigned long lastMeasurementTime = 0;

// Function to calculate median
int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

// Sensor Variables

// Structure to hold sensor data
struct SensorData {
  int waterLevel;
  uint16_t lightIntensity;
};

static SensorData receivedData;
bool newDataReceived = false;

struct EspData {
  double temperature;
  double tdsValue;
};

EspData espData;

EspData calcTsdTemp() {
  Read = analogRead(SENSOR_PIN); // Read ADC value
    Read = (VCC / 4095.0) * Read;  // Convert ADC to voltage
    VR = VCC - Read;
    RT = Read / (VR / R);          // Calculate resistance

    ln = log(RT / RT0);
    Temp = (1 / ((ln / B) + (1 / T0_temp))); // Temperature in Kelvin
    Temp = Temp - 273.15;                   // Convert to Celsius

    // 2. Read and Process TDS Sensor Data
    for (int i = 0; i < SCOUNT; i++)
    {
      analogBuffer[i] = analogRead(TdsSensorPin);
    }

    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];

    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4095.0;
    float compensationCoefficient = 1.0 + 0.02 * (Temp - 25.0); // Temperature compensation
    float compensationVoltage = averageVoltage / compensationCoefficient;
    tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage
                - 255.86 * compensationVoltage * compensationVoltage
                + 857.39 * compensationVoltage) * 0.5;

    espData.tdsValue = tdsValue;
    espData.temperature = Temp;
    return espData;
}

// ESP-NOW Callback Function
void onReceive(const uint8_t* macAddr, const uint8_t* incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData)); // Extract sensor data
  // Serial.printf("Received Water Level: %d, Light Intensity: %d\n", receivedData.waterLevel, receivedData.lightIntensity);

  // Send data to Blynk
  espData = calcTsdTemp();
  Blynk.virtualWrite(V2, espData.temperature);
  Blynk.virtualWrite(V3, espData.tdsValue);
  Blynk.virtualWrite(V0, receivedData.waterLevel);       // Send Water Level to Virtual Pin V1
  Blynk.virtualWrite(V1, receivedData.lightIntensity);   // Send Light Intensity to Virtual Pin V2
  Serial.printf("Temperature: %.2f C, TDS: %.2f ppm, Water Level: %d, Light Intensity: %d\n", espData.temperature, espData.tdsValue, receivedData.waterLevel, receivedData.lightIntensity);
}

void sendToGoogleSheets(int waterLevel, int lightIntensity, float temperature, float tdsValue) {
  HTTPClient http;

  // Create the URL with parameters
  String url = String(GOOGLE_SCRIPT_URL) + "?value1=" + String(waterLevel) +
               "&value2=" + String(lightIntensity) +
               "&value3=" + String(temperature, 2) +
               "&value4=" + String(tdsValue, 2);

  http.begin(url);  // Initialize HTTP request
  int httpCode = http.GET();  // Send GET request

  if (httpCode > 0) {
    Serial.printf("Data sent to Google Sheets. HTTP Response: %d\n", httpCode);
  } else {
    Serial.printf("Error sending to Google Sheets: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();  // Close connection
}

void sendLineNotify(String message) {
  HTTPClient http;

  // Line Notify API endpoint
  String url = "https://notify-api.line.me/api/notify";

  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + String(LINE_NOTIFY_TOKEN));

  // Send POST request with the message
  String payload = "message=" + message;
  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.printf("Line Notify Response: %d, %s\n", httpCode, http.getString().c_str());
  } else {
    Serial.printf("Line Notify Error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();  // Close connection
}

void setup() {
  Serial.begin(115200);

  pinMode(SENSOR_PIN, INPUT);
  pinMode(TdsSensorPin, INPUT);
  T0_temp = 25 + 273.15; // Reference temperature in Kelvin

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

  Serial.println("Ready to send data.");
}

void loop() {
  Blynk.run();  // Ensure Blynk updates happen in real time

  // Get the current time
  static unsigned long lastGoogleSheetUpdate = 0;  // Tracks the last time data was sent to Google Sheets
  static unsigned long lastLineNotify = 0;         // Tracks the last time Line Notify was sent
  unsigned long currentMillis = millis();

  // Send data to Google Sheets every 5000 milliseconds (5 seconds)
  if (currentMillis - lastGoogleSheetUpdate >= 5000) {
    lastGoogleSheetUpdate = currentMillis;

    // Send data to Google Sheets
    sendToGoogleSheets(receivedData.waterLevel, receivedData.lightIntensity, espData.temperature, espData.tdsValue);
  }

  // Check if thresholds are exceeded for Line Notify
  if ((receivedData.lightIntensity > 1000 || espData.tdsValue > 500 || espData.temperature > 45) &&
      (currentMillis - lastLineNotify >= 20000)) {  // Avoid spamming notifications, limit to 1 per minute
    lastLineNotify = currentMillis;

    String message = "Warning!\n";
    if (espData.temperature > 45) {
      message += "Temperature: " + String(espData.temperature, 2) + "°C\n";
    }
    if (espData.tdsValue > 500) {
      message += "TDS: " + String(espData.tdsValue, 2) + " ppm\n";
    }
    if (receivedData.lightIntensity > 1000) {
      message += "Light Intensity: " + String(receivedData.lightIntensity) + " lux\n";
    }
    sendLineNotify(message);  // Send the notification
    Serial.println(message);
  }
}
