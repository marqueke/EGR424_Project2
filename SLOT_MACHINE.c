/*****************************************************************
 *  SLOT_MACHINE.c
 *
 *  Authors       Kelsey Marquez and Jacob Kucinski
 *  Course        EGR 424: Design of Microcontroller Applications
 *  Instructor    Dr. Parikh
 *  Assignment    Project 2: Slot Machine
 *  Date          6/27/24
 *****************************************************************/

// Project specific includes
#include "LCD.h"  // For display functions
#include "ST7735.h"
#include "SLOT_MACHINE.h"
#include "TIME.h"

// standard includes
#include <stdlib.h>
#include <stdbool.h>

extern char row1[4], row2[4], row3[4];
extern const char *symbols[];

extern volatile bool spin;
extern volatile bool victory;

/*-------------------------------------------------------------------------------//
 * Function:        spin_reels(void)
 * Description:     Function that handles the movement and generation of the
 *                  slot machine symbols
 *------------------------------------------------------------------------------*/
void spin_reels(void) {
    int i = 0;

    // shift rows upwards. row2->row1, row3->row2
    for(i=0; i < 3; i++) {
        row1[i] = row2[i];
        row2[i] = row3[i];
    }

    //make new third row
    for(i = 0; i < 3; i++) {
        row3[i] = symbols[rand() % 9][0];
    }

    //display slots on LCD
    for(i=0; i < 3; i++) {
        display_slots(row1[i], 1, i);
        display_slots(row2[i], 2, i);
        display_slots(row3[i], 3, i);
    }

    check_victory();    //currently where we check_victory(). this game wins instantly upon 3 matching symbols.
                        //victory does NOT depend upon stopping the spinning reels
}

/*-------------------------------------------------------------------------------//
 * Function:        check_victory(void)
 * Description:     Function to check if the user has won on the slot machine.
 *                  Victory is achieved by matching 3 alike symbols in any row
 *------------------------------------------------------------------------------*/
void check_victory(void) {

    char victory_text[4][17]= {"Victory!", "You WIN!", "Spin", "again!"};
    int i = 0;

    // Check if any row has 3 of the same symbols
    if ((row1[0] == row1[1] && row1[1] == row1[2] && row1[0] != ' ') ||
        (row2[0] == row2[1] && row2[1] == row2[2] && row2[0] != ' ') ||
        (row3[0] == row3[1] && row3[1] == row3[2] && row3[0] != ' ')) {

        victory = true;
        spin = false;

        //display victory message to user
        ST7735_FillScreen(ST7735_WHITE);
        ST7735_DrawString(3, 2, victory_text[0], ST7735_BLACK);
        ST7735_DrawString(3, 4, victory_text[1], ST7735_BLACK);
        ST7735_DrawString(6, 11, victory_text[2], ST7735_BLACK);
        ST7735_DrawString(5, 13, victory_text[3], ST7735_BLACK);
    }

    //include the winning row in the victory message screen
    if (row1[0] == row1[1] && row1[1] == row1[2]){
        for(i=0; i < 3; i++) {
            display_slots(row1[i], 2, i);
        }
    }
    else if(row2[0] == row2[1] && row2[1] == row2[2]){
        for(i=0; i < 3; i++) {
            display_slots(row2[i], 2, i);
        }
    }
    else if(row3[0] == row3[1] && row3[1] == row3[2]){
        for(i=0; i < 3; i++) {
            display_slots(row3[i], 2, i);
        }
    }
}




