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

    bool allowRemotePinOutputs;

protected:

public:
    Automato(uint8_t networkid, void *databuf, uint16_t datalen, bool allowRemotePinOutputs);
    void init(float frequency = 915.0, uint8_t power = 13);

    void *databuf;
    uint16_t datalen;

    void clearScreen(void);

    void readTempHumidity();
    float getTemperature();
    float getHumidity();

    static uint64_t macAddress();

    // remote control functions.
    AutomatoResult remoteDigitalWrite(uint8_t network_id, uint8_t pin, uint8_t value);    // HIGH or LOW
    AutomatoResult remoteDigitalRead(uint8_t network_id, uint8_t pin, uint8_t *result);
    AutomatoResult remotePinMode(uint8_t network_id, uint8_t pin, uint8_t mode);    // INPUT, OUTPUT, or INPUT_PULLUP

    AutomatoResult remoteAnalogRead(uint8_t network_id, uint8_t pin, uint16_t *result);

    // 'raw' memory read/write.  use the macros below instead.
    AutomatoResult remoteMemWrite(uint8_t network_id, uint16_t address, uint8_t length, void *value);
    AutomatoResult remoteMemRead(uint8_t network_id, uint16_t address, uint8_t length, void *value);

    AutomatoResult remoteTemperature(uint8_t network_id, float &temperature);
    AutomatoResult remoteHumidity(uint8_t network_id, float &humidity);
    AutomatoResult remoteAutomatoInfo(uint8_t network_id, RemoteInfo &info);

    // receive and handle remote control messages.
    AutomatoResult doRemoteControl();

    // lower level message sending and receiving.
    AutomatoResult sendRequest(uint8_t network_id, Msgbuf &mb);
    AutomatoResult sendReply(uint8_t network_id, Payload &p);
    bool receiveMessage(uint8_t &from_id, Msgbuf &mb);
    AutomatoResult handleRcMessage(uint8_t &from_id, Msgbuf &mb);
};

// read/write from the memory map on a remote Automato.
#define remote_memwrite(dest, struct, field, val) remoteMemWrite(dest, (uint16_t)offsetof(struct, field), (uint8_t)sizeof(struct ::field), (void*)val)
#define remote_memread(dest, struct, field, val) remoteMemRead(dest, (uint16_t)offsetof(struct, field), (uint8_t)sizeof(struct ::field), (void*)val)

#endif /* AUTOMATO_SENSOR */
