#include "SerialReader.h"

#include <Arduino.h>

bool SerialReader::read()
{
    while (Serial.available() > 0) {
        uint8_t i = Serial.read();
        // Serial.print("serialreader: '");
        // Serial.print(i);
        // Serial.println("'");
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
                    // Serial.print("recieved: ");
                    // Serial.println(i);
                    mb.buf[received] = i;
                    received++;
                }
                else
                {
                    // Serial.print("ready: ");
                    serialState = Ready;
                    return true;
                }
                break;
        }
    }

    // no message ready yet!
    return false;
}
