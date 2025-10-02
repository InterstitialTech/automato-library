#include <Automato.h>

Automato automato(255, false);

// TODO: move these to a class method
uint8_t buf[250];
bool received = false;
void callback(const esp_now_recv_info* info, const uint8_t* data, int len) {
  memcpy(&buf, data, len);
  received = true;
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("-> Message received: ");
  Serial.println((char*)buf);
}

void setup() {

  Serial.begin(115200);
  while (!Serial) {}

  automato.init();
  automato.initEspNow();

  automato.setCallbackEspNow(callback);
  automato.printMacAddressEspNow(); // for debugging

}

void loop() {

  if (received) {
    received = false;
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(900);
  }

}


