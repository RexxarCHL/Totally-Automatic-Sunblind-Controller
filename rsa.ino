#include "tasc.h"

void setup() {
    Serial.begin(9600);

    // Initialize the sensors
    setup_rtc();
    setup_servo();
    setup_pir_sensor();
    setup_light_sensor();

    Serial.println("All sensors initialized");

    // Initialize the state
}

void loop() {
    // TODO: button control and state change
}

void adjust_blind_angle() {
    // NOTE: 4 turns from horizontal to fully closed
    
    /* Setup */
    digitalWrite(LIGHT_SENSOR_ENABLE, HIGH);  // Turn on the light sensor
    int initial_reading = get_light_reading(); // Get the initial reading

    /* Test which direction the motor needs to go to maximize the readings */
    move_blind_angle(HALF_TURN); // Move the blind in one direction for one turn
    int direction;
    if (get_light_reading() > initial_reading) {
        // Reading increased: we are moving in the right way!
        direction = 1;
    } 
    else {
        // Reading decreased: move to the other direction
        direction = -1;
        move_blind_angle(-1 * ONE_TURN); // Move the motor back for equal start point for both cases
    }

    // Note the blind have already been moved in this direction for HALF_TURN
    int current_offset = HALF_TURN;
    int current_reading = 0;
    int max_reading = 0;
    int decreasing_count = 0;
    while (current_offset < MAX_ANGLE_VALUE) {
        move_blind_angle(direction);
        current_offset++;

        current_reading = get_light_reading();

        if (current_reading <= max_reading){
            decreasing_count++;
            if (decreasing_count > MAX_DECREASING_COUNT){
                // We moved too much and the reading is decreasing consistently!
                break;
            }
        }
        else { // current_reading > max_reading
            decreasing_count = 0; // Not decreasing: reset the counter
            max_reading = current_reading; // Update max reading
        }
    }
    move_blind_angle(direction * -1 * decreasing_count); // Return to the max reading position

    // Turn off the light sensor
    digitalWrite(LIGHT_SENSOR_ENABLE, LOW);
}

void move_blind_angle(int offset){
    // Calculate the target blind angle and constrain to the range
    int target_blind_pos = constrain(current_blind_pos + offset, 0, MAX_ANGLE_VALUE);

    if (target_blind_pos == current_blind_pos)
        return;

    move_servo(target_blind_pos);
    current_blind_pos = target_blind_pos;
}