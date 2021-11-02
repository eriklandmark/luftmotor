#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
#include <math.h>
#include <TimerOne.h>

#define USE_PRESSURE_SENSOR false
#define USE_TIMERS false

#define PRINT_INFO false

#define TDC_INTERRUPT_PIN 2
#define BDC_INTERRUPT_PIN 3
#define VEL_PIN 8

#define INLET_VALVE_PIN 4
#define OUTLET_VALVE_PIN 5

#define START_MOTOR_BTN 6
#define STOP_MOTOR_BTN 7

MS5803 sensor(ADDRESS_HIGH);

// Constants
const float tdc_volume = 3.349518347057704e-7;
const float stroke_volume = 3.110176727053895e-05;
const float ambient_pressure = 1e5; //1.0183748e5;
const float pre_solenoid_starting_time = 7.0/960.0;
const float solenoid_act_time = 23.0/960.0;
const float cylinder_raduis = 15.0e-3;
const float leverarm_length = 22.0e-3;
const float connecting_rod_length = 90.0e-3;

const float tdc_inlet_offset_angle = 70; 
const float tdc_outlet_offset_angle =  0;
const float bdc_cycle_offset_angle = 0;

// Meassured value
float rotation_velocity = 0.0;
volatile unsigned long last_velocity_micros = 0;
volatile short rotations = 0;
unsigned short vel_changes = 0;
double pressure = 2e5;

//Flags
bool motor_started = false;
bool bdc_cycle_started = false;
bool tdc_cycle_started = false;
bool opened_outlet = false;
bool closed_outlet = false;
bool opened_inlet = false;
bool closed_inlet = false;
bool last_vel_state = false;
bool tdc_happend = false;
bool bdc_happend = false;

const float min_inlet_percent = 0.2;
const float max_inlet_percent = 0.3;
const float min_rotation_velocity = 50.0;
const float max_rotation_velocity = 1000.0;

//Other
int num_pressure_memory = 8;
double pressure_memory[8] = {};
byte pressure_memory_index = 0;

volatile unsigned long latest_micros = 0;
volatile unsigned long finish_cykel_micros = 0;
volatile unsigned long close_outlet_micros = 0;
volatile unsigned long open_inlet_micros = 0;
volatile unsigned long close_inlet_micros = 0;
volatile unsigned long open_outlet_micros = 0;
volatile unsigned long last_tdc_micros = 0;
volatile unsigned long last_bdc_micros = 0;

void setup() {
    pinMode(INLET_VALVE_PIN, OUTPUT);
    pinMode(OUTLET_VALVE_PIN, OUTPUT);
    pinMode(TDC_INTERRUPT_PIN, INPUT);
    pinMode(BDC_INTERRUPT_PIN, INPUT);
    pinMode(START_MOTOR_BTN, INPUT);
    pinMode(STOP_MOTOR_BTN, INPUT);
    

    if (USE_TIMERS) {
        Timer1.initialize(6e7/rotation_velocity);
        Timer1.attachInterrupt(tdcInterrupt);
    } else {
        attachInterrupt(digitalPinToInterrupt(TDC_INTERRUPT_PIN), tdcInterrupt, FALLING);
        attachInterrupt(digitalPinToInterrupt(BDC_INTERRUPT_PIN), bdcInterrupt, FALLING);
    }
    
    if (USE_PRESSURE_SENSOR) {
        Wire.begin();
        sensor.reset();
        sensor.begin();

        delay(10);
        pressure = sensor.getPressure(ADC_512)*100;
    }
    
    Serial.begin(9600);
}

void tdcInterrupt(void) {
    const unsigned long current_micros = micros();

    if (current_micros - last_tdc_micros >= 1e5 && !digitalRead(TDC_INTERRUPT_PIN) && !tdc_happend) {
        last_tdc_micros = current_micros;
        tdc_cycle_started = true;
        tdc_happend = true;
        bdc_happend = false;
        rotations++;
        
        float start_closing_inlet_percent = min(max_inlet_percent,max(min_inlet_percent,0.5 - 0.016666667*rotation_velocity * (pre_solenoid_starting_time + solenoid_act_time) - 0.15915494 * acos((0.00070685835*pressure * ((2001406.1 * pow(0.000031436719*ambient_pressure - 0.000079503087 * pressure, 2))/pow(pressure, 2) - 0.007616))/(0.0000013832156 * ambient_pressure - 0.0000034981358*pressure))));
        
        close_outlet_micros = current_micros + long (tdc_outlet_offset_angle/(6*rotation_velocity));
        open_inlet_micros = current_micros + long (tdc_inlet_offset_angle/(6*rotation_velocity)); //- long ((pre_solenoid_starting_time + solenoid_act_time)*1.0e6);
        close_inlet_micros = open_inlet_micros + long (start_closing_inlet_percent*6e7/rotation_velocity);
        
        opened_outlet = false;
        closed_outlet = false;
        opened_inlet = false;
        closed_inlet = false;
        
        if (PRINT_INFO) {
            Serial.print("TDC: ");
            Serial.print(motor_started);
            Serial.print("\t");
            Serial.print(rotations);
            Serial.print("\t");
            Serial.print(rotation_velocity);
            Serial.print("\t");
            Serial.print(vel_changes);
            Serial.print("\t");
            Serial.print(pressure);
            Serial.print("\t");
            Serial.println(start_closing_inlet_percent,8);
        }
        vel_changes = 0;
    }
}

void bdcInterrupt(void) {
    const unsigned long current_micros = micros();

    if (current_micros - last_bdc_micros >= 1e5 && !digitalRead(BDC_INTERRUPT_PIN) && !bdc_happend) {
        last_bdc_micros = current_micros;
        bdc_cycle_started = true;
        opened_outlet = false;
        tdc_happend = false;
        bdc_happend = true;
        
        open_outlet_micros = current_micros + long (bdc_cycle_offset_angle/(6*rotation_velocity));
        
        if (PRINT_INFO) {
            Serial.print("BDC: ");
            Serial.print("\t");
            Serial.print(motor_started);
            Serial.print("\t");
            Serial.println(rotation_velocity);
        }
    }
}

void loop() {
    const unsigned long current_micros = micros();

    bool current_vel_state = digitalRead(VEL_PIN);
    if (last_vel_state != current_vel_state) {
      rotation_velocity = min(max_rotation_velocity, max(min_rotation_velocity, 2e7/(6*(current_micros - last_velocity_micros))));
      
      last_velocity_micros = current_micros;
      last_vel_state = current_vel_state;
      vel_changes++;
    }
    
    unsigned long tdc_cycle_started_copy, bdc_cycle_started_copy;
    noInterrupts();
    tdc_cycle_started_copy = tdc_cycle_started;
    bdc_cycle_started_copy = bdc_cycle_started;
    interrupts();

    if (motor_started) {
        if (digitalRead(STOP_MOTOR_BTN)){
            motor_started = false;
            digitalWrite(INLET_VALVE_PIN, LOW);
            digitalWrite(OUTLET_VALVE_PIN, LOW);
        }
        if (tdc_cycle_started_copy) {
            if (current_micros >= close_outlet_micros && !closed_outlet) {
                digitalWrite(OUTLET_VALVE_PIN, LOW);
                closed_outlet = true;
            }
            if (current_micros >= open_inlet_micros && !opened_inlet) {
                digitalWrite(INLET_VALVE_PIN, HIGH);
                opened_inlet = true;
            }
            if (current_micros >= close_inlet_micros && !closed_inlet) {
                digitalWrite(INLET_VALVE_PIN, LOW);
                closed_inlet = true;
                tdc_cycle_started = false;
            }
        } else if (bdc_cycle_started_copy) {
            if (current_micros >= open_outlet_micros && !opened_outlet) {
                digitalWrite(OUTLET_VALVE_PIN, HIGH);
                opened_outlet = true;
                bdc_cycle_started = false;
                if (USE_PRESSURE_SENSOR) {
                    pressure = max(ambient_pressure + 100, sensor.getPressure(ADC_512)*100 - ambient_pressure);
                }
            }
        } else if (USE_PRESSURE_SENSOR) {
            pressure_memory[pressure_memory_index] = sensor.getPressure(ADC_2048)*100;
            pressure_memory_index++;
            if (pressure_memory_index >= num_pressure_memory) {
                pressure_memory_index = 0;
            }
            double tot_pressure_memory = 0;
            for (short i = 0; i < num_pressure_memory; i++) {
                tot_pressure_memory += pressure_memory[i];
            }
            pressure = tot_pressure_memory / (float) num_pressure_memory - ambient_pressure;
        }
    } else {
        if (digitalRead(START_MOTOR_BTN)) {
            motor_started = true;
            digitalWrite(OUTLET_VALVE_PIN, HIGH);
        }
    }
}
