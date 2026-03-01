#include <Arduino.h>
#include <VL53L1X.h>
#include <Servo.h>
#include <Wire.h>
VL53L1X sensor;
Servo lid;
const int servoPin = 5;
const int openAngle = 0;
const int closedAngle = 90;
const int threshold = 250; 
bool currentStatus = false; 
const unsigned long delayBeforeClose = 6000;
unsigned long openTime = 0;
int currentAngle = closedAngle;
unsigned long detectStart = 0;
void openLid() {
    lid.write(closedAngle);
}
void closeLid() {
    lid.write(openAngle);
}
void moveServo(int target) {
     if (currentAngle < target) {
        for (int pos = currentAngle; pos <= target; pos++) {
            lid.write(pos);
            delay(15);
        }
    } else {
        for (int pos = currentAngle; pos >= target; pos--) {
            lid.write(pos);
            delay(15);
        }
    }
    currentAngle = target;
}
void setup() {
    Serial.begin(115200);
    Wire.begin();
    sensor.setTimeout(500);
    if(!sensor.init()) {
        Serial.println("Failed to detect and initialize sensor!");
    }
    sensor.setDistanceMode(VL53L1X::Short);
    sensor.setROISize(8, 8);
    sensor.setMeasurementTimingBudget(50000);
    sensor.startContinuous(50);
    lid.attach(servoPin, 600, 2400);
    lid.write(closedAngle);
    currentAngle = closedAngle;
    delay(500);
}
void loop() {
    int distance = sensor.read();
    if (sensor.timeoutOccurred()) return;
    if (distance <= 0) return;
    if (!currentStatus) {
    if (distance < threshold) {
        if (detectStart == 0) detectStart = millis();    
        if (millis() - detectStart > 150) {
            Serial.println("Opening...");
            moveServo(openAngle);
            currentStatus = true;
            openTime = millis();
            detectStart = 0;
        }
        } else {
            detectStart = 0;
        }
    }
    if (currentStatus && millis() - openTime >= delayBeforeClose) {
        Serial.println("Closing...");
        moveServo(closedAngle);
        currentStatus = false;
    }
}