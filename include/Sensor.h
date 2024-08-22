#include <Arduino.h>

class Sensor
{
private:
    String mac;
    int location;
    int minTemperature;
    int maxTemperature;
    int minMoisture;
    int maxMoisture;
    int minLight;
    int maxLight;
    int minConductivity;
    int maxConductivity;

public:
    String getMac()
    {
        return this->mac;
    }

    void setMac(String mac)
    {
        this->mac = mac;
    }

    int getLocation()
    {
        return this->location;
    }

    void setLocation(int location)
    {
        this->location = location;
    }

    int getMinTemperature()
    {
        return this->minTemperature;
    }

    void setMinTemperature(int minTemperature)
    {
        this->minTemperature = minTemperature;
    }

    int getMaxTemperature()
    {
        return this->maxTemperature;
    }

    void setMaxTemperature(int maxTemperature)
    {
        this->maxTemperature = maxTemperature;
    }

    int getMinMoisture()
    {
        return this->minMoisture;
    }

    void setMinMoisture(int minMoisture)
    {
        this->minMoisture = minMoisture;
    }

    int getMaxMoisture()
    {
        return this->maxMoisture;
    }

    void setMaxMoisture(int maxMoisture)
    {
        this->maxMoisture = maxMoisture;
    }

    int getMinLight()
    {
        return this->minLight;
    }

    void setMinLight(int minLight)
    {
        this->minLight = minLight;
    }

    int getMaxLight()
    {
        return this->maxLight;
    }

    void setMaxLight(int maxLight)
    {
        this->maxLight = maxLight;
    }

    int getMinConductivity()
    {
        return this->minConductivity;
    }

    void setMinConductivity(int minConductivity)
    {
        this->minConductivity = minConductivity;
    }

    int getMaxConductivity()
    {
        return this->maxConductivity;
    }

    void setMaxConductivity(int maxConductivity)
    {
        this->maxConductivity = maxConductivity;
    }
};