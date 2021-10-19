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

Adafruit_ILI9341 tft = Adafruit_ILI9341(PIN_LCD_CS, PIN_LCD_DC, MOSI, SCK, PIN_LCD_RST, MISO);
SHTC3 shtc3;
float temperature, humidity;

void setup() {

    // user LED
    pinMode(PIN_LED, OUTPUT);

    // USB serial
    Serial.begin(115200);
    while(!Serial)

    // LCD
    pinMode(PIN_LED_LCD, OUTPUT);
    digitalWrite(PIN_LED_LCD, HIGH);
    tft.begin();
    tft.setRotation(1);

    // SHTC3
    Wire.begin();
    shtc3.begin();

    delay(1000);
    Serial.println("hello world!");

    Serial.println("automato mac:");
    Serial.println(Automato::macAddress());

}

void updateLCD() {

    tft.fillScreen(ILI9341_BLACK);

    tft.setCursor(0,0);
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(4);
    tft.println("Hello World!");
    tft.println();

    tft.setTextSize(2);

    tft.setTextColor(ILI9341_RED);
    tft.print("Temperature = ");
    tft.print(temperature);
    tft.println(" F");
    tft.println();

    tft.setTextColor(ILI9341_BLUE);
    tft.print("Humidity = ");
    tft.print(humidity);
    tft.println("%");
    tft.println();

}


void loop() {

    // get a reading from the temp/humidity sensor
    shtc3.update();
    temperature = shtc3.toDegF();
    humidity = shtc3.toPercent();

    // output to serial
    Serial.print("RH = ");
    Serial.print(shtc3.toPercent());
    Serial.print("%, T = ");
    Serial.print(shtc3.toDegF());
    Serial.println(" deg F");

    // output to LCD
    updateLCD();

    // blinky blink
    digitalWrite(PIN_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(50);                       // wait for a second
    digitalWrite(PIN_LED, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);

}
