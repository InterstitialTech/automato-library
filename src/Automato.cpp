/*

   An Arduino library for the Automato sensor board

 */

#include <Automato.h>
#include <Esp.h>
#include <RH_RF95.h>
#include <AutomatoMsg.h>
#include <RHMesh.h>

RH_RF95 rf95(PIN_LORA_CS, PIN_LORA_IRQ); // Slave select, interrupt pin for Automato Sensor Module.

msgbuf mb;
uint8_t from_id;

// see note in Automato.h
Adafruit_ILI9341 screen(PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RST);

Automato::Automato(uint8_t networkid, void *databuf, uint16_t datalen)
    : rhmesh(rf95, networkid), databuf(databuf), datalen(datalen)
{
}

void Automato::init(float frequency)
{
    rf95.init();

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    // specify by country?
    rf95.setFrequency(915.0);

    // user LED
    pinMode(PIN_LED, OUTPUT);

    // LCD
    pinMode(PIN_LED_LCD, OUTPUT);
    digitalWrite(PIN_LED_LCD, HIGH);
    screen.begin();
    screen.setRotation(1);

    // SHTC3
    Wire.begin();
    shtc3.begin();
}

void Automato::clearScreen(void)
{
    screen.fillScreen(ILI9341_BLACK);
    screen.setTextColor(ILI9341_GREEN);
    screen.setTextSize(2);
    screen.setCursor(0,0);
}

void Automato::readTempHumidity(void)
{
    shtc3.update();
    temperature = shtc3.toDegF();
    humidity = shtc3.toPercent();
}

float Automato::getTemperature(void)
{
    return temperature;
}

float Automato::getHumidity(void)
{
    return humidity;
}

uint64_t Automato::macAddress(void)
{
    return ESP.getEfuseMac();
}

bool Automato::remotePinMode(uint8_t network_id, uint8_t pin, uint8_t mode)
{
    setup_pinmode(mb.payload, pin, mode);
    return sendPayload(network_id, mb.payload);
}

bool Automato::remoteDigitalWrite(uint8_t network_id, uint8_t pin, uint8_t value)
{
    setup_writepin(mb.payload, pin, value);
    return sendPayload(network_id, mb.payload);
}

bool Automato::remoteDigitalRead(uint8_t network_id, uint8_t pin, uint8_t *result)
{
    setup_readpin(mb.payload, pin);
    if (sendPayload(network_id, mb.payload))
    {
        if (mb.payload.type == pt_readpinreply) {
            *result = mb.payload.pinval.state;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool Automato::remoteAnalogRead(uint8_t network_id, uint8_t pin, uint16_t *result)
{
    setup_readanalog(mb.payload, pin);
    if (sendPayload(network_id, mb.payload))
    {
        if (mb.payload.type == pt_readanalogreply) {
            *result = mb.payload.analogpinval.state;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool Automato::remoteMemWrite(uint8_t network_id, uint16_t address, uint8_t length, void *value)
{
    if (!setup_writemem(mb.payload, address, length, value))
        return false;

    if (sendPayload(network_id, mb.payload))
        return true;
    else
        return false;
}

bool Automato::remoteMemRead(uint8_t network_id, uint16_t address, uint8_t length, void *value)
{
    setup_readmem(mb.payload, address, length);

    if (sendPayload(network_id, mb.payload))
    {
        // TODO: use the length of the return message to compute length of data?
        // as a belt-and-suspenders check.

        // TODO support not copying the val to a destination, if desired.  Needless
        // memcpy if you're not going to keep the value.

        if (mb.payload.type == pt_readmemreply) {
            memcpy(value, (void*)&mb.payload.readmemreply.data, length);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool Automato::remoteTemperature(uint8_t network_id, float &temperature)
{
    setup_readtemperature(mb.payload);
    if (sendPayload(network_id, mb.payload))
    {
        if (mb.payload.type == pt_readtemperaturereply) {
            temperature = mb.payload.f;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool Automato::remoteHumidity(uint8_t network_id, float &humidity)
{
    setup_readhumidity(mb.payload);
    if (sendPayload(network_id, mb.payload))
    {
        if (mb.payload.type == pt_readhumidityreply) {
            humidity = mb.payload.f;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool Automato::remoteAutomatoInfo(uint8_t network_id, RemoteInfo &info)
{
    setup_readinfo(mb.payload);
    if (sendPayload(network_id, mb.payload))
    {
        if (mb.payload.type == pt_readinforeply) {
            memcpy((void*)&info, (void*)&mb.payload.remoteinfo, sizeof(RemoteInfo));
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool Automato::sendPayload(uint8_t network_id, Payload &p)
{
    if (rhmesh.sendtoWait((uint8_t*)&p, payloadSize(p), network_id) == RH_ROUTER_ERROR_NONE) {
        uint8_t from_id;

        // mesh already does an Ack behind the scenes, but only between this
        // node and the next.  So we have to do our own ack with the final
        // destination.
        if (receiveMessage(from_id, mb))
        {
            return succeeded(mb.payload);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool Automato::receiveMessage(uint8_t &from_id, msgbuf &mb)
{
    uint8_t len = sizeof(mb.buf); // TODO hardcode for speed.
    // TODO switch to non-timeout?
    mb.payload.f = 0;
    if (rhmesh.recvfromAckTimeout(mb.buf, &len, 1000, &from_id))
    {
        // Serial.print("received message, len: ");
        // Serial.println(len);
        // printPayload(mb.payload);
        return true;
    }
    else
    {
        return false;
    }
}

void Automato::handleRcMessage(uint8_t &from_id, msgbuf &mb)
{
    switch (mb.payload.type) {
        case pt_pinmode:
            if (0 <= mb.payload.pinmode.pin &&  mb.payload.pinmode.pin < 40) {
                pinMode(mb.payload.pinmode.pin, mb.payload.pinmode.mode);
                setup_ack(mb.payload);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            } else {
                // failed, invalid address.
                setup_fail(mb.payload, fc_invalid_pin_number);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            };
            break;
        case pt_writepin:
            if (0 <= mb.payload.pinval.pin &&  mb.payload.pinval.pin < 40) {
                if (mb.payload.pinval.state == 0) {
                    digitalWrite(mb.payload.pinval.pin, LOW);
                    setup_ack(mb.payload);
                    rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
                } else if (mb.payload.pinval.state == 1) {
                    digitalWrite(mb.payload.pinval.pin, HIGH);
                    setup_ack(mb.payload);
                    rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
                }
            } else {
                // failed, invalid address.
                setup_fail(mb.payload, fc_invalid_pin_number);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            };
            break;
        case pt_readpin:
            if (0 <= mb.payload.pin &&  mb.payload.pin < 40) {
                bool val = digitalRead(mb.payload.pin);
                setup_readpinreply(mb.payload, mb.payload.pin, val);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            } else {
                // failed, invalid address.
                setup_fail(mb.payload, fc_invalid_pin_number);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            };
            break;
        case pt_readanalog:
            if (0 <= mb.payload.pin &&  mb.payload.pin < 40) {
                int val = analogRead(mb.payload.pin);
                setup_readanalogreply(mb.payload, mb.payload.pin, val);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            } else {
                // failed, invalid address.
                setup_fail(mb.payload, fc_invalid_pin_number);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            };
            break;
        case pt_readmem:
            // range check.
            if (mb.payload.readmem.address >= this->datalen) {
                // failed, invalid address.
                setup_fail(mb.payload, fc_invalid_mem_address);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            }
            else if (mb.payload.readmem.address + mb.payload.readmem.length >= this->datalen) {
                // failed, invalid length.
                setup_fail(mb.payload, fc_invalid_mem_length);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            } else {
                // build reply and send.
                setup_readmemreply(mb.payload,
                    mb.payload.readmem.length,
                    databuf + mb.payload.readmem.address);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            };
            break;
        case pt_writemem:
            // range check.
            if (mb.payload.readmem.address >= this->datalen) {
                // failed, invalid address.
                setup_fail(mb.payload, fc_invalid_mem_address);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            }
            else if (mb.payload.readmem.address + mb.payload.readmem.length >= this->datalen) {
                // failed, invalid length.
                setup_fail(mb.payload, fc_invalid_mem_length);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            } else {
                memcpy(this->databuf + mb.payload.writemem.address,
                    mb.payload.writemem.data,
                    mb.payload.writemem.length);
                setup_ack(mb.payload);
                rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            };
            break;
        case pt_readinfo:
            setup_readinforeply(mb.payload, protoVersion, macAddress(), datalen);
            rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            break;
        case pt_readhumidity:
            readTempHumidity();
            setup_readhumidityreply(mb.payload, getHumidity());
            rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            break;
        case pt_readtemperature:
            readTempHumidity();
            setup_readtemperaturereply(mb.payload, getTemperature());
            rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            break;
        // error!  These should only be received in response to a request.
        case pt_readhumidityreply:
        case pt_readtemperaturereply:
        case pt_readmemreply:
        case pt_readinforeply:
        default:
            // failed, unsupported message type.
            setup_fail(mb.payload, fc_invalid_message_type);
            rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), from_id);
            break;
    };
}

// receives and handles remote control messages.
void Automato::doRemoteControl()
{
    if (receiveMessage(from_id, mb)) {
        handleRcMessage(from_id, mb);
    }
}


