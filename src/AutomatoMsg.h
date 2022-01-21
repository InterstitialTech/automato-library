#ifndef Automatomsg_hh_INCLUDED
#define Automatomsg_hh_INCLUDED

#include <Arduino.h>
// #include <Wire.h>
#include <RH_RF95.h>

// enum message_type {
//   mt_request,
//   mt_reply
// };

enum FailCode {
  fc_invalid_message_type,
  fc_invalid_pin_number,
  fc_invalid_mem_address,
  fc_invalid_mem_length,
};

enum PayloadType {
  pt_ack,
  pt_fail,
  pt_readpin,
  pt_readpinreply,
  pt_writepin,
  pt_readmem,
  pt_readmemreply,
  pt_writemem
};

struct Pinval {
  uint8_t pin;
  uint8_t state;
};

struct Readmem {
  uint16_t address;
  uint8_t length;
};

#define MAX_WRITEMEM RH_RF95_MAX_MESSAGE_LEN - sizeof(uint16_t) - sizeof(uint8_t) - sizeof(PayloadType)
#define MAX_READMEM RH_RF95_MAX_MESSAGE_LEN - sizeof(PayloadType) - sizeof(uint8_t)

struct ReadmemReply {
  uint8_t length;
  uint8_t data[MAX_READMEM];
};


struct Writemem {
  uint16_t address;
  uint8_t length;
  uint8_t data[MAX_WRITEMEM];
};

// pt_pinmode,
// struct Ack {
//   union { 
//     Pinval pinval;
//     uint8_t address;
//   };
// }

// struct Ack {
//   ack_code ac;
//   Data 

struct Data {
  union {
    Pinval pinval;
    Readmem readmem;
    ReadmemReply readmemreply;
    Writemem writemem;
    uint8_t failcode;
    uint8_t pin;
  };
};

struct Payload {
  PayloadType type;
  Data data; 
};


// used for non-mesh, non-routed comms.
struct Message {
  uint8_t fromid;
  uint8_t toid;
  Payload data;
};

// write pin
// write analog pin
// write byte
// write int
// write array
// write named int
// write named array
// 
// read pin
// read analog pin
// read byte
// read int
// read array
// read named int
// read named array
// 

struct msgbuf { 
  union {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    Message msg;				// as Message for router-less
    Payload payload;    // as Payload for RHMesh
  };
};

uint8_t payloadSize(Payload &p);

void printPayload(Payload &p); 

void setup_ack(Payload &p); 
void setup_fail(Payload &p, FailCode fc); 
void setup_readpin(Payload &p, uint8_t pin);
void setup_readpinreply(Payload &p, uint8_t pin, uint8_t state); 
void setup_writepin(Payload &p, uint8_t pin, uint8_t state); 

void setup_readmem(Payload &p, uint16_t address, uint8_t length);
bool setup_readmemreply(Payload &p, uint8_t length, void* mem); 
bool setup_writemem(Payload &p, uint16_t address, uint8_t length, void* mem); 

// void setupMessage(message &m, 
//   uint8_t fromid,
//   uint8_t toid,
//   char type,
//   int  address,
//   int  length,
//   int  payload);

bool succeeded(Payload &p);

#endif // Automatomsg_hh_INCLUDED

