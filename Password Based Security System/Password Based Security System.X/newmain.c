
#include <xc.h>
#include <string.h>

#define _XTAL_FREQ 20000000

// CONFIGURATION BITS
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

// LCD CONNECTIONS
#define RS RD0
#define EN RD1

// OUTPUTS
#define BUZZER RC0
#define RED_LED RC1
#define SERVO RC2
#define GREEN_LED RC3

// KEYPAD CONNECTIONS
// Rows: RB0-RB3
// Columns: RB4-RB7

char password[] = "1608";
char entered_password[5];
unsigned char index = 0;
unsigned char wrong_attempts = 0;

// LCD FUNCTIONS
void lcd_command(unsigned char cmd)
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

void lcd_init()
{
    __delay_ms(20);

    RS = 0;
    PORTD = (PORTD & 0x0F) | 0x30;
    EN = 1;
    __delay_ms(5);
    EN = 0;

    PORTD = (PORTD & 0x0F) | 0x30;
    EN = 1;
    __delay_ms(1);
    EN = 0;

    PORTD = (PORTD & 0x0F) | 0x30;
    EN = 1;
    __delay_ms(1);
    EN = 0;

    PORTD = (PORTD & 0x0F) | 0x20;
    EN = 1;
    __delay_ms(1);
    EN = 0;

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
}

void lcd_string(const char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}

void lcd_clear()
{
    lcd_command(0x01);
    __delay_ms(2);
}

// KEYPAD FUNCTION
char keypad_scan()
{
    const char keys[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    unsigned char row, col;

    for(row = 0; row < 4; row++)
    {
        PORTB = 0xFF;

        PORTB &= ~(1 << row);

        __delay_ms(2);

        for(col = 0; col < 4; col++)
        {
            if(!(PORTB & (1 << (col + 4))))
            {
                __delay_ms(30);

                if(!(PORTB & (1 << (col + 4))))
                {
                    while(!(PORTB & (1 << (col + 4))));

                    return keys[row][col];
                }
            }
        }
    }

    return 0;
}

// SERVO FUNCTIONS
void servo_unlock()
{
    unsigned char i;

    for(i = 0; i < 50; i++)
    {
        SERVO = 1;
        __delay_us(1000);
        SERVO = 0;
        __delay_us(19000);
    }
}

void servo_lock()
{
    unsigned char i;

    for(i = 0; i < 50; i++)
    {
        SERVO = 1;
        __delay_us(2000);
        SERVO = 0;
        __delay_us(18000);
    }
}

// MAIN PROGRAM
void main()
{
    char key;

    ADCON1 = 0x06;

    TRISD = 0x00;
    TRISB = 0xF0;
    TRISC = 0x00;

    PORTB = 0xFF;
    PORTC = 0x00;
    PORTD = 0x00;

    OPTION_REGbits.nRBPU = 0;

    lcd_init();

    servo_lock();

    RED_LED = 1;
    GREEN_LED = 0;
    BUZZER = 0;

    lcd_string("SECURITY SYSTEM");
    lcd_command(0xC0);
    lcd_string("INITIALIZING");
    __delay_ms(2000);

    while(1)
    {
        lcd_clear();
        lcd_string("ENTER PASSWORD:");
        lcd_command(0xC0);

        index = 0;
        memset(entered_password, 0, sizeof(entered_password));

        while(1)
        {
            key = keypad_scan();

            if(key == 0)
                continue;

            if(key == '*')
            {
                index = 0;
                memset(entered_password, 0, sizeof(entered_password));
                lcd_command(0xC0);
                lcd_string("                ");
                lcd_command(0xC0);
            }
            else if(key == '#')
            {
                break;
            }
            else if(key >= '0' && key <= '9' && index < 4)
            {
                entered_password[index++] = key;
                lcd_data('*');
            }
        }

        entered_password[index] = '\0';

        if(strcmp(entered_password, password) == 0)
        {
            lcd_clear();
            lcd_string("ACCESS GRANTED");

            GREEN_LED = 1;
            RED_LED = 0;
            BUZZER = 0;

            servo_unlock();

            __delay_ms(5000);

            servo_lock();

            GREEN_LED = 0;
            RED_LED = 1;

            wrong_attempts = 0;
        }
        else
        {
            wrong_attempts++;

            lcd_clear();
            lcd_string("WRONG PASSWORD");

            RED_LED = 1;
            GREEN_LED = 0;

            BUZZER = 1;
            __delay_ms(1000);
            BUZZER = 0;

            __delay_ms(1500);

            if(wrong_attempts >= 3)
            {
                lcd_clear();
                lcd_string("SYSTEM LOCKED");

                while(1)
                {
                    BUZZER = 1;
                    RED_LED = 1;
                    __delay_ms(300);
                    BUZZER = 0;
                    RED_LED = 0;
                    __delay_ms(300);
                }
            }
        }
    }
}
