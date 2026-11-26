
#include <xc.h>

#define _XTAL_FREQ 20000000

// Configuration bits
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

// LCD connections
#define RS RD0
#define EN RD1

unsigned int adc_value;

// Function declarations
void lcd_init(void);
void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_string(const char *);
void lcd_number(unsigned int);
void lcd_pulse(void);
void lcd_nibble(unsigned char);

void adc_init(void);
unsigned int adc_read(void);

// LCD pulse
void lcd_pulse(void)
{
    EN = 1;
    __delay_us(5);
    EN = 0;
    __delay_us(100);
}

// LCD nibble
void lcd_nibble(unsigned char data)
{
    PORTD = (PORTD & 0x0F) | (data & 0xF0);
    lcd_pulse();
}

// LCD command
void lcd_cmd(unsigned char cmd)
{
    RS = 0;

    lcd_nibble(cmd);
    lcd_nibble(cmd << 4);

    if(cmd == 0x01 || cmd == 0x02)
        __delay_ms(2);
}

// LCD data
void lcd_data(unsigned char data)
{
    RS = 1;

    lcd_nibble(data);
    lcd_nibble(data << 4);
}

// LCD initialization
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

// LCD string
void lcd_string(const char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}

// Display number
void lcd_number(unsigned int num)
{
    if(num >= 1000)
        lcd_data((num / 1000) + '0');

    if(num >= 100)
        lcd_data(((num / 100) % 10) + '0');

    if(num >= 10)
        lcd_data(((num / 10) % 10) + '0');
    else
        lcd_data('0');

    lcd_data((num % 10) + '0');
}

// ADC initialization
void adc_init(void)
{
    ADCON1 = 0x8E;
    ADCON0 = 0x81;

    __delay_ms(2);
}

// ADC reading
unsigned int adc_read(void)
{
    __delay_us(25);

    ADCON0bits.GO = 1;

    while(ADCON0bits.GO == 1);

    return ((ADRESH << 8) | ADRESL);
}

// Main program
void main(void)
{
    TRISD = 0x00;
    TRISAbits.TRISA0 = 1;

    PORTD = 0x00;

    adc_init();
    lcd_init();

    lcd_cmd(0x80);
    lcd_string("POT INTERFACING");

    __delay_ms(1500);

    while(1)
    {
        adc_value = adc_read();

        lcd_cmd(0x80);
        lcd_string("POT VALUE:     ");

        lcd_cmd(0xC0);
        lcd_number(adc_value);

        lcd_string("    ");

        __delay_ms(200);
    }
}
