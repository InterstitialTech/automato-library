#include "Arduino.h"

#include <iostream>

using namespace std;

size_t CoutSerial::write(uint8_t byte) {
  cout.put(byte);

  return 1;
}

CoutSerial Serial;

