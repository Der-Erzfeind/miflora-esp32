#include "control.h"

void emergency_shutdown(){
    digitalWrite(PIN_ARDUINO_PWR, LOW);
}

void init_US(int trigger_pin, int echo_pin) {
    // Configure the pins
    pinMode(trigger_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
}

float read_US(int trigger_pin, int echo_pin) {
    digitalWrite(trigger_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);

    // Measure the echo pulse duration
    long duration = pulseIn(echo_pin, HIGH);

    // Calculate the distance (in cm)
    float distance = duration * 0.034 / 2;
    return distance;
}

bool addWater(uint ml){
    Serial.println(CMD_PUMP1_ON);
    while((read_US(PIN_US1_TRIGGER, PIN_US1_ECHO)*C_DIST_VOL) < ml){delay(250);}
    Serial.println(CMD_PUMP1_OFF);
    int response = Serial.read();
    if (response != ACK_PUMP1_OFF){
        emergency_shutdown();
        return false;
    }
    return true;
}

bool addFertilizer(uint ml){
    Serial.println(CMD_PUMP2_ON);
    delay(ml*C_TIME_VOL);
    Serial.println(CMD_PUMP2_OFF);
    int response = Serial.read();
    if (response != ACK_PUMP2_OFF){
        emergency_shutdown();
        return false;
    }
    return true;
}

bool checkPH(float ph){
    int ml = ph * C_PH_VOL;
    float curr_ph;
    while(1){
        curr_ph = read_PH();
        if(curr_ph <= ph) 
            break;
        Serial.println(CMD_PUMP3_ON);
        delay(ml*C_TIME_VOL);
        Serial.println(CMD_PUMP3_OFF);
        int response = Serial.read();
        if (response != ACK_PUMP3_OFF){
            emergency_shutdown();
            return false;
        }
        delay(5 * 60000);
    }
    return true;
}

bool waterPlant(int pumpID){
    Serial.println(pumpID*10+1);                    //CMD_PUMPx_HIGH -> 0x1
    while((read_US(PIN_US2_TRIGGER, PIN_US2_ECHO)*C_DIST_VOL) > 0){delay(250);}
    Serial.println(pumpID*10+0);                    //CMD_PUMPx_LOW -> 0x0
    int response = Serial.read();
    if (response != (100+pumpID*10+0)){             //ACK_PUMPx_LOW -> 1x0 
        emergency_shutdown();
        return false;
    }
    return true;
}