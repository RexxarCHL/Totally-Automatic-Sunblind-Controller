#ifndef _TASC_H_
#define _TASC_H_

#include <Arduino.h>  // Arduino library
#include <math.h>     // Additional math for RTC script
#include <Servo.h>    // Servo library for the servo
#include <Wire.h>     // I2C library for the light sensor
#include <Adafruit_Sensor.h>  // For light sensor
#include "Adafruit_TSL2591.h" // For light sensor
#include "RTClib.h"           // For RTC

// RTC circuit
#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif
RTC_PCF8523 rtc;

void setup_rtc();
DateTime get_current_time();
void adjust_rtc_time(int year, int month, int day, int hour, int minute, int second);
bool is_night(DateTime now);
float degrees_to_radians(float degrees);

// Servo control
Servo servo_motor;
const int SERVO_PIN = 9;
const int SERVO_ENABLE = 10;
const int SERVO_DELAY = 15;

void setup_servo();
void move_servo(int pos, bool relative);

// Occupancy sensor
const int PIR_ENABLE = 3;
const int PIR_PIN = 2;

void setup_pir_sensor();
bool check_pir_sensor();

// Light sensor
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

void setup_light_sensor();
void configure_sensor();
int get_light_reading();

#endif