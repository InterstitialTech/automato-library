#include <Automato.h>

Automato automato(255, false);

// TODO: move these to a class method
uint8_t buf[250];
bool received = false;
void callback(const uint8_t *mac, const uint8_t *data, int len) {
  memcpy(&buf, data, len);
  received = true;
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("-> Message received: ");
  Serial.println((char*)buf);
}

void setup() {

  automato.init();
  automato.initEspNow();

  Serial.begin(115200);
  while (!Serial) {}

  automato.setCallbackEspNow(callback);
  automato.printMacAddressEspNow(); // for debugging

}

void loop() {

  if (received) {
    received = false;
    digitalWrite(PIN_LED, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);
    delay(900);
  }

}


