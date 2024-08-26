#include <Arduino.h>

class Box{

private:

    int volMix;
    int volWater;
    int volFertilizer;
    int volAcid;

public:

    int getvolMix()
    {
        return this->volMix;
    }

    void setvolMix(int volMix)
    {
        this->volMix = volMix;
    }

    int getvolWater()
    {
        return this->volWater;
    }

    void setvolWater(int volWater)
    {
        this->volWater = volWater;
    }

    int getvolFertilizer()
    {
        return this->volFertilizer;
    }

    void setvolFertilizer(int volFertilizer)
    {
        this->volFertilizer = volFertilizer;
    }

    int getvolAcid()
    {
        return this->volAcid;
    }

    void setvolAcid(int volAcid)
    {
        this->volAcid = volAcid;
    }
};