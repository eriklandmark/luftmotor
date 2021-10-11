#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
#include <math.h>
#include <TimerOne.h>

#define USE_PRESSURE_SENSOR false

#define PRINT_MICROS false

#define TDC_INTERRUPT_PIN 2
#define BDC_INTERRUPT_PIN 3

#define INLET_VALVE_PIN 4
#define OUTLET_VALVE_PIN 5

MS5803 sensor(ADDRESS_HIGH);

// Constants
const double tdc_volume = 3.349518347057704e-7;
const double stroke_volume = 3.110176727053895e-05;
const double ambient_preassure = 1.0183748e5;
const double pre_solenoid_starting_time = 7.0/960.0;
const double solenoid_act_time = 21.0/960.0;
const double cylinder_raduis = 15.0e-3;
const double leverarm_length = 22.0e-3;
const double connecting_rod_length = 90.0e-3;
unsigned long cykel_offset = 0;

// Meassured value
double rotation_velocity = 300.0;
double pressure = 0;

//Flags
bool cycle_started = false;
bool opened_outlet = false;
bool closed_outlet = false;
bool opened_inlet = false;
bool closed_inlet = false;

//Other
int num_pressure_memory = 8;
double pressure_memory[8] = {};
byte pressure_memory_index = 0;

unsigned long latest_micros = 0;
unsigned long finish_cykel_micros = 0;
unsigned long close_outlet_micros = 0;
unsigned long open_inlet_micros = 0;
unsigned long close_inlet_micros = 0;

void setup() {
    pinMode(INLET_VALVE_PIN, INPUT);
    pinMode(OUTLET_VALVE_PIN, INPUT);
    pinMode(INLET_VALVE_PIN, OUTPUT);
    pinMode(OUTLET_VALVE_PIN, OUTPUT);

    Timer1.initialize(6e7/rotation_velocity);
    Timer1.attachInterrupt(timeInterrupt);
    
    Wire.begin();
    Serial.begin(9600);

    if (USE_PRESSURE_SENSOR) {
        sensor.reset();
        sensor.begin();
    } else {
        pressure = 2e5;
    }
}

void timeInterrupt(void) {
    const unsigned long current_micros = micros();
    
    cycle_started = true;

    double sigma = tdc_volume*(pressure - ambient_preassure) + PI*pow(cylinder_raduis,2)*(leverarm_length*(pressure - 2*ambient_preassure) + connecting_rod_length*pressure);
    double start_closing_inlet_percent = (acos(PI*pressure*pow(cylinder_raduis,2)*(pow(leverarm_length,2) - pow(connecting_rod_length,2) + pow(sigma,2)/(pow(pressure,2)*pow(cylinder_raduis,4)*pow(PI,2)))/(2*leverarm_length*sigma)) - PI*rotation_velocity*(solenoid_act_time + pre_solenoid_starting_time)/30.0)/(2.0*PI);
    close_outlet_micros = current_micros + cykel_offset;
    open_inlet_micros = close_outlet_micros + (pre_solenoid_starting_time + solenoid_act_time)*1e6;
    close_inlet_micros = open_inlet_micros + (start_closing_inlet_percent*6e7/rotation_velocity);
    opened_outlet = false;
    closed_outlet = false;
    opened_inlet = false;
    closed_inlet = false;
    Serial.print(micros() - current_micros);
    Serial.print("\t");
    Serial.println(start_closing_inlet_percent,8);
}

void loop() {
    const unsigned long current_micros = micros();
    unsigned long cycle_started_copy;
    noInterrupts();
    cycle_started_copy = cycle_started;
    interrupts();
    
    if (cycle_started_copy) {
        if (current_micros >= close_outlet_micros && !closed_outlet) {
            digitalWrite(OUTLET_VALVE_PIN, false);
            closed_outlet = true;
        } else if (current_micros >= open_inlet_micros && !opened_inlet) {
            digitalWrite(INLET_VALVE_PIN, true);
            opened_inlet = true;
        } else if (current_micros >= close_inlet_micros && !closed_inlet) {
            digitalWrite(INLET_VALVE_PIN, false);
            closed_inlet = true;
            cycle_started = false;
        } else {

        }
    } else {
        if (USE_PRESSURE_SENSOR) {
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
}
