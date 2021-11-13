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

	// Automato_Status_TypeDef begin(TwoWire &wirePort = Wire);									   // Initializes the sensor
	static uint64_t macAddress();

  // initialize the register map here
  void init();

  void sendMessage();
  // probably called within a block of "if (automato.messageAvailable()) ..."
  void receiveMessage();
  // which parses the message and dispatches the writes, or else responds with a register read. probably a write command should solicit an "ACK/NAK" response, which is just another message type
  void handleMessage();

  bool remotePinMode(uint32_t address, uint8_t pin, uint8_t mode);
  bool remoteDigitalWrite(uint32_t address, uint8_t pin, uint8_t value);
  bool remoteDigitalRead(uint32_t address, uint8_t pin, uint8_t *result);

  void receiveMessage(msgbuf &mb);
  void processMessage(msgbuf &mb);

  void doRemoteControl();
};

#endif /* AUTOMATO_SENSOR */
