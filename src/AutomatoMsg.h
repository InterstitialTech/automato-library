#ifndef Automatomsg_hh_INCLUDED
#define Automatomsg_hh_INCLUDED

#include <RH_RF95.h>

enum message_type {
  mt_read,
  mt_write,
  mt_mode,
  mt_ack
};

enum ack_code {
  ac_success,
  ac_invalid_message_type,
  ac_invalid_address
};

struct message {
  char frommac[6];
  char tomac[6];
  char type;
  int  address;
  int  length;
  int  payload;
};

struct msgbuf { 
  union {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    message msg;
  };
};

void printMessage(message &m); 

void setupMessage(message &m, 
  uint64_t frommac,
  uint64_t tomac,
  char type,
  int  address,
  int  length,
  int  payload);

#endif // Automatomsg_hh_INCLUDED

