#ifndef SerialReader_h_INCLUDED
#define SerialReader_h_INCLUDED

#include "AutomatoMsg.h"

enum SerialState {
    Ready,
    LoraId,
    EspNowId,
    Length,
    Msg
};

enum IdType {
    Lora,
    EspNow,
};

class SerialReader {
public:
    SerialReader() : serialState(Ready), length(0), id_type(Lora), lora_id(0), esp_now_id({0}), received(0) {}
    SerialState serialState;
    int8_t length;
    int8_t received;

    IdType id_type;
    uint8_t lora_id;
    uint8_t esp_now_id[6];

    Msgbuf mb;

    bool read();
};

#endif // SerialReader_h_INCLUDED
