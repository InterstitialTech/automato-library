#ifndef AUTOMATO_BRAIN_H
#define AUTOMATO_BRAIN_H

#include <Arduino.h>

#include "SPI.h"
#include "SparkFun_SHTC3.h"
#include <AutomatoMsg.h>
#include <SerialReader.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>
#include <esp_wifi.h>

class Automato {

private:
  SHTC3 shtc3;
  float temperature, humidity;

  bool allowRemotePinOutputs;

  SerialReader serialReader;

  uint8_t espnow_buf[250];
  esp_now_peer_info_t espnow_peer_info;

public:
  // Simplest Automato constructor.
  Automato(uint8_t networkid, bool allowRemotePinOutputs);

  // Add a data area that can be read or written remotely.
  Automato(uint8_t networkid, void *databuf, uint16_t datalen,
           bool allowRemotePinOutputs);
  // Data area as above, but also a remotely-accessible memory map.
  Automato(uint8_t networkid, void *databuf, uint16_t datalen, void *mapentries,
           uint16_t mapentrycount, bool allowRemotePinOutputs);

  void init(float frequency = 915.0, uint8_t power = 13);

  void *databuf;
  uint16_t datalen;

  void *memoryMap;
  uint64_t fieldCount;

  //    void clearScreen(void);

  void readTempHumidity();
  float getTemperature();
  float getHumidity();

  static uint64_t macAddress();

  // receive and handle serial messages.
  AutomatoResult doSerial();

  /*
      // remote control functions.
      AutomatoResult remoteDigitalWrite(uint8_t network_id, uint8_t pin,
     uint8_t value);    // HIGH or LOW AutomatoResult
     remoteDigitalRead(uint8_t network_id, uint8_t pin, uint8_t *result);
     AutomatoResult remotePinMode(uint8_t network_id, uint8_t pin, uint8_t
     mode);    // INPUT, OUTPUT, or INPUT_PULLUP

      AutomatoResult remoteAnalogRead(uint8_t network_id, uint8_t pin,
     uint16_t *result);

      // 'raw' memory read/write.  use the macros below instead.
      AutomatoResult remoteMemWrite(uint8_t network_id, uint16_t address,
     uint8_t length, void *value); AutomatoResult remoteMemRead(uint8_t
     network_id, uint16_t address, uint8_t length, void *value);

      AutomatoResult remoteTemperature(uint8_t network_id, float
     &temperature); AutomatoResult remoteHumidity(uint8_t network_id, float
     &humidity); AutomatoResult remoteAutomatoInfo(uint8_t network_id,
     RemoteInfo &info);

      // receive and handle remote control messages.
      AutomatoResult doRemoteControl();

  */
  // lower level message sending and receiving.
  AutomatoResult sendRequest(const uint8_t *mac_dest, Msgbuf &mb);
  AutomatoResult handleEspNowMessage(const uint8_t *from_mac, Msgbuf &mb);
  AutomatoResult handleEspNowSerialMessage(const uint8_t *to_id, Msgbuf &mb);
  AutomatoResult handleLoraSerialMessage(const uint8_t to_id, Msgbuf &mb);
  void handleMessage(Msgbuf &mb);
  AutomatoResult sendReply(const uint8_t *dest_mac, Payload &p);
  /*
      bool receiveMessage(uint8_t &from_id, Msgbuf &mb);
      bool receiveSerialMessage();
      AutomatoResult handleLoraMessage(uint8_t from_id, Msgbuf &mb);

  */
  void initEspNow(void);
  void peerEspNow(const uint8_t *mac_dest);
  void sendStringEspNow(const uint8_t *mac_dest, const char *str);
  void setCallbackEspNow(esp_now_recv_cb_t cb);
  void printMacAddressEspNow(void);
};

void writeSerialMessage(uint8_t from_id, Msgbuf &mb);

// read/write from the memory map on a remote Automato.
#define remote_memwrite(dest, struct, field, val)                              \
  remoteMemWrite(dest, (uint16_t)offsetof(struct, field),                      \
                 (uint8_t)sizeof(struct ::field), (void *)val)
#define remote_memread(dest, struct, field, val)                               \
  remoteMemRead(dest, (uint16_t)offsetof(struct, field),                       \
                (uint8_t)sizeof(struct ::field), (void *)val)

// define memory map fields 'documentation' for remotes to read.
#define map_field(struct, field, format)                                       \
  MapField { #field, offsetof(struct, field), sizeof(struct ::field), format }

#endif /* AUTOMATO_SENSOR_H */
