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
const double pre_solenoid_starting_time = 7/960;
const double solenoid_act_time = 21/960;


// Meassured value
short rotation_velocity = 300;

int num_pressure_memory = 8;
double pressure_memory[8] = {};
byte pressure_memory_index = 0;
double pressure = 0;

unsigned long latest_micros = 0;
unsigned long finish_cykel_micros = 0;
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
    const unsigned long current_micros = micros();
    
    if (latest_micros > 0) {
      //Serial.println(current_micro - latest_micros);
    }
    double start_closing_inlet_percent = acos((2*tdc_volume + stroke_volume - (2*ambient_preassure*(tdc_volume + stroke_volume))/2e5)/stroke_volume)/(2*PI) - rotation_velocity*(pre_solenoid_starting_time - solenoid_act_time)/60;
    cycle_started = true;
    latest_micros = current_micros;
    finish_cykel_micros = current_micros + (start_closing_inlet_percent*6e7/rotation_velocity);
    Serial.print(current_micros);
    Serial.print("\t");
    Serial.println(pressure);
}

void loop() {
    const unsigned long current_micros = micros();
    unsigned long cycle_started_copy;
    noInterrupts();
    cycle_started_copy = cycle_started;
    interrupts();
    
    if (cycle_started_copy) {
        if (current_micros >= finish_cykel_micros) {
            cycle_started = false;
        }
    } else {
        pressure_memory[pressure_memory_index] = sensor.getPressure(ADC_2048)*100;
        pressure_memory_index++;
        if (pressure_memory_index >= num_pressure_memory) {
            pressure_memory_index = 0;
        }
        double tot_pressure_memory = 0;
        for (short i = 0; i < num_pressure_memory; i++) {
            tot_pressure_memory += pressure_memory[i];
        }
        pressure = tot_pressure_memory / (float) num_pressure_memory;
    }
}
