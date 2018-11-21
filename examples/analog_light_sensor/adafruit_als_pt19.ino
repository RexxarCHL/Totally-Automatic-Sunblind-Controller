/*
 * Adafruit ALS PT19
 * Connect - to ground, + to 2.5-5.5v. Measure analog voltage on the OUT pin
 */

const int SENSOR_PIN = A0;

void setup(){
    pinMode(SENSOR_PIN, INPUT);

    Serial.begin(9600);
}

void loop(){
    int sensor_value = analogRead(SENSOR_PIN);
    Serial.println(sensor_value);
}