// Basic demo for accelerometer readings from Adafruit MPU6050
#include <SPI.h>
#include <SD.h>
byte timerRunning;
byte timerRunning1;
#define MS_PER_SEC 1000
unsigned long stance;
unsigned long stanceStop;
unsigned long durationstance;
unsigned long swingStart;
unsigned long swingStop;
unsigned long durationswing;
unsigned long swing;
#define LOOP_STATE_STOPPED 0
#define LOOP_STATE_STARTED 1
File myFile;
const int chipSelect = 4;
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ezButton.h>
int steps = 0; 

float totaltime = 0;
int fsrReading;
int fsrPin;
int distance = 5;
float gaitspeed = 0;
int r = 0;
float ax, ay, az;
float last_az = 0;
int time, last_time = 0;
float cadence = 0;
Adafruit_MPU6050 mpu;
const int buttonPin = 2; 

const int ledPin =  8;
const int postponeLed = 6;
const int errorLed = 7;       
int buttonState = 0;         
ezButton button(buttonPin);


void setup(void) {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Wire.begin();
  mpu.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  //pinMode(buttonPin,INPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(postponeLed, OUTPUT);
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);
  button.setDebounceTime(10);
}
void SDcard(){
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
   
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

}

void defineerr() {
  
  if (!mpu.begin() && !SD.begin(4)) {
    Serial.println("Failed to find Sensor and SD");
    digitalWrite(errorLed, HIGH);  
    digitalWrite(ledPin, LOW);    
    delay(1000);
    digitalWrite(postponeLed, LOW);
    while (1) {
      delay(10);
    }
  }
  else if (!mpu.begin() && SD.begin(4)) {
    Serial.println("Failed to find Sensor");
    digitalWrite(errorLed, HIGH);   
                         
    digitalWrite(ledPin, LOW);    
    delay(1000);
    digitalWrite(postponeLed, LOW);
    while (1) {
      delay(10);
    }
  }
  else if (mpu.begin() && !SD.begin(4)) {
    Serial.println("Failed to find SD");
    digitalWrite(errorLed, HIGH);   /
    
    digitalWrite(ledPin, LOW);    
    delay(1000);
    digitalWrite(postponeLed, LOW);
    while (1) {
      delay(10);
    }
  }
   else {
    Serial.println("Normal condition");
    digitalWrite(errorLed, LOW);   
    digitalWrite(ledPin, HIGH);   
    digitalWrite(postponeLed, LOW);
    
  }

}
void counters() {
  
  sensors_event_t acc, g, temp;
  mpu.getEvent(&acc, &g, &temp);

 if( acc.acceleration.z <= -5.5 && acc.acceleration.x == 8 && timerRunning == 0  ){
       timerRunning == 1;
        Serial.println(acc.acceleration.z);
        Serial.println(acc.acceleration.x);
        Serial.println(acc.acceleration.y);
      }
      if(acc.acceleration.z >= 3 && acc.acceleration.x != 8 && timerRunning ==1 ){
         timerRunning =0;
             delay(800);
     steps = steps + 1; 
  Serial.print("Steps: ");
  Serial.print(steps);
  Serial.println(" ");
  myFile.print("Steps: ");
  myFile.print(steps);
  myFile.println(" ");
  delay(1);
}
}
void detect(){
  sensors_event_t acc, g, temp;
  mpu.getEvent(&acc, &g, &temp);
  time = millis();
  float delta_t = (time - last_time) / 1000.0;
  float delta_az = az - last_az;
  float acc_z = sqrt(acc.acceleration.x*acc.acceleration.x + acc.acceleration.y*acc.acceleration.y);
  float acc_mag = sqrt(acc.acceleration.x*acc.acceleration.x  + acc.acceleration.y*acc.acceleration.y  + acc.acceleration.z*acc.acceleration.z );
  float angle1 = atan2(acc_z,acc.acceleration.x)*180/PI;
  float angle = atan2(acc_z, acc.acceleration.z) * 180 / PI;
  last_az = acc.acceleration.z;
  last_time = time;
   Serial.println(angle);
  Serial.println(acc.acceleration.x);

  if ( angle > 100 && timerRunning ==0 && acc_mag > 9.4 ){ 
    timerRunning = 1;
    swingStart = millis();
    Serial.print("swingStart: ");
    Serial.print(swingStart);
    Serial.println(" ");
  }
 
  if(angle < 100 && timerRunning ==0 && acc_mag > 9.4){ 
    timerRunning = 0;
    swing = millis();
    Serial.print("swing: ");
    Serial.print(swing);
   Serial.println(" ");
  }
 
  if(angle <80 && timerRunning ==1 && acc_mag > 9.4){ 
    timerRunning = 0; 
    swingStop = millis();
    Serial.print("swingStop: ");
    Serial.print(swingStop);
    Serial.println(" ");
    durationswing = swingStop - swingStart;
    Serial.print("durationSwing: ");
    Serial.println(durationswing/1000.00);
    Serial.print(" seconds ");
    Serial.println(" ");
    myFile.print("durationSwing: ");
    myFile.println(durationswing/1000.00);
    myFile.print(" seconds ");
    myFile.println(" ");
    float Cadance = 60/(durationswing/1000.000);
    Serial.print("Cadance: ");
    Serial.println(Cadance);
    Serial.print("steps per min");
    Serial.print(" ");
    myFile.print("Cadance: ");
    myFile.println(Cadance);
    myFile.print("steps per min");
    myFile.print(" ");
   }
  delay(500);

}
//}
void SDcardwrite(){
  myFile = SD.open("test.txt", FILE_WRITE);

 
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.write(steps);

    myFile.close();
    Serial.println("done.");
  } else {
   
    Serial.println("error opening test.txt");
  }

}

void stancecal(){
  fsrReading = analogRead(fsrPin);
  float Voltage = fsrReading*(5.0/1023);


  if (Voltage > 1.9 && timerRunning1 == 0 ) { 
       timerRunning1 = 1;
    stance = millis();
    Serial.print("Stance Time start: ");
  Serial.println(Voltage);
    Serial.print("seconds");
    Serial.println(" ");

  }
  if (Voltage < 1.9 && timerRunning1 == 1 ) {
    timerRunning1 = 0;
    stanceStop = millis();
    Serial.print("Stance Time stop: ");
    Serial.print(stanceStop);
    Serial.print("seconds");
    Serial.println(" ");
    durationstance = stanceStop - stance;
    Serial.print("durationStance: ");
    Serial.println(durationstance/1000.00);
    myFile.print("durationStance: ");
    myFile.println(durationstance/1000.00);
    Serial.print("seconds");
    Serial.println(" ");
    myFile.print("durationStance: ");
    myFile.println(durationstance/1000);
    myFile.print("seconds");
    myFile.println(" ");

    
  delay(1000);
  }
  }

void loop() {
  button.loop();
  
  
  if (button.isPressed()){
    
    defineerr();
    digitalWrite(ledPin, HIGH);
    
    myFile = SD.open("test.txt",FILE_WRITE);
    r = 0;
    while(r == 0){  
        button.loop();
        digitalWrite(ledPin, HIGH);
        digitalWrite(postponeLed, LOW);
        counters();
        detect();
        stancecal();
        //results();
    
        if(button.isPressed()){
          r = 1;
          myFile.close();
          Serial.println("done");
          digitalWrite(ledPin, LOW);
          digitalWrite(postponeLed, HIGH);
          break;
        } 
        else {
          r = 0;
          continue;
        }
    
    }
    
  }

  
  delay(1);
}
