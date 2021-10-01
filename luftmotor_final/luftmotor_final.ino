#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
#include <math.h>
#include <TimerOne.h>

#define PRINT_MICROS false

MS5803 sensor(ADDRESS_HIGH);

// Constants
const double tdc_volume = 3.352858347057703e-7;
const double stroke_volume = 3.110176727053895e-5;
const double ambient_preassure = 1.0183748e5;


// Meassured value
short rotation_velocity = 160;
double pressure = 2e5;


unsigned long finish_cykel_micro = 0;
bool cycle_started = false;


void setup() {
    Timer1.initialize(6e7/rotation_velocity);
    Timer1.attachInterrupt(timeInterrupt);
    
    Wire.begin();
    Serial.begin(9600);
    
    sensor.reset();
    sensor.begin();
}

void timeInterrupt(void) {
    const unsigned long current_micro = micros();
    
    if (latest_micro > 0) {
      Serial.println(current_micro - latest_micro);
    }

    
    latest_micro = current_micro;
    latest
}

void loop() {
    const unsigned long current_micros = micros();
    unsigned long cycle_started_copy;  // holds a copy of the blinkCount
    noInterrupts();
    cycle_started_copy = cycle_started;
    interrupts();
    
    if (cycle_started_copy) {
        const unsigned long current_micro = micros();
    
        if (latest_micro > 0) {
          Serial.println(current_micro - latest_micro);
        }
      
        latest_micro = current_micro;

        if (current_micors => finish_micro) {
          
        }
        
    } else {
        delay(10);
        //pressure = sensor.getPressure(ADC_2048)*100;
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
