
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

// ROAD 1
#define R1_RED     RB0
#define R1_YELLOW  RB1
#define R1_GREEN   RB2

// ROAD 2
#define R2_RED     RB3
#define R2_YELLOW  RB4
#define R2_GREEN   RB5

// ROAD 3
#define R3_RED     RB6
#define R3_YELLOW  RB7
#define R3_GREEN   RC0

// ALL RED
void all_red()
{
    PORTB = 0x49;
    R3_GREEN = 0;
}

// ROAD 1 GREEN
void road1_green()
{
    all_red();

    R1_RED = 0;
    R1_GREEN = 1;

    __delay_ms(5000);
}

// ROAD 1 YELLOW
void road1_yellow()
{
    all_red();

    R1_RED = 0;
    R1_YELLOW = 1;

    __delay_ms(2000);
}

// ROAD 2 GREEN
void road2_green()
{
    all_red();

    R2_RED = 0;
    R2_GREEN = 1;

    __delay_ms(5000);
}

// ROAD 2 YELLOW
void road2_yellow()
{
    all_red();

    R2_RED = 0;
    R2_YELLOW = 1;

    __delay_ms(2000);
}

// ROAD 3 GREEN
void road3_green()
{
    all_red();

    R3_RED = 0;
    R3_GREEN = 1;

    __delay_ms(5000);
}

// ROAD 3 YELLOW
void road3_yellow()
{
    all_red();

    R3_RED = 0;
    R3_YELLOW = 1;

    __delay_ms(2000);
}

// MAIN PROGRAM
void main()
{
    ADCON1 = 0x06;

    TRISB = 0x00;
    TRISC = 0x00;

    PORTB = 0x00;
    PORTC = 0x00;

    while(1)
    {
        // ROAD 1
        road1_green();
        road1_yellow();

        // ROAD 2
        road2_green();
        road2_yellow();

        // ROAD 3
        road3_green();
        road3_yellow();
    }
}
