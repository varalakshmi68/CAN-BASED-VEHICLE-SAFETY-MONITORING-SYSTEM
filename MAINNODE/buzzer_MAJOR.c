// buzzer_major.c
// Buzzer driver for the vehicle safety system.
// The buzzer is connected to P0.20 and is controlled using active-low logic.

#include <lpc21xx.h>
#include "buzzer_major.h"

/* P0.20 is connected to the buzzer */
#define BUZZER 20

/* Initialize buzzer GPIO pin */
void Buzzer_init(void)
{
    /* Configure P0.20 as an output */
    IODIR0 |= (1 << BUZZER);

    /* Keep buzzer OFF initially
       Buzzer is active LOW, so LOW/CLR is used here according to the original code */
    IOCLR0 |= (1 << BUZZER);
}

/* Turn ON the buzzer */
void Buzzer_ON(void)
{
    /* Clear P0.20 to activate the buzzer */
    IOCLR0 |= (1 << BUZZER);
}

/* Turn OFF the buzzer */
void Buzzer_OFF(void)
{
    /* Set P0.20 HIGH to deactivate the buzzer */
    IOSET0 |= (1 << BUZZER);
}
