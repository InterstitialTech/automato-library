#include "SerialReader.h"

#include <Arduino.h>

bool SerialReader::read()
{
    while (Serial.available() > 0) {
        uint8_t i = Serial.read();
        switch (serialState) {
            case Ready:
                if (i == 'l') {
                    serialState = LoraId;
                    id_type = Lora;
                    received = 0;
                }
                else if (i == 'e') {
                    serialState = EspNowId;
                    id_type = EspNow;
                    received = 0;
                }
                break;
            case LoraId:
                lora_id = i;
                serialState = Length;
                break;
            case EspNowId:
                serialState = Length;
                if (received < 6)
                {
                  esp_now_id[received] = i;
                    received++;
                }
                else
                {
                    esp_now_id[received] = i; received = 0;
                    serialState = Length;
                }
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

