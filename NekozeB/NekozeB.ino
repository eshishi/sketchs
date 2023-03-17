#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define LED_PIN 12
#define DC_PIN 5
#define STATUS_LED 14
#define RESET_SW 26

#define NOMAL 0            // 正常
#define LITTLE_HUNCHBACK 1 // ちょい猫背
#define HUNCHBACK 2        // 猫背

#define QUEUE_LENGTH 20

#define SSID "maruyama"
#define WIFI_PASS "marufuck"
using QueueType = u8_t;

Adafruit_MPU6050 mpu;
WiFiMulti wifiMulti;
TaskHandle_t statusLed;
QueueHandle_t uploadQueue;

TaskHandle_t uploaders[2];

#define url_temp "https://nekoze-9ccfb-default-rtdb.asia-southeast1.firebasedatabase.app/test/%s/data.json"
const char *uid = "-NQcUOvNQPtZmec-M65n";
#define data_temp R"({"state": %d, "timestamp": {".sv": "timestamp"}})"
bool wifiIsRunning()
{
  return wifiMulti.run() == WL_CONNECTED;
}
/*
範囲出力関数
*/
void showRange()
{
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange())
  {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange())
  {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth())
  {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
}

void setup(void)
{
  pinMode(DC_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(RESET_SW, INPUT);
  wifiMulti.addAP(SSID, WIFI_PASS);
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  xTaskCreatePinnedToCore(blinkLed, "status", 4096, NULL, 3, &statusLed, 1);
  xTaskCreatePinnedToCore(wifiChecker, "wifiChecker", 4096, NULL, 2, NULL, 0);
  Serial.println("Adafruit MPU6050 test!");
  uploadQueue = xQueueCreate(QUEUE_LENGTH, sizeof(QueueType));

  // Try to initialize!
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (!mpu.begin())
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  showRange();

  Serial.println("");
  delay(100);
}

/*
現在の姿勢を判定する
*/
QueueType getState(sensors_event_t accel_event)
{

  if (accel_event.acceleration.x <= 2)
  {
    return NOMAL;
  }
  else if (accel_event.acceleration.x < 4)
  {
    return LITTLE_HUNCHBACK;
  }
  else
  {
    return HUNCHBACK;
  }
}

/*
姿勢の状態からLED, ブザーを操作する
*/
void setState(QueueType state)
{
  switch (state)
  {
  case 0:
    digitalWrite(DC_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    break;
  case LITTLE_HUNCHBACK:
    digitalWrite(DC_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
    break;
  case HUNCHBACK:
    digitalWrite(DC_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    break;
  }
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
    unsigned int size = (sizeof(url_temp) + sizeof(uid));
    char url[size];
    sprintf(url, url_temp, uid);
    Serial.printf("[%s] url = %s\n", pcTaskGetName(NULL), url);

    http.begin(url);
    size = (sizeof(data_temp) + 1);
    char data[size];
    sprintf(data, data_temp, input);
    Serial.printf("[%s] data = %s\n", pcTaskGetName(NULL), data);
    auto resCode = http.POST(data);
    if (resCode < 0)
    {
      Serial.printf("[%s][error] %s\n", pcTaskGetName(NULL), http.errorToString(resCode).c_str());
    }
    else
    {
      Serial.println(resCode);
    }
    http.end();
    delay(1);
  }
}

void loop()
{
  sensors_event_t accel_event, gyro, temp;
  mpu.getEvent(&accel_event, &gyro, &temp);

  /* Print out the values */
  Serial.printf("Acceleration X: %f, Y: %f, Z: %f m/s^2\n", accel_event.acceleration.x, accel_event.acceleration.y, accel_event.acceleration.z);
  Serial.printf("Rotation X: %f, Y: %f, Z: %f \n", gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
  Serial.printf("Temperature: %f degC\n", temp.temperature);

  auto state = getState(accel_event);
  setState(state);
  Serial.println(state);
  /* Get new sensor events with the readings */
  if (wifiIsRunning())
  {
    xQueueSend(uploadQueue, &state, 0);
  }
  Serial.println("----");
  delay(1000);
}
void blinkLed(void *args)
{
  for (auto i = true;; i = !i)
  {
    digitalWrite(STATUS_LED, i);
    delay(1000);
  }
}

void wifiChecker(void *args)
{
  Serial.printf("[%s] waiting wifi setup\n", pcTaskGetName(NULL));
  while (!wifiIsRunning())
  {
    delay(1);
  }
  Serial.printf("[%s] wifi setup done\n", pcTaskGetName(NULL));
  vTaskDelete(statusLed);
  digitalWrite(STATUS_LED, HIGH);
  xTaskCreatePinnedToCore(uploader, "uploader0", 8192, uploaders[0], 1, NULL, 0);
  xTaskCreatePinnedToCore(uploader, "uploader1", 8192, uploaders[1], 1, NULL, 0);
  xTaskCreatePinnedToCore(wifiWatchdog, "wifiWatchdog", 4096, NULL, 2, NULL, 0);
  vTaskDelete(NULL);
}

void wifiWatchdog(void *args)
{
  while (wifiIsRunning())
  {
    delay(1);
  }

  vTaskDelete(uploaders[0]);
  vTaskDelete(uploaders[1]);
  xTaskCreatePinnedToCore(wifiChecker, "wifiChecker", 4096, NULL, 2, NULL, 0);
  vTaskDelete(NULL);
}
