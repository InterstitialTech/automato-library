#ifndef SerialReader_h_INCLUDED
#define SerialReader_h_INCLUDED

#include "AutomatoMsg.h"

enum SerialState {
    Ready,
    ToId,
    Length,
    Msg
};

class SerialReader {
public:
    SerialReader() : serialState(Ready), length(0), to_id(0), received(0) {}
    SerialState serialState;
    uint8_t length;
    uint8_t received;
    // int8_t to_id;
    AutomatoAddress to_address;

    Msgbuf mb;

    bool read();
};


// FIX to handle non-lora addresses

class AutomatoSerial {
public: 
    AutomatoSerial(Automato &automato);

    Automato &automato;
    SerialReader serialReader;

    // receive and handle serial messages.
    AutomatoResult doSerial();
    bool receiveSerialMessage();
    AutomatoResult handleSerialMessage(uint8_t to_id, Msgbuf &mb);
}

void writeSerialMessage(uint8_t from_id, Msgbuf &mb);

#endif // SerialReader_h_INCLUDED
