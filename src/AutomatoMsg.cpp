#include <AutomatoMsg.h>

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
  p.data.failcode = fc;
} 
void setup_readpin(Payload &p, uint8_t pin) 
{
  p.type = pt_readpin;
  p.data.pin = pin;
}
void setup_readpinreply(Payload &p, uint8_t pin, uint8_t state) 
{
  p.type = pt_readpinreply;
  p.data.pinval.pin = pin;
  p.data.pinval.state = state;

} 
void setup_writepin(Payload &p, uint8_t pin, uint8_t state) 
{
  p.type = pt_writepin;
  p.data.pinval.pin = pin;
  p.data.pinval.state = state;
} 

void setup_readmem(Payload &p, uint16_t address, uint8_t length)
{
  p.type = pt_readmem;
  p.data.readmem.address = address;
  p.data.readmem.length = length;
}

bool setup_readmemreply(Payload &p, uint8_t length, void* mem)
{
  p.type = pt_readmemreply;
  // memory range should be checked on the other end too.  TODO: remove?
  if (length <= MAX_READMEM)
  {
    memcpy((void*)&p.data, mem, length);
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
    p.data.writemem.address = address;
    p.data.writemem.length = length;
    memcpy((void*)&p.data.writemem.data, mem, length);
    return true;
  }
  else 
    // invalid length.
    return false;
} 


// void setupMessage(Message &m, 
//   uint8_t frommac,
//   uint8_t tomac,
//   char type,
//   int  address,
//   int  length,
//   int  payload) 
// {
//   // memcpy((char*)m.frommac, (const char*)&frommac, 6);
//   // memcpy((char*)m.tomac, (const char*)&tomac, 6);
//   m.type = type;
//   m.address = address;
//   m.length = length;
//   m.payload = payload;
// }

uint8_t payloadSize(Payload &p) {
  switch (p.type) {
    case pt_ack: 
        return sizeof(PayloadType);
    case pt_fail: 
        return sizeof(PayloadType);
    case pt_readpin: 
        return sizeof(PayloadType) + sizeof(Pinval);
    case pt_readpinreply: 
        return sizeof(PayloadType) + sizeof(Pinval);
    case pt_writepin: 
        return sizeof(PayloadType) + sizeof(Pinval);
    case pt_readmem:
        return sizeof(PayloadType) + sizeof(Readmem);
    case pt_readmemreply:
      return sizeof(PayloadType) + sizeof(uint16_t) + sizeof(uint8_t) + p.data.readmemreply.length; 
    case pt_writemem:
      return sizeof(PayloadType) + sizeof(uint16_t) + sizeof(uint8_t) + p.data.readmemreply.length; 
    default: 
        return 0;
  }
}

void printPayload(Payload &p)
{
  Serial.println("message payload");

  // Serial.print("frommac: ");
  // uint64_t mac = 0;
  // memcpy(((char*)&mac), &m.frommac, 6); 
  // Serial.println(mac);

  // Serial.print("tomac: ");
  // mac = 0;
  // memcpy(((char*)&mac), &m.tomac, 6); 
  // Serial.println(mac);

  Serial.print("type: ");
  switch ((int)p.type) {
    case pt_ack:
      Serial.println("pt_ack");
      break;
    case pt_fail:
      Serial.print("pt_fail: ");
      switch (p.data.failcode) {
        fc_invalid_message_type:
          Serial.println("fc_invalid_message_type");
          break;
        fc_invalid_pin_number:
          Serial.println("fc_invalid_pin_number");
          break;
        default:
          Serial.print("unknown failcode: ");
          Serial.println(p.data.failcode);
          break;
      }
      break;
    case pt_readpin:
      Serial.println("pt_readpin");
      break;
    case pt_readpinreply:
      Serial.println("pt_readpinreply");
      break;
    case pt_writepin:
      Serial.println("pt_writepin");
      break;
    case pt_readmem:
      Serial.println("pt_readmem");
      break;
    case pt_readmemreply:
      Serial.println("pt_readmemreply");
      break;
    case pt_writemem:
      Serial.println("pt_writemem");
      break;

    default:
      Serial.print("unknown message type: ");
      Serial.println((int)p.type);
      break;
  }

  // Serial.print("address: ");
  // Serial.println(m.address);
  // Serial.print("payload: ");
  // Serial.println(m.payload);
}
// void printMessage(message &m) 
// {
//   Serial.println("message");

//   Serial.print("frommac: ");
//   uint64_t mac = 0;
//   memcpy(((char*)&mac), &m.frommac, 6); 
//   Serial.println(mac);

//   Serial.print("tomac: ");
//   mac = 0;
//   memcpy(((char*)&mac), &m.tomac, 6); 
//   Serial.println(mac);

//   Serial.print("type: ");
//   switch ((int)m.type) {
//     case mt_read: 
//       Serial.println("mt_read");
//       break;
//     case mt_write: 
//       Serial.println("mt_write");
//       break;
//     case mt_ack: 
//       Serial.println("mt_ack");
//       break;
//     default:
//       Serial.print("unknown message type: ");
//       Serial.println((int)m.type);
//       break;
//   }

//   Serial.print("address: ");
//   Serial.println(m.address);
//   Serial.print("payload: ");
//   Serial.println(m.payload);
// }



