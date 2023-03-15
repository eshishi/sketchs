#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
int LED_PIN = 12;
int DC_PIN = 5;

Adafruit_MPU6050 mpu;

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
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
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
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
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

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
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

  Serial.println("");
  delay(100);
}

int get_state()
{
}

void loop()
{
  /* Get new sensor events with the readings */
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  /* Print out the values */
  Serial.printf("Acceleration X: %f, Y: %f, Z: %f m/s^2\n", accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);
  Serial.printf("Rotation X: %f, Y: %f, Z: %f \n", gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
  Serial.printf("Temperature: %f degC\n", temp.temperature);

  if (accel.acceleration.x <= 2)
  {
    Serial.println("0");
    digitalWrite(DC_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
  else if (accel.acceleration.x < 4)
  {
    Serial.println("1");
    digitalWrite(DC_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    Serial.println("2");
    digitalWrite(DC_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  }
  Serial.println("----");

  delay(1000);
}
