#include <stdint.h>

// BUTTON input signal
uint8_t BUTTON = 40;

void setup()
{
    // set button input
    pinMode(BUTTON, INPUT);
    // obtain current
    while(1)
    {
        uint8_t in = digitalRead(BUTTON);
        Serial.println(in);
    }
}

void loop()
{

}