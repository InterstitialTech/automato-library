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
};

#endif /* AUTOMATO_SENSOR */
