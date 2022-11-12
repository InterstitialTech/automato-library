#ifndef AutomatoUdp_h_INCLUDED
#define AutomatoUdp_h_INCLUDED



#include <Arduino.h>
#include <AutomatoMsg.h>
#include <RHMesh.h>

class AutomatoUdp : public AutomatoProtocol {

private:
    RHMesh rhmesh;
    Automato &automato;
public:
    // Simplest AutomatoUdp constructor.
    AutomatoUdp(uint8_t networkid, Automato &automato);

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

}


#endif // AutomatoUdp_h_INCLUDED

