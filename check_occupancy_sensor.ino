// TODO: Change these pin numbers
const int PIR_ENABLE = 3;
const int PIR_PIN = 2;

void setup_pir_sensor() {
    // Set the pin modes
    pinMode(PIR_PIN, INPUT);
}

bool check_pir_sensor() {
    val = digitalRead(PIR_PIN);
    if (val == HIGH)
        return true;
    else
        return false;
}
