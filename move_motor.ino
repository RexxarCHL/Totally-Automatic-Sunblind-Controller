#include "tasc.h"

void setup_servo() {
    pinMode(SERVO_ENABLE, OUTPUT);
    servo_motor.attach(SERVO_PIN);

    Serial.println("Servo initialized");
}

void move_servo(int pos, bool relative=false) {
    digitalWrite(SERVO_ENABLE, HIGH);  // Turn on the servo
    if (relative) {
        int last_position = servo_motor.read()  // Get the last written position
        servo_motor.write(last_position + pos);
    }
    else {
        servo_motor.write(pos);
    }

    delay(SERVO_DELAY);  // Wait for the servo to get in position
    digitalWrite(SERVO_ENABLE, LOW);  // Turn off the servo
}
