/*

An Arduino library for the Automato sensor board

*/

#include <Automato.h>
#include <Esp.h>
#include <RH_RF95.h>
#include <AutomatoMsg.h>

uint64_t Automato::macAddress() 
{
  return ESP.getEfuseMac();
}

void setupMessage(message &m, 
  uint64_t frommac,
  uint64_t tomac,
  char type,
  int  address,
  int  length,
  int  payload) 
{
  memcpy((char*)m.frommac, (const char*)&frommac, 6);
  memcpy((char*)m.tomac, (const char*)&tomac, 6);
  m.type = type;
  m.address = address;
  m.length = length;
  m.payload = payload;
}

void printMessage(message &m) 
{
  Serial.println("message");

  Serial.print("frommac: ");
  uint64_t mac = 0;
  memcpy(((char*)&mac), &m.frommac, 6); 
  Serial.println(mac);

  Serial.print("tomac: ");
  mac = 0;
  memcpy(((char*)&mac), &m.tomac, 6); 
  Serial.println(mac);

  Serial.print("type: ");
  switch ((int)m.type) {
    case mt_read: 
      Serial.println("mt_read");
      break;
    case mt_write: 
      Serial.println("mt_write");
      break;
    case mt_ack: 
      Serial.println("mt_ack");
      break;
    default:
      Serial.print("unknown message type: ");
      Serial.println((int)m.type);
      break;
  }

  Serial.print("address: ");
  Serial.println(m.address);
  Serial.print("payload: ");
  Serial.println(m.payload);
}


bool Automato::remotePinMode(uint32_t address, uint8_t pin, uint8_t mode)
{

}
bool Automato::remoteDigitalWrite(uint32_t address, uint8_t pin, uint8_t value)
{

}
bool Automato::remoteDigitalRead(uint32_t address, uint8_t pin, uint8_t *result)
{

}
