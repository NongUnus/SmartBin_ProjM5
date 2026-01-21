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
void openLid() {
    lid.write(openAngle);
}
void closeLid() {
    lid.write(closedAngle);
}
void setup() {
    Serial.begin(115200);
    Wire.begin();
    if(!sensor.init()) {
        Serial.println("Failed to detect and initialize sensor!");
        while(1);
    }
    sensor.setDistanceMode(VL53L1X::Short);
    sensor.setMeasurementTimingBudget(50000);
    sensor.startContinuous(50);
    lid.attach(servoPin);
    closeLid();
}
void loop() {
    int distance = sensor.read();
    if (distance == 0 || distance > 4000) {
        Serial.println("Sensor Error / Out of Range");
    }else {
        Serial.print("Distance (mm): ");
        Serial.println(distance);
        if(distance < threshold) {
            openLid();
        }else {
            closeLid();
        }
    }
    delay(50);
}