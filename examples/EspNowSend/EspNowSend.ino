#include <Automato.h>

// this is the MAC Address of the destination/receiver
//uint8_t MAC_DEST[] = {0x10, 0x91, 0xa8, 0x20, 0xc0, 0xc4};
// uint8_t MAC_DEST[] = {0xe4, 0xb3, 0x23, 0x8d, 0x75, 0x58};
uint8_t MAC_DEST[] = {0xe4, 0xb3, 0x23, 0x8d, 0x66, 0x3c};

Automato automato(255, false);

void setup() {

  automato.init();

  automato.initEspNow();
  automato.peerEspNow(MAC_DEST);

}

void loop() {

  digitalWrite(LED_BUILTIN, HIGH);
  automato.sendStringEspNow(MAC_DEST, "hello Automato!");
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

}

