
#include <xc.h>

#define _XTAL_FREQ 20000000


#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF


#define RS RD0
#define EN RD1


void EEPROM_Write(unsigned char address, unsigned char data)
{
    EEADR = address;
    EEDATA = data;

  
    EECON1bits.EEPGD = 0;

  
    EECON1bits.WREN = 1;

    
    INTCONbits.GIE = 0;

    
    EECON2 = 0x55;
    EECON2 = 0xAA;

 
    EECON1bits.WR = 1;

    
    while(EECON1bits.WR);

   
    EECON1bits.WREN = 0;

   
    INTCONbits.GIE = 1;
}


unsigned char EEPROM_Read(unsigned char address)
{
    EEADR = address;

   
    EECON1bits.EEPGD = 0;


    EECON1bits.RD = 1;

    return EEDATA;
}


void lcd_cmd(unsigned char cmd)
{
    RS = 0;

    PORTD = (PORTD & 0x0F) | (cmd & 0xF0);

    EN = 1;
    __delay_ms(2);
    EN = 0;

    PORTD = (PORTD & 0x0F) | ((cmd << 4) & 0xF0);

    EN = 1;
    __delay_ms(2);
    EN = 0;
}


void lcd_data(unsigned char data)
{
    RS = 1;

    PORTD = (PORTD & 0x0F) | (data & 0xF0);

    EN = 1;
    __delay_ms(2);
    EN = 0;

    PORTD = (PORTD & 0x0F) | ((data << 4) & 0xF0);

    EN = 1;
    __delay_ms(2);
    EN = 0;
}


void lcd_string(const char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}


void lcd_init()
{
    __delay_ms(20);

    lcd_cmd(0x02);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);

    __delay_ms(5);
}

void lcd_number(unsigned char num)
{
    lcd_data((num / 100) + '0');
    lcd_data(((num / 10) % 10) + '0');
    lcd_data((num % 10) + '0');
}


void main()
{
    unsigned char value;
    unsigned char read_value;

 
    ADCON1 = 0x06;

    TRISD = 0x00;
    PORTD = 0x00;


    lcd_init();

 
    value = 81;


    lcd_cmd(0x80);
    lcd_string("EEPROM WRITE");

    EEPROM_Write(0x00, value);

    __delay_ms(1000);


    lcd_cmd(0x01);
    lcd_string("EEPROM READ");

    read_value = EEPROM_Read(0x00);

    __delay_ms(1000);

  
    lcd_cmd(0x01);
    lcd_string("Stored Value:");

    lcd_cmd(0xC0);
    lcd_number(read_value);

    while(1)
    {
        
    }
}
