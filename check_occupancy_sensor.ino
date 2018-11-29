#include "tasc.h"

void setup_pir_sensor() {
    // Set the pin modes
    pinMode(PIR_PIN, INPUT);

    Serial.println("PIR sensor initialized");
}

bool check_pir_sensor() {
    val = digitalRead(PIR_PIN);
    if (val == HIGH)
        return true;
    else
        return false;
}
