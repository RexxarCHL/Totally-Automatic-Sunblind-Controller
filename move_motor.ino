#include "tasc.h"

void setup_servo() {
    pinMode(STEPPER_ENABLE, OUTPUT);
    digitalWrite(STEPPER_ENABLE, LOW);

    stepper_motor.setSpeed(STEPS_PER_REV);

    //Serial.println("Servo initialized");
}

void move_servo(int target_blind_pos) {
    int target_stepper_position = (float) target_blind_pos / ONE_TURN * STEPS_PER_REV;
    int error = target_stepper_position - stepper_position;

    digitalWrite(STEPPER_ENABLE, HIGH);
    // delay(10);

    stepper_motor.step(error);
    stepper_position = target_stepper_position;

    digitalWrite(STEPPER_ENABLE, LOW);

    // delay(1000);
}
