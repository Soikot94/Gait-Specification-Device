#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const int sampleSize = 10;
const float distance = 1.0; // Distance in meters
const float timeWindow = 10.0; // Time window for step time and swing time calculation in seconds

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
int stepCount = 0;
float stepLength = 0;
float stepTime = 0;
float swingTime = 0;
float cadence = 0;
float gaitSpeed = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float axg = ax / 16384.0;
  float ayg = ay / 16384.0;
  float azg = az / 16384.0;
  float aTotal = sqrt(axg * axg + ayg * ayg + azg * azg);
  
  if (aTotal > 1.2) {
    stepCount++;
  }

  currentMillis = millis();
  if ((currentMillis - previousMillis) > (timeWindow * 1000)) {
    stepLength = distance / stepCount;
    stepTime = (currentMillis - previousMillis) / (float)stepCount;
    swingTime = stepTime / 2.0;
    cadence = (float)stepCount / ((currentMillis - previousMillis) / 60000.0);
    gaitSpeed = distance / ((currentMillis - previousMillis) / 1000.0);
    stepCount = 0;
    previousMillis = currentMillis;
 // display the results in the Serial Monitor
    Serial.print("Cadence: ");
    Serial.print(cadence);
    Serial.print(" steps/min\tGait Speed: ");
    Serial.print(gaitSpeed);
    Serial.print(" m/s\tStep Length: ");
    Serial.print(stepLength);
    Serial.print(" m\tStep Time: ");
    Serial.print(stepTime);
    Serial.print(" ms\tSwing Time: ");
    Serial.print(swingTime);
    Serial.println(" ms");
  }
}

