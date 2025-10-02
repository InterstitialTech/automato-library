#if defined(ARDUINO_AUTOMATO_BRAIN)

#include <Automato_Brain.h>
#include <Esp.h>
//#include <AutomatoMsg.h>

//Msgbuf mb;
uint8_t from_id;

Automato::Automato(uint8_t networkid, bool allowRemotePinOutputs)
    : databuf(nullptr),
    datalen(0),
    memoryMap(nullptr),
    fieldCount(0),
    allowRemotePinOutputs(allowRemotePinOutputs)
{
}

void Automato::init(float frequency, uint8_t power)
{
    // user LED
    pinMode(PIN_LED, OUTPUT);

    // SHTC3
  /*
    Wire.begin();
    shtc3.begin();
  */
}

void Automato::readTempHumidity(void)
{
    shtc3.update();
    temperature = shtc3.toDegF();
    humidity = shtc3.toPercent();
}

float Automato::getTemperature(void)
{
    return temperature;
}

float Automato::getHumidity(void)
{
    return humidity;
}

uint64_t Automato::macAddress(void)
{
    return ESP.getEfuseMac();
}

void Automato::initEspNow(void) {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
}

void Automato::peerEspNow(const uint8_t *mac_dest) {
  memcpy(this->espnow_peer_info.peer_addr, mac_dest, 6);
  this->espnow_peer_info.channel = 0;  
  this->espnow_peer_info.encrypt = false;
  if (esp_now_add_peer(&this->espnow_peer_info) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void Automato::setCallbackEspNow(esp_now_recv_cb_t cb) {
  esp_now_register_recv_cb(cb);
}

void Automato::sendStringEspNow(const uint8_t *mac_dest, const char *str) {
  esp_err_t rc;
  size_t len = strlen(str);

  if (len < 250) {
    strcpy((char *)this->espnow_buf, str);
    rc = esp_now_send(mac_dest, this->espnow_buf, len+1);
  } else {
    strncpy((char *)this->espnow_buf, str, 249);
    this->espnow_buf[249] = '\0';
    rc = esp_now_send(mac_dest, this->espnow_buf, min(len, (size_t)250));
  }

  if (rc != ESP_OK) {
    Serial.println("Error sending data over esp-now");
  }
}

void Automato::printMacAddressEspNow(void) {
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.print("MAC address: ");
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
    Serial.println();
  } else {
    Serial.println("Failed to read MAC address");
  }
}

#endif
