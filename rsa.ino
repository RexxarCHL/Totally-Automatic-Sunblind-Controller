#include "tasc.h"

void setup() {
    Serial.begin(57600);
    //Serial.println("setup: Initializing...");

    // Initialize the sensors
    setup_rtc();
    setup_servo();
    setup_light_sensor();
    setup_pir_sensor();
    setup_others();

    //Serial.println("setup: All sensors initialized");

    // // Adjust to the best blind angle
    adjust_blind_angle();
    //Serial.println("Blind angle adjusted");

    // DEMO: Set RTC to different time for night/day behaviors
    // adjust_rtc_time(2018, 12, 6, 18, 58, 0); // Set to 2018/12/06 6:58 pm
    adjust_rtc_time(2018, 12, 6, 9, 59, 0); // Set to 2018/12/06 8:58 am

    // // Initialize the state
    current_system_state = AUTO;
    // current_system_state = MANUAL;
}

void loop() {
    uint32_t current_unix_time = get_current_time().unixtime(); // Get current time
    if (current_unix_time - last_battery_check > BATTERY_CHECK_INTERVAL)
        check_battery_level(); // Wait for some time before checking battery
    
    delay(100);
    //Serial.print("System: "); Serial.println(current_system_state);

    switch (current_system_state) {
        case AUTO:
            digitalWrite(MODE_LED_PIN, LOW);

            // Check if it is night currently
            if (is_night()) {
                // It's night! Close the blind and do nothing else
                if (current_blind_pos > 0) {
                    // Move the blind back to fully closed position
                    move_blind_angle(-current_blind_pos); 
                }

                // Sleep for a given amount of time
                for (int sleep = 0; sleep < NIGHT_SLEEP_INTERVAL/8; sleep++) {
                    delay(8000);
                    // LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
                }

                return;
            }


            // Check PIR sensor
            check_pir_sensor();
            if ((pir_state == HIGH) && 
                ((current_unix_time - last_blind_adjust) > BLIND_ADJUST_INTERVAL)) {
                // Human presence detected and some time have passed since last adjustment
                adjust_blind_angle();
            }
            else {
                // No presence detected. Check time since last adjustment
                if ((current_unix_time - last_blind_adjust) > BLIND_ADJUST_INTERVAL_NO_PRESENCE) {
                    // One hour has passed since last blind adjustment
                    adjust_blind_angle();
                }
            }

            // Check buttons for AUTO/MANUAL switch
            if (digitalRead(MANUAL_BUTTON_PIN) == LOW) {
                // Manual mode button pressed! Switch to manual mode
                current_system_state = MANUAL;
                last_manual_action = current_unix_time;
                //Serial.println("Switching to MANUAL");
                delay(100);
            }

            break;

        case MANUAL:
            digitalWrite(MODE_LED_PIN, HIGH);
            // TODO: Add an led for manual/auto indication, if possible
            if ((current_unix_time - last_manual_action > AUTO_MODE_SWITCH_INTERVAL) || 
                (digitalRead(MANUAL_BUTTON_PIN) == LOW)) {
                // One hour have passed since last manual action or manual mode button is pressed again
                // Switch back to AUTO
                current_system_state = AUTO;
                //Serial.println("Switching to AUTO");
                delay(100);
            }
            else {
                // Check up/down buttons and adjust the blind accordingly
                int offset = 0;
                if (digitalRead(OPEN_BUTTON_PIN) == LOW) {
                    offset = 1;
                    delay(100);
                }
                else if (digitalRead(CLOSE_BUTTON_PIN) == LOW) {
                    offset = -1;
                    delay(100);
                }

                // Adjust the blind
                move_blind_angle(offset);
            }
            break;
    }
}

void setup_others() {
    // Battery level check related pins
    battery.begin(5000, 9.0/4.0); // Reference voltage 5v, divider ratio 2.0

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
    int max_reading = get_light_reading();
    if (max_reading > initial_reading) {
        // Reading increased: we are moving in the right way!
        direction = 1;
    } 
    else {
        // Reading decreased: move to the other direction
        direction = -1;
        move_blind_angle(-1 * ONE_TURN); // Move the motor back for equal start point for both cases
        max_reading = get_light_reading();
    }

    //Serial.print("direction:"); Serial.println(direction);

    // Note the blind have already been moved in this direction for HALF_TURN
    int current_offset = HALF_TURN;
    int current_reading = 0;
    int decreasing_count = 0;
    bool status = false;
    while (current_offset < MAX_ANGLE_VALUE) {
        status = move_blind_angle(direction);

        if (!status) {
            // Turn off the light sensor
            digitalWrite(LIGHT_SENSOR_ENABLE, LOW);
            last_blind_adjust = get_current_time().unixtime();
            return; // Battery level was not enough!
        }
        current_offset++;

        delay(50); // Delay a bit?
        current_reading = get_light_reading();

        //Serial.print("Offset: "); Serial.println(current_offset);
        //Serial.print("Light: "); Serial.println(current_reading);
        //Serial.print("Max: "); Serial.println(max_reading);

        if (current_reading <= (max_reading*1.1)){
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

    last_blind_adjust = get_current_time().unixtime();
}

bool move_blind_angle(int offset){
    // Calculate the target blind angle and constrain to the range
    int target_blind_pos = constrain(current_blind_pos + offset, 0, MAX_ANGLE_VALUE);

    if (target_blind_pos == current_blind_pos)
        return true;

    move_servo(target_blind_pos);
    current_blind_pos = target_blind_pos;

    return check_battery_level();
    // return true;
}

bool check_battery_level() {
    // Check battery level and block until battery is charged enough
    // Analog read returns in the range of [0, 1023]
    // Battery voltage = 9V ~ 6V
    // Analog read results = 950 ~ 630
    // int battery_level = analogRead(BATTERY_CHECK_PIN);
    int battery_level = battery.level();
    // Serial.println(battery_level);

    int status = 0;
    // if (battery_level > 870) {
    if (battery_level > 50) {
        status = 0;
        //Serial.println("Battery level: HIGH");
    }
    // else if (battery_level > 790) {
    else if (battery_level > 35) {
        status = 1;
        //Serial.println("Battery level: MEDIUM");
    }
    // else if (battery_level > 650) {
    else if (battery_level > 10) {
        status = 2;
        //Serial.println("Battery level: LOW");
    }
    else {
        status = 3;
        //Serial.println("Battery level: CRITICAL");
    }

    last_battery_check = get_current_time().unixtime();

    if (status == 3) {
        // Block until charged
        int k = 0;
        do {
            led_status(status);
            delay(1000);
            k += 1;
            // Serial.println(analogRead(BATTERY_CHECK_PIN));
        // } while (analogRead(BATTERY_CHECK_PIN) < 700);
            // Serial.println(battery.level());
        } while(battery.level() < 20);

        // if (k%2) // Odd number of times executed. Turn on the LED by calling led_status one more time
        //     led_status(status)
        return false;
    }
    else {
        led_status(status);
        return true;
    }
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
            last_state = !last_state; // Switch state
            break;
    }
}

void set_color(int red, int green, int blue) {
    // Write to the RGB pins of the LED
    analogWrite(LED_RED_PIN, red);
    analogWrite(LED_GREEN_PIN, green);
    analogWrite(LED_BLUE_PIN, blue);  
}