#include "math.h"

#define TEST_SOLENOID false
#define TEST_SENSOR false
#define TEST_MOTOR true
#define TEST_LIGHT true

int led = LED_BUILTIN;
int external_led = 8;
int valve_pin = 7;
int sensor = A7;
int sensor_2 = A6;
int sensor_3 = A5;
int sensor_4 = A4;
int light_sensor = A0;

const double tdc_volume = 3.352858347057703e-7;
const double stroke_volume = 3.110176727053895e-5;


int motorPin1 = 9; // Blue - 28BYJ48 pin 1
int motorPin2 = 10; // Pink - 28BYJ48 pin 2
int motorPin3 = 11; // Yellow - 28BYJ48 pin 3
int motorPin4 = 12; // Orange - 28BYJ48 pin 4
int motorSpeed = 780; //variable to set stepper speed
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

void setup() {
    Serial.begin(9600);
    pinMode(sensor, INPUT);
    pinMode(sensor_2, INPUT);
    pinMode(sensor_3, INPUT);
    pinMode(sensor_4, INPUT);
    pinMode(external_led, OUTPUT);
    pinMode(valve_pin, OUTPUT);
    pinMode(light_sensor, INPUT);
    //analogReadResolution(13);
    
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
}

void loop() {
  if (TEST_SOLENOID) {
    delay(4000);
    digitalWrite(external_led, HIGH);
    digitalWrite(valve_pin, HIGH);
    delay(4000);
    digitalWrite(external_led, LOW);
    digitalWrite(valve_pin, LOW);
  }
  if (TEST_SENSOR){
    int val_1 = analogRead(sensor);
    int val_2 = analogRead(sensor_2);
    int val_3 = analogRead(sensor_3);
    int val_4 = analogRead(sensor_4);
    Serial.print(micros());
    Serial.print("\t");
    Serial.print(val_1);
    Serial.print("\t");
    Serial.print(val_2);
    Serial.print("\t");
    Serial.print(val_3);
    Serial.print("\t");
    Serial.println(val_4);
    if (!TEST_MOTOR) {
      delay(10);
    }
  }
  if (TEST_MOTOR){
    clockwise();
  }
  if (TEST_LIGHT) {
    int light_val = analogRead(light_sensor);
    Serial.print(micros());
    Serial.print("\t");
    Serial.println(light_val);
    if (!TEST_MOTOR) {
      delay(10);
    }
  }
  /*
  double pressure = 2e5;
  unsigned long start_time = micros();
  double inlet_closing = acos((2*tdc_volume + stroke_volume - (2e5*(tdc_volume + stroke_volume))/pressure)/stroke_volume);
  unsigned long stop_time = micros();
  //Serial.println(inlet_closing, 5);
  delay(10);
  */
}

void anticlockwise() {
  for(int i = 0; i < 8; i++) {
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}
void clockwise() {
  for(int i = 7; i >= 0; i--) {
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}
void setOutput(int out){
  digitalWrite(motorPin1, bitRead(lookup[out], 0));
  digitalWrite(motorPin2, bitRead(lookup[out], 1));
  digitalWrite(motorPin3, bitRead(lookup[out], 2));
  digitalWrite(motorPin4, bitRead(lookup[out], 3));
}
