#include <Arduino.h>
#include "ph_sensor.cpp"

#define PIN_ARDUINO_PWR 2
#define PIN_US1_TRIGGER 1
#define PIN_US1_ECHO 1
#define PIN_US2_TRIGGER 1
#define PIN_US2_ECHO 1

#define CMD_PUMP1_OFF 010
#define CMD_PUMP2_OFF 020
#define CMD_PUMP3_OFF 030
#define CMD_PUMP4_OFF 040
#define CMD_PUMP5_OFF 050
#define CMD_PUMP6_OFF 060

#define CMD_PUMP1_ON 011
#define CMD_PUMP2_ON 021
#define CMD_PUMP3_ON 031
#define CMD_PUMP4_ON 041
#define CMD_PUMP5_ON 051
#define CMD_PUMP6_ON 061

#define ACK_PUMP1_OFF 110
#define ACK_PUMP2_OFF 120
#define ACK_PUMP3_OFF 130
#define ACK_PUMP4_OFF 140
#define ACK_PUMP5_OFF 150
#define ACK_PUMP6_OFF 160

#define ACK_PUMP1_ON 111
#define ACK_PUMP2_ON 121
#define ACK_PUMP3_ON 131
#define ACK_PUMP4_ON 141
#define ACK_PUMP5_ON 151
#define ACK_PUMP6_ON 161

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