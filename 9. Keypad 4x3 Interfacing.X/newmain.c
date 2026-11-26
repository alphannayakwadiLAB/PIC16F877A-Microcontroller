
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

// LCD pins
#define RS RD0
#define EN RD1

// LCD functions
void lcd_pulse(void);
void lcd_nibble(unsigned char);
void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_init(void);
void lcd_string(const char *);
char keypad_scan(void);

// LCD enable pulse
void lcd_pulse(void)
{
    EN = 1;
    __delay_us(5);
    EN = 0;
    __delay_us(100);
}

// Send upper nibble
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
    __delay_us(150);

    lcd_cmd(0x28);  // 4-bit, 2-line
    lcd_cmd(0x0C);  // Display ON
    lcd_cmd(0x06);  // Auto increment
    lcd_cmd(0x01);  // Clear LCD

    __delay_ms(5);
}


void lcd_string(const char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}


char keypad_scan(void)
{
    unsigned char row;

    const char keys[4][3] = {
        {'1','2','3'},
        {'4','5','6'},
        {'7','8','9'},
        {'*','0','#'}
    };

    for(row = 0; row < 4; row++)
    {
        PORTB = 0x0F;

        switch(row)
        {
            case 0: RB0 = 0; break;
            case 1: RB1 = 0; break;
            case 2: RB2 = 0; break;
            case 3: RB3 = 0; break;
        }

        __delay_ms(2);

        if(RB4 == 0)
        {
            __delay_ms(20);
            if(RB4 == 0) return keys[row][0];
        }

        if(RB5 == 0)
        {
            __delay_ms(20);
            if(RB5 == 0) return keys[row][1];
        }

        if(RB6 == 0)
        {
            __delay_ms(20);
            if(RB6 == 0) return keys[row][2];
        }
    }

    return 0;
}

void main(void)
{
    char key;
    char last_key = 0;

    ADCON1 = 0x06;

    TRISD = 0x00;
    TRISB = 0xF0;

    PORTD = 0x00;
    PORTB = 0x0F;

    OPTION_REGbits.nRBPU = 0;

    lcd_init();

    lcd_cmd(0x80);
    lcd_string("4x3 KEYPAD");
    lcd_cmd(0xC0);
    lcd_string("Press a key");

    while(1)
    {
        key = keypad_scan();

        if(key != 0 && key != last_key)
        {
            lcd_cmd(0xC0);
            lcd_string("Key Pressed: ");
            lcd_data(key);
        }

        if(key == 0)
            last_key = 0;
        else
            last_key = key;

        __delay_ms(30);
    }
}
