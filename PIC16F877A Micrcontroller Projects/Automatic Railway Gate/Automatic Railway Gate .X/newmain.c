
#include <xc.h>

// CONFIGURATION BITS
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000

// BUTTON INPUTS
#define BUTTON1 RB0
#define BUTTON2 RB1

// OUTPUT PINS
#define BUZZER RC0
#define RED_LED RC1
#define SERVO RC2
#define GREEN_LED RC3

// LCD PINS
#define RS RD0
#define EN RD1

// SERVO POSITIONS
#define GATE_OPEN  1000
#define GATE_CLOSE 2000

// ---------------- LCD FUNCTIONS ----------------

void lcd_enable()
{
    EN = 1;
    __delay_us(2);
    EN = 0;
    __delay_us(50);
}

void lcd_nibble(unsigned char data)
{
    PORTD = (PORTD & 0x0F) | (data & 0xF0);
    lcd_enable();
}

void lcd_command(unsigned char cmd)
{
    RS = 0;

    lcd_nibble(cmd & 0xF0);
    lcd_nibble((cmd << 4) & 0xF0);

    if(cmd == 0x01 || cmd == 0x02)
        __delay_ms(2);
}

void lcd_data(unsigned char data)
{
    RS = 1;

    lcd_nibble(data & 0xF0);
    lcd_nibble((data << 4) & 0xF0);
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
}

void lcd_init()
{
    RS = 0;
    EN = 0;

    __delay_ms(50);

    lcd_nibble(0x30);
    __delay_ms(5);

    lcd_nibble(0x30);
    __delay_ms(5);

    lcd_nibble(0x30);
    __delay_ms(1);

    lcd_nibble(0x20);
    __delay_ms(1);

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);

    __delay_ms(5);
}

// ---------------- SERVO FUNCTION ----------------

// Generate approximately 20ms servo frame
void servo_frame(unsigned int pulse)
{
    unsigned int i;

    SERVO = 1;

    for(i = 0; i < pulse / 100; i++)
    {
        __delay_us(100);
    }

    SERVO = 0;

    for(i = 0; i < (20000 - pulse) / 100; i++)
    {
        __delay_us(100);
    }
}

// Hold servo position
void servo_hold(unsigned int pulse,
                unsigned int frames)
{
    unsigned int i;

    for(i = 0; i < frames; i++)
    {
        servo_frame(pulse);
    }
}

// ---------------- GATE OPEN FUNCTION ----------------

void gate_open()
{
    unsigned int pos;

    lcd_clear();
    lcd_string("GATE OPENING");

    BUZZER = 0;
    RED_LED = 0;
    GREEN_LED = 0;

    // Slowly move servo to OPEN position
    for(pos = GATE_CLOSE; pos >= GATE_OPEN; pos -= 20)
    {
        servo_hold(pos, 2);

        if(pos == GATE_OPEN)
            break;
    }

    servo_hold(GATE_OPEN, 5);

    RED_LED = 0;
    GREEN_LED = 1;
    SERVO = 0;

    lcd_clear();
    lcd_string("GATE IS OPEN");
}

// ---------------- GATE CLOSE FUNCTION ----------------

void gate_close()
{
    unsigned int pos;

    lcd_clear();
    lcd_string("TRAIN DETECTED");

    RED_LED = 1;
    GREEN_LED = 0;

    // Buzzer alert
    BUZZER = 1;
    __delay_ms(1000);
    BUZZER = 0;

    lcd_clear();
    lcd_string("GATE CLOSING");

    // Slowly move servo to CLOSED position
    for(pos = GATE_OPEN; pos <= GATE_CLOSE; pos += 20)
    {
        servo_hold(pos, 2);
    }

    servo_hold(GATE_CLOSE, 5);

    SERVO = 0;

    lcd_clear();
    lcd_string("GATE CLOSED");
}

// ---------------- MAIN PROGRAM ----------------

void main()
{
    // Configure digital I/O
    ADCON1 = 0x06;

    // Port direction
    TRISB = 0x03;
    TRISC = 0x00;
    TRISD = 0x00;

    // Initial output values
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    // Initialize LCD
    lcd_init();

    // Initial status
    BUZZER = 0;
    RED_LED = 0;
    GREEN_LED = 1;
    SERVO = 0;

    lcd_string("RAILWAY GATE");

    lcd_command(0xC0);
    lcd_string("SYSTEM READY");

    __delay_ms(2000);

    // Initially open gate
    gate_open();

    while(1)
    {
        // BUTTON 1 PRESSED
        // Logic HIGH = Train approaching

        if(BUTTON1 == 1)
        {
            __delay_ms(50);

            if(BUTTON1 == 1)
            {
                // Close railway gate
                gate_close();

                lcd_clear();
                lcd_string("TRAIN PASSING");

                // Wait for Button 2
                // Logic HIGH = Train passed

                while(BUTTON2 == 0)
                {
                    servo_frame(GATE_CLOSE);
                }

                __delay_ms(50);

                if(BUTTON2 == 1)
                {
                    lcd_clear();
                    lcd_string("TRAIN PASSED");

                    __delay_ms(1000);

                    // Open railway gate
                    gate_open();

                    // Wait for Button 2 release
                    while(BUTTON2 == 1)
                    {
                        servo_frame(GATE_OPEN);
                    }

                    // Wait for Button 1 release
                    while(BUTTON1 == 1)
                    {
                        servo_frame(GATE_OPEN);
                    }
                }
            }
        }
    }
}
