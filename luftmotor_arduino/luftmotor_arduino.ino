#include "math.h"

#define TEST_SOLENOID false

int led = LED_BUILTIN;
int external_led = 7;
int valve_pin = 8;
int sensor = A0;

const float tdc_volume = 0.0000003352858347057703;
const float stroke_volume = 0.00003110176727053895;

void setup() {
  Serial.begin(9600);
  pinMode(sensor, INPUT);
  pinMode(external_led, OUTPUT);
  pinMode(valve_pin, OUTPUT);
}

void loop() {
  if (TEST_SOLENOID) {
    delay(2000);
    digitalWrite(external_led, HIGH);
    digitalWrite(valve_pin, HIGH);
    delay(5000);
    digitalWrite(external_led, LOW);
    digitalWrite(valve_pin, LOW);
  } else {
    //Serial.println(analogRead(sensor));
    short pressure = 800000;
    unsigned long start_time = micros();
    float inlet_closing = (acos((2*tdc_volume + stroke_volume - (2*(100000*tdc_volume + 100000*stroke_volume))/pressure)/stroke_volume))/(2*PI);
    unsigned long stop_time = micros();
    Serial.println(inlet_closing);
    delay(10);
  }
}
