#include <Arduino.h>
#include <string.h>

#define ARDUINO_PWR_PIN 6
#define C_TIME_VOL 3
#define C_DIST_VOL 3

typedef struct{
    int id;
    int trigger_pin;
    int echo_pin;
}sensor_US;

void emergency_shutdown(){
    digitalWrite(ARDUINO_PWR_PIN, LOW);
}

void init_US(sensor_US sensor) {
    // Configure the pins
    static int id = 1;
    sensor.id = id;
    pinMode(sensor.trigger_pin, OUTPUT);
    pinMode(sensor.echo_pin, INPUT);
    id++;
}

float read_US(sensor_US sensor) {
    digitalWrite(sensor.trigger_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(sensor.trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensor.trigger_pin, LOW);

    // Measure the echo pulse duration
    long duration = pulseIn(sensor.echo_pin, HIGH);

    // Calculate the distance (in cm)
    float distance = duration * 0.034 / 2;
    return distance;
}

bool addWater(uint ml){
    Serial.println("cmd_high_pump_1");
    while((read_US(us1)*C_DIST_VOL) < ml){delay(250);}
    Serial.println("cmd_low_pump_1");
    string response = Serial.read();
    if (response != "low_pump_1"){
        emergency_shutdown();
    }
    return true;
}

bool addFertilizer(uint ml){
    Serial.println("cmd_high_pump_2");
    delay(ml*C_TIME_VOL);
    Serial.println("cmd_low_pump_2");
    string response = Serial.read();
    if (response != "low_pump_1"){
        emergency_shutdown();
    }
    return true;
}

bool checkPH(int ph){
    Serial.println("cmd_high_pump_2");
    delay(ml*C_TIME_VOL);
    Serial.println("cmd_low_pump_2");
    string response = Serial.read();
    if (response != "low_pump_1"){
        emergency_shutdown();
    }
    return true;
}