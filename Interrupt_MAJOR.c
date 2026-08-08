// interrupt_MAJOR.c
// External interrupt handling for the Main Node.
// EINT0 -> Mode switch
// EINT1 -> Left indicator switch
// EINT2 -> Right indicator switch

#include <LPC21xx.h>             
#include "major_type.h"            

extern volatile u8 mode;           // Stores vehicle mode: 0 = Forward, 1 = Reverse.
extern volatile u8 left_flag;      // Stores left-indicator ON/OFF state.
extern volatile u8 right_flag;     // Stores right-indicator ON/OFF state.
extern volatile u8 tx_flag;        // Requests CAN transmission from the main program.
extern volatile u8 indicator_cmd;  // Stores indicator command: 'L', 'R' or 'S'.



// External interrupt service-routine declarations.
// __irq tells the compiler that these functions are interrupt handlers.
void eint0_isr(void)__irq;         // ISR for EINT0: Mode switch.
void eint1_isr(void)__irq;         // ISR for EINT1: Left indicator switch.
void eint2_isr(void)__irq;         // ISR for EINT2: Right indicator switch.



// Configure EINT0, EINT1 and EINT2 external interrupts.
void interrupt_enable(void)
{
    // Clear the PINSEL settings for the pins used by EINT0, EINT1 and EINT2.
    PINSEL0 &= ~(
                    (3<<2)  |
                    (3<<6)  |
                    (3<<14)
                );


    // Configure the required pins as external interrupt inputs.
    // P0.1  -> EINT0
    // P0.3  -> EINT1
    // P0.7  -> EINT2
    PINSEL0 |= (
                    (3<<2)  |
                    (3<<6)  |
                    (3<<14)
                );
  // Clear any pending EINT0, EINT1 and EINT2 interrupt flags.
    EXTINT = 0x07;
  // Configure all three external interrupts as edge-triggered.
    EXTMODE = 0x07;
 // Select falling-edge triggering for EINT0, EINT1 and EINT2.
    EXTPOLAR = 0x00;
  // Configure all interrupts as IRQ interrupts rather than FIQ.
    VICIntSelect = 0x00;
  // Enable EINT0, EINT1 and EINT2 in the Vectored Interrupt Controller.
    // Interrupt numbers:
    // 14 -> EINT0
    // 15 -> EINT1
    // 16 -> EINT2
    VICIntEnable =
                    (1<<14) |
                    (1<<15) |
                    (1<<16);
  // Enable VIC vector slot 0 and assign interrupt source 14 (EINT0).
    VICVectCntl0 = (1<<5) | 14;
  // Store the address of the EINT0 interrupt service routine.
    VICVectAddr0 = (u32)eint0_isr;
 // Enable VIC vector slot 1 and assign interrupt source 15 (EINT1).
    VICVectCntl1 = (1<<5) | 15;
  // Store the address of the EINT1 interrupt service routine.
    VICVectAddr1 = (u32)eint1_isr;
  // Enable VIC vector slot 2 and assign interrupt source 16 (EINT2).
    VICVectCntl2 = (1<<5) | 16;
  // Store the address of the EINT2 interrupt service routine.
    VICVectAddr2 = (u32)eint2_isr;
}

/****************************************
        MODE SWITCH - EINT0
****************************************/

// This ISR executes whenever the Mode switch generates EINT0.
void eint0_isr(void)__irq
{
    // Toggle the vehicle mode.
    // 0 -> 1 : Forward to Reverse
    // 1 -> 0 : Reverse to Forward
    mode = !mode;
  // Request the main program to send the new mode
    // to the other CAN node.
    tx_flag = 2;
  // Clear the EINT0 interrupt flag after servicing the interrupt.
    EXTINT = 1<<0;
 // Signal to the VIC that the current interrupt has been serviced.
    VICVectAddr = 0;
}

/****************************************
        LEFT SWITCH - EINT1
****************************************/

// This ISR executes whenever the Left Indicator switch generates EINT1.
void eint1_isr(void)__irq
{
    // Check whether the left indicator is currently OFF.
    if(left_flag == 0)
    {
        // Turn the left indicator ON.
        left_flag = 1;
      // Make sure the right indicator is turned OFF.
        right_flag = 0;
      // Send the left-indicator command.
        indicator_cmd = 'L';
    }
    else
    {
        // Turn the left indicator OFF.
        left_flag = 0;
      // Send the indicator OFF command.
        indicator_cmd = 'S';
    }
  // Request the main program to transmit the
    // updated indicator command through CAN.
    tx_flag = 1;
  // Clear the EINT1 interrupt flag.
    EXTINT = 1<<1;
 // Signal that the interrupt has been serviced.
    VICVectAddr = 0;
}

/****************************************
        RIGHT SWITCH - EINT2
****************************************/

// This ISR executes whenever the Right Indicator switch generates EINT2.
void eint2_isr(void)__irq
{
    // Check whether the right indicator is currently OFF.
    if(right_flag == 0)
    {
        // Turn the right indicator ON.
        right_flag = 1;
      // Make sure the left indicator is turned OFF.
        left_flag = 0;
      // Send the right-indicator command.
        indicator_cmd = 'R';
    }
    else
    {
        // Turn the right indicator OFF.
        right_flag = 0;
      // Send the indicator OFF command.
        indicator_cmd = 'S';
    }
  // Request the main program to transmit the
    // updated indicator command through CAN.
    tx_flag = 1;
  // Clear the EINT2 interrupt flag.
    EXTINT = 1<<2;
  // Signal that the interrupt has been serviced.
    VICVectAddr = 0;
}
