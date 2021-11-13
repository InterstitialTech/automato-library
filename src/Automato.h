/*

An Arduino library for the Automato sensor board

*/

#ifndef AUTOMATO_SENSOR
#define AUTOMATO_SENSOR

#include <Arduino.h>
#include <Wire.h>
#include <AutomatoMsg.h>

class Automato
{
private:
protected:
public:
	Automato(); // Constructor

	static uint64_t macAddress();

  // initialize automato networking.
  void init(float frequency = 915.0);

  // remote control functions.
  bool remotePinMode(uint64_t destination_mac, uint8_t pin, uint8_t mode);
  bool remoteDigitalWrite(uint64_t destination_mac, uint8_t pin, uint8_t value);
  bool remoteDigitalRead(uint64_t destination_mac, uint8_t pin, uint8_t *result);

  // receive and handle remote control messages.
  void doRemoteControl();

  // lower level message sending and receiving.
  bool sendMessage(message &m);
  bool receiveMessage(msgbuf &mb);
  void handleRcMessage(msgbuf &mb);
};

#endif /* AUTOMATO_SENSOR */
