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
    check_battery_level();
    switch (current_system_state) {
        case AUTO:
            //TODO: check PIR, check current time, adjust blind angle if necessary
            // Check buttons for AUTO/MANUAL switch
            break;
        case MANUAL:
            // TODO: use buttons to turn the blind up and down
            // Also check buttons for AUTO/MANUAL switch
            // resume to AUTO if enough time has passed
            break;
    }
}

void setup_others() {
    // Battery level check related pins
    pinMode(BATTERY_CHECK_PIN, INPUT);

    // UI control related pins
    pinMode(MANUAL_BUTTON_PIN, INPUT_PULLUP);
    pinMode(OPEN_BUTTON_PIN, INPUT_PULLUP);
    pinMode(CLOSE_BUTTON_PIN, INPUT_PULLUP);
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
    bool status = false;
    while (current_offset < MAX_ANGLE_VALUE) {
        status = move_blind_angle(direction);
        if (~status) {
            return; // Battery level was not enough!
        }
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
    status = move_blind_angle(direction * -1 * decreasing_count); // Return to the max reading position

    // Turn off the light sensor
    digitalWrite(LIGHT_SENSOR_ENABLE, LOW);
}

bool move_blind_angle(int offset){
    // Calculate the target blind angle and constrain to the range
    int target_blind_pos = constrain(current_blind_pos + offset, 0, MAX_ANGLE_VALUE);

    if (target_blind_pos == current_blind_pos)
        return false;

    move_servo(target_blind_pos);
    current_blind_pos = target_blind_pos;

    if (~check_battery_level())
        return false; // Battery level was too low!
    return true;
}

bool check_battery_level() {
    // Check battery level and block until battery is charged enough
    int battery_level = analogRead(BATTERY_CHECK_PIN);

    int status = 0;
    if (battery_level > 128) 
        status = 0;
    else if (battery_level > 64)
        status = 1;
    else if (battery_level > 32)
        status = 2;
    else
        status = 3;

    if (status == 3) {
        // Block until charged
        int k = 0;
        do {
            led_status(status)
            delay(1000);
            k += 1;
        } while (analogRead(BATTERY_CHECK_PIN) < 32);

        // if (k%2) // Odd number of times executed. Turn on the LED by calling led_status one more time
        //     led_status(status)
    }
    else
        led_status(status);

    return (status == 3);
}

void led_status(int state) {
    // Set different colors according to different power states
    static bool last_state = false;
    switch (state) {
        case 0: // Battery level normal
            set_color(0, 255, 0); // Set color to be green
            break;
        case 1: // Battery level medium
            set_color(255, 255, 0); // Set color to be yellow
            break;
        case 2: // Battery level low
            set_color(255, 0, 0); // Set color to be red
            break;
        case 3: // Battery level critical
            // Blink red
            if (last_state)
                set_color(0, 0, 0); // Turn off
            else
                set_color(255, 0, 0); // Set to red
            last_state = ~last_state; // Switch state
            break;
    }
}

void set_color(int red, int green, int blue) {
    // Write to the RGB pins of the LED
    analogWrite(LED_RED_PIN, red);
    analogWrite(LED_GREEN_PIN, green);
    analogWrite(LED_BLUE_PIN, blue);  
}