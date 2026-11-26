
/*
 * File: newmain.c
 * Project: LM35 Temperature Measurement
 * Controller: PIC16F877A
 * Compiler: MPLAB X XC8
 */

#include <xc.h>

#define _XTAL_FREQ 20000000


#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define RS RD0
#define EN RD1

unsigned int adc_value;
unsigned int temperature;
unsigned int decimal;


void lcd_pulse(void);
void lcd_nibble(unsigned char);
void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_init(void);
void lcd_string(const char *);
void lcd_number(unsigned int);


void adc_init(void);
unsigned int adc_read(void);


void lcd_pulse(void)
{
    EN = 1;
    __delay_us(5);
    EN = 0;
    __delay_us(100);
}


void lcd_nibble(unsigned char data)
{
    PORTD = (PORTD & 0x0F) | (data & 0xF0);
    lcd_pulse();
}


void lcd_cmd(unsigned char cmd)
{
    RS = 0;

    lcd_nibble(cmd);
    lcd_nibble(cmd << 4);

    if(cmd == 0x01 || cmd == 0x02)
        __delay_ms(2);
}


void lcd_data(unsigned char data)
{
    RS = 1;

    lcd_nibble(data);
    lcd_nibble(data << 4);
}

void lcd_init(void)
{
    __delay_ms(30);

    RS = 0;
    EN = 0;

    lcd_nibble(0x30);
    __delay_ms(5);

    lcd_nibble(0x30);
    __delay_us(150);

    lcd_nibble(0x30);
    __delay_us(150);

    lcd_nibble(0x20);

    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);

    __delay_ms(5);
}


void lcd_string(const char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}


void lcd_number(unsigned int num)
{
    if(num >= 100)
    {
        lcd_data((num / 100) + '0');
    }

    if(num >= 10)
    {
        lcd_data(((num / 10) % 10) + '0');
    }
    else
    {
        lcd_data('0');
    }

    lcd_data((num % 10) + '0');
}


void adc_init(void)
{
    ADCON1 = 0x8E;
    ADCON0 = 0x81;

    __delay_ms(2);
}


unsigned int adc_read(void)
{
    __delay_us(25);

    ADCON0bits.GO = 1;

    while(ADCON0bits.GO == 1);

    return ((ADRESH << 8) | ADRESL);
}


void main(void)
{
    unsigned long temp10;


    TRISD = 0x00;
    TRISAbits.TRISA0 = 1;

    PORTD = 0x00;

  
    adc_init();
    lcd_init();

    lcd_cmd(0x80);
    lcd_string("ADC Interfacing");

    __delay_ms(1500);

    lcd_cmd(0x01);

    while(1)
    {
       
        adc_value = adc_read();

      
        temp10 = ((unsigned long)adc_value * 5000UL) / 1023UL;

        temperature = temp10 / 10;
        decimal = temp10 % 10;

       
        lcd_cmd(0x80);
        lcd_string("Temperature:");

        lcd_cmd(0xC0);
        lcd_string("Temp: ");

        lcd_number(temperature);

        lcd_data('.');
        lcd_data(decimal + '0');

        lcd_string(" C   ");

        __delay_ms(500);
    }
}
