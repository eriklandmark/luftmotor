#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
#include <math.h>

#define TEST_SOLENOID true
#define TEST_BUTTON false
#define TEST_MOTOR false
#define TEST_LIGHT false
#define TEST_SENSOR false
#define TEST_VEL false

MS5803 sensor(ADDRESS_HIGH);

#define TDC_INTERRUPT_PIN 2
#define BDC_INTERRUPT_PIN 3
#define VEL_PIN 8

#define INLET_VALVE_PIN 4
#define OUTLET_VALVE_PIN 5

#define START_MOTOR_BTN 6
#define STOP_MOTOR_BTN 7

#define LIGHT_SENSOR_1 A0
#define LIGHT_SENSOR_2 A1
#define LIGHT_SENSOR_THRESHOLD A2

#define VEL_SENSOR A3
#define VEL_SENSOR_THESHOLD A4
#define VEL_SENSOR_DIG 8

int motorPin1 = 9; // Blue - 28BYJ48 pin 1
int motorPin2 = 10; // Pink - 28BYJ48 pin 2
int motorPin3 = 11; // Yellow - 28BYJ48 pin 3
int motorPin4 = 12; // Orange - 28BYJ48 pin 4
int motorSpeed = 800; //variable to set stepper speed
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

void setup() {
    
    pinMode(INLET_VALVE_PIN, OUTPUT);
    pinMode(OUTLET_VALVE_PIN, OUTPUT);
    pinMode(TDC_INTERRUPT_PIN, INPUT);
    pinMode(BDC_INTERRUPT_PIN, INPUT);
    pinMode(START_MOTOR_BTN, INPUT);
    pinMode(STOP_MOTOR_BTN, INPUT);

    
    if (!TEST_SENSOR) {
      pinMode(A4, INPUT);
    }
    
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);

    if (TEST_SENSOR) {
      Wire.begin();
      sensor.reset();
      sensor.begin();    
    }
    Serial.begin(9600);
    
}

void loop() {
  if (TEST_SOLENOID) {
    delay(1000);
    digitalWrite(INLET_VALVE_PIN, HIGH);
    digitalWrite(OUTLET_VALVE_PIN, HIGH);
    delay(1000);
    digitalWrite(INLET_VALVE_PIN, LOW);
    digitalWrite(OUTLET_VALVE_PIN, LOW);
  }
  if (TEST_MOTOR){
    clockwise();
  }
  if (TEST_LIGHT) {
    int light_sensor_val_1 = analogRead(LIGHT_SENSOR_1);
    int light_sensor_val_2 = analogRead(LIGHT_SENSOR_2);
    int voltage_val = analogRead(LIGHT_SENSOR_THRESHOLD);
    int output_val_1 = digitalRead(TDC_INTERRUPT_PIN);
    int output_val_2 = digitalRead(BDC_INTERRUPT_PIN);
    //Serial.print(micros());
    //Serial.print("\t");
    Serial.print(voltage_val);
    Serial.print("\t");
    Serial.print(light_sensor_val_1);
    Serial.print("\t");
    Serial.print(light_sensor_val_2);
    Serial.print("\t");
    Serial.print(output_val_1*30);
    Serial.print("\t");
    Serial.println(output_val_2*30);
    if (!TEST_MOTOR) {
      delayMicroseconds(1000);
    }
  }
  if (TEST_SENSOR) {
    double pressure = sensor.getPressure(ADC_2048)*100;
    //Serial.print(micros());
    //Serial.print("\t");
    Serial.println(pressure);
  }
  if (TEST_BUTTON) {
    int output_val_1 = digitalRead(START_MOTOR_BTN);
    int output_val_2 = digitalRead(STOP_MOTOR_BTN);
    Serial.print(output_val_1);
    Serial.print("\t");
    Serial.println(output_val_2);
    delay(500);
  }
  if (TEST_VEL) {
    int analog_val = analogRead(VEL_SENSOR);
    int digital_val = digitalRead(VEL_SENSOR_DIG);
    if (!TEST_SENSOR) {
      Serial.print(analogRead(VEL_SENSOR_THESHOLD));
      Serial.print("\t");
    }
    Serial.print(analog_val);
    Serial.print("\t");
    Serial.println(digital_val*30);
    if (!TEST_MOTOR) {
      delay(1);
    }
  }
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
