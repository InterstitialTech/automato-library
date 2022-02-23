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
        Automato(uint8_t networkid, void *databuf, uint16_t datalen);
        void init(float frequency = 915.0);

        void *databuf;
        uint16_t datalen;

        void clearScreen(void);

        void readTempHumidity();
        float getTemperature();
        float getHumidity();

        static uint64_t macAddress();

        // TODO rename destination_id -> network_id
        // remote control functions.
        bool remoteDigitalWrite(uint8_t network_id, uint8_t pin, uint8_t value);
        bool remoteDigitalRead(uint8_t network_id, uint8_t pin, uint8_t *result);
        bool remotePinMode(uint8_t network_id, uint8_t pin, uint8_t mode);

        bool remoteAnalogRead(uint8_t network_id, uint8_t pin, uint16_t *result);

        bool remoteMemWrite(uint8_t network_id, uint16_t address, uint8_t length, void *value);
        bool remoteMemRead(uint8_t network_id, uint16_t address, uint8_t length, void *value);

        bool remoteTemperature(uint8_t network_id, float &temperature);
        bool remoteHumidity(uint8_t network_id, float &humidity);

        bool remoteAutomatoInfo(uint8_t network_id, RemoteInfo &info);

        // receive and handle remote control messages.
        void doRemoteControl();

        // lower level message sending and receiving.
        bool sendPayload(uint8_t network_id, Payload &p);
        bool receiveMessage(uint8_t &from_id, msgbuf &mb);
        void handleRcMessage(uint8_t &from_id, msgbuf &mb);
};


#define remote_memwrite(dest, struct, field, val) remoteMemWrite(dest, (uint16_t)offsetof(struct, field), (uint8_t)sizeof(struct::field), (void*)val)
#define remote_memread(dest, struct, field, val) remoteMemRead(dest, (uint16_t)offsetof(struct, field), (uint8_t)sizeof(struct::field), (void*)val)
 
#endif /* AUTOMATO_SENSOR */
