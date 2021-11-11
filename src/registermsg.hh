#ifndef registermsg_hh_INCLUDED
#define registermsg_hh_INCLUDED

enum message_type {
  mt_read,
  mt_write,
  mt_ack
};

// MSG_LEN | SRC_ID | DEST_ID | MSG_TYPE | MSG_PAYLOAD
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

#endif // registermsg_hh_INCLUDED

