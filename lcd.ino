#include <stdint.h>
#include <INA226.h>
#include <Wire.h>

///////////////////
//   PIN USAGE   //
///////////////////

// needed to place INA in SCOPE
INA226 INA(0x40);

// RESISTOR
uint8_t R = 100;

// BUTTON input signal
uint8_t BUTTON = 40;

// 7-Seg Display OUTPUTS
uint8_t seg7[] = {3, 4, 5, 6, 7, 8, 9};
uint8_t DP = 10;    // decimal point used when reading < 10 so we get more accurate reading
uint8_t LED1 = 11;  // used to turn on / off MSD of output
uint8_t LED2 = 12;  // used to turn on / off LSD of output
uint8_t NUM_SEGS = 7; // # of segments

//////////////////
// DATA STORAGE //
//////////////////

// differentiates modes
// mode is determined by button press
// when the button is not being pressed, there is no signal... thus in VOLTAGE mode
// when the button is being pressed, signal appears... thus in CURRENT mode
// the LCD will display depending on button press (note only changes mode AS the button is pressed, not WHENEVER the button is being pressed)
typedef enum
{
    VOLTAGE,  // 0
    CURRENT,  // 1
}
lcd_mode_t;

// determines precision 
// NO_PRECISION --> when reading >= 10; no precision (just a pure integer reading)
// ONE_PRECISION --> when reading < 10, reading >= 1; one decimal precision (this'll be most cases)
// TWO_PRECISION --> when reading < 1; ensures when < 1 the reading is showing a value below 1 
typedef enum
{
    NO_PRECISION, 
    ONE_PRECISION,
    TWO_PRECISION
}
lcd_precision_t;

// struct consisting of all information regarding data being received / transmitted for LCD screen
typedef struct 
{
    lcd_mode_t mode;                  // either reading volts or current
    lcd_precision_t precision;        // so we can get the right precision based on the reading
    float reading;                    // from analog to digital converter. reading around 3-6 A, or 3-6 V
    uint8_t digit1;                   // extracted from reading
    uint8_t digit2;                   // extracted from reading
    uint8_t display1;                 // display LED for dig1
    uint8_t display2;                 // display LED for dig2
}
lcd_t;

// handles input / output setup
void setup() 
{
    // setup INA226 & error check
    Wire.begin();
    if(!INA.begin())
    {
        Serial.println("INA226 had an error!");
    }
    // set button input
    pinMode(BUTTON, INPUT);
    // set outputs (driver to 7-Seg display)
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(DP, OUTPUT);
    for(int i = 0; i < NUM_SEGS; i++)
    {
        pinMode(seg7[i], OUTPUT);
    }
    // ensure both output LEDs are currently off
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW); 
    // infinite loop which repeatedly updates reading & updates displays accordingly
    while(1)
    {
        // initialize struct
        lcd_t lcd = {.mode = VOLTAGE, .precision = NO_PRECISION, 
        .reading = 0, .digit1 = 0, .digit2 = 0, .display1 = 0, .display2 = 0};
        // update reading & struct accordingly
        update_mode(&lcd);
        extract_reading(&lcd);
        get_digits_from_reading(&lcd);
        lcd.display1 = get_display_led_from_digit(lcd.digit1);
        lcd.display2 = get_display_led_from_digit(lcd.digit2);
        // display1 active high
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, LOW);
        update_display(lcd.display1);
        update_dp(lcd.precision, HIGH);
        // display2 active high, display1 off 
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, HIGH);
        update_dp(lcd.precision, LOW);
        update_display(lcd.display2);
    }
}

void loop()
{

}

///////////////////
//   FUNCTIONS   //
///////////////////

// updates mode based on the the signal being transmitted by the button
void update_mode(lcd_t *lcd)
{
    // if button is read as a non-0 signal then in CURRENT mode; otherwise in VOLTAGE source (aka button is unpressed)
    lcd->mode = digitalRead(BUTTON) ? CURRENT : VOLTAGE;
}

// INA266 Device Address -> 0x40
// Bus Voltage Register -> 02h
// Shunt Register -> 01h
// Config Register 00h
void extract_reading(lcd_t *lcd)
{
    // reading extraction
    if(lcd->mode == CURRENT)  
    {
        float shuntVoltage = INA.getShuntVoltage();
        float current = shuntVoltage / R;
        lcd->reading = current;
    }
    else                      
    {
        float voltage = INA.getBusVoltage();
        lcd->reading = voltage;
    }
    // determine precision based on reading
    if(lcd->reading >= 10.0)
    {
        lcd->precision = NO_PRECISION;
    }
    else if(lcd->reading >= 1.0)
    {
        lcd->precision = ONE_PRECISION;
    }
    else
    {
        lcd->precision = TWO_PRECISION;
    }
}  

// takes the reading and gets its digits
// set fields in the struct
// assume reading is between 0-99
void get_digits_from_reading(lcd_t *lcd)
{
    // store the reading in a local variable so we can manipulate without affecting struct
    float reading = lcd->reading;
    int int_reading;  // needs to be declared before using in switch
    // reading is extracted differently based on precision
    switch(lcd->precision)
    {
        case NO_PRECISION:                      // just extract directly
          int_reading = reading;
          lcd->digit1 = int_reading % 10;
          lcd->digit2 = int_reading / 10;
          break;
        case ONE_PRECISION:                     // multiply by 10 so we can get the fractional portion 
          int_reading = (int) (reading * 10);
          lcd->digit1 = int_reading % 10;
          lcd->digit2 = int_reading / 10;
          break;
        case TWO_PRECISION:                     // also multiply by 10
          int_reading = (int) (reading * 10);
          lcd->digit1 = 0;
          lcd->digit2 = int_reading;
          break;
    }
}

// simple switch statement
// called on individual digits
// returns the bit combination required to display the specified digit onto the 7-segment display
uint8_t get_display_led_from_digit(uint8_t digit)
{
    // switch to handle possible digits
    switch(digit)
    {
        case 0:
          return ~0b1111110;
        case 1: 
          return ~0b0110000;
        case 2: 
          return ~0b1101101;
        case 3: 
          return ~0b1111001;
        case 4: 
          return ~0b0110011;
        case 5: 
          return ~0b1011011;
        case 6: 
          return ~0b1110000;
        case 7: 
          return ~0b1110000;
        case 8: 
          return ~0b1111111;
        case 9: 
          return ~0b1111011;
    }
    // in case digit is some invalid value
    return 0b0000000;
}

void update_dp(lcd_precision_t precision, int signal)
{
    // only show DP on LED1 if there is precision (<10 reading) & LED1 is on
    if(precision != NO_PRECISION)
    {
        digitalWrite(DP, signal);
    }
    else  // definitely LOW if precision not considered
    {
        digitalWrite(DP, LOW);
    }
}

// updates display outputs via bit shifting on the passed in display 
// called on a singular display
void update_display(uint8_t display)
{
    for(int i = 0; i < NUM_SEGS; i++)
    {
        digitalWrite(seg7[i], 1 & (display >> i));
    }
}      
