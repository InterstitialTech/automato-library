/*
 * Automato "Hello World"
 *  - Display temperature, humidity, and chip ID on the screen
 *  
 *  Interstitial Technology, 20210721
 *  info@interstitial.coop
 *  
 */

#include <Automato.h>

Automato automato(255, false);  // network ID, allow remote pin outputs

void setup() {

    automato.init();

}

void loop() {

    // get a reading from the temp/humidity sensor
    automato.readTempHumidity();

    // output to LCD
    updateLCD();

    delay(300);

}

void updateLCD() {

    automato.clearScreen();

    screen.println("Hello world!");

    screen.print("Temperature = ");
    screen.print(automato.getTemperature());
    screen.println(" F");

    screen.print("Humidity = ");
    screen.print(automato.getHumidity());
    screen.println("%");

    screen.print("ID = ");
    screen.println(automato.macAddress());

}

