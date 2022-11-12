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


// this *should* be a member of the Automato class, but that's currently
// impossible because Adafruit_ILI9341 does not define a default constructor
// so for now, declare it globally
extern Adafruit_ILI9341 screen;

class Automato {

private:
    SHTC3 shtc3;

    float temperature, humidity;

    bool allowRemotePinOutputs;

public:
    // Simplest Automato constructor.
    Automato(bool allowRemotePinOutputs = false);
    // Add a data area that can be read or written remotely.
    Automato(void *databuf,
        uint16_t datalen,
        bool allowRemotePinOutputs);
    // Data area as above, but also a remotely-accessible memory map.
    Automato(void *databuf,
        uint16_t datalen,
        void *mapentries,
        uint16_t mapentrycount,
        bool allowRemotePinOutputs);

    void *databuf;
    uint16_t datalen;

    void *memoryMap;
    uint64_t fieldCount;

    void clearScreen(void);

    void readTempHumidity();
    float getTemperature();
    float getHumidity();

    static uint64_t macAddress();

    void handleMessage(Msgbuf &mb);
};



// read/write from the memory map on a remote Automato.
#define remote_memwrite(dest, struct, field, val) remoteMemWrite(dest, (uint16_t)offsetof(struct, field), (uint8_t)sizeof(struct ::field), (void*)val)
#define remote_memread(dest, struct, field, val) remoteMemRead(dest, (uint16_t)offsetof(struct, field), (uint8_t)sizeof(struct ::field), (void*)val)


// define memory map fields 'documentation' for remotes to read.
#define map_field(struct, field, format) MapField { #field, offsetof(struct, field), sizeof(struct ::field), format }


#endif /* AUTOMATO_SENSOR */
