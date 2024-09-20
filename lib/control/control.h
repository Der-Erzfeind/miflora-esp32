#include <Arduino.h>
#include "ph_sensor.h"

#ifndef control.h
    #define control.h

    #define PIN_ARDUINO_PWR 2
    #define PIN_US1_TRIGGER 12
    #define PIN_US1_ECHO 13
    #define PIN_US2_TRIGGER 33
    #define PIN_US2_ECHO 25
    #define PIN_US3_TRIGGER 27
    #define PIN_US3_ECHO 26
    #define PIN_PH_PO 14


    #define CMD_PUMP_POT1_OFF "CMD_PUMP1_OFF"
    #define CMD_PUMP_POT2_OFF "CMD_PUMP2_OFF"
    #define CMD_PUMP_POT3_OFF "CMD_PUMP3_OFF"
    #define CMD_PUMP_WATER_OFF "CMD_PUMP4_OFF"
    #define CMD_PUMP_FERTILIZER_OFF "CMD_PUMP5_OFF"
    #define CMD_PUMP_ACID_OFF "CMD_PUMP6_OFF"

    #define CMD_PUMP_POT1_ON "CMD_PUMP1_ON"
    #define CMD_PUMP_POT2_ON "CMD_PUMP2_ON"
    #define CMD_PUMP_POT3_ON "CMD_PUMP3_ON"
    #define CMD_PUMP_WATER_ON "CMD_PUMP4_ON"
    #define CMD_PUMP_FERTILIZER_ON "CMD_PUMP5_ON"
    #define CMD_PUMP_ACID_ON "CMD_PUMP6_ON"

    #define ACK_PUMP_POT1_OFF "ACK_PUMP1_OFF"
    #define ACK_PUMP_POT2_OFF "ACK_PUMP2_OFF"
    #define ACK_PUMP_POT3_OFF "ACK_PUMP3_OFF"
    #define ACK_PUMP_WATER_OFF "ACK_PUMP4_OFF"
    #define ACK_PUMP_FERTILIZER_OFF "ACK_PUMP5_OFF"
    #define ACK_PUMP_ACID_OFF "ACK_PUMP6_OFF"

    #define ACK_PUMP_POT1_ON "ACK_PUMP1_ON"
    #define ACK_PUMP_POT2_ON "ACK_PUMP2_ON"
    #define ACK_PUMP_POT3_ON "ACK_PUMP3_ON"
    #define ACK_PUMP_WATER_ON "ACK_PUMP4_ON"
    #define ACK_PUMP_FERTILIZER_ON "ACK_PUMP5_ON"
    #define ACK_PUMP_ACID_ON "ACK_PUMP6_ON"

    #define C_TIME_VOL 6
    #define C_DIST_VOL 3
    #define C_PH_VOL 4


    void emergency_shutdown();
    void initHardware();
    float readUltraSonic(int trigger_pin, int echo_pin);
    bool addWater(uint ml);
    bool addFertilizer(uint ml);
    bool checkPH(float ph);
    bool waterPlant(int pumpID);
    int checkWaterLevel();
    int checkFertilizerLevel();
    int checkAcidLevel();

#endif