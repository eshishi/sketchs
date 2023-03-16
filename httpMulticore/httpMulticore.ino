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

const char *url = "http://192.168.137.1:8010";

using QueueType = u8_t;
boolean swDataOld = false;
boolean swData;

QueueHandle_t uploadQueue;
TaskHandle_t uploadTask;
TaskHandle_t statusLed;

bool wifiIsRunning()
{
    return wifiMulti.run() != WL_CONNECTED;
}

void setup()
{
    // TaskHandle_t wifiChecker;
    Serial.begin(115200);
    wifiMulti.addAP(SSID, WIFI_PASS);
    xTaskCreatePinnedToCore(blinkLed, "status", 4096, NULL, 3, &statusLed, 1);
    xTaskCreatePinnedToCore(wifiChecker, "wifiChecker", 4096, NULL, 2, NULL, 0);

    uploadQueue = xQueueCreate(QUEUE_LENGTH, sizeof(QueueType));
    // xTaskCreatePinnedToCore(uploader, "uploader", 8192, NULL, 1, &uploadTask, 0);
    // xTaskCreatePinnedToCore(uploader, "uploader", 8192, NULL, 2, &uploadTask, 0);

    pinMode(SW1, INPUT);
    pinMode(LED, OUTPUT);
}
void loop()
{
    swData = digitalRead(SW1);
    if (swData != swDataOld && swData)
    {
        Serial.println("pull");
        QueueType a = 0;
        xQueueSend(uploadQueue, &a, 0);
    }
    swDataOld = swData;

    delay(10);
}

void uploader(void *args)
{
    // while (wifiMulti.run() != WL_CONNECTED)
    // {
    //     delay(1);
    // }
    HTTPClient http;
    while (true)
    {
        QueueType input;
        xQueueReceive(uploadQueue, &input, portMAX_DELAY);
        Serial.println("on");
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

void blinkLed(void *args)
{
    for (auto i = true;; i = !i)
    {
        digitalWrite(LED, i);
        delay(1000);
    }
    digitalWrite(LED, HIGH);
    vTaskDelete(NULL);
}

void wifiChecker(void *args)
{
    Serial.println("[uploader] waiting wifi setup");
    while (wifiMulti.run() != WL_CONNECTED)
    {
        delay(1);
    }
    Serial.println("[uploader] wifi setup done");
    vTaskDelete(statusLed);
    digitalWrite(LED, HIGH);
    xTaskCreatePinnedToCore(uploader, "uploader", 8192, NULL, 1, &uploadTask, 0);
    xTaskCreatePinnedToCore(uploader, "uploader", 8192, NULL, 1, &uploadTask, 0);
    vTaskDelete(NULL);
}
