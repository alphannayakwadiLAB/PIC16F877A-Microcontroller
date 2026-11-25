
/*
 * File:   newmain.c
 * Author: DABBA
 * PIC16F877A - 16x2 LCD Interfacing
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

#define RS PORTDbits.RD0
#define RW PORTDbits.RD1
#define EN PORTDbits.RD2

void lcd_data(unsigned char data)
{
    PORTC = data;
    RS = 1;
    RW = 0;
    EN = 1;
    __delay_ms(2);
    EN = 0;
    __delay_ms(2);
}


void lcd_command(unsigned char cmd)
{
    PORTC = cmd;
    RS = 0;
    RW = 0;
    EN = 1;
    __delay_ms(2);
    EN = 0;
    __delay_ms(2);
}

void lcd_string(const unsigned char *str, unsigned char num)
{
    unsigned char i;

    for(i = 0; i < num; i++)
    {
        lcd_data(str[i]);
    }
}


void lcd_initialize(void)
{
    __delay_ms(20);

    lcd_command(0x38);  
    lcd_command(0x06); 
    lcd_command(0x0C);  
    lcd_command(0x01);  

    __delay_ms(2);
}

// Main Function
void main(void)
{
    TRISC = 0x00;   
    TRISD = 0x00;   

    PORTC = 0x00;
    PORTD = 0x00;

    lcd_initialize();

    while(1)
    {
        lcd_command(0x83);
        lcd_string(" ALPHAN", 7);

        lcd_command(0xC3);
        lcd_string("NAYAKWADI", 9);
    }
}
