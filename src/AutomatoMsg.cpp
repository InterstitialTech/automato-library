#include <AutomatoMsg.h>

float protoVersion(0.1);

bool succeeded(Payload &p)
{
  return p.type != pt_fail;
}

void setup_ack(Payload &p) 
{
  p.type = pt_ack;
} 
void setup_fail(Payload &p, FailCode fc) 
{
  p.type = pt_fail;
  p.failcode = fc;
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


void setup_readmem(Payload &p, uint16_t address, uint8_t length)
{
  p.type = pt_readmem;
  p.readmem.address = address;
  p.readmem.length = length;
}

bool setup_readmemreply(Payload &p, uint8_t length, void* mem)
{
  p.type = pt_readmemreply;
  // memory range should be checked on the other end too.  TODO: remove?
  if (length <= MAX_READMEM)
  {
    p.readmemreply.length = length;
    memcpy((void*)&p.readmemreply.data, mem, length);
    return true;
  }
  else 
    // invalid length.
    return false;
} 
bool setup_writemem(Payload &p, uint16_t address, uint8_t length, void* mem)
{
  p.type = pt_writemem;
  // memory range should be checked on the other end too.  TODO: remove?
  if (length <= MAX_WRITEMEM)
  {
    p.writemem.address = address;
    p.writemem.length = length;
    memcpy((void*)&p.writemem.data, mem, length);
    return true;
  }
  else 
    // invalid length.
    return false;
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
      return sizeof(PayloadType);
    case pt_fail: 
      return sizeof(PayloadType);
    case pt_pinmode: 
      return sizeof(PayloadType) + sizeof(Pinmode);
    case pt_readpin: 
      return sizeof(PayloadType) + sizeof(Pinval);
    case pt_readpinreply: 
      return sizeof(PayloadType) + sizeof(Pinval);
    case pt_writepin: 
      return sizeof(PayloadType) + sizeof(Pinval);
    case pt_readmem:
      return sizeof(PayloadType) + sizeof(Readmem);
    case pt_readmemreply:
      return sizeof(PayloadType) + sizeof(uint8_t) + p.readmemreply.length; 
    case pt_writemem:
      return sizeof(PayloadType) + sizeof(uint16_t) + sizeof(uint8_t) + p.readmemreply.length; 
    case pt_readinfo:
      return sizeof(PayloadType);
    case pt_readinforeply:
      return sizeof(PayloadType)+ sizeof(RemoteInfo); 
    case pt_readhumidity:
      return sizeof(PayloadType);
    case pt_readhumidityreply:
      return sizeof(PayloadType) + sizeof(float);
    case pt_readtemperature:
      return sizeof(PayloadType);
    case pt_readtemperaturereply:
      return sizeof(PayloadType) + sizeof(float);
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
      Serial.print("pt_fail: ");
      switch (p.failcode) {
        fc_invalid_message_type:
          Serial.println("fc_invalid_message_type");
          break;
        fc_invalid_pin_number:
          Serial.println("fc_invalid_pin_number");
          break;
        default:
          Serial.print("unknown failcode: ");
          Serial.println(p.failcode);
          break;
      }
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
