#include <Arduino.h>
#include "ph_sensor.cpp"

#define PIN_ARDUINO_PWR 2
#define CMD_PUMP1_LOW 10
#define CMD_PUMP2_LOW 20
#define CMD_PUMP3_LOW 30
#define CMD_PUMP1_HIGH 11
#define CMD_PUMP2_HIGH 21
#define CMD_PUMP3_HIGH 31
#define C_TIME_VOL 
#define C_DIST_VOL 3
#define C_PH_VOL 4


typedef struct{
    int id;
    int trigger_pin;
    int echo_pin;
}sensor_US;

void emergency_shutdown(){
    digitalWrite(PIN_ARDUINO_PWR, LOW);
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
    Serial.println("CMD_PUMP1_HIGH");
    while((read_US(us1)*C_DIST_VOL) < ml){delay(250);}
    Serial.println("CMD_PUMP1_LOW");
    String response = Serial.read();
    if (response != "low_CMD_PUMP_1"){
        emergency_shutdown();
        return false;
    }
    return true;
}

bool addFertilizer(uint ml){
    Serial.println("CMD_PUMP2_HIGH");
    delay(ml*C_TIME_VOL);
    Serial.println("CMD_PUMP2_LOW");
    String response = Serial.read();
    if (response != "low_CMD_PUMP_1"){
        emergency_shutdown();
        return false;
    }
    return true;
}

bool checkPH(float ph){
    int ml = ph * C_PH_VOL;
    float curr_ph;
    while(1){
        Serial.println("CMD_PUMP3_HIGH");
        delay(ml*C_TIME_VOL);
        Serial.println("CMD_PUMP3_LOW");
        String response = Serial.read();
        if (response != "low_CMD_PUMP_1"){
            emergency_shutdown();
            return false;
        }
        curr_ph = read_PH();
    }
    return true;
}