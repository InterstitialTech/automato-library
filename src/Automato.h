/*
An Arduino library for the Automato sensor board
*/

#ifndef AUTOMATO_SENSOR
#define AUTOMATO_SENSOR

#include <Arduino.h>

#include <Wire.h>
#include <AutomatoMsg.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "SparkFun_SHTC3.h"
#include <RHMesh.h>


// this *should* be a member of the Automato class, but that's currently
// impossible because Adafruit_ILI9341 does not define a default constructor
// so for now, declare it globally
extern Adafruit_ILI9341 screen;

class Automato {

    private:
        SHTC3 shtc3;
        float temperature, humidity;

        RHMesh rhmesh;

    protected:

    public:
        Automato(uint8_t networkid = -1);
        void init(float frequency = 915.0);

        void clearScreen(void);

        void readTempHumidity();
        float getTemperature();
        float getHumidity();

        static uint64_t macAddress();

        // remote control functions.
        // bool remotePinMode(uint8_t destination_id, uint8_t pin, uint8_t mode);
        bool remoteDigitalWrite(uint8_t destination_id, uint8_t pin, uint8_t value);
        bool remoteDigitalRead(uint8_t destination_id, uint8_t pin, uint8_t *result);

        // receive and handle remote control messages.
        void doRemoteControl();

        // lower level message sending and receiving.
        bool sendPayload(uint8_t destination_id, Payload &p);
        // bool sendPayload(uint8_t destination_id, Payload &p);
        // bool sendMessage(uint8_t destination_id, Message &m);
        // bool receiveMessage(uint8_t &from_id, msgbuf &mb);
        // void handleRcMessage(uint8_t &from_id, msgbuf &mb);
        bool receiveMessage(uint8_t &from_id, msgbuf &mb);
        void handleRcMessage(uint8_t &from_id, msgbuf &mb);
};
// write pin
// write analog pin
// write byte
// write int
// write array
// write named int
// write named array
// 
// read pin
// read analog pin
// read byte
// read int
// read array
// read named int
// read named array
// 

#endif /* AUTOMATO_SENSOR */
