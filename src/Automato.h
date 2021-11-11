/*

An Arduino library for the Automato sensor board

*/

#ifndef AUTOMATO_SENSOR
#define AUTOMATO_SENSOR

#include <Arduino.h>
#include <Wire.h>

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
};

#endif /* AUTOMATO_SENSOR */
