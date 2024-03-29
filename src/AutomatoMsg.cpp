#include <AutomatoMsg.h>

float protoVersion(0.1);

bool succeeded(Payload &p)
{
    return p.type != pt_fail;
}

bool isReply(PayloadType pt)
{
    switch (pt) {
        case pt_ack:
            return true;
        case pt_fail:
            // a failure is a reply, just not the one we wanted
            return true;
        case pt_pinmode:
            return false;
        case pt_readpin:
            return false;
        case pt_readpinreply:
            return true;
        case pt_writepin:
            return false;
        case pt_readmem:
            return false;
        case pt_readmemreply:
            return true;
        case pt_writemem:
            return false;
        case pt_readinfo:
            return false;
        case pt_readinforeply:
            return true;
        case pt_readhumidity:
            return false;
        case pt_readhumidityreply:
            return true;
        case pt_readtemperature:
            return false;
        case pt_readtemperaturereply:
            return true;
        case pt_readanalog:
            return false;
        case pt_readanalogreply:
            return true;
        case pt_readfield:
            return false;
        case pt_readfieldreply:
            return true;
        default:
            return false;
    }
}

AutomatoResult arFromReply(Payload &p)
{
    if (p.type == pt_fail)
        return AutomatoResult((ResultCode)p.failcode);
    else if (isReply((PayloadType)p.type))
        return AutomatoResult(rc_ok);
    else
        return AutomatoResult(rc_invalid_reply_message);
}

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
    uint16_t datalen,
    uint16_t fieldcount)
{
    p.type = pt_readinforeply;
    p.remoteinfo.protoversion = protoversion;
    p.remoteinfo.macAddress = macAddress;
    p.remoteinfo.datalen = datalen;
    p.remoteinfo.fieldcount = fieldcount;
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

void setup_readfield(Payload &p, uint16_t fieldindex)
{
    p.type = pt_readfield;
    p.readfield.fieldindex = fieldindex;
}
void setup_readfieldreply(Payload &p, uint16_t fieldindex, MapField &fieldinfo)
{
    p.type = pt_readfieldreply;
    strncpy(p.readfieldreply.name, fieldinfo.name, 24);   // max of 25!
    p.readfieldreply.name[24] = '\0';  // ensure null termination
    p.readfieldreply.fieldindex = fieldindex;
    p.readfieldreply.offset = fieldinfo.offset;
    p.readfieldreply.length = fieldinfo.length;
    p.readfieldreply.format = fieldinfo.format;
}

uint8_t payloadSize(Payload &p) {
    switch (p.type) {
        case pt_ack:
            return sizeof(uint8_t);
        case pt_fail:
            return sizeof(uint8_t) + sizeof(uint8_t);
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
            return sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t) + p.writemem.length;
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
        case pt_readfield:
            return sizeof(uint8_t) + sizeof(uint16_t);
        case pt_readfieldreply:
            return sizeof(uint8_t) + sizeof(uint16_t) * 3 + sizeof(uint8_t) + 25;
        default:
            return 0;
    }
}

bool printPayload(Payload &p)
{
    Serial.println("message payload");

    Serial.print("type: ");
    Serial.print(p.type);
    Serial.print(" ");

    switch ((int)p.type) {
        case pt_ack:
            Serial.println("pt_ack");
            return true;
        case pt_fail:
            Serial.print("pt_fail; code: ");
            Serial.println(p.failcode);
            Serial.println(resultString((ResultCode)p.failcode));
            return true;
        case pt_pinmode:
            Serial.println("pt_pinmode");
            Serial.print("pin: ");
            Serial.println(p.pinmode.pin);
            Serial.print("mode: ");
            Serial.println(p.pinmode.mode);
            return true;
        case pt_readpin:
            Serial.println("pt_readpin");
            Serial.print("pin: ");
            Serial.println(p.pin);
            return true;
        case pt_readpinreply:
            Serial.println("pt_readpinreply");
            Serial.print("pin: ");
            Serial.println(p.pinval.pin);
            Serial.print("state: ");
            Serial.println(p.pinval.state);
            return true;
        case pt_writepin:
            Serial.println("pt_writepin");
            Serial.print("pin: ");
            Serial.println(p.pinval.pin);
            Serial.print("state: ");
            Serial.println(p.pinval.state);
            return true;
        case pt_readanalog:
            Serial.println("pt_readanalog");
            Serial.print("pin: ");
            Serial.println(p.pin);
            return true;
        case pt_readanalogreply:
            Serial.println("pt_readanalogreply");
            Serial.print("pin: ");
            Serial.println(p.pin);
            Serial.print("state: ");
            Serial.println(p.analogpinval.state);
            return true;
        case pt_readmem:
            Serial.println("pt_readmem");
            Serial.print("address");
            Serial.println(p.readmem.address);
            Serial.print("length");
            Serial.println(p.readmem.length);
            return true;
        case pt_readmemreply:
            Serial.println("pt_readmemreply");
            Serial.print("length: ");
            Serial.println(p.readmemreply.length);
            Serial.print("mem: ");
            for (int i = 0; i < p.readmemreply.length; ++i) {
                Serial.print(p.readmemreply.data[i], HEX);
            }
            Serial.println();
            return true;
        case pt_writemem:
            Serial.println("pt_writemem");
            Serial.print("address ");
            Serial.println(p.writemem.address);
            Serial.print("length ");
            Serial.println(p.writemem.length);
            Serial.print("mem: ");
            for (int i = 0; i < p.writemem.length; ++i) {
                Serial.print(p.writemem.data[i], HEX);
            }
            Serial.println();
            return true;
        case pt_readinfo:
            Serial.println("pt_readinfo");
            return true;
        case pt_readinforeply:
            Serial.println("pt_readinforeply");
            Serial.print("protoversion:");
            Serial.println(p.remoteinfo.protoversion);
            Serial.print("macAddress:");
            Serial.println(p.remoteinfo.macAddress);
            Serial.print("datalen:");
            Serial.println(p.remoteinfo.datalen);
            Serial.print("fieldcount:");
            Serial.println(p.remoteinfo.fieldcount);
            return true;
        case pt_readhumidity:
            Serial.println("pt_readhumidity");
            return true;
        case pt_readhumidityreply:
            Serial.println("pt_readhumidityreply");
            Serial.print("humidity:");
            Serial.println(p.f);
            return true;
        case pt_readtemperature:
            Serial.println("pt_readtemperature");
            return true;
        case pt_readtemperaturereply:
            Serial.println("pt_readtemperaturereply");
            Serial.print("temperature:");
            Serial.println(p.f);
            return true;
        case pt_readfield:
            Serial.println("pt_readfield");
            Serial.print("fieldindex: ");
            Serial.println(p.readfield.fieldindex);
            return true;
        case pt_readfieldreply:
            Serial.println("pt_readfieldreply");
            Serial.print("fieldindex: ");
            Serial.println(p.readfieldreply.fieldindex);
            Serial.print("offset: ");
            Serial.println(p.readfieldreply.offset);
            Serial.print("length: ");
            Serial.println(p.readfieldreply.length);
            Serial.print("format: ");
            Serial.println(p.readfieldreply.format);
            Serial.print("name: ");
            Serial.println(p.readfieldreply.name);
            return true;
        default:
            Serial.print("unknown message type: ");
            Serial.println((int)p.type);
            return false;
    }
}
