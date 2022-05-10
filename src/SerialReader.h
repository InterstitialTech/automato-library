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
    int8_t length;
    int8_t received;
    int8_t to_id;

    Msgbuf mb;

    bool read();
};

#endif // SerialReader_h_INCLUDED
