#ifndef AutomatoSerial_h_INCLUDED
#define AutomatoSerial_h_INCLUDED

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

#endif // AutomatoSerial_h_INCLUDED

