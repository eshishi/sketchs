/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/
#define LED_BUILTIN 26
#define INPUT_0 14

RTC_DATA_ATTR int bootCount = 0;
boolean inputDetaOld;
boolean inputData;
boolean farstData;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  bootCount++;
  Serial.printf("起動回数: %d ", bootCount);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INPUT_0, INPUT);  
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  farstData = digitalRead(INPUT_0);
}

// the loop function runs over and over again forever
void loop() {
  inputData = digitalRead(INPUT_0);
  if (farstData){
      if (inputData){
        return;
      }
      else{
        farstData = false;
      }
      
    }
  if (!inputData && inputDetaOld){
       esp_deep_sleep_start();
  }
  inputDetaOld = inputData;
  delay(10);  
}
