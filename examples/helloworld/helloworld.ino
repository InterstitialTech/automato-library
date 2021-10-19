/*
 * Automato "Hello World"
 *  - Blink an LED
 *  - Print to serial
 *  - Read the Temperature sensor
 *  - Display the temperature on the screen
 *  
 *  Chris Chronopoulos 20210721
 *  chrono@interstitial.coop
 *  
 */

#include <Automato.h>


//
Automato automato;
//

void setup() {

    automato.init();

}

void updateLCD() {

    automato.clearScreen();

    automato.appendToScreenLn("Hello world!");

    automato.appendToScreen("Temperature = ");
    automato.appendToScreen(automato.getTemperature());
    automato.appendToScreenLn(" F");

    automato.appendToScreen("Humidity = ");
    automato.appendToScreen(automato.getHumidity());
    automato.appendToScreenLn("%");

}


void loop() {

    // get a reading from the temp/humidity sensor
    automato.readTempHumidity();

    // output to LCD
    updateLCD();

}
