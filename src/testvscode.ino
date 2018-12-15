#include "scrolltext.h"
#include "display.h"
#ifndef _Adafruit_SSD1306_H
#define _Adafruit_SSD1306_H
#include <Adafruit_SSD1306.h>
#endif
#include <OneWire.h>
/*
 * Project kitclo
 * Description:
 * Author:
 * Date:
 */

// use hardware SPI
#define OLED_DC D3
#define OLED_CS D4
#define OLED_RESET D5
#define TEMP_WIRE D6

#define TEMP D0
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
Display d;
ScrollText s1(128, 4, 0.05);
ScrollText s2(128, 4, 0.05);
String hello = "";
String times;
String dates;
int lastMinute = 61;
unsigned long lastForecastMessage = 0 - 5400 * 1000;
bool forecastPending = false;
bool tempTest = false;
OneWire ds = OneWire(TEMP_WIRE); // 1-wire signal on pin D4

unsigned long lastUpdate = 0;

float lastTemp;

// setup() runs once, when the device is first turned on.
void setup()
{
    Serial.begin(9600);
    noBreathingWhenOK();
    pinMode(D1, OUTPUT);

    Particle.function("setMessage", setMessage);
    Particle.subscribe("hook-response/temp", setForecastMessage, MY_DEVICES);

    d.start(&display);
    d.setBrightnessLevel(0);

    s1.setText(hello);
    s2.setText(hello);
    s1.start();
    s2.start();
    Time.beginDST();
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
    // Serial.println("loop");
    //ts.update();
    //setTemp();
    setTime();
    s1.update();
    s2.update();
    String *message = s1.getCurrentText();
    int messageOffset = s1.getCurrentOffset();
    String *timeStr = s2.getCurrentText();
    int timeOffset = s2.getCurrentOffset();
    display.clearDisplay();

    display.setCursor(messageOffset, 36);
    display.print(*message);
    display.setCursor(timeOffset, 0);
    display.print(*timeStr);
    display.display();
    doForecast();
    // tempSensor();
}

void setTime()
{
    const int currentTime = Time.now();
    const int m = Time.minute(currentTime);
    if (m != lastMinute)
    {
        char myTimeString[125] = "";
        int h = Time.hour(currentTime);
        sprintf(myTimeString, "%02d:%02d", h, m);
        times = String(myTimeString, 5);
        s2.setText(times);
        lastMinute = m;
        if (h == 12 && m == 0)
        {
            Particle.syncTime();
        }
        if (h == 12 && m == 0)
        {
            Particle.syncTime();
        }
        int day = Time.day(currentTime);
        int month = Time.month(currentTime);
        sprintf(myTimeString, "%02d/%02d", day, month);
        dates = String(myTimeString, 5);
        if (hello.length() == 0)
        {
            s1.setText(dates);
        }
    };
}
int setMessage(String msg)
{
    hello = msg;
    if (hello.length() == 0)
    {
        s1.setText(dates);
    }
    else
    {
        tone(D1, 100, 50);

        s1.setText(hello);
    }
    return 1;
}

void setForecastMessage(const char *event, const char *data)
{
    lastForecastMessage = millis();
    forecastPending = false;
    String s(data);
    setMessage(s);
}

void doForecast()
{
    unsigned long now = millis();
    if (forecastPending)
    {
        if (now - lastForecastMessage > 60000)
        {
            lastForecastMessage = now;
            Particle.publish("temp", "", PRIVATE);
            return;
        }
    }
    if (now - lastForecastMessage > 5400 * 1000)
    {
        lastForecastMessage = now;
        forecastPending = true;
        Particle.publish("temp", "", PRIVATE);
    }
}

/*
 * No green breathing led if ok
 */

void noBreathingWhenOK()
{
    LEDSystemTheme theme;
    theme.setColor(LED_SIGNAL_CLOUD_CONNECTED, 0x00000000); // Set LED_SIGNAL_NETWORK_ON to no color
    theme.apply();                                          // Apply theme settings
}

void tempSensor()
{
    if (tempTest)
    {
        return;
    }
    tempTest = true;
    byte i;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    float celsius, fahrenheit;

    if (!ds.search(addr))
    {
        Serial.println("No more addresses.");
        Serial.println();
        ds.reset_search();
        delay(250);
        return;
    }

    // The order is changed a bit in this example
    // first the returned address is printed

    Serial.print("ROM =");
    for (i = 0; i < 8; i++)
    {
        Serial.write(' ');
        Serial.print(addr[i], HEX);
    }

    // second the CRC is checked, on fail,
    // print error and just return to try again

    if (OneWire::crc8(addr, 7) != addr[7])
    {
        Serial.println("CRC is not valid!");
        return;
    }
    Serial.println();

    // we have a good address at this point
    // what kind of chip do we have?
    // we will set a type_s value for known types or just return

    // the first ROM byte indicates which chip
    switch (addr[0])
    {
    case 0x10:
        Serial.println("  Chip = DS1820/DS18S20");
        type_s = 1;
        break;
    case 0x28:
        Serial.println("  Chip = DS18B20");
        type_s = 0;
        break;
    case 0x22:
        Serial.println("  Chip = DS1822");
        type_s = 0;
        break;
    case 0x26:
        Serial.println("  Chip = DS2438");
        type_s = 2;
        break;
    default:
        Serial.println("Unknown device type.");
        return;
    }

    // this device has temp so let's read it

    ds.reset();      // first clear the 1-wire bus
    ds.select(addr); // now select the device we just found
    // ds.write(0x44, 1);     // tell it to start a conversion, with parasite power on at the end
    ds.write(0x44, 0); // or start conversion in powered mode (bus finishes low)

    // just wait a second while the conversion takes place
    // different chips have different conversion times, check the specs, 1 sec is worse case + 250ms
    // you could also communicate with other devices if you like but you would need
    // to already know their address to select them.

    delay(1000); // maybe 750ms is enough, maybe not, wait 1 sec for conversion

    // we might do a ds.depower() (parasite) here, but the reset will take care of it.

    // first make sure current values are in the scratch pad

    present = ds.reset();
    ds.select(addr);
    ds.write(0xB8, 0); // Recall Memory 0
    ds.write(0x00, 0); // Recall Memory 0

    // now read the scratch pad

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE, 0); // Read Scratchpad
    if (type_s == 2)
    {
        ds.write(0x00, 0); // The DS2438 needs a page# to read
    }

    // transfer and print the values

    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
    for (i = 0; i < 9; i++)
    { // we need 9 bytes
        data[i] = ds.read();
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s == 2)
        raw = (data[2] << 8) | data[1];
    byte cfg = (data[4] & 0x60);

    switch (type_s)
    {
    case 1:
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10)
        {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
        celsius = (float)raw * 0.0625;
        break;
    case 0:
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00)
            raw = raw & ~7; // 9 bit resolution, 93.75 ms
        if (cfg == 0x20)
            raw = raw & ~3; // 10 bit res, 187.5 ms
        if (cfg == 0x40)
            raw = raw & ~1; // 11 bit res, 375 ms
        // default is 12 bit resolution, 750 ms conversion time
        celsius = (float)raw * 0.0625;
        break;

    case 2:
        data[1] = (data[1] >> 3) & 0x1f;
        if (data[2] > 127)
        {
            celsius = (float)data[2] - ((float)data[1] * .03125);
        }
        else
        {
            celsius = (float)data[2] + ((float)data[1] * .03125);
        }
    }

    // remove random errors
    if ((((celsius <= 0 && celsius > -1) && lastTemp > 5)) || celsius > 125)
    {
        celsius = lastTemp;
    }

    fahrenheit = celsius * 1.8 + 32.0;
    lastTemp = celsius;
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");

    // now that we have the readings, we can publish them to the cloud
    String temperature = String(celsius);                  // store temp in "temperature" string
    Particle.publish("temperature", temperature, PRIVATE); // publish to cloud
                                                           // delay(5000); // 5 second delay
    setMessage(temperature);
}
