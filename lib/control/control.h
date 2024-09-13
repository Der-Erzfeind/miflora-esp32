#include <Arduino.h>
#include "ph_sensor.h"

#ifndef control.h
    #define control.h

    #define PIN_ARDUINO_PWR 2
    #define PIN_US1_TRIGGER 12
    #define PIN_US1_ECHO 13
    #define PIN_US2_TRIGGER 33
    #define PIN_US2_ECHO 25
    #define PIN_US2_TRIGGER 27
    #define PIN_US2_ECHO 26
    #define PIN_PH_PO 14


    #define CMD_PUMP1_OFF "101"
    #define CMD_PUMP2_OFF "102"
    #define CMD_PUMP3_OFF "103"
    #define CMD_PUMP4_OFF "104"
    #define CMD_PUMP5_OFF "105"
    #define CMD_PUMP6_OFF "106"
    #define CMD_PUMP1_ON "111"
    #define CMD_PUMP2_ON "112"
    #define CMD_PUMP3_ON "113"
    #define CMD_PUMP4_ON "114"
    #define CMD_PUMP5_ON "115"
    #define CMD_PUMP6_ON "116"
    #define ACK_PUMP1_OFF "201"
    #define ACK_PUMP2_OFF "202"
    #define ACK_PUMP3_OFF "203"
    #define ACK_PUMP4_OFF "204"
    #define ACK_PUMP5_OFF "205"
    #define ACK_PUMP6_OFF "206"
    #define ACK_PUMP1_ON "211"
    #define ACK_PUMP2_ON "212"
    #define ACK_PUMP3_ON "213"
    #define ACK_PUMP4_ON "214"
    #define ACK_PUMP5_ON "215"
    #define ACK_PUMP6_ON "216"

    #define C_TIME_VOL 6
    #define C_DIST_VOL 3
    #define C_PH_VOL 4


    void emergency_shutdown();
    void init_US(int trigger_pin, int echo_pin);
    float read_US(int trigger_pin, int echo_pin);
    bool addWater(uint ml);
    bool addFertilizer(uint ml);
    bool checkPH(float ph);
    bool waterPlant(int pumpID);

#endif