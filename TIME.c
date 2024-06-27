/*****************************************************************
 *  TIME.c
 *
 *  Authors       Kelsey Marquez and Jacob Kucinski
 *  Course        EGR 424: Design of Microcontroller Applications
 *  Instructor    Dr. Parikh
 *  Assignment    Project 2: Slot Machine
 *  Date          6/27/24
 *****************************************************************/

#include "GPIO.h"
#include "msp.h"
#include "TIME.h"

/*-------------------------------------------------------------------------------//
 * Function:        set_clk48MHz(void)
 * Description:     *This function was posted by Dr. Kandalaft
 *                  This function sets up the MCLK for 48 MHz and the SMCLK for
 *                  12 MHz.
 *------------------------------------------------------------------------------*/
void set_clk48MHz(void)
{
    // Configure Flash wait-state to 1 for both banks 0 & 1
    // *Require 1 wait state for flash access when clk speed > 24MHz
       FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK0_RDCTL_WAIT_1;
       FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK1_RDCTL_WAIT_1;

    //Configure HFXT to use 48MHz crystal, source to MCLK & HSMCLK*
       PJ->SEL0 |= BIT2 | BIT3;                     // Configure PJ.2/3 for HFXT function
       PJ->SEL1 &= ~(BIT2 | BIT3);
       CS->KEY = CS_KEY_VAL ;                       // Unlock CS module for register access
       CS->CTL2 |= CS_CTL2_HFXT_EN |                // Enable HFXT oscillator
                CS_CTL2_HFXTFREQ_6 |                // Set freq to 40+MHz
                CS_CTL2_HFXTDRIVE;
          while(CS->IFG & CS_IFG_HFXTIFG)
                    CS->CLRIFG |= CS_CLRIFG_CLR_HFXTIFG;

    /* Select MCLK & HSMCLK = HFXT, no divider */
      CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK     |
                              CS_CTL1_DIVM_MASK     |
                              CS_CTL1_SELS_MASK     |
                              CS_CTL1_DIVHS_MASK)   |   // Clr dividers
                              CS_CTL1_SELM__HFXTCLK |   // MCLK sourced from HFXTCLK
                              CS_CTL1_SELS__HFXTCLK;    // SMCLK & HSMCLK sourced from HFXTCLK

      CS->CTL1 = CS->CTL1 |CS_CTL1_DIVS_2;    // Change the SMCLK clock speed to 12 MHz.

      CS->KEY = 0;                            // Lock CS module from unintended accesses
}

int debounce(int ms, uint8_t button) {
    if (!(P4->IN & button)) {       // Initial button press detected
        delay_ms(ms);               // Wait for specified milliseconds
        if (!(P4->IN & button)) {   // Check button state again after delay
            while (!(P4->IN & button));     // Wait until the button is released
            delay_ms(ms);           // Additional delay after release for stability
            return 1;               // Return true if button was consistently pressed and then released
        }
    }
    return 0;   // Return false if button state was not stable
}

/*-------------------------------------------------------------------------------//
 * Function:        sys_init()
 * Description:     This function sets up the SysTick timer under the 48 MHz
 *                  clock. The SysTick timer is enabled with interrupts.
 *------------------------------------------------------------------------------*/
void sys_init() {
    SysTick->CTRL = 0;              // Disable SysTick during setup
    SysTick->LOAD = 48000 - 1;      // Maximum count value (not practical for timing purposes)
    SysTick->VAL = 0;               // Clear the current value register
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;   // Enable SysTick, use processor clock
}

/*-------------------------------------------------------------------------------//
 * Function:        delay_ms(uint32_t ms)
 * Description:     This is a delay function that utilizes the SysTick timer.
 *------------------------------------------------------------------------------*/
void delay_ms(uint32_t ms) {
    uint32_t i;

    for (i = 0; i < ms; i++) {
        SysTick->VAL = 0;           // Clear the SysTick value to avoid any effects from previous delays
        while (!(SysTick->CTRL & BIT(16)));  // Wait for the count flag to be set
    }
}


