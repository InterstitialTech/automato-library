/*

An Arduino library for the Automato sensor board

*/

#include <Automato.h>
#include <Esp.h>

// see note in Automato.h
Adafruit_ILI9341 screen(PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RST);

Automato::Automato()
{
// default constructor
}

void Automato::init()
{

    // user LED
    pinMode(PIN_LED, OUTPUT);

    // LCD
    pinMode(PIN_LED_LCD, OUTPUT);
    digitalWrite(PIN_LED_LCD, HIGH);
    screen.begin();
    screen.setRotation(1);

    // SHTC3
    Wire.begin();
    shtc3.begin();

}

void Automato::clearScreen(void)
{
    screen.fillScreen(ILI9341_BLACK);
    screen.setTextColor(ILI9341_GREEN);
    screen.setTextSize(2);
    screen.setCursor(0,0);
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


