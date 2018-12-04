#include "tasc.h"

void setup_servo() {
    stepper_motor.setSpeed(STEPS_PER_REV*2);

    Serial.println("Servo initialized");
}

void move_servo(int target_blind_pos) {
    int target_stepper_position = (float) target_blind_pos / ONE_TURN * STEPS_PER_REV;
    int error = target_stepper_position - stepper_position;

    stepper_motor.step(error);
    stepper_position = target_stepper_position;
}

/* Leftover code for DC motor */
// void setup_servo() {
//     // Set pin modes for H-bridge control pins
//     pinMode(MOTOR_ENABLE, OUTPUT);
//     pinMode(MOTOR_PIN1, OUTPUT);
//     pinMode(MOTOR_PIN2, OUTPUT);
//     // Write initial values to control pins
//     digitalWrite(MOTOR_ENABLE, LOW);
//     digitalWrite(MOTOR_PIN1, LOW);
//     digitalWrite(MOTOR_PIN2, LOW);

//     // Set pin modes and interrupts for motor encoder pins
//     pinMode(MOTOR_ENCODER_A, INPUT_PULLUP);
//     pinMode(MOTOR_ENCODER_B, INPUT_PULLUP);
//     attachInterrupt(digitalPinToInterrupt(MOTOR_ENCODER_A), 
//                     isr_ch_a, CHANGE);
//     attachInterrupt(digitalPinToInterrupt(MOTOR_ENCODER_B), 
//                     isr_ch_b, CHANGE);

//     // noInterrupts(); // disable interrupts for now

//     Serial.println("Servo initialized");
// }

// void move_servo(int target_blind_pos) {
//     // interrupts(); // enable interrupts

//     // Map blind angle to servo position
//     int target_servo_position = target_blind_pos * EVENT_COUNT_PER_REV / ONE_TURN;

//     Serial.print("blind_pos: "); Serial.println(target_blind_pos);
//     Serial.print("target: "); Serial.println(target_servo_position);
//     Serial.print("current: "); Serial.println(servo_position);

//     // Calculate error
//     float error, previous_error, deriv_error;
//     int ctl_signal;
//     unsigned long previous_time = millis();
    
//     do {
//         error = float(target_servo_position - servo_position) / EVENT_COUNT_PER_REV * 2*M_PI;
//         // deriv_error = (error - previous_error) / (float(DELTA_T)/1000.0);
//         enable_motor_direction(error >= 0); // Turn on the motor

//         // Calculate control signal
//         ctl_signal = constrain(abs(K_prop * error + K_D * deriv_error) / 5.0 * 255, 0, 255);

//         // Drive the motor with the contol signal
//         analogWrite(MOTOR_ENABLE, ctl_signal);

//         Serial.print("target: "); Serial.println(target_servo_position);
//         Serial.print("current: "); Serial.println(servo_position);
//         Serial.print("error: "); Serial.println(error);
//         Serial.print("signal: "); Serial.println(ctl_signal);
//         Serial.println("----------------");
//         while(millis() - previous_time < DELTA_T); // do nothing

//         previous_time = millis();
//         previous_error = error;

//         if (!check_battery_level()){
//             Serial.println("Battery low!");
//             break;
//         }
//     } while(abs(error) > 0.01);

//     // Turn off the motor
//     disable_motor();

//     // noInterrupts(); // Disable interrupts
// }

// void isr_ch_a() {
//     // Read the encoder 
//     bool ch_a = digitalRead(MOTOR_ENCODER_A);
//     bool ch_b = digitalRead(MOTOR_ENCODER_B);

//     // Add or subtract from the operator
//     if (ch_a == ch_b) // Both channels are HIGH
//         servo_position--; // Subtract 1 from the counter
//     else  // One of the channels is HIGH
//         servo_position++; // Add 1 to the counter
// }

// void isr_ch_b() {
//     // Read the encoder 
//     bool ch_a = digitalRead(MOTOR_ENCODER_A);
//     bool ch_b = digitalRead(MOTOR_ENCODER_B);

//     // Add or subtract from the operator
//     if (ch_a == ch_b) // Both channels are HIGH
//         servo_position++; // Subtract 1 from the counter
//     else // One of the channels is HIGH
//         servo_position--; // Add 1 to the counter
// }

// void enable_motor_direction(bool dir) {
//     if (dir) { // CW
//         digitalWrite(MOTOR_PIN1, HIGH);
//         digitalWrite(MOTOR_PIN2, LOW);
//     }
//     else { // CCW
//         digitalWrite(MOTOR_PIN1, LOW);
//         digitalWrite(MOTOR_PIN2, HIGH);
//     }
// }

// void disable_motor(){
//     digitalWrite(MOTOR_ENABLE, LOW);
//     digitalWrite(MOTOR_PIN1, LOW);
//     digitalWrite(MOTOR_PIN2, LOW);
// }