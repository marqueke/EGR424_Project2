/*****************************************************************
 *  LCD.c
 *
 *  Authors       Kelsey Marquez and Jacob Kucinski
 *  Course        EGR 424: Design of Microcontroller Applications
 *  Instructor    Dr. Parikh
 *  Assignment    Project 2: Slot Machine
 *  Date          6/27/24
 *****************************************************************/

#include "LCD.h"
#include "ST7735.h"
#include "images.h"
#include <string.h>

// reference to main.c global variable
extern char row1[4], row2[4], row3[4];

/*-------------------------------------------------------------------------------//
 * Function:        start_screen(void)
 * Description:     Welcome screen to display that prompts the user to begin
 *                  playing the slot machine
 *------------------------------------------------------------------------------*/
void start_screen(void)
{
    char start_txt[5][17]= {"Welcome to", "the slot", "machine!", "Spin to", "play!"};
    ST7735_FillScreen(ST7735_WHITE);
    ST7735_DrawString(1, 2, start_txt[0], ST7735_BLACK);
    ST7735_DrawString(3, 4, start_txt[1], ST7735_BLACK);
    ST7735_DrawString(3, 6, start_txt[2], ST7735_BLACK);
    ST7735_DrawString(3, 10, start_txt[3], ST7735_BLACK);
    ST7735_DrawString(6, 12, start_txt[4], ST7735_BLACK);

    // Clear previous symbols from all rows
    memset(row1, ' ', 3);
    memset(row2, ' ', 3);
    memset(row3, ' ', 3);
}

/*-------------------------------------------------------------------------------//
 * Function:        void display_slots(int num, int row, int col)
 * Description:     Function used by spin_reels() that will display the symbols
 *                  for the slot machine rows on the LCD
 *------------------------------------------------------------------------------*/
void display_slots(int num, int row, int col){
    int x = 0;
    int y = 0;
    int w = 30;
    int h = 30;

    switch(row){
    case 1:  y = 42;
             break;
    case 2:  y = 95;
             break;
    case 3:  y = 148;
             break;
    default: break;
    }

    switch(col){
    case 0:  x = 7;
             break;
    case 1:  x = 49;
             break;
    case 2:  x = 91;
             break;
    default: break;
    }

    switch(num){
    case 48: ST7735_DrawBitmap(x, y, sun, w, h);
            break;
    case 49: ST7735_DrawBitmap(x, y, star, w, h);
            break;
    case 50: ST7735_DrawBitmap(x, y, cat, w, h);
            break;
    case 51: ST7735_DrawBitmap(x, y, heart, w, h);
            break;
    case 52: ST7735_DrawBitmap(x, y, ladybug, w, h);
            break;
    case 53: ST7735_DrawBitmap(x, y, fish, w, h);
            break;
    case 54: ST7735_DrawBitmap(x, y, clover, w, h);
            break;
    case 55: ST7735_DrawBitmap(x, y, pot_of_gold, w, h);
            break;
    case 56: ST7735_DrawBitmap(x, y, money_sign, w, h);
            break;
    default: break;
    }
}
