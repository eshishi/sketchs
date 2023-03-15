#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define SW1 14
#define LED 25

#define QUEUE_LENGTH 20
#define SSID "maruyama"
#define WIFI_PASS "marufuck"
// #define QUEUE_ITEM_SIZE 1

WiFiMulti wifiMulti;

const char *url = "http://192.168.0.51:8010";

using QueueType = u8_t;
boolean swDataOld = false;
boolean swData;

QueueHandle_t uploadQueue;
TaskHandle_t uploadTask;
void setup()
{
    Serial.begin(115200);
    wifiMulti.addAP(SSID, WIFI_PASS);
    uploadQueue = xQueueCreate(QUEUE_LENGTH, sizeof(QueueType));

    pinMode(swData, INPUT);
    pinMode(LED, OUTPUT);
}
void loop() {}

void uploader()
{
    Serial.println("[uploader] waiting wifi setup");
    while (wifiMulti.run() != WL_CONNECTED)
    {
        delay(10);
    }
    digitalWrite(LED, HIGH);
    Serial.println("[uploader] wifi setup done");
    HTTPClient http;
    while (true)
    {
        QueueType input;
        xQueueReceive(uploadQueue, &input, portMAX_DELAY);
        http.begin(url);
        auto resCode = http.POST(std::to_string(input).c_str());
        if (resCode < 0)
        {
            Serial.printf("[error] %s", http.errorToString(resCode).c_str());
        }
        else
        {
            Serial.println(resCode);
        }
        http.end();
        delay(1);
    }
}
