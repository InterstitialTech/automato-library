/*

   An Arduino library for the Automato sensor board

 */

#include <Automato.h>
#include <Esp.h>
#include <RH_RF95.h>
#include <AutomatoMsg.h>
#include <RHMesh.h>

RH_RF95 rf95(PIN_LORA_CS, PIN_LORA_IRQ); // Slave select, interrupt pin for Automato Sensor Module.

Msgbuf mb;
uint8_t from_id;

// see note in Automato.h
Adafruit_ILI9341 screen(PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RST);

// convert a RH router code to AutomatoResult.
AutomatoResult arFromRc(uint8_t rc)
{
    switch (rc) {
        case RH_ROUTER_ERROR_NONE:
            return AutomatoResult(rc_ok);
        case RH_ROUTER_ERROR_INVALID_LENGTH:
            return AutomatoResult(rc_rh_router_error_invalid_length);
        case RH_ROUTER_ERROR_NO_ROUTE:
            return AutomatoResult(rc_rh_router_error_no_route);
        case RH_ROUTER_ERROR_TIMEOUT:
            return AutomatoResult(rc_rh_router_error_timeout);
        case RH_ROUTER_ERROR_NO_REPLY:
            return AutomatoResult(rc_rh_router_error_no_reply);
        case RH_ROUTER_ERROR_UNABLE_TO_DELIVER:
            return AutomatoResult(rc_rh_router_error_unable_to_deliver);
        default:
            return AutomatoResult(rc_invalid_rh_router_error);
    }
}

Automato::Automato(uint8_t networkid, void *databuf, uint16_t datalen, bool allowRemotePinOutputs, float frequency)
    : rhmesh(rf95, networkid), databuf(databuf), datalen(datalen), allowRemotePinOutputs(allowRemotePinOutputs)
{
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    // specify by country?
    rf95.setFrequency(frequency);
}

void Automato::init()
{
    rf95.init();

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

AutomatoResult Automato::remotePinMode(uint8_t network_id, uint8_t pin, uint8_t mode)
{
    setup_pinmode(mb.payload, pin, mode);
    return sendRequestPayload(network_id, mb);
}

AutomatoResult Automato::remoteDigitalWrite(uint8_t network_id, uint8_t pin, uint8_t value)
{
    setup_writepin(mb.payload, pin, value);
    return sendRequestPayload(network_id, mb);
}

AutomatoResult Automato::remoteDigitalRead(uint8_t network_id, uint8_t pin, uint8_t *result)
{
    setup_readpin(mb.payload, pin);
    auto ar = sendRequestPayload(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readpinreply) {
            *result = mb.payload.pinval.state;
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult Automato::remoteAnalogRead(uint8_t network_id, uint8_t pin, uint16_t *result)
{
    setup_readanalog(mb.payload, pin);
    auto ar = sendRequestPayload(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readanalogreply) {
            *result = mb.payload.analogpinval.state;
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult Automato::remoteMemWrite(uint8_t network_id, uint16_t address, uint8_t length, void *value)
{
    AutomatoResult ar;
    bool tmp = (ar = setup_writemem(mb.payload, address, length, value))
        && (ar = sendRequestPayload(network_id, mb));

    return ar;
}

AutomatoResult Automato::remoteMemRead(uint8_t network_id, uint16_t address, uint8_t length, void *value)
{
    AutomatoResult ar;

    if ((ar = setup_readmem(mb.payload, address, length)) && (ar = sendRequestPayload(network_id, mb)))
    {
        // TODO: use the length of the return message to compute length of data?
        // as a belt-and-suspenders check.

        // TODO support not copying the val to a destination, if desired.  Needless
        // memcpy if you're not going to keep the value.

        if (mb.payload.type == pt_readmemreply) {
            memcpy(value, (void*)&mb.payload.readmemreply.data, length);
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult Automato::remoteTemperature(uint8_t network_id, float &temperature)
{
    setup_readtemperature(mb.payload);
    auto ar = sendRequestPayload(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readtemperaturereply) {
            temperature = mb.payload.f;
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult Automato::remoteHumidity(uint8_t network_id, float &humidity)
{
    setup_readhumidity(mb.payload);
    auto ar = sendRequestPayload(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readhumidityreply) {
            humidity = mb.payload.f;
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult Automato::remoteAutomatoInfo(uint8_t network_id, RemoteInfo &info)
{
    setup_readinfo(mb.payload);
    auto ar = sendRequestPayload(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readinforeply) {
            memcpy((void*)&info, (void*)&mb.payload.remoteinfo, sizeof(RemoteInfo));
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult Automato::sendRequestPayload(uint8_t network_id, Msgbuf &mb)
{
    uint8_t err;
    if ((err = rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), network_id)) == RH_ROUTER_ERROR_NONE) {
        uint8_t from_id;

        // mesh already does an Ack behind the scenes, but only between this
        // node and the next.  So we have to do our own ack with the final
        // destination.
        if (receiveMessage(from_id, mb))
        {
            return ArFromReply(mb.payload);
        }
        else
        {
            return AutomatoResult(rc_reply_timeout);
        }
    }
    else
    {
        return arFromRc(err);
    }
}

AutomatoResult Automato::sendReplyPayload(uint8_t network_id, Payload &p)
{
    return arFromRc(rhmesh.sendtoWait((uint8_t*)&p, payloadSize(p), network_id));
}

bool Automato::receiveMessage(uint8_t &from_id, Msgbuf &mb)
{
    uint8_t len = sizeof(mb.buf); // TODO hardcode for speed.
    // TODO switch to non-timeout?
    mb.payload.f = 0;
    if (rhmesh.recvfromAckTimeout(mb.buf, &len, 1000, &from_id))
    {
        return true;
    }
    else
    {
        return false;
    }
}


AutomatoResult Automato::handleRcMessage(uint8_t &from_id, Msgbuf &mb)
{
    switch (mb.payload.type) {
        case pt_readpin:
            if (0 <= mb.payload.pin &&  mb.payload.pin < 40) {
                bool val = digitalRead(mb.payload.pin);
                setup_readpinreply(mb.payload, mb.payload.pin, val);
                return sendReplyPayload(from_id, mb.payload);
            } else {
                // failed, invalid address.
                setup_fail(mb.payload, rc_invalid_pin_number);
                return sendReplyPayload(from_id, mb.payload);
            };
        case pt_pinmode:
            if (this->allowRemotePinOutputs) {
                if (0 <= mb.payload.pinmode.pin &&  mb.payload.pinmode.pin < 40) {
                    pinMode(mb.payload.pinmode.pin, mb.payload.pinmode.mode);
                    setup_ack(mb.payload);
                    return sendReplyPayload(from_id, mb.payload);
                } else {
                    // failed, invalid address.
                    setup_fail(mb.payload, rc_invalid_pin_number);
                    return sendReplyPayload(from_id, mb.payload);
                };
            } else
            {
                // failed, pin ops not allowed.
                setup_fail(mb.payload, rc_operation_forbidden);
                return sendReplyPayload(from_id, mb.payload);
            }
        case pt_writepin:
            if (this->allowRemotePinOutputs) {
                if (0 <= mb.payload.pinval.pin &&  mb.payload.pinval.pin < 40) {
                    if (mb.payload.pinval.state == 0) {
                        digitalWrite(mb.payload.pinval.pin, LOW);
                        setup_ack(mb.payload);
                        return sendReplyPayload(from_id, mb.payload);
                    } else if (mb.payload.pinval.state == 1) {
                        digitalWrite(mb.payload.pinval.pin, HIGH);
                        setup_ack(mb.payload);
                        return sendReplyPayload(from_id, mb.payload);
                    }
                } else {
                    // failed, invalid address.
                    setup_fail(mb.payload, rc_invalid_pin_number);
                    return sendReplyPayload(from_id, mb.payload);
                };
            } else {
                // failed, pin ops not allowed.
                setup_fail(mb.payload, rc_operation_forbidden);
                return sendReplyPayload(from_id, mb.payload);
            }
        case pt_readanalog:
            if (0 <= mb.payload.pin &&  mb.payload.pin < 40) {
                int val = analogRead(mb.payload.pin);
                setup_readanalogreply(mb.payload, mb.payload.pin, val);
                return sendReplyPayload(from_id, mb.payload);
            } else {
                // failed, invalid address.
                setup_fail(mb.payload, rc_invalid_pin_number);
                return sendReplyPayload(from_id, mb.payload);
            };
        case pt_readmem:
            // range check.
            if (mb.payload.readmem.address >= this->datalen) {
                // failed, invalid address.
                setup_fail(mb.payload, rc_invalid_mem_address);
                return sendReplyPayload(from_id, mb.payload);
            }
            else if (mb.payload.readmem.address + mb.payload.readmem.length >= this->datalen) {
                // failed, invalid length.
                setup_fail(mb.payload, rc_invalid_mem_length);
                return sendReplyPayload(from_id, mb.payload);
            } else {
                // build reply and send.
                setup_readmemreply(mb.payload,
                    mb.payload.readmem.length,
                    databuf + mb.payload.readmem.address);
                return sendReplyPayload(from_id, mb.payload);
            };
        case pt_writemem:
            // range check.
            if (mb.payload.readmem.address >= this->datalen) {
                // failed, invalid address.
                setup_fail(mb.payload, rc_invalid_mem_address);
                return sendReplyPayload(from_id, mb.payload);
            }
            else if (mb.payload.readmem.address + mb.payload.readmem.length >= this->datalen) {
                // failed, invalid length.
                setup_fail(mb.payload, rc_invalid_mem_length);
                return sendReplyPayload(from_id, mb.payload);
            } else {
                memcpy(this->databuf + mb.payload.writemem.address,
                    mb.payload.writemem.data,
                    mb.payload.writemem.length);
                setup_ack(mb.payload);
                return sendReplyPayload(from_id, mb.payload);
            };
        case pt_readinfo:
            setup_readinforeply(mb.payload, protoVersion, macAddress(), datalen);
            return sendReplyPayload(from_id, mb.payload);
        case pt_readhumidity:
            readTempHumidity();
            setup_readhumidityreply(mb.payload, getHumidity());
            return sendReplyPayload(from_id, mb.payload);
        case pt_readtemperature:
            readTempHumidity();
            setup_readtemperaturereply(mb.payload, getTemperature());
            return sendReplyPayload(from_id, mb.payload);
        // error!  These should only be received in response to a request.
        case pt_readhumidityreply:
        case pt_readtemperaturereply:
        case pt_readmemreply:
        case pt_readinforeply:
        default:
            // failed, unsupported message type.
            setup_fail(mb.payload, rc_invalid_message_type);
            return sendReplyPayload(from_id, mb.payload);
    };
}

// receives and handles remote control messages.
AutomatoResult Automato::doRemoteControl()
{
    if (receiveMessage(from_id, mb)) {
        return handleRcMessage(from_id, mb);
    }
    else
    {
        return AutomatoResult(rc_no_message_received);
    }
}
