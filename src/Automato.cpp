/*

   An Arduino library for the Automato sensor board

 */

#include <Automato.h>
#include <Esp.h>
#include <RH_RF95.h>
#include <AutomatoMsg.h>
#include <RHMesh.h>

// Msgbuf mb;
// uint8_t from_id;

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

Automato::Automato(bool allowRemotePinOutputs)
    databuf(nullptr),
    datalen(0),
    memoryMap(nullptr),
    fieldCount(0),
    allowRemotePinOutputs(allowRemotePinOutputs)
{
}

Automato::Automato(void *databuf, uint16_t datalen, bool allowRemotePinOutputs)
    datalen(datalen),
    memoryMap(nullptr),
    fieldCount(0),
    allowRemotePinOutputs(allowRemotePinOutputs)
{
}

Automato::Automato(
    void *databuf,
    uint16_t datalen,
    void *mapentries,
    uint16_t mapentrycount,
    bool allowRemotePinOutputs)
    : databuf(databuf),
    datalen(datalen),
    memoryMap(mapentries),
    fieldCount(mapentrycount),
    allowRemotePinOutputs(allowRemotePinOutputs)
{
}

void Automato::init(float frequency, uint8_t power)
{
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

void Automato::handleMessage(Msgbuf &mb)
{
    switch (mb.payload.type) {
        case pt_readpin:
            if (0 <= mb.payload.pin &&  mb.payload.pin < 40) {
                bool val = digitalRead(mb.payload.pin);
                setup_readpinreply(mb.payload, mb.payload.pin, val);
                return;
            } else {
                // failed, invalid address.
                setup_fail(mb.payload, rc_invalid_pin_number);
                return;
            };
        case pt_pinmode:
            if (this->allowRemotePinOutputs) {
                if (0 <= mb.payload.pinmode.pin &&  mb.payload.pinmode.pin < 40) {
                    pinMode(mb.payload.pinmode.pin, mb.payload.pinmode.mode);
                    setup_ack(mb.payload);
                    return;
                } else {
                    // failed, invalid address.
                    setup_fail(mb.payload, rc_invalid_pin_number);
                    return;
                };
            } else
            {
                // failed, pin ops not allowed.
                setup_fail(mb.payload, rc_operation_forbidden);
                return;
            }
        case pt_writepin:
            if (this->allowRemotePinOutputs) {
                if (0 <= mb.payload.pinval.pin &&  mb.payload.pinval.pin < 40) {
                    if (mb.payload.pinval.state == 0) {
                        digitalWrite(mb.payload.pinval.pin, LOW);
                        setup_ack(mb.payload);
                        return;
                    } else if (mb.payload.pinval.state == 1) {
                        digitalWrite(mb.payload.pinval.pin, HIGH);
                        setup_ack(mb.payload);
                        return;
                    }
                } else {
                    // failed, invalid address.
                    setup_fail(mb.payload, rc_invalid_pin_number);
                    return;
                };
            } else {
                // failed, pin ops not allowed.
                setup_fail(mb.payload, rc_operation_forbidden);
                return;
            }
        case pt_readanalog:
            if (0 <= mb.payload.pin &&  mb.payload.pin < 40) {
                int val = analogRead(mb.payload.pin);
                setup_readanalogreply(mb.payload, mb.payload.pin, val);
                return;
            } else {
                // failed, invalid address.
                setup_fail(mb.payload, rc_invalid_pin_number);
                return;
            };
        case pt_readmem:
            // range check.
            if (mb.payload.readmem.address >= this->datalen) {
                // failed, invalid address.
                setup_fail(mb.payload, rc_invalid_mem_address);
                return;
            }
            else if (mb.payload.readmem.address + mb.payload.readmem.length > this->datalen) {
                // failed, invalid length.
                setup_fail(mb.payload, rc_invalid_mem_length);
                return;
            } else {
                // build reply and send.
                setup_readmemreply(mb.payload,
                    mb.payload.readmem.length,
                    databuf + mb.payload.readmem.address);
                return;
            };
        case pt_writemem:
            // range check.
            if (mb.payload.readmem.address >= this->datalen) {
                // failed, invalid address.
                setup_fail(mb.payload, rc_invalid_mem_address);
                return;
            }
            else if (mb.payload.readmem.address + mb.payload.readmem.length >= this->datalen) {
                // failed, invalid length.
                setup_fail(mb.payload, rc_invalid_mem_length);
                return;
            } else {
                memcpy(this->databuf + mb.payload.writemem.address,
                    mb.payload.writemem.data,
                    mb.payload.writemem.length);
                setup_ack(mb.payload);
                return;
            };
        case pt_readinfo:
            setup_readinforeply(mb.payload, protoVersion, macAddress(), datalen, fieldCount);
            return;
        case pt_readhumidity:
            readTempHumidity();
            setup_readhumidityreply(mb.payload, getHumidity());
            return;
        case pt_readtemperature:
            readTempHumidity();
            setup_readtemperaturereply(mb.payload, getTemperature());
            return;
        case pt_readfield:
            if (mb.payload.readfield.fieldindex < this->fieldCount) {
                MapField *mf = (MapField*)(memoryMap + mb.payload.readfield.fieldindex * sizeof(MapField));
                setup_readfieldreply(mb.payload, mb.payload.readfield.fieldindex, *mf);
            } else {
                setup_fail(mb.payload, rc_invalid_mapfield_index);
            }
            return;
        // error!  These should only be received in response to a request.
        case pt_readhumidityreply:
        case pt_readtemperaturereply:
        case pt_readmemreply:
        case pt_readinforeply:
        default:
            // failed, unsupported message type.
            setup_fail(mb.payload, rc_invalid_message_type);
            return;
    };
}

