/*
An Arduino library for the Automato sensor board
*/

#ifndef AUTOMATO_SENSOR
#define AUTOMATO_SENSOR

#include <Arduino.h>

#include <Wire.h>
<<<<<<< HEAD
#include <AutomatoMsg.h>
=======
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
        void init(float frequency = 915.0);

        void clearScreen(void);

        void readTempHumidity();
        float getTemperature();
        float getHumidity();

        static uint64_t macAddress();

        // remote control functions.
        bool remotePinMode(uint64_t destination_mac, uint8_t pin, uint8_t mode);
        bool remoteDigitalWrite(uint64_t destination_mac, uint8_t pin, uint8_t value);
        bool remoteDigitalRead(uint64_t destination_mac, uint8_t pin, uint8_t *result);

        // receive and handle remote control messages.
        void doRemoteControl();

        // lower level message sending and receiving.
        bool sendMessage(message &m);
        bool receiveMessage(msgbuf &mb);
        void handleRcMessage(msgbuf &mb);
};

#endif /* AUTOMATO_SENSOR */
