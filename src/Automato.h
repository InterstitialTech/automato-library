/*
An Arduino library for the Automato sensor board
*/

#ifndef AUTOMATO_SENSOR
#define AUTOMATO_SENSOR

#include <Arduino.h>

#include <Wire.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "SparkFun_SHTC3.h"

// this *should* be a member of the Automato class, but that's currently
// impossible because Adafruit_ILI9341 does not define a default constructor
// so for now, declare it globally
extern Adafruit_ILI9341 screen;

class Automato {

    private:
        SHTC3 shtc3;
        float temperature, humidity;

    protected:

    public:
        Automato();
        void init();

        void clearScreen(void);

        void readTempHumidity();
        float getTemperature();
        float getHumidity();

        static uint64_t macAddress();

};

#endif /* AUTOMATO_SENSOR */
