#ifndef Automatomsg_hh_INCLUDED
#define Automatomsg_hh_INCLUDED

#include <Arduino.h>
#include <RH_RF95.h>
#include <AutomatoResult.h>

extern float protoVersion;

// --------------------------------------------------------
// message structs.
// --------------------------------------------------------

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
    pt_readtemperaturereply,
    pt_readanalog,
    pt_readanalogreply
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

struct AnalogPinval {
    uint8_t pin;
    uint16_t state;
} __attribute__((packed));

struct Pinmode {
    uint8_t pin;
    uint8_t mode;
} __attribute__((packed));

struct Readmem {
    uint16_t address;
    uint8_t length;
} __attribute__((packed));

#define MAX_WRITEMEM RH_RF95_MAX_MESSAGE_LEN - sizeof(uint16_t) - sizeof(uint8_t) - sizeof(uint8_t)
#define MAX_READMEM RH_RF95_MAX_MESSAGE_LEN - sizeof(uint8_t) - sizeof(uint8_t)

struct ReadmemReply {
    uint8_t length;
    uint8_t data[MAX_READMEM];
} __attribute__((packed));


struct Writemem {
    uint16_t address;
    uint8_t length;
    uint8_t data[MAX_WRITEMEM];
} __attribute__((packed));

struct Payload {
    uint8_t type; // PayloadType
    union {
        Pinval pinval;
        Pinmode pinmode;
        AnalogPinval analogpinval;
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
        Message msg;     // as Message for router-less
        Payload payload; // as Payload for RHMesh
    };
} __attribute__((packed));

// --------------------------------------------------------
// message functions.
// --------------------------------------------------------

uint8_t payloadSize(Payload &p);
void printPayload(Payload &p);
bool isReply(PayloadType pt);
AutomatoResult ArFromReply(Payload &p);

void setup_ack(Payload &p);
void setup_fail(Payload &p, ResultCode rc);

void setup_pinmode(Payload &p, uint8_t pin, uint8_t mode);
void setup_readpin(Payload &p, uint8_t pin);
void setup_readpinreply(Payload &p, uint8_t pin, uint8_t state);
void setup_writepin(Payload &p, uint8_t pin, uint8_t state);

void setup_readanalog(Payload &p, uint8_t pin);
void setup_readanalogreply(Payload &p, uint8_t pin, uint16_t state);

AutomatoResult setup_readmem(Payload &p, uint16_t address, uint8_t length);
AutomatoResult setup_readmemreply(Payload &p, uint8_t length, void* mem);
AutomatoResult setup_writemem(Payload &p, uint16_t address, uint8_t length, void* mem);

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

const char* resultString(ResultCode rc);

#endif // Automatomsg_hh_INCLUDED
