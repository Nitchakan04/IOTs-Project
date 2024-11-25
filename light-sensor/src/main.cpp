#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "../lib/BH1750FVI/src/BH1750FVI.h"

// Replace with your WiFi credentials
const char *ssid = "Nawa";
const char *password = "12345678";

// Create an AsyncWebServer object on port 80
AsyncWebServer server(80);

// BH1750 Sensor Object
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

// LED Pin
const int ledPin = 23;

// Function to get light intensity
uint16_t getLightIntensity()
{
    uint16_t lux = LightSensor.GetLightIntensity();
    if (lux == 0)
    {
        Serial.println("Error: Light intensity is 0. Check sensor.");
    }
    return lux;
}

// Handle the root webpage
void handleRoot(AsyncWebServerRequest *request)
{
    File file = SPIFFS.open("/index.html", "r");
    if (!file)
    {
        Serial.println("Cannot open file");
        request->send(500, "text/plain", "Cannot open file");
        return;
    }
    request->send(SPIFFS, "/index.html", String(), false);
    file.close();
}

// Handle light intensity endpoint
void handleLightIntensity(AsyncWebServerRequest *request)
{
    uint16_t lux = getLightIntensity();            // Get current light intensity
    request->send(200, "text/plain", String(lux)); // Send lux value as response
}

void setup()
{
    Serial.begin(115200);

    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }

    // Initialize LED pin
    pinMode(ledPin, OUTPUT);

    // Initialize BH1750 sensor
    LightSensor.begin();
    Serial.println("BH1750 sensor initialized.");

    // Check if sensor is responding
    uint16_t lux = LightSensor.GetLightIntensity();
    if (lux == 0)
    {
        Serial.println("Warning: Sensor is not responding. Check connection.");
    }

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting To WiFi Network .");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Serve the HTML page
    server.on("/", HTTP_GET, handleRoot);

    // Serve light intensity readings
    server.on("/light", HTTP_GET, handleLightIntensity);

    // Start the server
    server.begin();
}

void loop()
{
    uint16_t lux = getLightIntensity();

    // Control LED based on light intensity
    if (lux < 400)
    {
        digitalWrite(ledPin, HIGH); // Turn LED ON
        Serial.println("LED ON");
    }
    else
    {
        digitalWrite(ledPin, LOW); // Turn LED OFF
        Serial.println("LED OFF");
    }

    // Print light intensity to serial monitor
    Serial.print("Light Intensity: ");
    Serial.print(lux);
    Serial.println(" lux");

    delay(1000);
}