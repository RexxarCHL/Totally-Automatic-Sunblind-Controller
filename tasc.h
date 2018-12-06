#ifndef _TASC_H_
#define _TASC_H_

#include <Arduino.h>  // Arduino library
#include <math.h>     // Additional math for RTC script
#include <Wire.h>     // I2C library for the light sensor
#include <Adafruit_Sensor.h>  // For light sensor
#include "Adafruit_TSL2591.h" // For light sensor
#include "RTClib.h"           // For RTC
#include <Stepper.h>
#include "Battery.h"
#include "LowPower.h"

// RTC circuit
RTC_PCF8523 rtc;

void setup_rtc();
DateTime get_current_time();
void adjust_rtc_time(int year, int month, int day, int hour, int minute, int second);
bool is_night();
float degrees_to_radians(float degrees);

// Servo control
// const int MOTOR_PIN1 = 7;
// const int MOTOR_PIN2 = 8;
// const int MOTOR_ENABLE = 9;
// const int MOTOR_ENCODER_A = 2;
// const int MOTOR_ENCODER_B = 3;
const int STEPPER_ENABLE = 8;
const int STEPPER_IN1 = A0;
const int STEPPER_IN2 = A1;
const int STEPPER_IN3 = 2;
const int STEPPER_IN4 = 3;
const int STEPS_PER_REV = 2038;
Stepper stepper_motor(8, STEPPER_IN1, STEPPER_IN2, STEPPER_IN3, STEPPER_IN4);
// volatile int servo_position = 0;
int stepper_position = 0;

/*
Hobby servo:
4 turns is too much with out a gear box
DC Motor:
The angle that we need to move is too small for a DC motor to activate
Once activated, the motor would overshhot and become unstable
Stepper:
Trying out now.
Wires for +, -, in3, in4 are borrowed
*/

// 4 counts per cycle, 12 cycles per revolution
// 34 revolutions per output shaft revolution
const float EVENT_COUNT_PER_REV = 4*12*34;
const int DELTA_T = 30;
const float K_prop = 3.0;
const float K_D = 0.0;

void setup_servo();
void move_servo(int blind_pos);

// Occupancy sensor
// const int PIR_ENABLE = 3;
const int PIR_PIN = 4;
bool pir_state = LOW;

void setup_pir_sensor();
bool check_pir_sensor();

// Light sensor
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)
const int LIGHT_SENSOR_ENABLE = 8; // connect the pin to the 5v in for the sensor?

void setup_light_sensor();
void configure_sensor();
int get_light_reading();

// Various control functions and variables
enum SystemState {
    INIT,
    MANUAL,
    AUTO
} current_system_state;

void setup_others();
void adjust_blind_angle();
bool move_blind_angle(int offset);
void check_button(); // TODO
bool check_battery_level(); // TODO
void set_color(int red, int green, int blue);
void led_status(int state);
const int BATTERY_CHECK_PIN = A3;
const int MANUAL_BUTTON_PIN = 5;
const int OPEN_BUTTON_PIN = 6;
const int CLOSE_BUTTON_PIN = 7;
const int LED_RED_PIN = 11;
const int LED_GREEN_PIN = 12;
const int LED_BLUE_PIN = 10;
const int MODE_LED_PIN = 13;

Battery battery = Battery(6200, 9000, BATTERY_CHECK_PIN);

// It takes about 4 turns from open to fully closed
// current_blind_pos = [0, 64] and there are 16 positions in each turn 
const int TURNS_TO_CLOSE = 4;
const int MAX_ANGLE_VALUE = 64;
const int ONE_TURN = MAX_ANGLE_VALUE / TURNS_TO_CLOSE;
const int HALF_TURN = ONE_TURN / 2;
const int MAX_DECREASING_COUNT = 2;
int current_blind_pos = 0;
uint32_t last_blind_adjust = 0;
uint32_t last_manual_action = 0;
uint32_t last_battery_check = 0;

// Parametric timing constants for the program, in seconds
const int BATTERY_CHECK_INTERVAL = 10;
const int BLIND_ADJUST_INTERVAL = 60;
const int BLIND_ADJUST_INTERVAL_NO_PRESENCE = 60;
const int AUTO_MODE_SWITCH_INTERVAL = 60;
const int NIGHT_SLEEP_INTERVAL = 24; // Must be multiples of 8



#endif