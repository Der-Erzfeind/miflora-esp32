#include <Arduino.h>
#include <BLEDevice.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "PubSubClient.h"
#include "config.h"
#include "Sensor.h"
//#include "control.h"
#include "Box.h"

/**
   A BLE client for the Xiaomi Mi Plant Sensor, pushing measurements to an MQTT server.

   See https://github.com/nkolban/esp32-snippets/blob/master/Documentation/BLE%20C%2B%2B%20Guide.pdf
   on how bluetooth low energy and the library used are working.

   See https://github.com/ChrisScheffler/miflora/wiki/The-Basics for details on how the 
   protocol is working.
   
   MIT License

   Copyright (c) 2017 Sven Henkel
   Multiple units reading by Grega Lebar 2018

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

// device count
static int deviceCount;

// the remote service we wish to connect to
static const BLEUUID serviceUUID("00001204-0000-1000-8000-00805f9b34fb");

// the characteristic of the remote service we are interested in
static const BLEUUID uuid_version_battery("00001a02-0000-1000-8000-00805f9b34fb");
static const BLEUUID uuid_sensor_data("00001a01-0000-1000-8000-00805f9b34fb");
static const BLEUUID uuid_write_mode("00001a00-0000-1000-8000-00805f9b34fb");

// the device topic
static const String deviceBaseTopic = String(MQTT_BASE_TOPIC) + "/" + String(DEVICE_ID);

static const String parameterRequestTopic = String(MQTT_BASE_TOPIC) + "/parameterrequest";

static const String parameterReceiveTopic = String(MQTT_BASE_TOPIC) + "/" + String(DEVICE_ID) + "/sensorparameter";

static const String meassurementTopic = String(MQTT_BASE_TOPIC) + "/" + String(DEVICE_ID) + "/meassurement";

static const String parameterRequest = String(DEVICE_ID);


// the capacity of the device json document
static const size_t deviceCapacity = JSON_OBJECT_SIZE(5) + 80;

// the capacity of the sensor json document
static const size_t sensorCapacity = 2000;       //JSON_OBJECT_SIZE(16) + 230;

static const size_t commandCapacity = 200;

// boot count used to check if battery status should be read
RTC_DATA_ATTR int bootCount = 0;

TaskHandle_t hibernateTaskHandle = NULL;

WiFiClient espClient;
PubSubClient client(espClient);

Sensor sensorArray[3];
Box box;

void callback(char* topic, byte* payload, unsigned int length);

void connectWifi(ArduinoJson::JsonDocument &jsonDocument)
{
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected\n");

  jsonDocument["id"] = DEVICE_ID;
  jsonDocument["ipAddress"] = WiFi.localIP().toString();
  jsonDocument["mac"] = WiFi.macAddress();
  jsonDocument["channel"] = WiFi.channel();
  jsonDocument["rssi"] = WiFi.RSSI();
}

void disconnectWifi()
{
  WiFi.disconnect(true);
  Serial.println("WiFi disonnected");
}

void connectMqtt()
{
  Serial.println("Connecting to MQTT...");
  client.setServer(MQTT_HOST, MQTT_PORT);
  String lwtTopic = deviceBaseTopic + "/lwt";
  String deviceMac = WiFi.macAddress();
  String part = deviceMac.substring(9);
  part.replace(":", "");
  String mqttClientId = String(DEVICE_ID) + "_" + part;

  Serial.printf("- MQTT Client ID: %s\n", mqttClientId.c_str());

  while (!client.connected())
  {
    if (!client.connect(mqttClientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD, lwtTopic.c_str(), 1, true, "offline"))
    //if (!client.connect(mqttClientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD))
    {
      Serial.print("MQTT connection failed:");
      Serial.print(client.state());
      Serial.println("Retrying...");
      delay(MQTT_RETRY_WAIT);
    }
    else
    {
      client.publish(lwtTopic.c_str(), "online", MQTT_RETAIN);
    }
  }
  Serial.println("MQTT connected");
  Serial.println("");
  client.setBufferSize(sensorCapacity);
  Serial.printf("BufferSize set to %d\n", client.getBufferSize());
  client.setCallback(callback);
  Serial.println("Callback set");
}

void disconnectMqtt()
{
  client.publish(String(deviceBaseTopic + "/lwt").c_str(), "offline", MQTT_RETAIN);
  client.disconnect();
  Serial.println("MQTT disconnected");
}

BLEClient *getFloraClient(BLEAddress floraAddress)
{
  BLEClient *floraClient = BLEDevice::createClient();

  if (!floraClient->connect(floraAddress))
  {
    Serial.println("- Connection failed, skipping");
    return nullptr;
  }
  Serial.println("- Connection successful");
  return floraClient;
}

BLERemoteService *getFloraService(BLEClient *floraClient)
{
  BLERemoteService *floraService = nullptr;

  try
  {
    floraService = floraClient->getService(serviceUUID);
  }
  catch (...)
  {
    // something went wrong
  }
  if (floraService == nullptr)
  {
    Serial.println("- Failed to find data service");
  }
  else
  {
    Serial.println("- Found data service");
  }

  return floraService;
}

bool forceFloraServiceDataMode(BLERemoteService *floraService)
{
  BLERemoteCharacteristic *floraCharacteristic;

  // get device mode characteristic, needs to be changed to read data
  Serial.println("- Force device in data mode");
  floraCharacteristic = nullptr;
  try
  {
    floraCharacteristic = floraService->getCharacteristic(uuid_write_mode);
  }
  catch (...)
  {
    // something went wrong
  }
  if (floraCharacteristic == nullptr)
  {
    Serial.println("-- Failed, skipping device");
    return false;
  }

  // write the magic data
  uint8_t buf[2] = {0xA0, 0x1F};
  floraCharacteristic->writeValue(buf, 2, true);

  delay(500);
  return true;
}

byte calculateMeasurementLevel(int current, int min, int max)
{
  if (current <= min)
  {
    return 0; // low
  }
  else if (current >= max)
  {
    return 2; // high
  }
  else
  {
    return 1; // medium
  }
}

bool readFloraDataCharacteristic(BLERemoteService *floraService, ArduinoJson::JsonDocument &jsonDocument, Sensor sensor)
{
  BLERemoteCharacteristic *floraCharacteristic = nullptr;

  // get the main device data characteristic
  Serial.println("- Access characteristic from device");
  try
  {
    floraCharacteristic = floraService->getCharacteristic(uuid_sensor_data);
  }
  catch (...)
  {
    // something went wrong
  }
  if (floraCharacteristic == nullptr)
  {
    Serial.println("-- Failed, skipping device");
    return false;
  }

  // read characteristic value
  Serial.println("- Read value from characteristic");
  std::string value;
  try
  {
    value = floraCharacteristic->readValue();
  }
  catch (...)
  {
    // something went wrong
    Serial.println("-- Failed, skipping device");
    return false;
  }
  const char *val = value.c_str();

  Serial.print("Hex: ");
  for (int i = 0; i < 16; i++)
  {
    Serial.print((int)val[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");

  int16_t *temp_raw = (int16_t *)val;
  float temperature = (*temp_raw) / ((float)10.0);
  sensor.settemperature(temperature);
  Serial.printf("-- Temperature: %f\n", sensor.gettemperature());

  int moisture = val[7];
  sensor.setmoisture(moisture);
  Serial.printf("-- Moisture: %d\n", moisture);

  int light = val[3] + val[4] * 256;
  sensor.setlight(light);
  Serial.printf("-- Light: %d\n", light);

  int conductivity = val[8] + val[9] * 256;
  sensor.setconductivity(conductivity);
  Serial.printf("-- Conductivity: %d\n", conductivity);

  if (temperature < -20 || temperature > 50)
  {
    Serial.println("-- Unreasonable values received for temperature, skip publish");
    return false;
  }

  if (moisture < 0 || moisture > 100)
  {
    Serial.println("-- Unreasonable values received for moisture, skip publish");
    return false;
  }

  if (light < 0)
  {
    Serial.println("-- Unreasonable values received for light, skip publish");
    return false;
  }

  if (conductivity < 0 || conductivity > 5000)
  {
    Serial.println("-- Unreasonable values received for conductivity, skip publish");
    return false;
  }

  // Create an array for sensors
  JsonArray sensorJsonArray = jsonDocument.createNestedArray("sensors");

  // Create and add the first sensor object
  
  JsonObject sensorJsonObject = sensorJsonArray.createNestedObject();
    sensorJsonObject["Mac"] = sensor.getMac();
    sensorJsonObject["temperature"] = sensor.gettemperature();
    sensorJsonObject["ph"] = sensor.getph();
    sensorJsonObject["moisture"] =sensor.getmoisture();
    sensorJsonObject["light"] = sensor.getlight();
    sensorJsonObject["conductivity"] = sensor.getconductivity();
    sensorJsonObject["battery"] = sensor.getbattery();

  return true;
}

bool readFloraBatteryCharacteristic(BLERemoteService *floraService, ArduinoJson::JsonDocument &jsonDocument)
{
  BLERemoteCharacteristic *floraCharacteristic = nullptr;

  // get the device battery characteristic
  Serial.println("- Access battery characteristic from device");
  try
  {
    floraCharacteristic = floraService->getCharacteristic(uuid_version_battery);
  }
  catch (...)
  {
    // something went wrong
  }
  if (floraCharacteristic == nullptr)
  {
    Serial.println("-- Failed, skipping battery level");
    return false;
  }

  // read characteristic value
  Serial.println("- Read value from characteristic");
  std::string value;
  try
  {
    value = floraCharacteristic->readValue();
  }
  catch (...)
  {
    // something went wrong
    Serial.println("-- Failed, skipping battery level");
    return false;
  }
  const char *val2 = value.c_str();
  int battery = val2[0];

  jsonDocument["battery"] = battery;
  jsonDocument["batteryLow"] = battery <= BATTERY_THRESHOLD_LOW;
  jsonDocument["batteryLevel"] = calculateMeasurementLevel(battery, String(BATTERY_THRESHOLD_LOW).toInt(), String(BATTERY_THRESHOLD_MED).toInt());

  return true;
}

bool processFloraService(BLERemoteService *floraService, bool readBattery, ArduinoJson::JsonDocument &jsonDocument, Sensor sensor)
{
  // set device in data mode
  if (!forceFloraServiceDataMode(floraService))
  {
    return false;
  }

  bool dataSuccess = readFloraDataCharacteristic(floraService, jsonDocument, sensor);

  bool batterySuccess = true;
  if (readBattery)
  {
    batterySuccess = readFloraBatteryCharacteristic(floraService, jsonDocument);
  }

  return dataSuccess && batterySuccess;
}

bool processFloraDevice(BLEAddress floraAddress, bool getBattery, int tryCount, ArduinoJson::JsonDocument &jsonDocument, Sensor sensor)
{
  Serial.print("Processing Flora device at ");
  Serial.print(floraAddress.toString().c_str());
  Serial.print(" (try ");
  Serial.print(tryCount);
  Serial.println(")");

  // connect to flora ble server
  BLEClient *floraClient = getFloraClient(floraAddress);
  if (floraClient == nullptr)
  {
    return false;
  }

  // connect data service
  BLERemoteService *floraService = getFloraService(floraClient);
  if (floraService == nullptr)
  {
    floraClient->disconnect();
    return false;
  }

  Serial.printf("- %s has ble RSSI of %d\n", floraAddress.toString().c_str(), floraClient->getRssi());  
  jsonDocument["rssi"] = floraClient->getRssi();

  // process devices data
  bool success = processFloraService(floraService, getBattery, jsonDocument, sensor);

  // disconnect from device
  floraClient->disconnect();

  return success;
}

void hibernate()
{
  esp_sleep_enable_timer_wakeup(SLEEP_DURATION * 1000000ll);
  Serial.println("Going to sleep now.");
  delay(100);
  esp_deep_sleep_start();
}

void hibernateAfterIrrigation()
{
  esp_sleep_enable_timer_wakeup(20 * 60  * 1000000ll);
  Serial.println("Going to sleep now.");
  delay(100);
  esp_deep_sleep_start();
}

void delayedHibernate(void *parameter)
{
  delay(EMERGENCY_HIBERNATE * 1000); // delay
  Serial.println("Something got stuck, entering emergency hibernate...");
  hibernate();
}


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

  message.trim();

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

  if (!sensor_conf.is<JsonArray>()) {
    Serial.println("Error: JSON root is not an array.");
    return;
  }

  // Get the array
  JsonArray sensor_conf_a = sensor_conf.as<JsonArray>();

  deviceCount = sensor_conf_a.size();
  Serial.printf("Received parameters for %d sensors\n", deviceCount);

  // Iterate through the array
  for (int i = 0; i < deviceCount; i++) {
    JsonObject sensorData = sensor_conf_a[i];

    sensorArray[i].setMac(sensorData["Mac"]);
    sensorArray[i].setPot(sensorData["Pot"]);
    sensorArray[i].setMinPh(sensorData["MinPh"]);
    sensorArray[i].setMaxPh(sensorData["MaxPh"]);
    sensorArray[i].setMinMoisture(sensorData["MinMoisture"]);
    sensorArray[i].setMaxMoisture(sensorData["MaxMoisture"]);
    sensorArray[i].setMinConductivity(sensorData["MinConductivity"]);
    sensorArray[i].setMaxConductivity(sensorData["MaxConductivity"]);

    Serial.printf("Sensor %d:\n", i + 1);
    Serial.println(sensorArray[i].getMac());
    Serial.println(sensorArray[i].getPot());
    Serial.println(sensorArray[i].getMinPh());
    Serial.println(sensorArray[i].getMaxPh());
    Serial.println(sensorArray[i].getMinMoisture());
    Serial.println(sensorArray[i].getMaxMoisture());
    Serial.println(sensorArray[i].getMinConductivity());
    Serial.println(sensorArray[i].getMaxConductivity());
  }
}


void setup()
{
  // all action is done when device is woken up
  Serial.begin(115200);
  delay(1000);

  // increase boot count
  bootCount++;

  // create a hibernate task in case something gets stuck
  xTaskCreate(delayedHibernate, "hibernate", 4096, NULL, 1, &hibernateTaskHandle);

  // create device Json Document
  DynamicJsonDocument deviceJson(deviceCapacity);

  // connecting wifi and mqtt server
  connectWifi(deviceJson);
  connectMqtt();

  //subscribing to receive sensor parameters
  if(client.subscribe(parameterReceiveTopic.c_str()))
  {
    Serial.print("subscribed to ");
    Serial.printf("%s\n", parameterReceiveTopic.c_str());
  }
  else
  {
    Serial.print("- subscribing to ");
    Serial.printf("%s\n", parameterReceiveTopic.c_str());
    Serial.printf(" failed\n");
  }

  //request for sensor parameters
  if(client.publish(parameterRequestTopic.c_str(), parameterRequest.c_str(), MQTT_RETAIN))
  {
    Serial.print("publishing sensor request to ");
    Serial.printf("%s\n", parameterRequestTopic.c_str());
  }
  else
  {
    Serial.println("- Publishing sensor request failed!");
  }

  while(sensorArray[0].getMac() == ""){
    client.loop();
  }

  client.unsubscribe(parameterReceiveTopic.c_str());

  Serial.println("Initialize BLE client...");
  BLEDevice::init("");
  BLEDevice::setPower(ESP_PWR_LVL_P9);

  // publish device status
  char payload[deviceCapacity];
  serializeJson(deviceJson, payload);
  String deviceStatusTopic = deviceBaseTopic + "/status";

  Serial.printf("Device status: %s\n", payload);
  if (client.publish(deviceStatusTopic.c_str(), payload, MQTT_RETAIN))
  {
    Serial.printf("- Publishing device status -> %s\n", deviceStatusTopic.c_str());
  }
  else
  {
    Serial.println("- Publishing device status failed!");
  }

  // check if battery status should be read - based on boot count
  bool readBattery = ((bootCount % BATTERY_INTERVAL) == 0);

  DynamicJsonDocument sensorJson(sensorCapacity);

    // Add main data
  sensorJson["Mac"] = WiFi.macAddress();
  sensorJson["volmix"] = 60;
  sensorJson["volwater"] = 20;
  sensorJson["volfertilizer"] = 60;
  sensorJson["volacid"] = 60;

  // process devices
  for (int i = 0; i < deviceCount; i++)
  {
    Serial.println();
    int retryCount = 0;

    // create sensor Json Document

    BLEAddress bleAddress(sensorArray[i].getMac().c_str());
    while (retryCount < SENSOR_RETRY)
    {
      retryCount++;
      sensorJson["retryCount"] = retryCount;

      // create sensor topic

      if (processFloraDevice(bleAddress, readBattery, retryCount, sensorJson, sensorArray[i]))
      {
       /*  if(sensorJson["moistureLevel"] == 0){
          addWater(200);
          if(sensorJson["conductivityLevel"] == 0){
            addFertilizer(5);
          }
          checkPH(7);
          waterPlant(sensorArray[i].getPot());
          hibernateAfterIrrigation();
        } */
        char payload[sensorCapacity];
        serializeJson(sensorJson, payload);
        Serial.printf("- Sensor payload: %s\n", payload);
        if (client.publish(meassurementTopic.c_str(), payload, MQTT_RETAIN))
        {
          Serial.printf("-- Publishing --> %s\n", meassurementTopic.c_str());
        }
        else
        {
          Serial.println("-- Publishing failed!");
        }
        break;
      }
      delay(3000); // wait for another try
    }
    delay(2000); // wait for next sensor
  }

  // disconnect mqtt and wifi
  disconnectMqtt();
  disconnectWifi();

  // delete emergency hibernate task
  vTaskDelete(hibernateTaskHandle);

  // go to sleep now
  hibernate();
}

void loop()
{
  /// we're not doing anything in the loop, only on device wakeup
  delay(10000);
}