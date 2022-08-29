#include <AutomatoResult.h>
#include <iostream>
#include <Arduino.h>
#include <AutomatoMsg.h>
#include <cstdio>
#include <string>

using namespace std;


void readMsg(const char *dir, Msgbuf &mb, const char *fname)
{
    std::cout << fname << std::endl;

    string s(dir);
    s += "//";
    s += fname;

    FILE * filp = fopen(s.c_str(), "rb");
    int bytes_read = fread(mb.buf, sizeof(char), RH_RF95_MAX_MESSAGE_LEN, filp);

    bool debug = false;
    if (debug) 
    {
        std::cout << "read bytes: " << bytes_read << std::endl;
        for (int i = 0; i < bytes_read; ++i)
        {
            std::cout << (int)mb.buf[i] << " " << endl;
        }

    }

    printPayload(mb.payload);

    cout << std::endl; 
}


void writeMsg(const char *dir, Msgbuf &mb, const char * fname)
{
    string s(dir);
    s += "//";
    s += fname;

    size_t sz = payloadSize(mb.payload);
    cout << fname << " sz: " << sz << endl;
    FILE * filp = fopen(s.c_str(), "wb");
    int bytes_read = fwrite(mb.buf, sizeof(char), sz, filp);
}

void writeMsgs(const char *dir)
{
    Msgbuf mb;

    setup_ack(mb.payload);
    writeMsg(dir, mb, "ack.bin");

    setup_fail(mb.payload, rc_invalid_rh_router_error);
    writeMsg(dir, mb, "fail.bin");

    setup_pinmode(mb.payload, 26, 2);
    writeMsg(dir, mb, "pinmode.bin");

    setup_readpin(mb.payload, 22);
    writeMsg(dir, mb, "readpin.bin");

    setup_readpinreply(mb.payload, 26, 1);
    writeMsg(dir, mb, "readpinreply.bin");

    setup_writepin(mb.payload, 15, 1);
    writeMsg(dir, mb, "writepin.bin");

    setup_readanalog(mb.payload, 27);
    writeMsg(dir, mb, "readanalog.bin");

    setup_readanalogreply(mb.payload, 6, 500);
    writeMsg(dir, mb, "readanalogreply.bin");

    setup_readmem(mb.payload, 1500, 75);
    writeMsg(dir, mb, "readmem.bin");

    const uint8_t testrm[] = {1, 2, 3, 4, 5};
    setup_readmemreply(mb.payload, 5, (void*)testrm);
    writeMsg(dir, mb, "readmemreply.bin");

    const uint8_t testwm[] = {5, 4, 3, 2, 1};
    setup_writemem(mb.payload, 5678, 5, (void*)testwm);
    writeMsg(dir, mb, "writemem.bin");

    setup_readinfo(mb.payload);
    writeMsg(dir, mb, "readinfo.bin");

    setup_readinforeply(mb.payload, 1.1, 5678, 5000, 5);
    writeMsg(dir, mb, "readinforeply.bin");

    setup_readhumidity(mb.payload);
    writeMsg(dir, mb, "readhumidity.bin");

    setup_readhumidityreply(mb.payload, 45.7);
    writeMsg(dir, mb, "readhumidityreply.bin");

    setup_readtemperature(mb.payload);
    writeMsg(dir, mb, "readtemperature.bin");

    setup_readtemperaturereply(mb.payload, 98.6);
    writeMsg(dir, mb, "readtemperaturereply.bin");

    setup_readfield(mb.payload, 1);
    writeMsg(dir, mb, "readfield.bin");

    auto mf = MapField { "wat", 77, 20, ff_uint32 };
    setup_readfieldreply(mb.payload, 7, mf);
    writeMsg(dir, mb, "readfieldreply.bin");
}

bool readMsgs(const char *dir)
{
    Msgbuf mb;

    readMsg( dir,mb, "ack.bin");
    if (mb.payload.type != pt_ack) {
      cout << "ack msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "fail.bin");
    if (mb.payload.type != pt_fail || mb.payload.failcode != rc_invalid_rh_router_error) {
      cout << "fail msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "pinmode.bin");
    if (mb.payload.type != pt_pinmode || mb.payload.pinmode.pin != 26 || mb.payload.pinmode.mode != 2) {
      cout << "pinmode msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "readpin.bin");
    if (mb.payload.type != pt_readpin || mb.payload.pin != 22) {
      cout << "readpin msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "readpinreply.bin");
    if (mb.payload.type != pt_readpinreply || mb.payload.pinval.pin != 26 || mb.payload.pinval.state != 1) {
      cout << "readpin msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "writepin.bin");
    if (mb.payload.type != pt_writepin || mb.payload.pinval.pin != 15 || mb.payload.pinval.state != 1) {
      cout << "writepin msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "readanalog.bin");
    if (mb.payload.type != pt_readanalog || mb.payload.pin != 27) {
      cout << "readanalog msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "readanalogreply.bin");
    if (mb.payload.type != pt_readanalogreply || mb.payload.analogpinval.pin != 6 || mb.payload.analogpinval.state != 500) {
      cout << "readanalogreply msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "readmem.bin");
    if (mb.payload.type != pt_readmem || mb.payload.readmem.address != 1500 || mb.payload.readmem.length != 75) {
      cout << "readmem msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "readmemreply.bin");
    const uint8_t testrm[] = {1, 2, 3, 4, 5};

    cout << "wat: " <<  memcmp(mb.payload.readmemreply.data, (void*)testrm, 4) << endl;
    if (mb.payload.type != pt_readmemreply ||
        mb.payload.readmemreply.length != 5 ||
        memcmp(mb.payload.readmemreply.data, testrm, 5) != 0) {
      cout << "readmemreply msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "writemem.bin");
    const uint8_t testwm[] = {5, 4, 3, 2, 1};
    if (mb.payload.type != pt_writemem ||
        mb.payload.writemem.length != 5 ||
        memcmp(mb.payload.writemem.data, testwm, 5) != 0) {
      cout << "writemem msg failed" << endl;
      return false;
    }
    readMsg( dir,mb, "readinfo.bin");
    if (mb.payload.type != pt_readinfo) {
      cout << "readinfo msg failed" << endl;
      return false;
    }

    readMsg( dir,mb, "readinforeply.bin");

    if (mb.payload.type != pt_readinforeply ||
        (mb.payload.remoteinfo.protoversion - 1.1) > 0.0000001 ||
        mb.payload.remoteinfo.macAddress != 5678 ||
        mb.payload.remoteinfo.datalen != 5000 ||
        mb.payload.remoteinfo.fieldcount != 5 )  {
      cout << "readinforeply msg failed" << endl;
      return false;
    }
    readMsg( dir,mb, "readhumidity.bin");
    if (mb.payload.type != pt_readhumidity) {
      cout << "readhumidity msg failed" << endl;
      return false;
    }
    readMsg( dir,mb, "readhumidityreply.bin");
    if (mb.payload.type != pt_readhumidityreply ||
        (mb.payload.f - 45.7) > 0.000001) {
      cout << "readhumidityreply msg failed" << endl;
      return false;
    }
    readMsg( dir,mb, "readtemperature.bin");
    if (mb.payload.type != pt_readtemperature) {
      cout << "readtemperature msg failed" << endl;
      return false;
    }
    readMsg( dir,mb, "readtemperaturereply.bin");
    if (mb.payload.type != pt_readtemperaturereply ||
        (mb.payload.f - 98.6) > 0.000001) {
      cout << "readtemperaturereply msg failed" << endl;
      return false;
    }
    readMsg( dir,mb, "readfield.bin");
    if (mb.payload.type != pt_readfield ||
        mb.payload.readfield.fieldindex != 1) {
      cout << "readfield msg failed" << endl;
      return false;
    }
    readMsg( dir,mb, "readfieldreply.bin");
    if (mb.payload.type != pt_readfieldreply ||
        mb.payload.readfieldreply.fieldindex != 7 ||
        mb.payload.readfieldreply.offset != 77 ||
        mb.payload.readfieldreply.length != 20 ||
        mb.payload.readfieldreply.format != ff_uint32)
     {
      cout << "readfieldreply msg failed" << endl;
      return false;
    }

    cout << "tests succeeded" << endl;
    return true;
}


void showSyntax() {
    cout << "msgs syntax:" << endl
         << "msgs in <dirname>" << endl
         << "msgs out <dirname>" << endl;
}


// these test files are generated from the rust code.
int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        if (strcmp(argv[1], "in") == 0) {
            readMsgs(argv[2]);
            return 0;
        }
        else if (strcmp(argv[1], "out") == 0) {
            writeMsgs(argv[2]);
            return 0;
        }
        else
        {
            showSyntax();
            return 1;
        }
    }
    else
    {
        showSyntax();
        return 1;
    }
}
