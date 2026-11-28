
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

// COMMON CATHODE 7-SEGMENT
// RD0=A, RD1=B, RD2=C
// RD3=D, RD4=E, RD5=F
// RD6=G, RD7=DP

const unsigned char segment[10] =
{
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F   // 9
};

// DISPLAY FUNCTION
void display_number(unsigned char num)
{
    PORTD = segment[num];
}

// MAIN FUNCTION
void main()
{
    unsigned char count = 0;

    // DIGITAL CONFIGURATION
    ADCON1 = 0x06;

    // PORT CONFIGURATION
    TRISB = 0x01;  // RB0 INPUT
    TRISD = 0x00;  // PORTD OUTPUT

    PORTB = 0x01;
    PORTD = 0x00;

    // INITIAL DISPLAY
    display_number(count);

    while(1)
    {
        // CHECK PUSH BUTTON
        if(BUTTON == 0)
        {
            __delay_ms(50);

            // BUTTON DEBOUNCING
            if(BUTTON == 0)
            {
                count++;

                // RESET AFTER 9
                if(count > 9)
                {
                    count = 0;
                }

                // DISPLAY COUNT
                display_number(count);

                // WAIT FOR BUTTON RELEASE
                while(BUTTON == 0)
                {
                    // Wait
                }

                __delay_ms(50);
            }
        }
    }
}
