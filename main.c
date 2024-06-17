/*******************************************************************************************************
 *Author        Dustin Matthews and Kelsey Marquez
 *Course        EGR 326: Microcontroller Programming and Applications
 *Assignment    Lab 06: Interfacing a Graphic LCD to the MSP432
 *Date          10/6/22
 *Instructor    Professor Krug
 *Description   Pt3
 *              Interface MSP432 with 128x160 ST7735 LCD :
 *              Implement a 'counter' such that in each corner, a character counts from 0-9, updating
 *              once per second. Delay, then a single character counts 0-9 in the center of the screen,
 *              updating once per second.
 *Notes:        Includes modified Valvano graphics library for interfacing with the ST7735 LCD
 ****************************************************************************************************/
#include "msp.h"
#include <ST7735.h>
#include "images.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void clock_pin_check();
void set_clk48MHz(void);                             //Initialize the clock
void start_screen();
void menu_screen();
void countup();
/**********PIN MAP************
 * * * * * * * * * * * * * * *
 ******** ST7735 LCD  ********
 *
 * CLK                   >P9.5
 * SDA                   >P9.7
 * RS (A0)               >P9.2
 * RST                   >P9.3
 * CS                    >P9.4
******************************/

/*-------------------------------------------------------------------------------//
 * Function     main()
 *-------------------------------------------------------------------------------*/
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    set_clk48MHz();                             // Initialize the MCLK to 48MHz
                                                   // and SMCLK to 12MHz
    ST7735_InitR(INITR_REDTAB);
    //start_screen();
    //menu_screen();
    ST7735_SetRotation(1);                          // rotate display 90*
    ST7735_FillScreen(ST7735_WHITE);
    ST7735_DrawBitmap(100, 100, cat, 30, 30);        // set image on screen
    while (1){
        //countup();
    }
}


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
       CS->CTL2 |= CS_CTL2_HFXT_EN |                // enable HFXT oscillator
                CS_CTL2_HFXTFREQ_6 |                // set freq to 40+MHz
                CS_CTL2_HFXTDRIVE;
          while(CS->IFG & CS_IFG_HFXTIFG)
                    CS->CLRIFG |= CS_CLRIFG_CLR_HFXTIFG;

    /* Select MCLK & HSMCLK = HFXT, no divider */
      CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK     |
                              CS_CTL1_DIVM_MASK     |
                              CS_CTL1_SELS_MASK     |
                              CS_CTL1_DIVHS_MASK)   |   // clr dividers
                              CS_CTL1_SELM__HFXTCLK |   // MCLK sourced from HFXTCLK
                              CS_CTL1_SELS__HFXTCLK;    // SMCLK & HSMCLK sourced from HFXTCLK

      CS->CTL1 = CS->CTL1 |CS_CTL1_DIVS_2;    // change the SMCLK clock speed to 12 MHz.

      CS->KEY = 0;                            // Lock CS module from unintended accesses
}

/*-------------------------------------------------------------------------------//
 * Function:        start_screen()
 * Description:     Set image on screen
 *                  * when setting rotation, must also set parameters of draw
 *                  function to match the 'new' dimensions
 *
 *------------------------------------------------------------------------------*/
void start_screen()
{
    char start_txt[2][17]= {"EMBEDDED", "SYSTEMS"};
    ST7735_SetRotation(1);                              // rotate display 90*
    ST7735_DrawBitmap(0, 128, triangle, 160, 128);        // set image on screen
    ST7735_DrawString(6, 4, start_txt[0], ST7735_BLACK);// print 1st string
    ST7735_DrawString(6, 6, start_txt[1], ST7735_BLACK);// print 2nd string, set
                                                        // with 1 row space btwn 1st
    Delay1ms(3000);                                     // delay 3000ms
}

/*-------------------------------------------------------------------------------//
 * Function:        menu_screen()
 * Description:     blackout screen and then display menu options
 *
 *------------------------------------------------------------------------------*/
void menu_screen()
{
    char menu_txt[4][20] = {"1.SET TIME", "2.SET BEAT", "3.SET SCALE"};
    ST7735_FillScreen(ST7735_BLACK);                    // set to black
    Delay1ms(500);
    ST7735_FillScreen(ST7735_WHITE);                    // set to white
    ST7735_SetRotation(1);                              // rotate display 90*
    ST7735_DrawString(0, 3, menu_txt[0], ST7735_BLACK); // print 1st string
    ST7735_DrawString(0, 6, menu_txt[1], ST7735_BLACK); // print 2nd string, set
                                                        // with 1 row space btwn
    ST7735_DrawString(0, 9, menu_txt[2], ST7735_BLACK); // print 3nd string, set
                                                        // with 1 row space btwn
    Delay1ms(1000);
}


void countup()
{
    char crnr_cnt[11] ={'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int i;
    ST7735_FillScreen(ST7735_WHITE);                    // set to white

    for(i = 0; i < 10; i++)
    {
        ST7735_DrawChar(0, 0, crnr_cnt[i], ST7735_BLACK, ST7735_WHITE, 4);
        ST7735_DrawChar(0, 96, crnr_cnt[i], ST7735_BLACK, ST7735_WHITE, 4);
        ST7735_DrawChar(136, 0, crnr_cnt[i], ST7735_BLACK, ST7735_WHITE, 4);
        ST7735_DrawChar(136, 96, crnr_cnt[i], ST7735_BLACK, ST7735_WHITE, 4);

        Delay1ms(1000);
    }
    ST7735_FillScreen(ST7735_WHITE);                    // set to white
    for(i = 0; i < 10; i++)
    {
        ST7735_DrawChar(68, 48, crnr_cnt[i], ST7735_BLACK, ST7735_WHITE, 4);

        Delay1ms(500);
    }

    Delay1ms(3000);

}

/*-------------------------------------------------------------------------------//
 * Function:        clock_pin_check()
 * Description:     Outputs clock to gpio pins- this allows the oscope to be used
 *                  to verify clock frequency
 *
 *------------------------------------------------------------------------------*/
void clock_pin_check()
{
    //Output MCLK and SMCLK to port pin to demonstrate 48MHz operation
    P4->DIR  |=   (BIT3 | BIT4);
    P4->SEL0 |=   (BIT3 | BIT4);                       //Output MCLK
    P4->SEL1 &= ~ (BIT3 | BIT4);

    P7->DIR  |=   (BIT0);
    P7->SEL0 |=   (BIT0);                              //Output SMCLK
    P7->SEL1 &= ~ (BIT0);
}
