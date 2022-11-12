/*

   An Arduino library for the Automato sensor board

 */

#include <Automato.h>
#include <Esp.h>
#include <RH_RF95.h>
#include <AutomatoMsg.h>
#include <RHMesh.h>

RH_RF95 rf95(PIN_LORA_CS, PIN_LORA_IRQ); // Slave select, interrupt pin for Automato Sensor Module.

uint8_t from_id;

// convert a RH router code to AutomatoResult.
AutomatoResult arFromRc(uint8_t rc)
{
    switch (rc) {
        case RH_ROUTER_ERROR_NONE:
            return AutomatoResult(rc_ok);
        case RH_ROUTER_ERROR_INVALID_LENGTH:
            return AutomatoResult(rc_rh_router_error_invalid_length);
        case RH_ROUTER_ERROR_NO_ROUTE:
            return AutomatoResult(rc_rh_router_error_no_route);
        case RH_ROUTER_ERROR_TIMEOUT:
            return AutomatoResult(rc_rh_router_error_timeout);
        case RH_ROUTER_ERROR_NO_REPLY:
            return AutomatoResult(rc_rh_router_error_no_reply);
        case RH_ROUTER_ERROR_UNABLE_TO_DELIVER:
            return AutomatoResult(rc_rh_router_error_unable_to_deliver);
        default:
            return AutomatoResult(rc_invalid_rh_router_error);
    }
}

AutomatoLoraMesh::AutomatoLoraMesh(uint8_t networkid, Automato &automato)
    : rhmesh(rf95, networkid), automato(automato)
{
}

void AutomatoLoraMesh::init(float frequency, uint8_t power)
{
    rf95.init();

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    // specify by country?
    rf95.setFrequency(frequency);
    rf95.setTxPower(power);
}

AutomatoResult AutomatoLoraMesh::remotePinMode(uint8_t network_id, uint8_t pin, uint8_t mode)
{
    setup_pinmode(mb.payload, pin, mode);
    return sendRequest(network_id, mb);
}

AutomatoResult AutomatoLoraMesh::remoteDigitalWrite(uint8_t network_id, uint8_t pin, uint8_t value)
{
    setup_writepin(mb.payload, pin, value);
    return sendRequest(network_id, mb);
}

AutomatoResult AutomatoLoraMesh::remoteDigitalRead(uint8_t network_id, uint8_t pin, uint8_t *result)
{
    setup_readpin(mb.payload, pin);
    auto ar = sendRequest(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readpinreply) {
            *result = mb.payload.pinval.state;
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult AutomatoLoraMesh::remoteAnalogRead(uint8_t network_id, uint8_t pin, uint16_t *result)
{
    setup_readanalog(mb.payload, pin);
    auto ar = sendRequest(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readanalogreply) {
            *result = mb.payload.analogpinval.state;
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult AutomatoLoraMesh::remoteMemWrite(uint8_t network_id, uint16_t address, uint8_t length, void *value)
{
    AutomatoResult ar;
    bool tmp = (ar = setup_writemem(mb.payload, address, length, value))
        && (ar = sendRequest(network_id, mb));

    return ar;
}

AutomatoResult AutomatoLoraMesh::remoteMemRead(uint8_t network_id, uint16_t address, uint8_t length, void *value)
{
    AutomatoResult ar;

    if ((ar = setup_readmem(mb.payload, address, length)) && (ar = sendRequest(network_id, mb)))
    {
        if (mb.payload.type == pt_readmemreply) {
            memcpy(value, (void*)&mb.payload.readmemreply.data, length);
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult AutomatoLoraMesh::remoteTemperature(uint8_t network_id, float &temperature)
{
    setup_readtemperature(mb.payload);
    auto ar = sendRequest(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readtemperaturereply) {
            temperature = mb.payload.f;
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult AutomatoLoraMesh::remoteHumidity(uint8_t network_id, float &humidity)
{
    setup_readhumidity(mb.payload);
    auto ar = sendRequest(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readhumidityreply) {
            humidity = mb.payload.f;
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult AutomatoLoraMesh::remoteAutomatoInfo(uint8_t network_id, RemoteInfo &info)
{
    setup_readinfo(mb.payload);
    auto ar = sendRequest(network_id, mb);
    if (ar)
    {
        if (mb.payload.type == pt_readinforeply) {
            memcpy((void*)&info, (void*)&mb.payload.remoteinfo, sizeof(RemoteInfo));
            return AutomatoResult(rc_ok);
        }
        else
            return AutomatoResult(rc_invalid_reply_message);
    }
    else
        return ar;
}

AutomatoResult AutomatoLoraMesh::sendRequest(uint8_t network_id, Msgbuf &mb)
{
    uint8_t err;
    if ((err = rhmesh.sendtoWait((uint8_t*)&mb.payload, payloadSize(mb.payload), network_id)) == RH_ROUTER_ERROR_NONE) {
        uint8_t from_id;

        // mesh already does an Ack behind the scenes, but only between this
        // node and the next.  So we have to do our own ack with the final
        // destination.
        while (receiveLoraMessage(from_id, mb))
        {
            if (mb.payload.type == pt_fail)
                return AutomatoResult((ResultCode)mb.payload.failcode);
            else if (isReply((PayloadType)mb.payload.type))
                return AutomatoResult(rc_ok);
            else
            {
                // not a reply, a request.  process and listen for a reply.
                // TODO: remove?
                handleLoraMessage(from_id, mb);
            }
        }

        return AutomatoResult(rc_reply_timeout);
    }
    else
    {
        return arFromRc(err);
    }
}



AutomatoResult AutomatoLoraMesh::sendReply(uint8_t network_id, Payload &p)
{
    return arFromRc(rhmesh.sendtoWait((uint8_t*)&p, payloadSize(p), network_id));
}

bool AutomatoLoraMesh::receiveLoraMessage(uint8_t &from_id, Msgbuf &mb)
{
    uint8_t len = sizeof(mb.buf);
    mb.payload.f = 0;  // remove? TODO
    if (rhmesh.recvfromAckTimeout(mb.buf, &len, 1000, &from_id))
    {
        return true;
    }
    else
    {
        return false;
    }
}


AutomatoResult AutomatoLoraMesh::handleLoraMessage(uint8_t from_id, Msgbuf &mb)
{
    handleMessage(mb);
    return sendReply(from_id, mb.payload);
}

AutomatoResult AutomatoLoraMesh::handleSerialMessage(uint8_t to_id, Msgbuf &mb)
{
    if (to_id == rhmesh.thisAddress())
    {
        handleMessage(mb);
        return AutomatoResult(rc_ok);
    }
    else
    {
        // forward to another automato!
        return sendRequest(to_id, mb);
    }
}


// receives and handles remote control messages.
AutomatoResult AutomatoLoraMesh::doRemoteControl()
{
    if (receiveLoraMessage(from_id, mb)) {
        return handleLoraMessage(from_id, mb);
    }
    else
    {
        return AutomatoResult(rc_no_message_received);
    }
}


// receives and handles remote control messages.
// AutomatoResult AutomatoLoraMesh::doSerial()
// {
//     if (receiveSerialMessage()) {
//         do
//         {
//             handleSerialMessage(serialReader.to_id, serialReader.mb);
//             // write the response back through serial
//             writeSerialMessage(serialReader.to_id, serialReader.mb);
//         } while (receiveSerialMessage());
//         return AutomatoResult(rc_ok);
//     }
//     else
//     {
//         return AutomatoResult(rc_no_message_received);
//     }
// }




