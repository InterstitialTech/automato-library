#ifndef Automatomsg_hh_INCLUDED
#define Automatomsg_hh_INCLUDED

#include <Arduino.h>
// #include <Wire.h>
#include <RH_RF95.h>

// enum message_type {
//   mt_request,
//   mt_reply
// };

extern float protoVersion;

enum FailCode {
  fc_invalid_message_type,
  fc_invalid_pin_number,
  fc_invalid_mem_address,
  fc_invalid_mem_length,
};

enum PayloadType {
  pt_ack,
  pt_fail,
  pt_pinmode,
  pt_readpin,
  pt_readpinreply,
  pt_writepin,
  pt_readmem,
  pt_readmemreply,
  pt_writemem,
  pt_readinfo,
  pt_readinforeply,
  pt_readhumidity,
  pt_readhumidityreply,
  pt_readtemperature,
  pt_readtemperaturereply
};

struct RemoteInfo {
  float protoversion;
  uint64_t macAddress;
  uint16_t datalen;
} __attribute__((packed));

struct Pinval {
  uint8_t pin;
  uint8_t state;
} __attribute__((packed));

struct Pinmode {
  uint8_t pin;
  uint8_t mode;
} __attribute__((packed));

struct Readmem {
  uint16_t address;
  uint8_t length;
} __attribute__((packed));

#define MAX_WRITEMEM RH_RF95_MAX_MESSAGE_LEN - sizeof(uint16_t) - sizeof(uint8_t) - sizeof(PayloadType)
#define MAX_READMEM RH_RF95_MAX_MESSAGE_LEN - sizeof(PayloadType) - sizeof(uint8_t)

struct ReadmemReply {
  uint8_t length;
  uint8_t data[MAX_READMEM];
} __attribute__((packed));


struct Writemem {
  uint16_t address;
  uint8_t length;
  uint8_t data[MAX_WRITEMEM];
} __attribute__((packed));

// struct Data {
//   union {
//     Pinval pinval;
//     Pinmode pinmode;
//     Readmem readmem;
//     ReadmemReply readmemreply;
//     Writemem writemem;
//     RemoteInfo remoteinfo;
//     uint8_t failcode;
//     uint8_t pin;
//     float f;
//   };
// } __attribute__((packed));

struct Payload {
  PayloadType type;
  union {
    Pinval pinval;
    Pinmode pinmode;
    Readmem readmem;
    ReadmemReply readmemreply;
    Writemem writemem;
    RemoteInfo remoteinfo;
    uint8_t failcode;
    uint8_t pin;
    float f;
  };
} __attribute__((packed));

// used for non-mesh, non-routed comms.
struct Message {
  uint8_t fromid;
  uint8_t toid;
  Payload data;
} __attribute__((packed));

struct msgbuf { 
  union {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    Message msg;				// as Message for router-less
    Payload payload;    // as Payload for RHMesh
  };
} __attribute__((packed));

uint8_t payloadSize(Payload &p);
void printPayload(Payload &p); 

void setup_ack(Payload &p); 
void setup_fail(Payload &p, FailCode fc); 

void setup_pinmode(Payload &p, uint8_t pin, uint8_t mode); 
void setup_readpin(Payload &p, uint8_t pin);
void setup_readpinreply(Payload &p, uint8_t pin, uint8_t state); 
void setup_writepin(Payload &p, uint8_t pin, uint8_t state); 

void setup_readmem(Payload &p, uint16_t address, uint8_t length);
bool setup_readmemreply(Payload &p, uint8_t length, void* mem); 
bool setup_writemem(Payload &p, uint16_t address, uint8_t length, void* mem); 

void setup_readhumidity(Payload &p);
void setup_readhumidityreply(Payload &p, float humidity);

void setup_readtemperature(Payload &p);
void setup_readtemperaturereply(Payload &p, float temperature);

void setup_readinfo(Payload &p);
void setup_readinforeply(Payload &p,
                           float protoversion,
                           uint64_t macAddress,
                           uint16_t datalen);

bool succeeded(Payload &p);

#endif // Automatomsg_hh_INCLUDED

