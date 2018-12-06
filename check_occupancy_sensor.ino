#include "tasc.h"

void setup_pir_sensor() {
    // Set the pin modes
    pinMode(PIR_PIN, INPUT);

    //Serial.println("PIR sensor initialized");
}

bool check_pir_sensor() {
    bool val = digitalRead(PIR_PIN);  // read input value
    if (val == HIGH) {            // check if the input is HIGH
        if (pir_state == LOW) {
            // we have just turned on
            //Serial.println("Motion detected!");
            // We only want to print on the output change, not state
            pir_state = HIGH;
        }
    }
    else {
        if (pir_state == HIGH){
            // we have just turned off
            //Serial.println("Motion ended!");
            // We only want to print on the output change, not state
            pir_state = LOW;
        }
    }
}
