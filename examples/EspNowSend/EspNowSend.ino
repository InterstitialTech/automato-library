#include <Automato.h>

// this is the MAC Address of the destination/receiver
uint8_t MAC_DEST[] = {0x10, 0x91, 0xa8, 0x20, 0xc0, 0xc4};

Automato automato(255, false);

void setup() {

  automato.init();
  automato.initEspNow();
  automato.peerEspNow(MAC_DEST);

}

void loop() {

  digitalWrite(PIN_LED, HIGH);
  automato.sendStringEspNow(MAC_DEST, "hello Automato!");
  delay(100);
  digitalWrite(PIN_LED, LOW);
  delay(1000);

}

