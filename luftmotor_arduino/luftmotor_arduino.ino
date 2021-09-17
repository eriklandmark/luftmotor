#include "math.h"

#define TEST_SOLENOID false

int led = LED_BUILTIN;
int external_led = 7;
int valve_pin = 8;
int sensor = A0;

const double tdc_volume = 3.352858347057703e-7;
const double stroke_volume = 3.110176727053895e-5;

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
    Serial.println(analogRead(sensor));
    double pressure = 2e5;
    unsigned long start_time = micros();
    double inlet_closing = acos((2*tdc_volume + stroke_volume - (2e5*(tdc_volume + stroke_volume))/pressure)/stroke_volume);
    unsigned long stop_time = micros();
    //Serial.println(inlet_closing, 5);
    delay(10);
  }
}
