#include <stdint.h>
#include <INA226.h>
#include <Wire.h>

// needed to place INA in SCOPE
INA226 INA(0x40);

// RESISTOR
uint8_t R = 100;

void setup()
{
    // setup INA226 & error check
    Wire.begin();
    if(!INA.begin())
    {
        Serial.println("INA226 had an error!");
    }
    // obtain voltage
    while(1)
    {
        float voltage = INA.getBusVoltage();
        Serial.println(voltage);
    }
}

void loop()
{

}