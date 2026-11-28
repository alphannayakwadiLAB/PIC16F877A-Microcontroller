
#include <xc.h>

// CONFIGURATION BITS
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000

#define BUTTON RB0

// 7 SEGMENT DISPLAY
// Common Cathode
// RD0=A, RD1=B, RD2=C
// RD3=D, RD4=E, RD5=F
// RD6=G, RD7=DP

const unsigned char dice[7] =
{
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D   // 6
};

// DISPLAY FUNCTION
void display_number(unsigned char num)
{
    if(num <= 6)
    {
        PORTD = dice[num];
    }
}

// MAIN FUNCTION
void main()
{
    unsigned char count = 0;
    unsigned char result = 0;

    // DIGITAL I/O CONFIGURATION
    ADCON1 = 0x06;

    TRISB = 0x01;  // RB0 input
    TRISD = 0x00;  // PORTD output

    PORTB = 0x01;
    PORTD = 0x00;

    // INITIAL DISPLAY
    display_number(0);

    while(1)
    {
        // Continuously change counter
        count++;

        if(BUTTON == 0)
        {
            __delay_ms(30);

            if(BUTTON == 0)
            {
                // ROLLING DICE DISPLAY
                while(BUTTON == 0)
                {
                    count++;

                    result = (count % 6) + 1;

                    display_number(result);

                    __delay_ms(50);
                }

                // BUTTON RELEASED
                __delay_ms(30);

                // FINAL DICE RESULT
                display_number(result);
            }
        }
    }
}
