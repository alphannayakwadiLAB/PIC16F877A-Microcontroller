/*
 * File:   newmain.c
 * Author: DABBA
 *
 * Created on November 25, 2026, 6:25 PM
 */


#include <xc.h>

#define _XTAL_FREQ 20000000
#define Segment_ON 0
#define Segment_OFF 1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)



#include <xc.h>
unsigned char number = 81;
unsigned char i,j;
unsigned char seg[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x6f};
void main(void)
{
    TRISB = 0x00; //Port B Pins as Output
    TRISC = 0x00; //Port C Pins as Output
    while(1)
    {
        i = number / 10; //First Digit
        j = number % 10; //Second Digit
        PORTCbits.RC0 = Segment_ON; PORTCbits.RC1 = Segment_OFF;
        PORTB = seg[i];
        __delay_ms(10);
         PORTCbits.RC0 = Segment_OFF; PORTCbits.RC1 = Segment_ON;
        PORTB = seg[j];
        __delay_ms(10);
        
      
        }
        
        
 
}
