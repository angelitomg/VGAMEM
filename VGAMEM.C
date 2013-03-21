
/*
 *
 * VGA Memorize - vgamem.c
 *
 * A simple memory game for DOS
 *
 * Angelito M. Goulart
 *
 * www.angelitomg.com
 *
 * 09/01/2013
 *
 * Must be compiled in DJGPP
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dos.h>
#include <sys/nearptr.h>

// Config constants
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define VIDEO_INIT 0x10
#define SET_MODE 0x00
#define VGA_256_COLOR_MODE 0x013
#define TEXT_MODE 0x03

// Colors constants
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_RED 12
#define COLOR_YELLOW 14

// Extra configs
#define ARRAY_SIZE 52
#define LEVELS 25
#define REDUCE_DELAY 60000
#define INCREASE_SEQUENCES 2
#define START_SEQUENCES 4
#define START_DELAY 2000000
#define START_LEVEL 1
#define INTERVAL_DELAY 30000

// Type byte
typedef unsigned char byte;

// Pointer to manipulate screen
byte *VGA = (byte *) 0xA0000;

// Function to enter/exit VGA mode
void set_mode(byte mode)
{

        union REGS regs;

        regs.h.ah = SET_MODE;
        regs.h.al = mode;
        int86(VIDEO_INIT, &regs, &regs);

}

//////////////////////////////////////////////////////
// Function to get a internal value of a key in game
// 0 -> Fail
// 1 -> Blue
// 2 -> Green
// 3 -> Red
// 4 -> Yellow
//////////////////////////////////////////////////////
byte key_value(byte key)
{
        if (key == 66 || key == 98)
        {
                return 1;
        }
        else if (key == 71 || key == 103)
        {
                return 2;
        }
        else if (key == 12 || key == 114)
        {
                return 3;
        }
        else if (key == 89 || key == 121)
        {
                return 4;
        }
        else
        {
                return 0;
        }
}

// Function to clear a array with zeros
void clear_array(byte *p, byte size)
{
        byte i;
        for(i = 0; i < size; i++)
        {
                *p = 0;
                p++;
        }       
}

// Function to get vga color code based in internal color code
byte color_code(byte code)
{
        switch(code)
        {
                case 1: return COLOR_BLUE; break;
                case 2: return COLOR_GREEN; break;
                case 3: return COLOR_RED; break;
                case 4: return COLOR_YELLOW; break;
                default: return 0; break;
        }
}

// Function to fill screen with a color
void fill_screen(byte color)
{
        unsigned short x, y, offset;
        for (x = 0; x < SCREEN_WIDTH ;x++)
        {
                for (y = 0; y < SCREEN_HEIGHT; y++)
                {
                        offset = SCREEN_WIDTH * y + x;
                        VGA[offset] = color;
                }
        }
}

// Main function
int main()
{

        byte i, loose=1, key, level, sequences, sequence[ARRAY_SIZE], response[ARRAY_SIZE];
        int delay;

        // Needed to DJGPP
        VGA += __djgpp_conventional_base;        

        // Enter in VGA MODE
        set_mode(VGA_256_COLOR_MODE);
        
        do {

                // If user loses, reset the game
                fill_screen(0);
                if (loose == 1)
                {
                        
                        level = START_LEVEL;
                        sequences = START_SEQUENCES;
                        delay = START_DELAY;
                        loose = 0;
                        
                        printf("\n============= VGA MEMORIZE =============\n\n");
                        printf("         Angelito M. Goulart            \n");
                        printf("         www.angelitomg.com             \n");
                        printf("========================================\n");
                        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
                        printf("Press any key to start game or ESC to exit.");
                        
                        // Verify ESC key
                        if (getch() == 27)
                        {
                                break;
                        }

                }

                // Show level
                fill_screen(0);
                printf("\n                 LEVEL %d                  \n", level);
                sleep(1);

                // Clear sequence array
                clear_array(sequence, ARRAY_SIZE);

                // Seed random generator
                srand(time(NULL));

                // Generate sequences
                for (i = 0; i < sequences; i++)
                {
                        sequence[i] = (rand() % 4) + 1;
                        fill_screen(0);
                        usleep(INTERVAL_DELAY);
                        fill_screen(color_code(sequence[i]));
                        usleep(delay); 
                        fill_screen(0);
                }    
                
                i = 0;

                // Informative message
                printf("Don`t forget: \n");
                printf("B for Blue\n");
                printf("G for Green\n");
                printf("R for Red\n");
                printf("Y for Yellow\n");
                printf("Now, type sequence: ");                

                // Verify user responses
                while(sequence[i] != 0 && (key = getch()))
                {
                        if (key_value(key) != sequence[i])
                        {
                                loose = 1;
                                break;
                        }
                        i++;
                }

                // Verify if user wins game
                if (loose == 0 && level == LEVELS)
                {
                        printf("\n\n     -- CONGRATULATIONS! YOU WIN! --     \n");
                        loose = 1;
                        getch();
                }

                // Verify if all user responses are correct and level up
                if (loose == 0)
                {
                        level++;
                        sequences += INCREASE_SEQUENCES;
                        delay -= REDUCE_DELAY;
                }

        } while (1);
        
        // Returns to text mode
        set_mode(TEXT_MODE);
        return 0;

}
