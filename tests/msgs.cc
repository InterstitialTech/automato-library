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
    cout << fname << " sz: " << sz << endl;
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
    readMsg("rustmsgs-out/ack.bin");



    readMsg("rustmsgs-out/fail.bin");
    readMsg("rustmsgs-out/pinmode.bin");
    readMsg("rustmsgs-out/readpin.bin");
    readMsg("rustmsgs-out/readpinreply.bin");
    readMsg("rustmsgs-out/writepin.bin");
    readMsg("rustmsgs-out/readanalog.bin");
    readMsg("rustmsgs-out/readanalogreply.bin");
    readMsg("rustmsgs-out/readmem.bin");
    readMsg("rustmsgs-out/readmemreply.bin");
    readMsg("rustmsgs-out/writemem.bin");
    readMsg("rustmsgs-out/readinfo.bin");
    readMsg("rustmsgs-out/readinforeply.bin");
    readMsg("rustmsgs-out/readhumidity.bin");
    readMsg("rustmsgs-out/readhumidityreply.bin");
    readMsg("rustmsgs-out/readtemperature.bin");
    readMsg("rustmsgs-out/readtemperaturereply.bin");
    readMsg("rustmsgs-out/readfield.bin");
    readMsg("rustmsgs-out/readfieldreply.bin");
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
