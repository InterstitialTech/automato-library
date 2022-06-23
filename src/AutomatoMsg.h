#ifndef Automatomsg_hh_INCLUDED
#define Automatomsg_hh_INCLUDED

#include <Arduino.h>
#include <RH_RF95.h>
#include <AutomatoResult.h>

extern float protoVersion;

// --------------------------------------------------------
// message structs.
// --------------------------------------------------------

enum PayloadType : uint8_t {
    pt_ack = 0,
    pt_fail = 1,
    pt_pinmode = 2,
    pt_readpin = 3,
    pt_readpinreply = 4,
    pt_writepin = 5,
    pt_readmem = 6,
    pt_readmemreply = 7,
    pt_writemem = 8,
    pt_readinfo = 9,
    pt_readinforeply = 10,
    pt_readhumidity = 11,
    pt_readhumidityreply = 12,
    pt_readtemperature = 13,
    pt_readtemperaturereply = 14,
    pt_readanalog = 15,
    pt_readanalogreply = 16,
    pt_readfield = 17,
    pt_readfieldreply = 18,
    pt_count = 19 // not a payload type; just the number of payload types.
};

struct RemoteInfo {
    float protoversion;
    uint64_t macAddress;
    uint16_t datalen;
    uint16_t fieldcount;
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

struct ReadFieldInfo {
    uint16_t fieldindex;
} __attribute__((packed));

struct FieldInfo {
    uint16_t fieldindex;
    uint16_t offset;
    uint8_t length;
    uint8_t format;
    char name[25];
} __attribute__((packed));

struct Payload {
    PayloadType type;
    union {
        Pinval pinval;
        Pinmode pinmode;
        AnalogPinval analogpinval;
        Readmem readmem;
        ReadmemReply readmemreply;
        Writemem writemem;
        RemoteInfo remoteinfo;
        ReadFieldInfo readfield;
        FieldInfo readfieldreply;
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

struct Msgbuf {
    union {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        Message msg;     // as Message for router-less
        Payload payload; // as Payload for RHMesh
    };
} __attribute__((packed));

// -------------------------------------------------------
// Memory Map structs; not messages per se, but used to
// build the FieldInfo message.
// -------------------------------------------------------

enum FieldFormat : uint8_t {
    ff_char,
    ff_float,
    ff_uint8,
    ff_uint16,
    ff_uint32,
    ff_int8,
    ff_int16,
    ff_int32,
    ff_other
};

struct MapField {
    const char * name;
    uint16_t offset;
    uint16_t length;
    FieldFormat format;
};

// --------------------------------------------------------
// message functions.
// --------------------------------------------------------

uint8_t payloadSize(Payload &p);
bool printPayload(Payload &p);
bool isReply(PayloadType pt);
AutomatoResult arFromReply(Payload &p);

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
    uint16_t datalen,
    uint16_t fieldcount);

void setup_readfield(Payload &p, uint16_t fieldindex);
void setup_readfieldreply(Payload &p, uint16_t fieldindex, MapField &fieldinfo);

bool succeeded(Payload &p);

#endif // Automatomsg_hh_INCLUDED
