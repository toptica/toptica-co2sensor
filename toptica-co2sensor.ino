#include <I2C.h>
#include <II2C.h>
#include <MPU6050.h>
#include <MultiFuncShield.h>
#include <SparkFun_SCD30_Arduino_Library.h>
#include <Wire.h>


SCD30       airSensor;          // Object to interface with sensor
int         co2Value = 0;       // Sensor read-out value
const int   threshold = 1000;   // Threshold value


void setup() {
    // Setup serial port
    int baudrate = 9600;
    Serial.begin(baudrate);

    // Initialize Wire and MFS
    Wire.begin();    
    MFS.initialize();

    // Write TOPTICA to MFS
    MFS.beep();
    MFS.write("top");
    delay(1500);
    MFS.write("tica");
    delay(1500);
    MFS.write("C02");
    delay(2000);
    MFS.beep();

    // Check for errors in the initialisation
    if (airSensor.begin() == false) {
        Serial.println("Error. SCD30 not detected. Please check the Connection."
            "Press RESET to proceed.");
    }
}


void loop() {
    if (airSensor.dataAvailable()) {
        // Get the data from the sensor
        co2Value = airSensor.getCO2();

        // Write it to the MFS and on serial port
        MFS.write(co2Value);
        Serial.println(co2Value);
    }

    if(co2Value >= threshold){
        MFS.beep();
    }

    delay(500);
}
