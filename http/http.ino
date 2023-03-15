
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>

#define USE_SERIAL Serial
#define SW1 14
#define LED 25

WiFiMulti wifiMulti;

const char *url_base = "https://nekoze-9ccfb-default-rtdb.asia-southeast1.firebasedatabase.app/test";
const char *url_suffix = ".json";
const char *timeStampData = R"({".sv":"timestamp"})";

boolean swDataOld = false;
boolean swData;

void setup()
{

    USE_SERIAL.begin(115200);
    wifiMulti.addAP("maruyama", "marufuck");

    pinMode(swData, INPUT);
    pinMode(LED, OUTPUT);

    for (uint8_t t = 4; t > 0; t--)
    {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
}

void loop()
{
    // wait for WiFi connection
    swData = digitalRead(SW1);
    if ((wifiMulti.run() != WL_CONNECTED))
    {
        return;
    }
    digitalWrite(LED, HIGH);

    if (swData != swDataOld && swData)
    {
        Serial.println("pull");
        HTTPClient http;
        String url = String(url_base);
        // url += url_suffix;
        http.begin(url + url_suffix);
        http.setReuse(true);
        http.addHeader("Content-Type", "application/json");
        auto resCode = http.POST(R"({"state":3})");
        Serial.println(resCode);
        if (resCode < 0)
        {
            Serial.printf("[error] %s", http.errorToString(resCode).c_str());
            http.end();
        }
        else
        {
            DynamicJsonDocument doc(1024);
            // deserializeJson(doc, http.getString());
            // const char *name = doc["name"];
            // Serial.printf("%s", name);
            auto ret = http.getString();
            // Serial.println(ret);
            deserializeJson(doc, ret.c_str());
            const char *name = doc["name"];
            Serial.printf("%s", name);
            http.end();
            http.begin(url + "/" + name + "/timestamp" + url_suffix);
            resCode = http.PUT(R"({".sv":"timestamp"})");
            if (resCode < 0)
            {
                Serial.println(http.errorToString(resCode));
            }
            else
            {
                Serial.println(resCode);
                Serial.println(http.getString());
            }
        }
    }

    // HTTPClient http;

    //        USE_SERIAL.print("[HTTP] begin...\n");
    // configure traged server and url
    // http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
    // http.begin(url); // HTTP

    //        USE_SERIAL.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    // int httpCode = http.GET();

    // httpCode will be negative on error
    // if (httpCode > 0)
    // {
    //     // HTTP header has been send and Server response header has been handled
    //     USE_SERIAL.printf("%d\n", httpCode);

    //     // file found at server
    //     if (httpCode == HTTP_CODE_OK)
    //     {
    //         String payload = http.getString();
    //         USE_SERIAL.println(payload);
    //     }
    // }
    // else
    // {
    //     USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    // }

    // http.end();

    delay(10);
    swDataOld = swData;
}
