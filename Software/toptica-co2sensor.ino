#include <Wire.h>

#include <SparkFun_SCD30_Arduino_Library.h>
#include <I2C.h>
#include <II2C.h>
#include <MPU6050.h>
#include <MultiFuncShield.h>

#include <Wire.h>

//object to interface with Sensor/use the functions in the SCD30 library
SCD30 airSensor;

//variables, which later contain the diffrent measurement values
int co2Value;
float humidity;
float temperature;

//choose the CO2-value at which the device starts to buzz and blink
const int ALARMVALUE = 1000;

//variable, which controls the buzzer through 0 and 1
char buzzer;

//by default the device starts with measuring CO2 ^= mode 0
char mode = 0;

//variable for input through buttons
byte btn;

//define how long the buzzer should be interupted with Button 3
long buzzerInterupt = 60000;

//variables used to perform delays without stopping the programm
long timerBuzzer;
int timerBuzzerDelay = 1000;
int timerCalDelay = 1000;
long timerStartBuzzer;
long timerStartBuzzerDelay;
long timerStartCal;

//function predefinition to claim some storage space beforehand
void displayCo2();
void displayTemperature();
void displayHumidity();
void newCalibrationSCD30();

void setup()
{

    int baudrate = 9600;
    Serial.begin(baudrate);

    //initializations of the Multifunctionboard and the I2C-Arduino Port
    Wire.begin();
    MFS.initialize();

    //bootingsequence
    startingSequence();

    //checking the sensor connection
    if (airSensor.begin() == false)
    {
        Serial.println("Error. SCD30 not detected. Please check the Connection. Press RESET to proceed.");
    }
}

void loop()
{

    //switching modes through pressing the first button
    btn = MFS.getButton();

    //By pressing the first button, one can choose between the three diffrent modes
    if (btn == BUTTON_1_PRESSED)
    {
        mode++;
        if (mode == 4)
        {
            mode = 0;
        }

        //defining the different modes names
        switch (mode)
        {
        case 0:
            MFS.write("Co2");
            delay(500);
            break;
        case 1:
            MFS.write("TENN");
            MFS.writeLeds(LED_ALL, OFF);
            delay(500);
            break;
        case 2:
            MFS.write("HUNN");
            delay(500);
            break;
        }
    }

    //through long-pressing the second button the device can be calibrated
    if (btn == BUTTON_2_LONG_RELEASE)
    {
        newCalibrationSCD30();
    }

    //defining the different modes: measuring co2, temperature or humidity
    switch (mode)
    {
    case 0:
        displayCo2();
        break;
    case 1:
        displayTemperature();
        break;
    case 2:
        displayHumidity();
        break;
    }
}

//*************************************************************************************************************************************************************

//the starting sequence used in void setup()
void startingSequence()
{

    MFS.beep();

    MFS.write("top");
    MFS.writeLeds(LED_4, ON);
    delay(1500);
    MFS.write("tica");
    MFS.writeLeds(LED_3, ON);
    delay(1500);
    MFS.write("C02");
    MFS.writeLeds(LED_2, ON);
    delay(1500);
    MFS.write("SENS");
    MFS.writeLeds(LED_1, ON);
    delay(2000);

    MFS.writeLeds(LED_ALL, OFF);
    MFS.beep();
    delay(250);
    MFS.writeLeds(LED_ALL, ON);
    MFS.beep();
    delay(250);
    MFS.writeLeds(LED_ALL, OFF);
    delay(100);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

//The devices main function: measuring CO2
void displayCo2()
{

    //the SCD30 only provides new data every 2 seconds. This if-clause prevents the usage of unessecary processing power
    if (airSensor.dataAvailable())
    {
        Serial.print("CO2: ");
        Serial.println(airSensor.getCO2());

        //the co2 value is transfered into the variable and shown on the display
        co2Value = airSensor.getCO2();
        MFS.write(co2Value);
    }

    //checking whether the co2-value has surpassed the threshold
    if (co2Value >= ALARMVALUE)
    {
        buzzer = 1;
    }
    else
    {
        buzzer = 0;
    }

    //function for buzzering the buzzer
    if (buzzer == 1)
    {

        MFS.writeLeds(LED_ALL, ON);
        MFS.blinkLeds(LED_ALL, ON);

        //by pressing the third button, while the co2-value has surpassed the threshold, one can pause the buzzering
        if (btn == BUTTON_3_PRESSED)
        {
            timerStartBuzzer = millis();
            //in order to prevent, that the buzzer won't work for the duration of buzzerInterupt when the device ist newly booted, the variable timerStartBuzzer is just now defined
            timerBuzzer = buzzerInterupt;
            Serial.print("Timerstart Pause: ");
            Serial.println(timerStartBuzzer);
        }

        if (millis() < (timerStartBuzzer + timerBuzzer))
        {
        }
        else
        {
            //pausing the buzzer for a second without using any delay too ensure optimal button input
            if (millis() > (timerStartBuzzerDelay + timerBuzzerDelay))
            {
                MFS.beep();
                timerStartBuzzerDelay = millis();
            }
        }
    }
    else
    {
        MFS.writeLeds(LED_ALL, OFF);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

//diplaying the temperature the same way the co2-value is diplayed (without buzzer and threshold)
void displayTemperature()
{
    if (airSensor.dataAvailable())
    {
        temperature = airSensor.getTemperature(), 1;
        Serial.print("Temperatur: ");
        Serial.print(temperature);
        Serial.println("°C");
        MFS.write(temperature);
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

//diplaying the humidity the same way the co2-value is displayed (without buzzer and threshold)
void displayHumidity()
{
    if (airSensor.dataAvailable())
    {
        humidity = airSensor.getHumidity(), 1;
        Serial.print("Luftfeuchtigkeit: ");
        Serial.print(humidity);
        Serial.println("%");
        MFS.write(humidity);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//function the enforce a new calibration
void newCalibrationSCD30()
{
    MFS.writeLeds(LED_ALL, OFF);
    MFS.write("NEW");
    delay(850);
    MFS.write("CALI");
    delay(850);
    MFS.write("BRA");
    delay(850);
    MFS.write("TION");
    delay(850);
    MFS.write("EX");
    delay(850);
    MFS.write("POSE");
    delay(850);
    MFS.write("TO");
    delay(850);
    MFS.write("FRE");
    delay(850);
    MFS.write("SH");
    delay(850);
    MFS.write("AIR");
    delay(850);

    //countdown until the new calibration
    for (int i = 120; i > 0; i--)
    {
        Serial.println(i);
        MFS.write(i);
        timerStartCal = millis();
        while (millis() < (timerStartCal + timerCalDelay))
        {
        }
    }

    airSensor.setForcedRecalibrationFactor(400);
    MFS.write("COMP");
    delay(850);
    MFS.write("LETE");
    delay(850);
    Serial.println("Calibration complete");
}
