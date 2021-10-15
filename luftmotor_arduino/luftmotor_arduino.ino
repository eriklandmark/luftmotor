#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
#include <math.h>

#define TEST_SOLENOID false
#define TEST_MOTOR false
#define TEST_LIGHT true
#define TEST_SENSOR false

MS5803 sensor(ADDRESS_HIGH);

int led = LED_BUILTIN;
int external_led = 8;
int valve_pin = 7;
int light_sensor_1 = A1;
int light_sensor_2 = A2;
int sensor_offset_pin = A0;
int sensor_digital_pin_1 = 2;
int sensor_digital_pin_2 = 3;

int motorPin1 = 9; // Blue - 28BYJ48 pin 1
int motorPin2 = 10; // Pink - 28BYJ48 pin 2
int motorPin3 = 11; // Yellow - 28BYJ48 pin 3
int motorPin4 = 12; // Orange - 28BYJ48 pin 4
int motorSpeed = 800; //variable to set stepper speed
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

void setup() {
    Wire.begin();
    Serial.begin(9600);
    pinMode(external_led, OUTPUT);
    pinMode(valve_pin, OUTPUT);
    pinMode(light_sensor_1, INPUT);
    pinMode(light_sensor_2, INPUT);
    pinMode(sensor_digital_pin_1, INPUT);
    pinMode(sensor_digital_pin_2, INPUT);
    pinMode(sensor_offset_pin, INPUT);
    
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);

    sensor.reset();
    sensor.begin();
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
  if (TEST_MOTOR){
    clockwise();
  }
  if (TEST_LIGHT) {
    int light_sensor_val_1 = analogRead(light_sensor_1);
    int light_sensor_val_2 = analogRead(light_sensor_2);
    int voltage_val = analogRead(sensor_offset_pin);
    int output_val_1 = digitalRead(sensor_digital_pin_1);
    int output_val_2 = digitalRead(sensor_digital_pin_2);
    Serial.print(micros());
    Serial.print("\t");
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
      delay(1);
    }
  }
  if (TEST_SENSOR) {
    double pressure = sensor.getPressure(ADC_2048)*100;
    //Serial.print(micros());
    //Serial.print("\t");
    Serial.println(pressure);
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
