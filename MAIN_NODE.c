// main_node.c
// Main Node of the CAN-Based Vehicle Safety & Monitoring System.
// Handles engine temperature monitoring, indicator control,
// CAN communication, vehicle mode display and reverse safety alert.

#include <LPC21xx.h>              
#include "major_type.h"           
#include "major_lcd.h"            
#include "major_lcd_defines.h"   
#include "DELAY_MAJOR.h"          
#include "DS18B20_MAJOR.h"        
#include "can_major.h"            
#include "can_defines_major.h"    
#include "buzzer_major.h"         

// Vehicle operating mode.
// 0 = Forward mode
// 1 = Reverse mode
volatile u8 mode = 0;


// Indicator switch status updated by the interrupt routine.
volatile u8 left_flag = 0;
volatile u8 right_flag = 0;


// Indicates which CAN message needs to be transmitted.
// 0 = No transmission
// 1 = Indicator command
// 2 = Vehicle mode
volatile u8 tx_flag = 0;


// Stores reverse-alert information received from the Reverse Node.
volatile u8 reverse_alert = 0;


// Current indicator command.
// 'L' = Left indicator
// 'R' = Right indicator
// 'S' = Indicators OFF
volatile u8 indicator_cmd = 'S';


// Used to create the blinking effect on the LCD.
u8 blink = 0;


// Temperature value read from the DS18B20 sensor.
f32 temp;


// Integer part of the temperature used for LCD display.
s32 tp;


// Fractional temperature digit used to display .0 or .5.
u8 tpd;


// Obstacle distance received from the Reverse Node through CAN.
u32 dist = 0;


// Custom LCD character patterns for the indicator symbols.
// First 8 bytes  = first indicator symbol.
// Next 8 bytes   = second indicator symbol.
u8 indicator_lut[] =
{
    0x01,
    0x03,
    0x07,
    0x0F,
    0x1F,
    0x0F,
    0x07,
    0x03,

    0x10,
    0x18,
    0x1C,
    0x1E,
    0x1F,
    0x1E,
    0x1C,
    0x18
};


// Enables the external interrupts used by the Main Node.
void interrupt_enable(void);


int main()
{
    // tx = CAN frame used for transmission.
    // rx = CAN frame used for received messages.
    struct CAN_Frame tx, rx;


    // Initialize the LCD.
    InitLCD();


    // Initialize CAN1 for communication with other nodes.
    Init_CAN1();


    // Enable the required external interrupts.
    interrupt_enable();


    // Initialize the buzzer.
    Buzzer_init();


    // Load the indicator symbols into the LCD CGRAM.
    // They can later be displayed using CharLCD(0) and CharLCD(1).
    BuildCGRAM(indicator_lut, 15);


    /******** SYSTEM DISPLAY ********/

    // Display the system startup message on LCD Line 1.
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("Vehicle Safety And");


    // Display the system startup message on LCD Line 2.
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("Monitoring System ");


    // Keep the startup message visible for 2 seconds.
    delay_ms(2000);


    // Clear the LCD after the startup message.
    CmdLCD(0x01);


    // Main continuous monitoring loop.
    while(1)
    {
        /****************************************
                LINE1 : TEMPERATURE
        ****************************************/

        // Move to the beginning of LCD Line 1.
        CmdLCD(0x80);


        // Clear the previous temperature display.
        StrLCD("               ");


        // Return to the beginning of Line 1.
        CmdLCD(0x80);


        // Read engine temperature from the DS18B20 sensor.
        temp = ReadTemp();


        // Check whether the temperature sensor failed.
        if(temp == -1)
        {
            // Display the sensor failure message.
            StrLCD("sensor fail");
        }
        else
        {
            // Extract the integer part of the temperature.
            tp = ((int)temp) >> 4;


            // Determine the fractional part.
            // Display .5 when bit 3 is set; otherwise display .0.
            tpd = (((int)temp) & 0x08) ? '5' : '0';


            // Display the engine temperature label.
            StrLCD("ENG TEMP:");


            // Display the integer temperature value.
            U32LCD(tp);


            // Display the decimal point.
            CharLCD('.');


            // Display the fractional digit.
            CharLCD(tpd);


            // Display the degree symbol.
            CharLCD(223);


            // Display Celsius.
            CharLCD('C');


            // Add spacing after the temperature.
            StrLCD(" ");
        }


        /****************************************
            LINE2 : INDICATOR STATUS
        ****************************************/

        // Toggle blink between 0 and 1 to create
        // the ON/OFF indicator blinking effect.
        blink = !blink;


        // Move to LCD Line 2.
        CmdLCD(0xc0);


        // Clear the previous indicator information.
        StrLCD("             ");


        // Return to the beginning of Line 2.
        CmdLCD(0xc0);


        // Display the left indicator status.
        if(indicator_cmd == 'L')
        {
            // Show the left indicator symbol only
            // during the ON phase of blinking.
            if(blink)
                CharLCD(0);
            else
                CharLCD(' ');


            // Display the second custom indicator symbol.
            CharLCD(1);


            // Display left indicator status.
            StrLCD(" LEFT IND");
        }


        // Display the right indicator status.
        else if(indicator_cmd == 'R')
        {
            // Display the first custom indicator symbol.
            CharLCD(0);
          // Show the right indicator symbol only
            // during the ON phase of blinking.
            if(blink)
                CharLCD(1);
            else
                CharLCD(' ');


            // Display right indicator status.
            StrLCD(" RIGHT IND");
        }


        // No indicator is active.
        else
        {
            // Display both indicator symbols.
            CharLCD(0);
            CharLCD(1);


            // Display indicator OFF status.
            StrLCD(" IND OFF");
        }
      /****************************************
                CAN TRANSMISSION
        ****************************************/

        // tx_flag = 1 means an indicator command
        // needs to be sent to the Indicator Node.
        if(tx_flag == 1)
        {
            // CAN ID 0x101 is used for indicator commands.
            tx.ID = 0x101;


            // RTR = 0 means this is a normal CAN data frame.
            tx.vbf.RTR = 0;


            // DLC = 1 means one data byte is being transmitted.
            tx.vbf.DLC = 1;


            // Put the indicator command into the CAN data field.
            // 'L' = Left, 'R' = Right, 'S' = OFF.
            tx.Data1 = indicator_cmd;


            // Transmit the indicator command through CAN1.
            CAN1_Tx(tx);


            // Clear the transmission request.
            tx_flag = 0;
        }


        // tx_flag = 2 means the vehicle mode
        // needs to be transmitted.
        else if(tx_flag == 2)
        {
            // CAN ID 0x102 is used for vehicle mode information.
            tx.ID = 0x102;


            // Send a normal CAN data frame.
            tx.vbf.RTR = 0;


            // The message contains one data byte.
            tx.vbf.DLC = 1;


            // Send the current vehicle mode.
            // 0 = Forward, 1 = Reverse.
            tx.Data1 = mode;


            // Transmit the mode information through CAN1.
            CAN1_Tx(tx);


            // Clear the transmission request.
            tx_flag = 0;
        }


        /****************************************
            RECEIVE DISTANCE + ALERT
        ****************************************/

        // Check whether a CAN message is available
        // in the CAN1 receive buffer.
        if(C1GSR & RBS_BIT_READ)
        {
            // Read the received CAN frame.
            CAN1_Rx(&rx);


            // CAN ID 0x201 is used for Reverse Node information.
            if(rx.ID == 0x201)
            {
                // Data1 contains the measured obstacle distance.
                dist = rx.Data1;


                // Data2 contains the reverse-alert information.
                reverse_alert = rx.Data2;
            }
        }


        /****************************************
                LINE3 : MODE DISPLAY
        ****************************************/

        // Move to the beginning of LCD Line 3.
        CmdLCD(0x94);


        // Display the current vehicle operating mode.
        if(mode)
        {
            // mode = 1 means Reverse mode.
            StrLCD("MODE: REVERSE ");
        }
        else
        {
            // mode = 0 means Forward mode.
            StrLCD("MODE: FORWARD ");
        }


        /****************************************
            LINE4 : STATUS / DISTANCE
        ****************************************/

        // Move to the beginning of LCD Line 4.
        CmdLCD(0xd4);


        // Display reverse-distance information
        // only when the vehicle is in Reverse mode.
        if(mode)
        {
            // Display the distance label.
            StrLCD("DIST:");


            // Display the obstacle distance received from
            // the Reverse Node.
            U32LCD(dist);


            // Display the distance unit.
            StrLCD("cm ");


            // Generate a safety alert when an obstacle
            // is detected closer than 20 cm.
            if(dist < 20)
            {
                // Display the warning message.
                StrLCD("ALERT   ");


                // Activate the buzzer to warn the driver.
                Buzzer_ON();
            }
            else
            {
                // Display the safe condition.
                StrLCD("SAFE   ");


                // Keep the buzzer OFF.
                Buzzer_OFF();
            }
        }
        else
        {
            // Display normal forward-driving status.
            StrLCD("FORWARD DRIVE MODE");


            // Reverse warning buzzer is not required in Forward mode.
            Buzzer_OFF();
        }


        // Wait 300 ms before starting the next monitoring cycle.
        delay_ms(300);
    }
}
