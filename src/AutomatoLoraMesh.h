#ifndef AutomatoLoraMesh_h
#define AutomatoLoraMesh_h

#include <Arduino.h>
#include <AutomatoMsg.h>
#include <RHMesh.h>

class AutomatoLoraMesh : public AutomatoProtocol {

private:
    RHMesh rhmesh;
    Automato &automato;
public:
    // Simplest AutomatoLoraMesh constructor.
    AutomatoLoraMesh(uint8_t networkid, Automato &automato);

    void init(float frequency = 915.0, uint8_t power = 13);

    // remote control functions.

    // digital pins
    AutomatoResult remoteDigitalWrite(uint8_t network_id, uint8_t pin, uint8_t value);    // HIGH or LOW
    AutomatoResult remoteDigitalRead(uint8_t network_id, uint8_t pin, uint8_t *result);
    AutomatoResult remotePinMode(uint8_t network_id, uint8_t pin, uint8_t mode);    // INPUT, OUTPUT, or INPUT_PULLUP

    // Analog pins
    AutomatoResult remoteAnalogRead(uint8_t network_id, uint8_t pin, uint16_t *result);

    // 'raw' memory read/write.  use the macros below instead.
    AutomatoResult remoteMemWrite(uint8_t network_id, uint16_t address, uint8_t length, void *value);
    AutomatoResult remoteMemRead(uint8_t network_id, uint16_t address, uint8_t length, void *value);

    AutomatoResult remoteTemperature(uint8_t network_id, float &temperature);
    AutomatoResult remoteHumidity(uint8_t network_id, float &humidity);
    AutomatoResult remoteAutomatoInfo(uint8_t network_id, RemoteInfo &info);

    // receive and handle remote control messages.
    AutomatoResult doRemoteControl();

    // receive and handle serial messages.
    // AutomatoResult doSerial();

    // lower level message sending and receiving.
    AutomatoResult sendRequest(uint8_t network_id, Msgbuf &mb);
    AutomatoResult sendReply(uint8_t network_id, Payload &p);

    bool receiveLoraMessage(uint8_t &from_id, Msgbuf &mb);
    bool receiveLoraMessage(uint8_t &from_id, Msgbuf &mb);
    // bool receiveSerialMessage();
    AutomatoResult handleLoraMessage(uint8_t from_id, Msgbuf &mb);
    // AutomatoResult handleSerialMessage(uint8_t to_id, Msgbuf &mb);
    // void handleMessage(Msgbuf &mb);

};


// read/write from the memory map on a remote Automato.
#define remote_memwrite(dest, struct, field, val) remoteMemWrite(dest, (uint16_t)offsetof(struct, field), (uint8_t)sizeof(struct ::field), (void*)val)
#define remote_memread(dest, struct, field, val) remoteMemRead(dest, (uint16_t)offsetof(struct, field), (uint8_t)sizeof(struct ::field), (void*)val)


// define memory map fields 'documentation' for remotes to read.
// #define map_field(struct, field, format) MapField { #field, offsetof(struct, field), sizeof(struct ::field), format }


#endif
