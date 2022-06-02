#include <AutomatoResult.h>
#include <iostream>
#include <Arduino.h>
#include <AutomatoMsg.h>
#include <cstdio>

using namespace std;


void readMsg(const char *fname)
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


void writeMsg(Msgbuf &mb, const char * fname)
{
  size_t sz = payloadSize(mb.payload);
  FILE * filp = fopen(fname, "wb");
  int bytes_read = fwrite(mb.buf, sizeof(char), sz, filp);
}

void writeMsgs()
{
    Msgbuf mb;

    setup_ack(mb.payload);
    writeMsg(mb, "ack.bin");

    setup_fail(mb.payload, rc_invalid_rh_router_error);
    writeMsg(mb, "fail.bin");

    setup_pinmode(mb.payload, 26, 2);
    writeMsg(mb, "pinmode.bin");

    setup_readpin(mb.payload, 22);
    writeMsg(mb, "readpin.bin");

    setup_readpinreply(mb.payload, 26, 1);
    writeMsg(mb, "readpinreply.bin");

    setup_writepin(mb.payload, 15, 1);
    writeMsg(mb, "writepin.bin");

    setup_readanalog(mb.payload, 27);
    writeMsg(mb, "readanalog.bin");

    setup_readanalogreply(mb.payload, 6, 500);
    writeMsg(mb, "readanalogreply.bin");

    setup_readmem(mb.payload, 1500, 75);
    writeMsg(mb, "readmem.bin");

    const uint8_t testrm[] = {1, 2, 3, 4, 5};
    setup_readmemreply(mb.payload, 5, (void*)testrm);
    writeMsg(mb, "readmemreply.bin");

    const uint8_t testwm[] = {5, 4, 3, 2, 1};
    setup_writemem(mb.payload, 5678, 5, (void*)testwm);
    writeMsg(mb, "writemem.bin");

    setup_readinfo(mb.payload);
    writeMsg(mb, "readinfo.bin");

    setup_readinforeply(mb.payload, 1.1, 5678, 5000, 5);

    std::cout << "RI length: " << (int)payloadSize(mb.payload) << std::endl;
    std::cout << "RI calced:: " <<  sizeof(uint8_t) +  sizeof(float) +  sizeof(uint64_t) +  sizeof(uint16_t) +  sizeof(uint16_t)  << std::endl;

    writeMsg(mb, "readinforeply.bin");

    setup_readhumidity(mb.payload);
    writeMsg(mb, "readhumidity.bin");

    setup_readhumidityreply(mb.payload, 45.7);
    writeMsg(mb, "readhumidityreply.bin");

    setup_readtemperature(mb.payload);
    writeMsg(mb, "readtemperature.bin");

    setup_readtemperaturereply(mb.payload, 98.6);
    writeMsg(mb, "readtemperaturereply.bin");

    setup_readfield(mb.payload, 1);
    writeMsg(mb, "readfield.bin");

    auto mf = MapField { "wat", 1, 20, ff_uint32 };
    setup_readfieldreply(mb.payload, 7, mf);
    writeMsg(mb, "readfieldreply.bin");
}

void readMsgs()
{
  readMsg("ack.bin");
  readMsg("fail.bin");
  readMsg("pinmode.bin");
  readMsg("readpin.bin");
  readMsg("readpinreply.bin");
  readMsg("writepin.bin");
  readMsg("readanalog.bin");
  readMsg("readanalogreply.bin");
  readMsg("readmem.bin");
  readMsg("readmemreply.bin");
  readMsg("writemem.bin");
  readMsg("readinfo.bin");
  readMsg("readinforeply.bin");
  readMsg("readhumidity.bin");
  readMsg("readhumidityreply.bin");
  readMsg("readtemperature.bin");
  readMsg("readtemperaturereply.bin");
  readMsg("readfield.bin");
  readMsg("readfieldreply.bin");
}


// these test files are generated from the rust code.
int main(int argc, char *argv[])
{
  writeMsgs();
  // readMsgs();
  return 0;
}
