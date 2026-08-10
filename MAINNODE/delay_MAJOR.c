// Delay functions used throughout the project.
#include "DELAY_MAJOR.h"
#include "major_type.h"

/****************************************
        MILLISECOND DELAY
****************************************/

// Generates an approximate delay in milliseconds.
// dly = number of milliseconds to wait.
void delay_MS(unsigned int dly)
{
    // Loop counter used to create the required delay.
    unsigned int i;
  // Repeat the delay loop until the requested
    // number of milliseconds has been completed.
    for(; dly > 0; dly--)
    {
        // Software loop that consumes processor time.
        // The exact delay depends on the CPU clock speed
        // and compiler-generated instructions.
        for(i = 0; i < 1200; i++);
    }
}

/****************************************
        MICROSECOND DELAY
****************************************/

// Generates an approximate delay in microseconds.
// dlyus = number of microseconds to wait.
void delay_us(u32 dlyus)
{
    // Multiply the requested delay by 12 to create
    // the required number of software-loop iterations.
    // Example: delay_us(10) -> approximately 120 loop iterations.
    // The actual timing depends on the processor clock
    // and compiler optimization.
    for(dlyus *= 12; dlyus > 0; dlyus--);
}

/****************************************
        MILLISECOND DELAY
****************************************/

// Generates an approximate delay in milliseconds.
// dlyms = number of milliseconds to wait.
void delay_ms(u32 dlyms)
{
    // Multiply the requested milliseconds by 12000
    // to create the required software-loop iterations.
    // Example: delay_ms(10) -> approximately 120000 loop iterations.
    for(dlyms *= 12000; dlyms > 0; dlyms--);
}
/****************************************
        SECOND DELAY
****************************************/

// Generates an approximate delay in seconds.
// dlys = number of seconds to wait.
void delay_s(u32 dlys)
{
    // Multiply the requested seconds by 12000000
    // to create the required software-loop iterations.
    // Example: delay_s(1) -> approximately 12000000 loop iterations.
    for(dlys *= 12000000; dlys > 0; dlys--);
}
