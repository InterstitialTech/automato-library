/*

An Arduino library for the Automato sensor board

*/

#include <Automato.h>
#include <Esp.h>
#include <RH_RF95.h>
#include <registermsg.hh>

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
  memcpy((char*)m.frommac, (const char*)&frommac, 4);
  memcpy((char*)m.tomac, (const char*)&tomac, 4);
  m.type = type;
  m.address = address;
  m.length = length;
  m.payload = payload;
}

void printMac(const char *cmac)
{
  Serial.print("mac0: ");
  Serial.println((int)cmac[0]);
  Serial.print("mac1: ");
  Serial.println((int)cmac[1]);
  Serial.print("mac2: ");
  Serial.println((int)cmac[2]);
  Serial.print("mac3: ");
  Serial.println((int)cmac[3]);
  Serial.print("mac4: ");
  Serial.println((int)cmac[4]);
  Serial.print("mac5: ");
  Serial.println((int)cmac[5]);
  Serial.print("mac6: ");
  Serial.println((int)cmac[6]);
  Serial.print("mac7: ");
  Serial.println((int)cmac[7]);
}

void printMessage(message &m) 
{
  Serial.println("message");

  // Serial.print("frommac: ");
  // printMac((char*)m.frommac);

  Serial.println("frommac: ");
  printMac((char*)&m.frommac);
  // uint64_t mac = 0;
  // memcpy(((char*)&mac), &m.frommac, 6); 
  // Serial.println(mac);

  Serial.println("tomac: ");
  printMac((char*)&m.tomac);
  // mac = 0;
  // memcpy(((char*)&mac), &m.tomac, 6); 
  // Serial.println(mac);

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

