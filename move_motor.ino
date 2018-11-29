#include "tasc.h"

void setup_servo() {
    // Set pin modes for H-bridge control pins
    pinMode(MOTOR_ENABLE, OUTPUT);
    pinMode(MOTOR_PIN1, OUTPUT);
    pinMode(MOTOR_PIN2, OUTPUT);
    // Write initial values to control pins
    digitalWrite(MOTOR_ENABLE, LOW);
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, LOW);

    // Set pin modes and interrupts for motor encoder pins
    pinMode(MOTOR_ENCODER_A, INPUT_PULLUP);
    pinMode(MOTOR_ENCODER_B, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(MOTOR_ENCODER_A), 
                    isr_ch_a, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOTOR_ENCODER_B), 
                    isr_ch_b, CHANGE);

    noInterrupts(); // disable interrupts for now

    Serial.println("Servo initialized");
}

const int MOTOR_PIN1 = 6;
const int MOTOR_PIN2 = 5;
const int MOTOR_ENABLE = 11;
const int MOTOR_ENCODER_A = 2;
const int MOTOR_ENCODER_B = 3;

void move_servo(int target_blind_pos) {
    // Map blind angle to servo position
    int target_servo_position = target_blind_pos / ONE_TURN * EVENT_COUNT_PER_REV

    // Calculate error
    float error, previous_error, deriv_error;
    int ctl_signal;
    unsigned long previous_time = millis();
    
    do {
        error = float(target_servo_position - servo_position) / EVENT_COUNT_PER_REV * 2*M_PI
        // deriv_error = (error - previous_error) / (float(DELTA_T)/1000.0);
        enable_motor_direction(error >= 0); // Turn on the motor

        // Calculate control signal
        ctl_signal = constrain(abs(K_prop * error + K_D * deriv_error) / 5.0 * 255, 0, 255);

        // Drive the motor with the contol signal
        analogWrite(MOTOR_ENABLE, ctl_signal);

        while(millis() - previous_time < DELTA_T); // do nothing

        previous_time = millis();
        previous_error = error;
    } while(abs(error) > 0.0001)

    // Turn off the motor
    disable_motor();
}

void isr_ch_a() {
    // Read the encoder 
    bool ch_a = digitalRead(MOTOR_ENCODER_A);
    bool ch_b = digitalRead(MOTOR_ENCODER_B);

    // Add or subtract from the operator
    if (ch_a == ch_b) // Both channels are HIGH
        servo_position--; // Subtract 1 from the counter
    else  // One of the channels is HIGH
        servo_position++; // Add 1 to the counter
}

void isr_ch_b() {
    // Read the encoder 
    bool ch_a = digitalRead(MOTOR_ENCODER_A);
    bool ch_b = digitalRead(MOTOR_ENCODER_B);

    // Add or subtract from the operator
    if (ch_a == ch_b) // Both channels are HIGH
        servo_position++; // Subtract 1 from the counter
    else // One of the channels is HIGH
        servo_position--; // Add 1 to the counter
}

void enable_motor_direction(bool dir) {
    if (dir) { // CW
        digitalWrite(BRIDGE_INPUT1, HIGH);
        digitalWrite(BRIDGE_INPUT2, LOW);
    }
    else { // CCW
        digitalWrite(BRIDGE_INPUT1, LOW);
        digitalWrite(BRIDGE_INPUT2, HIGH);
    }
}

void disable_motor(){
    digitalWrite(MOTOR_ENABLE, LOW);
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, LOW);
}