#include <Arduino.h>
#include <VL53L1X.h>
#include <Servo.h>
#include <Wire.h>
VL53L1X sensor;
Servo lid;
const int servoPin = 5;
const int openAngle = 90;
const int closedAngle = 0;
const int threshold = 100;
bool currentStatus = false;
void openLid() {
    lid.write(openAngle);
}
void closeLid() {
    lid.write(closedAngle);
}
void setup() {
    Serial.begin(115200);
    Wire.begin();
    sensor.setTimeout(500);
    if(!sensor.init()) {
        Serial.println("Failed to detect and initialize sensor!");
    }
    sensor.setDistanceMode(VL53L1X::Short);
    sensor.setMeasurementTimingBudget(50000);
    sensor.startContinuous(50);
    lid.attach(servoPin);
    lid.write(closedAngle);
    closeLid();
}
void loop() {
    int distance = sensor.read();
    if (sensor.timeoutOccurred()) {
        Serial.println("TIMEOUT!");
        return;
    }
    if (distance > 0 && distance < 2000) {
        Serial.print("Distance (mm): ");
        Serial.println(distance);
        if(distance < threshold && currentStatus == false) {
            lid.write(openAngle);
            currentStatus = true;
            Serial.println("Action: Opening...");
            delay(500); 
        }else if(distance >= threshold && currentStatus == true) {
            lid.write(closedAngle);
            currentStatus = false;
            Serial.println("Action: Closing...");
            delay(500);
        }
    }
    delay(100);
}