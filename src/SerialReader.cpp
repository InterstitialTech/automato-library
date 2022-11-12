#include "SerialReader.h"

#include <Arduino.h>

bool SerialReader::read()
{
    while (Serial.available() > 0) {
        uint8_t i = Serial.read();
        switch (serialState) {
            case Ready:
                if (i == 'm') {
                    serialState = ToId;
                    received = 0;
                }
                break;
            case ToId:
                to_id = i;
                serialState = Length;
                break;
            case Length:
                length = i;
                serialState = Msg;
                if (length == 0)
                {
                    serialState = Ready;
                    return true; // if msg length is zero, were done.
                }
                break;
            case Msg:
                if (received < length)
                {
                    mb.buf[received] = i;
                    received++;
                }
                else
                {
                    serialState = Ready;
                    return true;
                }
                break;
        }
    }

    // no message ready yet!
    return false;
}

void writeSerialMessage(uint8_t from_id, Msgbuf &mb)
{
    uint8_t ps = payloadSize(mb.payload);
    Serial.write('m');       // 'm' for message
    Serial.write(from_id);   // from which automato.
    Serial.write(ps);        // payload length
    Serial.write(mb.buf, ps);
}

AutomatoResult Automato::handleSerialMessage(uint8_t to_id, Msgbuf &mb)
{
    if (to_id == rhmesh.thisAddress())
    {
        automato.handleMessage(mb);
        return AutomatoResult(rc_ok);
    }
    else
    {
        // forward to another automato!
        return sendRequest(to_id, mb);
    }
}

// receives and handles remote control messages.
AutomatoResult Automato::doSerial()
{
    if (receiveSerialMessage()) {
        do
        {
            handleSerialMessage(serialReader.to_id, serialReader.mb);
            // write the response back through serial
            writeSerialMessage(serialReader.to_id, serialReader.mb);
        } while (receiveSerialMessage());
        return AutomatoResult(rc_ok);
    }
    else
    {
        return AutomatoResult(rc_no_message_received);
    }
}

bool Automato::receiveSerialMessage()
{
    return serialReader.read();
}
