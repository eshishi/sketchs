#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#define LED_PIN 12
#define DC_PIN 5

#define NOMAL 0            // 正常
#define LITTLE_HUNCHBACK 1 // ちょい猫背
#define HUNCHBACK 2        // 猫背

Adafruit_MPU6050 mpu;
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
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

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
int get_state(sensors_event_t accel_event)
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
void setState(int state)
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

void loop()
{
  sensors_event_t accel_event, gyro, temp;
  mpu.getEvent(&accel_event, &gyro, &temp);

  /* Print out the values */
  Serial.printf("Acceleration X: %f, Y: %f, Z: %f m/s^2\n", accel_event.acceleration.x, accel_event.acceleration.y, accel_event.acceleration.z);
  Serial.printf("Rotation X: %f, Y: %f, Z: %f \n", gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
  Serial.printf("Temperature: %f degC\n", temp.temperature);
  Serial.println(get_state(accel_event));
  /* Get new sensor events with the readings */
  delay(1000);
  Serial.println("----");
}
