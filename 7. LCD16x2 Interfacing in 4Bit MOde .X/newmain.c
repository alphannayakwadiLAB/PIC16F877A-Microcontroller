
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

#define RS PORTBbits.RB0
#define RW PORTBbits.RB1
#define EN PORTBbits.RB2

void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init(void);
void lcd_string(const unsigned char *str);

void lcd_cmd(unsigned char cmd)
{
    RS = 0;
    RW = 0;

    PORTB = (PORTB & 0x0F) | (cmd & 0xF0);
    EN = 1;
    __delay_ms(2);
    EN = 0;

    PORTB = (PORTB & 0x0F) | ((cmd << 4) & 0xF0);
    EN = 1;
    __delay_ms(2);
    EN = 0;

    __delay_ms(2);
}

void lcd_data(unsigned char data)
{
    RS = 1;
    RW = 0;

    PORTB = (PORTB & 0x0F) | (data & 0xF0);
    EN = 1;
    __delay_ms(2);
    EN = 0;

    PORTB = (PORTB & 0x0F) | ((data << 4) & 0xF0);
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

void lcd_init(void)
{
    __delay_ms(20);

    lcd_cmd(0x02);  // 4-bit mode
    lcd_cmd(0x28);  // 2 lines, 5x7 matrix
    lcd_cmd(0x0C);  // Display ON, Cursor OFF
    lcd_cmd(0x06);  // Auto increment cursor
    lcd_cmd(0x01);  // Clear display

    __delay_ms(2);
}

void lcd_string(const unsigned char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}

void main(void)
{
    TRISB = 0x00;
    PORTB = 0x00;

    lcd_init();

    while(1)
    {
        lcd_cmd(0x80);
        lcd_string("    ALPHAN");

        lcd_cmd(0xC0);
        lcd_string("   NAYAKWADI");

        __delay_ms(500);
    }
}
