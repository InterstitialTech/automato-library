#include <AutomatoNetwork.h>

AutomatoResult Automato::handleSerialMessage(uint8_t to_id, Msgbuf &mb)
{
    if (to_id == rhmesh.thisAddress())
    {
        automato.handleMessage(mb);
        return AutomatoResult(rc_ok);
    }
    else
    {
        // forward to another automato!
        return sendRequest(to_id, mb);
    }
}

// receives and handles remote control messages.
AutomatoResult Automato::doSerial()
{
    if (receiveSerialMessage()) {
        do
        {
            handleSerialMessage(serialReader.to_id, serialReader.mb);
            // write the response back through serial
            writeSerialMessage(serialReader.to_id, serialReader.mb);
        } while (receiveSerialMessage());
        return AutomatoResult(rc_ok);
    }
    else
    {
        return AutomatoResult(rc_no_message_received);
    }
}


