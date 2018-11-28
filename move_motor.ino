#include <Servo.h>

// TODO: Change pin assignment
const int SERVO_PIN = 9;  // Control pin for the servo
const int SERVO_ENABLE = 10;  // Transistor base pin

Servo servo_motor;
const int SERVO_DELAY = 15;  // Time to wait for servo to get in position

void setup_servo() {
    pinMode(SERVO_ENABLE, OUTPUT);
    servo_motor.attach(SERVO_PIN);
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
