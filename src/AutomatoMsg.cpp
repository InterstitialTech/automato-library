#include <AutomatoMsg.h>

float protoVersion(0.1);

bool succeeded(Payload &p)
{
    return p.type != pt_fail;
}

const char* resultString(ResultCode rc)
{
    switch (rc) {
        case rc_ok:
            return "ok";
        case rc_no_message_received:
            return "no message received";
        case rc_invalid_message_type:
            return "invalid message type";
        case rc_invalid_pin_number:
            return "invalid pin number";
        case rc_invalid_mem_address:
            return "invalid mem address";
        case rc_invalid_mem_length:
            return "invalid mem length";
        case rc_invalid_reply_message:
            return "expected a reply message";
        case rc_operation_forbidden:
            return "operation forbidden";
        case rc_reply_timeout:
            return "timeout waiting for reply message";
        case rc_rh_router_error_invalid_length:
            return "router error invalid length";
        case rc_rh_router_error_no_route:
            return "router error no route";
        case rc_rh_router_error_timeout:
            return "router error timeout";
        case rc_rh_router_error_no_reply:
            return "router error no reply";
        case rc_rh_router_error_unable_to_deliver:
            return "router error unable to deliver";
        case rc_invalid_rh_router_error:
            return "invalid rh router error code";
        default:
            return "unknown error code";
    }
}

// --------------------------------------------------------------------------------------------------

AutomatoResult::operator bool () {
    if (this->rc == rc_ok)
        return true;
    else
        return false;
}

const char* AutomatoResult::as_string() const
{
    return resultString(rc);
}

ResultCode AutomatoResult::resultCode() const
{
    return this->rc;
}


AutomatoResult::AutomatoResult()
{
    this->rc = rc_ok;
}

AutomatoResult::AutomatoResult(ResultCode rc)
{
    this->rc = rc;
}

AutomatoResult::AutomatoResult(Payload &p)
{
    switch (p.type) {
        case pt_ack:
            this->rc = rc_ok;
            break;

        case pt_fail:
            this->rc = (ResultCode)p.failcode;
            break;

        case pt_pinmode:
            this->rc = rc_invalid_reply_message;
            break;

        case pt_readpin:
            this->rc = rc_invalid_reply_message;
            break;

        case pt_readpinreply:
            this->rc = rc_ok;
            break;

        case pt_writepin:
            this->rc = rc_invalid_reply_message;
            break;

        case pt_readmem:
            this->rc = rc_invalid_reply_message;
            break;

        case pt_readmemreply:
            this->rc = rc_ok;
            break;

        case pt_writemem:
            this->rc = rc_invalid_reply_message;
            break;

        case pt_readinfo:
            this->rc = rc_invalid_reply_message;
            break;

        case pt_readinforeply:
            this->rc = rc_ok;
            break;

        case pt_readhumidity:
            this->rc = rc_invalid_reply_message;
            break;

        case pt_readhumidityreply:
            this->rc = rc_ok;
            break;

        case pt_readtemperature:
            this->rc = rc_invalid_reply_message;
            break;

        case pt_readtemperaturereply:
            this->rc = rc_ok;
            break;

        case pt_readanalog:
            this->rc = rc_invalid_reply_message;
            break;

        case pt_readanalogreply:
            this->rc = rc_ok;
            break;
        default:
            this->rc = rc_invalid_reply_message;
            break;
    }
}


// --------------------------------------------------------------------------------------------------

void setup_ack(Payload &p)
{
    p.type = pt_ack;
}

void setup_fail(Payload &p, ResultCode rc)
{
    p.type = pt_fail;
    p.failcode = rc;
}

void setup_pinmode(Payload &p, uint8_t pin, uint8_t mode)
{
    p.type = pt_pinmode;
    p.pinmode.pin = pin;
    p.pinmode.mode = mode;
}

void setup_readpin(Payload &p, uint8_t pin)
{
    p.type = pt_readpin;
    p.pin = pin;
}


void setup_readpinreply(Payload &p, uint8_t pin, uint8_t state)
{
    p.type = pt_readpinreply;
    p.pinval.pin = pin;
    p.pinval.state = state;
}

void setup_writepin(Payload &p, uint8_t pin, uint8_t state)
{
    p.type = pt_writepin;
    p.pinval.pin = pin;
    p.pinval.state = state;
}

void setup_readanalog(Payload &p, uint8_t pin)
{
    p.type = pt_readanalog;
    p.pin = pin;
}

void setup_readanalogreply(Payload &p, uint8_t pin, uint16_t state)
{
    p.type = pt_readanalogreply;
    p.analogpinval.pin = pin;
    p.analogpinval.state = state;
}

AutomatoResult setup_readmem(Payload &p, uint16_t address, uint8_t length)
{
    p.type = pt_readmem;
    p.readmem.address = address;
    p.readmem.length = length;

    return AutomatoResult(rc_ok);

}

AutomatoResult setup_readmemreply(Payload &p, uint8_t length, void* mem)
{
    p.type = pt_readmemreply;
    if (length <= MAX_READMEM)
    {
        p.readmemreply.length = length;
        memcpy((void*)&p.readmemreply.data, mem, length);
        return AutomatoResult(rc_ok);
    }
    else
        return AutomatoResult(rc_invalid_mem_length);
}
AutomatoResult setup_writemem(Payload &p, uint16_t address, uint8_t length, void* mem)
{
    p.type = pt_writemem;
    if (length <= MAX_WRITEMEM)
    {
        p.writemem.address = address;
        p.writemem.length = length;
        memcpy((void*)&p.writemem.data, mem, length);
        return AutomatoResult(rc_ok);
    }
    else
        return AutomatoResult(rc_invalid_mem_length);
}

void setup_readinfo(Payload &p)
{
    p.type = pt_readinfo;
}

void setup_readinforeply(Payload &p,
    float protoversion,
    uint64_t macAddress,
    uint16_t datalen)
{
    p.type = pt_readinforeply;
    p.remoteinfo.protoversion = protoversion;
    p.remoteinfo.macAddress = macAddress;
    p.remoteinfo.datalen = datalen;
}

void setup_readhumidity(Payload &p)
{
    p.type = pt_readhumidity;
}

void setup_readhumidityreply(Payload &p, float humidity)
{
    p.type = pt_readhumidityreply;
    p.f = humidity;
}

void setup_readtemperature(Payload &p)
{
    p.type = pt_readtemperature;
}

void setup_readtemperaturereply(Payload &p, float temperature)
{
    p.type = pt_readtemperaturereply;
    p.f = temperature;
}

uint8_t payloadSize(Payload &p) {
    switch (p.type) {
        case pt_ack:
            return sizeof(uint8_t);
        case pt_fail:
            return sizeof(uint8_t);
        case pt_pinmode:
            return sizeof(uint8_t) + sizeof(Pinmode);
        case pt_readpin:
            return sizeof(uint8_t) + sizeof(uint8_t);
        case pt_readpinreply:
            return sizeof(uint8_t) + sizeof(Pinval);
        case pt_writepin:
            return sizeof(uint8_t) + sizeof(Pinval);
        case pt_readanalog:
            return sizeof(uint8_t) + sizeof(uint8_t);
        case pt_readanalogreply:
            return sizeof(uint8_t) + sizeof(AnalogPinval);
        case pt_readmem:
            return sizeof(uint8_t) + sizeof(Readmem);
        case pt_readmemreply:
            return sizeof(uint8_t) + sizeof(uint8_t) + p.readmemreply.length;
        case pt_writemem:
            return sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t) + p.readmemreply.length;
        case pt_readinfo:
            return sizeof(uint8_t);
        case pt_readinforeply:
            return sizeof(uint8_t) + sizeof(RemoteInfo);
        case pt_readhumidity:
            return sizeof(uint8_t);
        case pt_readhumidityreply:
            return sizeof(uint8_t)+ sizeof(float);
        case pt_readtemperature:
            return sizeof(uint8_t);
        case pt_readtemperaturereply:
            return sizeof(uint8_t)+ sizeof(float);
        default:
            return 0;
    }
}

void printPayload(Payload &p)
{
    Serial.println("message payload");

    Serial.print("type: ");
    Serial.print(p.type);
    Serial.print(" ");

    switch ((int)p.type) {
        case pt_ack:
            Serial.println("pt_ack");
            break;
        case pt_fail:
            Serial.print("pt_fail; code: ");
            Serial.println(p.failcode);
            Serial.println(resultString((ResultCode)p.failcode));
            break;
        case pt_pinmode:
            Serial.println("pt_pinmode");
            Serial.print("pin: ");
            Serial.println(p.pinmode.pin);
            Serial.print("mode: ");
            Serial.println(p.pinmode.mode);
            break;
        case pt_readpin:
            Serial.println("pt_readpin");
            Serial.print("pin: ");
            Serial.println(p.pin);
            break;
        case pt_readpinreply:
            Serial.println("pt_readpinreply");
            Serial.print("pin: ");
            Serial.println(p.pinval.pin);
            Serial.print("state: ");
            Serial.println(p.pinval.state);
            break;
        case pt_writepin:
            Serial.println("pt_writepin");
            Serial.print("pin: ");
            Serial.println(p.pinval.pin);
            Serial.print("state: ");
            Serial.println(p.pinval.state);
            break;
        case pt_readanalog:
            Serial.println("pt_readanalog");
            Serial.print("pin: ");
            Serial.println(p.pin);
            break;
        case pt_readanalogreply:
            Serial.println("pt_readanalogreply");
            Serial.print("pin: ");
            Serial.println(p.pin);
            Serial.print("state: ");
            Serial.println(p.analogpinval.state);
            break;
        case pt_readmem:
            Serial.println("pt_readmem");
            Serial.print("address");
            Serial.println(p.readmem.address);
            Serial.print("length");
            Serial.println(p.readmem.length);
            break;
        case pt_readmemreply:
            Serial.println("pt_readmemreply");
            Serial.print("length: ");
            Serial.println(p.readmemreply.length);
            Serial.print("mem: ");
            for (int i = 0; i < p.readmemreply.length; ++i) {
                Serial.print(p.readmemreply.data[i], HEX);
            }
            Serial.println();
            break;
        case pt_writemem:
            Serial.println("pt_writemem");
            Serial.print("address");
            Serial.println(p.writemem.address);
            Serial.print("length");
            Serial.println(p.writemem.length);
            Serial.print("mem: ");
            for (int i = 0; i < p.readmemreply.length; ++i) {
                Serial.print(p.writemem.data[i], HEX);
            }
            Serial.println();
            break;
        case pt_readinfo:
            Serial.println("pt_readinfo");
            break;
        case pt_readinforeply:
            Serial.println("pt_readinforeply");
            Serial.print("protoversion:");
            Serial.println(p.remoteinfo.protoversion);
            Serial.print("macAddress:");
            Serial.println(p.remoteinfo.macAddress);
            Serial.print("datalen:");
            Serial.println(p.remoteinfo.datalen);
            break;
        case pt_readhumidity:
            Serial.println("pt_readhumidity");
            break;
        case pt_readhumidityreply:
            Serial.println("pt_readhumidityreply");
            Serial.print("humidity:");
            Serial.println(p.f);
            break;
        case pt_readtemperature:
            Serial.println("pt_readtemperature");
            break;
        case pt_readtemperaturereply:
            Serial.println("pt_readtemperaturereply");
            Serial.print("temperature:");
            Serial.println(p.f);
            break;
        default:
            Serial.print("unknown message type: ");
            Serial.println((int)p.type);
            break;
    }
}
