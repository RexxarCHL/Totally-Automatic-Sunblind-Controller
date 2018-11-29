#include "tasc.h"

void setup_pir_sensor() {
    // Set the pin modes
    pinMode(PIR_PIN, INPUT);

    Serial.println("PIR sensor initialized");
}

bool check_pir_sensor() {
    return digitalRead(PIR_PIN);
}
