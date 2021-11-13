/*

An Arduino library for the Automato sensor board

*/

#include <Automato.h>
#include <Esp.h>
#include <RH_RF95.h>
#include <AutomatoMsg.h>

RH_RF95 rf95(PIN_LORA_CS, PIN_LORA_IRQ); // Slave select, interrupt pin for Automato Sensor Module.

msgbuf mb;

// see note in Automato.h
Adafruit_ILI9341 screen(PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RST);

Automato::Automato()
{
// default constructor
}

void Automato::init(float frequency)
{
    rf95.init();

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    // specify by country?
    rf95.setFrequency(915.0);

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



bool Automato::remotePinMode(uint64_t destination_mac, uint8_t pin, uint8_t mode)
{
  setupMessage(mb.msg, macAddress(), destination_mac, mt_pinmode, pin, 1, mode);
  sendMessage(mb.msg);
}

bool Automato::remoteDigitalWrite(uint64_t destination_mac, uint8_t pin, uint8_t value)
{
  setupMessage(mb.msg, macAddress(), destination_mac, mt_write, pin, 1, value);
  sendMessage(mb.msg);
}

bool Automato::remoteDigitalRead(uint64_t destination_mac, uint8_t pin, uint8_t *result)
{
  setupMessage(mb.msg, macAddress(), destination_mac, mt_read, pin, 1, 0);
  sendMessage(mb.msg);
}

bool Automato::sendMessage(message &m)
{
  rf95.send((uint8_t*)&m, sizeof(message));
  rf95.waitPacketSent();
  receiveMessage(mb);
  return succeeded(mb.msg);
}

bool Automato::receiveMessage(msgbuf &mb)
{
  if (rf95.waitAvailableTimeout(3000))
  { 
    // is there a message for us?
    uint8_t len = sizeof(mb.buf);
    return rf95.recv(mb.buf, &len);
  }
  else
    return false;
}


void Automato::handleRcMessage(msgbuf &mb)
{
  uint64_t tomac = 0;
  memcpy(((char*)&tomac), &mb.msg.tomac, 6); 
  
  if (tomac == Automato::macAddress()) 
  {
    switch (mb.msg.type) {
      case mt_write:
        if (0 <= mb.msg.address && mb.msg.address < 40) {
          if (mb.msg.payload == 0) {
            digitalWrite(mb.msg.address, LOW);
            // indicate success
            mb.msg.type = mt_ack;
            mb.msg.payload = ac_success;
            rf95.send((const uint8_t*)&mb.msg, sizeof(message));
            rf95.waitPacketSent();
          } else if (mb.msg.payload == 1) {
            digitalWrite(mb.msg.address, HIGH);
            // indicate success
            mb.msg.type = mt_ack;
            mb.msg.payload = ac_success;
            rf95.send((const uint8_t*)&mb.msg, sizeof(message));
            rf95.waitPacketSent();
          }
        } else {
          // failed, invalid address.
          mb.msg.type = mt_ack;
          mb.msg.payload = ac_invalid_address;
          rf95.send((const uint8_t*)&mb.msg, sizeof(message));
          rf95.waitPacketSent();
        };
        break;
      default:
        // failed, unsupported message type.
        mb.msg.type = mt_ack;
        mb.msg.payload = ac_invalid_message_type;
        rf95.send((const uint8_t*)&mb.msg, sizeof(message));
        rf95.waitPacketSent();
        break;
    };
  }
  else {
    Serial.print("ignoring message for ");
    Serial.println(tomac);
    Serial.print("because my mac is ");
    Serial.println(Automato::macAddress());
  }
}

// receives and handles remote control messages.
void Automato::doRemoteControl()
{
  if (receiveMessage(mb)) {
    handleRcMessage(mb);
  }
}


