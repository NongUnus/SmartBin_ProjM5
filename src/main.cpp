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
unsigned long lastDetectedTime = 0; 
const unsigned long delayBeforeClose = 3000;
void openLid() {
    lid.write(openAngle);
}
void closeLid() {
    lid.write(closedAngle);
}
void moveServo(int target) {
    int current = lid.read();
    if (current < target) {
        for (int pos = current; pos <= target; pos++) {
            lid.write(pos);
            delay(15);
        }
    } else {
        for (int pos = current; pos >= target; pos--) {
            lid.write(pos);
            delay(15);
        }
    }
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
    lid.attach(servoPin, 600, 2400);
    lid.write(closedAngle);
    delay(500);
}
void loop() {
    int distance = sensor.read();
    if (sensor.timeoutOccurred()) {
        Serial.println("TIMEOUT!");
        return;
    }
    if (distance > 0 && distance < 2000) {
        if (distance < threshold) {
            lastDetectedTime = millis();            
            if (currentStatus == false) {
                Serial.print("Distance (mm): ");
                Serial.println(distance);
                Serial.println("Action: Opening...");
                moveServo(openAngle);
                currentStatus = true;
            }
        } 
        else if (distance >= threshold && currentStatus == true) {
            if (millis() - lastDetectedTime >= delayBeforeClose) {
                Serial.println("Action: Closing...");
                moveServo(closedAngle);
                currentStatus = false;
            }
        }
    }
    delay(50); 
}