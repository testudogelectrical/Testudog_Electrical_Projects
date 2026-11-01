Testudog Specs:
24 V
3 A

Need to:
Display voltages & currents to a screen.
This device acts like a multimeter.

Can do this by:
Use a ADC to convert readings of voltages & currents into digital values.
Read these values using some device.
Use programming code for determine the “size” of the voltage / current.
For example, if the voltage / current is REALLY small, might want to multiply by factor of 1000 and then reduce the size (i.e. from A → mA, mA → μA, etc.)
Probably don’t have to worry about really big or really small voltages / currents, so probably only worth it to implement from micro to standard.
Send the output and the size indicator (standard, m, μ) out using a DAC.
Find some way to obtain all 4 digits (will assume display allows for 4 digits).
Display these using some codes (for example BCD codes).
Actually can use programming code for this too.
Take mA / V output and convert each with BCD, will just use 8421 because don’t care about complementing.
Then can send out BCD for each, which will then convert to corresponding digits to display.

4 Digits:
first three digits are WHOLE, last is DECIMAL.
if provided more digits on display can display more decimals, but if only 4 then just one decimal.
for example: 001.3 = 1.3 
So if ADC gets a value < 1 or >= 1000 need to fix the size! 

Materials Needed:
Analog to Digital Converter
Microcontroller
Arduino?
STM?
Code is written in C.
Code gets sent back to microcontroller on another peripheral.
Materials for digital display:
7-Segment Display.
Small covering.

How are the BCD digits actually sent out?
They are sent out like this:
A3A2A1A0
B3B2B1B0
C3C2C1C0
D3D2D1D0 → Decimal Digit
All of the above are actually next to each other.
These are all sent to individual peripherals.
We want to maintain info on the value (0-9).
This will need a lot of peripherals…
how to reduce amount of peripherals to use?

If there are only 2 digits required then…

Range: 3-6 A

Analog to digital converter:
Reports binary number (peak-to-peak?)
Need ADC pins for both voltage & current.
Don’t really know how many required… at least 2 I suspect.

Microcontrollers like arduinos have ADC pins. I’m guessing so do some of the other ones.
So the microcontroller will be able to get the value. Then I just need to take that value, then extract each individual digits. Then configure the display accordingly (based on requirements from the 7-segment display. 

teensy4.1 → Microcontroller being bought.

Want a button which can be used to switch between measuring voltage & current (they are always being measured, but only one displayed at a time). This is so we don’t need more displays.

INA226 chip for current and voltage measurements. Communication between the chip's internal registers and microcontroller through I2C. 
https://github.com/RobTillaart/INA226

2 digit seven segment display



Code worked on so far:
https://github.com/abdurrafay-atif/Testudog-LCD
LCD2 is the actual one, LCD1 was what I worked on before refining requirements.

2-6 V
3

digital display

analog → digital
digital 
→ digits
each digit → code for display
→ display

pins (teensy 4.1):
A0 / A1 → Current / Voltage
Digital Pin for Push Button

how to extract pin values to program?
how to write to output.

https://gist.github.com/somebox/d969f8a97e5a4362af5049ed554a9e69

how to drive to multiple LEDs:
Use multiplexing.
Connect multiplexers to same pins.
Drive the input of dig1 with output 1 for its turn on state.
Then turn off dig1, turn on dig2 with output 1 for its turn on state.
This works because although this is sequential, it is so fast that it might as well be constant.

https://www.tinkercad.com/things/cdDEi5Un6tK/editel?returnTo=%2Fdashboard&sharecode=PgEVcys5yUX3hpkLWMwqdVEcHrvH9arZUOUIKDFmJhE
