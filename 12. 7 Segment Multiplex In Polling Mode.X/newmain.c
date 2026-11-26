
#include <xc.h>

#define _XTAL_FREQ 20000000

// Configuration bits
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

// 7 segment codes (Common Cathode)
unsigned char seg_code[10] =
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

unsigned int count = 0;

void display(unsigned int num)
{
    unsigned char tens;
    unsigned char units;

    tens = num / 10;
    units = num % 10;

    // Display tens digit
    PORTBbits.RB0 = 1;
    PORTBbits.RB1 = 0;

    PORTD = seg_code[tens];
    __delay_ms(5);

    // Display units digit
    PORTBbits.RB0 = 0;
    PORTBbits.RB1 = 1;

    PORTD = seg_code[units];
    __delay_ms(5);

    // Turn off both digits
    PORTBbits.RB0 = 0;
    PORTBbits.RB1 = 0;
}

void main(void)
{
    ADCON1 = 0x06;

    TRISD = 0x00;
    TRISB = 0x00;

    PORTD = 0x00;
    PORTB = 0x00;

    while(1)
    {
        for(count = 0; count <= 99; count++)
        {
            unsigned int i;

            // Multiplex display for approximately 1 second
            for(i = 0; i < 100; i++)
            {
                display(count);
            }
        }
    }
}
