#ifndef Arduino_h_INCLUDED
#define Arduino_h_INCLUDED

typedef unsigned short uint16_t;
typedef bool boolean;
typedef unsigned char uint8_t;
typedef unsigned long uint64_t;

#include <stdlib.h>
#include <string.h>

#include "Print.h"

class CoutSerial : public Print
{
public:
  size_t write(uint8_t);
};

extern CoutSerial Serial;

#endif // Arduino_h_INCLUDED

