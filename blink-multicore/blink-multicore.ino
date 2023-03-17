#define INCLUDE_eTaskGetState 1
#define SW1 14
#define LED 25
boolean swDataOld = false;
boolean swData;
TaskHandle_t blinkTask = NULL;

bool isTaskRunning()
{
    if (blinkTask == NULL)
    {
        return false;
    }
    switch (eTaskGetState(blinkTask))
    {
    case eRunning:
        Serial.println("eRunning");
        break;
    case eReady:
        Serial.println("eReady");
        break;
    case eBlocked:
        Serial.println("eBlocked");
        break;
    case eSuspended:
        Serial.println("eSuspended");
        break;
    case eDeleted:
        Serial.println("eDeleted");
        break;
    case eInvalid:
        Serial.println("eInvalid");
        break;
    }
    return eTaskGetState(blinkTask) == eRunning || eTaskGetState(blinkTask) == eBlocked;
}
int a = 0;
void setup()
{
    Serial.begin(115200);
    pinMode(SW1, INPUT);
    pinMode(LED, OUTPUT);
    // digitalWrite(LED, HIGH);
}

void loop()
{
    swData = digitalRead(SW1);
    // Serial.printf("%d, %d\n", swData, swDataOld);
    if (swData != swDataOld && swData)
    {
        a += 1;
        a %= 3;
        Serial.println(a);
        switch (a)
        {
        case 0:
            if (isTaskRunning())
            {

                vTaskDelete(blinkTask);
            }
            digitalWrite(LED, LOW);
            break;
        case 1:
            if (isTaskRunning())
            {

                vTaskDelete(blinkTask);
            }
            digitalWrite(LED, HIGH);
            break;
        case 2:
            xTaskCreatePinnedToCore(blink, "", 4096, NULL, 1, &blinkTask, 1);
            break;

        default:
            break;
        }
    }
    swDataOld = swData;
    delay(10);
}

void blink(void *a)
{
    while (1)
    {
        digitalWrite(LED, HIGH);
        delay(100);
        digitalWrite(LED, LOW);
        delay(100);
    }
}
