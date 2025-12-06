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
                // Serial.print("id");
                // Serial.println(i);
                if (received < 5)
                {
                    esp_now_id[received] = i;
                    received++;
                }
                else
                {
                    esp_now_id[received] = i;
                    serialState = Length;
                }
                break;
            case Length:
                length = i;
                // Serial.print("len");
                // Serial.print(i);
                serialState = Msg;
                received = 0;
                if (length == 0)
                {
                    serialState = Ready;
                    return true; // if msg length is zero, were done.
                }
                break;
            case Msg:
                if (received < length)
                {
                    // Serial.print("rec");
                    // Serial.print(i);
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
        // Serial.print("av");
        // Serial.println(Serial.available());
    }

    // Serial.println("readover");

    // no message ready yet!
    return false;
}

