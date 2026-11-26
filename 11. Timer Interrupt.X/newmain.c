
/*
 * File:   newmain.c
 * Author: DABBA
 */

#include <xc.h>

#define _XTAL_FREQ 20000000


#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

volatile unsigned char timer_count = 0;


void __interrupt() timer_0(void)
{
    if(INTCONbits.TMR0IF == 1)
    {
        TMR0 = 59;             
        timer_count++;

        INTCONbits.TMR0IF = 0;  
    }
}

void main(void)
{

    TRISBbits.TRISB0 = 0;
    PORTBbits.RB0 = 0;


    OPTION_REG = 0x07;  
    TMR0 = 59;


    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    INTCONbits.GIE = 1;

    while(1)
    {
        if(timer_count < 100)
        {
            PORTBbits.RB0 = 1;
        }

        if(timer_count >= 100 && timer_count < 200)
        {
            PORTBbits.RB0 = 0;
        }

        if(timer_count >= 200)
        {
            timer_count = 0;
        }
    }
}
