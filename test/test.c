#include <Arduino.h>
#include <BLEDevice.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "PubSubClient.h"
#include "config.h"
#include "Sensor.h"
#include "control.h"

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  // Convert the byte array to a String
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Create a DynamicJsonDocument to hold the parsed JSON
  DynamicJsonDocument sensor_conf(sensorCapacity);

  // Deserialize the JSON data
  DeserializationError error = deserializeJson(sensor_conf, message);

  // Check for errors in deserialization
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }


   for (int i = 0; i < 3; i++){
    FLORA_DEVICES[i][0] = sensor_conf["mac"];
    FLORA_DEVICES[i][1] = sensor_conf["location"];
    FLORA_DEVICES[i][2] = sensor_conf["plant_id"];
    FLORA_DEVICES[i][3].toInt() = sensor_conf["min_temperature"];
    FLORA_DEVICES[i][4].toInt() = sensor_conf["max_temperature"];
    FLORA_DEVICES[i][5].toInt() = sensor_conf["min_moisture"];
    FLORA_DEVICES[i][6].toInt() = sensor_conf["max_moisture"];
    FLORA_DEVICES[i][7].toInt() = sensor_conf["min_light"];
    FLORA_DEVICES[i][8].toInt() = sensor_conf["max_light"];
    FLORA_DEVICES[i][9].toInt() = sensor_conf["min_conductivity"];
    FLORA_DEVICES[i][10].toInt() = sensor_conf["max_conductivity"];
  }
}

