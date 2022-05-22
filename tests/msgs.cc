#include <AutomatoResult.h>
#include <iostream>
#include <Arduino.h>
#include <AutomatoMsg.h>
#include <cstdio>

using namespace std;


void testMsg(const char *fname) 
{
  std::cout << std::endl << fname << std::endl;
  Msgbuf mb;

  FILE * filp = fopen(fname, "rb"); 
  int bytes_read = fread(mb.buf, sizeof(char), RH_RF95_MAX_MESSAGE_LEN, filp);

  std::cout << "read bytes: " << bytes_read << std::endl;

  for (int i = 0; i < bytes_read; ++i)
  {
    std::cout << (int)mb.buf[i] << " " << endl;
  }
  
  printPayload(mb.payload);
}

int main(int argc, char *argv[])
{
  testMsg("ack.bin");
  testMsg("fail.bin");
  testMsg("pinmode.bin");
  testMsg("readpin.bin");
  testMsg("readpinreply.bin");
  testMsg("writepin.bin");
  testMsg("readanalog.bin");
  testMsg("readanalogreply.bin");
  testMsg("readmem.bin");
  testMsg("readmemreply.bin");
  testMsg("writemem.bin");
  testMsg("readinfo.bin");
  testMsg("readinforeply.bin");
  testMsg("readhumidity.bin");
  testMsg("readhumidityreply.bin");
  testMsg("readtemperature.bin");
  testMsg("readtemperaturereply.bin");
  return 0;
}


