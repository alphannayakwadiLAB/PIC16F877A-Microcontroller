
#include <xc.h>

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
#define RS RD1
#define EN RD2

// DS1307 I2C ADDRESS
#define RTC_WRITE 0xD0
#define RTC_READ  0xD1

// INITIAL DATE AND TIME
// 27 November 2026, 21:55:00

#define SET_SECONDS  0
#define SET_MINUTES  55
#define SET_HOURS    21
#define SET_WEEKDAY  6
#define SET_DATE     27
#define SET_MONTH    11
#define SET_YEAR     26

// ------------------------------------------------
// LCD FUNCTIONS
// ------------------------------------------------

void LCD_Pulse(void)
{
    EN = 1;
    __delay_us(2);
    EN = 0;
    __delay_us(50);
}

void LCD_Nibble(unsigned char data)
{
    PORTD = (PORTD & 0x0F) | (data & 0xF0);
    LCD_Pulse();
}

void LCD_Command(unsigned char cmd)
{
    RS = 0;

    LCD_Nibble(cmd);
    LCD_Nibble(cmd << 4);

    __delay_ms(2);
}

void LCD_Data(unsigned char data)
{
    RS = 1;

    LCD_Nibble(data);
    LCD_Nibble(data << 4);

    __delay_ms(2);
}

void LCD_String(const char *str)
{
    while(*str)
    {
        LCD_Data(*str++);
    }
}

void LCD_Init(void)
{
    TRISD = 0x00;
    PORTD = 0x00;

    __delay_ms(20);

    RS = 0;
    EN = 0;

    LCD_Nibble(0x30);
    __delay_ms(5);

    LCD_Nibble(0x30);
    __delay_us(150);

    LCD_Nibble(0x30);
    LCD_Nibble(0x20);

    LCD_Command(0x28);
    LCD_Command(0x0C);
    LCD_Command(0x06);
    LCD_Command(0x01);

    __delay_ms(5);
}

// ------------------------------------------------
// I2C FUNCTIONS
// ------------------------------------------------

unsigned char I2C_WaitFlag(void)
{
    unsigned int timeout = 60000;

    while(!PIR1bits.SSPIF)
    {
        if(timeout-- == 0)
            return 0;
    }

    PIR1bits.SSPIF = 0;

    return 1;
}

unsigned char I2C_WaitIdle(void)
{
    unsigned int timeout = 60000;

    while((SSPCON2 & 0x1F) || SSPSTATbits.R_W)
    {
        if(timeout-- == 0)
            return 0;
    }

    return 1;
}

void I2C_Init(void)
{
    TRISC3 = 1;
    TRISC4 = 1;

    SSPSTAT = 0x80;
    SSPCON = 0x28;
    SSPCON2 = 0x00;

    SSPADD = 49;       // 100 kHz at 20 MHz

    PIR1bits.SSPIF = 0;
}

unsigned char I2C_Start(void)
{
    if(!I2C_WaitIdle())
        return 0;

    PIR1bits.SSPIF = 0;
    SSPCON2bits.SEN = 1;

    return I2C_WaitFlag();
}

unsigned char I2C_Restart(void)
{
    if(!I2C_WaitIdle())
        return 0;

    PIR1bits.SSPIF = 0;
    SSPCON2bits.RSEN = 1;

    return I2C_WaitFlag();
}

unsigned char I2C_Stop(void)
{
    if(!I2C_WaitIdle())
        return 0;

    PIR1bits.SSPIF = 0;
    SSPCON2bits.PEN = 1;

    return I2C_WaitFlag();
}

unsigned char I2C_Write(unsigned char data)
{
    if(!I2C_WaitIdle())
        return 0;

    PIR1bits.SSPIF = 0;

    SSPBUF = data;

    if(!I2C_WaitFlag())
        return 0;

    if(SSPCON2bits.ACKSTAT)
        return 0;

    return 1;
}

unsigned char I2C_Read(unsigned char send_ack,
                       unsigned char *data)
{
    if(!I2C_WaitIdle())
        return 0;

    PIR1bits.SSPIF = 0;

    SSPCON2bits.RCEN = 1;

    if(!I2C_WaitFlag())
        return 0;

    *data = SSPBUF;

    if(!I2C_WaitIdle())
        return 0;

    PIR1bits.SSPIF = 0;

    // ACK = 0, NACK = 1
    SSPCON2bits.ACKDT = send_ack ? 0 : 1;
    SSPCON2bits.ACKEN = 1;

    return I2C_WaitFlag();
}

// ------------------------------------------------
// RTC FUNCTIONS
// ------------------------------------------------

unsigned char DEC_TO_BCD(unsigned char value)
{
    return ((value / 10) << 4) | (value % 10);
}

unsigned char BCD_TO_DEC(unsigned char value)
{
    return ((value >> 4) * 10) + (value & 0x0F);
}

unsigned char RTC_SetTime(void)
{
    unsigned char values[7];
    unsigned char i;

    values[0] = DEC_TO_BCD(SET_SECONDS);
    values[1] = DEC_TO_BCD(SET_MINUTES);
    values[2] = DEC_TO_BCD(SET_HOURS);
    values[3] = DEC_TO_BCD(SET_WEEKDAY);
    values[4] = DEC_TO_BCD(SET_DATE);
    values[5] = DEC_TO_BCD(SET_MONTH);
    values[6] = DEC_TO_BCD(SET_YEAR);

    if(!I2C_Start())
        return 0;

    if(!I2C_Write(RTC_WRITE))
        goto error;

    if(!I2C_Write(0x00))
        goto error;

    for(i = 0; i < 7; i++)
    {
        if(!I2C_Write(values[i]))
            goto error;
    }

    return I2C_Stop();

error:
    I2C_Stop();
    return 0;
}

unsigned char RTC_ReadTime(unsigned char *data)
{
    unsigned char i;

    if(!I2C_Start())
        return 0;

    if(!I2C_Write(RTC_WRITE))
        goto error;

    if(!I2C_Write(0x00))
        goto error;

    if(!I2C_Restart())
        goto error;

    if(!I2C_Write(RTC_READ))
        goto error;

    for(i = 0; i < 7; i++)
    {
        if(!I2C_Read(i < 6, &data[i]))
            goto error;
    }

    return I2C_Stop();

error:
    I2C_Stop();
    return 0;
}

// ------------------------------------------------
// RTC DATA VALIDATION
// ------------------------------------------------

unsigned char ValidBCD(unsigned char value)
{
    if((value & 0x0F) > 9)
        return 0;

    if(((value >> 4) & 0x0F) > 9)
        return 0;

    return 1;
}

unsigned char RTC_Valid(unsigned char *d)
{
    unsigned char sec, min, hour;
    unsigned char date, month;

    if(d[0] & 0x80)
        return 0;       // Clock stopped

    if(d[2] & 0x40)
        return 0;       // 12-hour mode not used

    if(!ValidBCD(d[0] & 0x7F)) return 0;
    if(!ValidBCD(d[1] & 0x7F)) return 0;
    if(!ValidBCD(d[2] & 0x3F)) return 0;
    if(!ValidBCD(d[4] & 0x3F)) return 0;
    if(!ValidBCD(d[5] & 0x1F)) return 0;
    if(!ValidBCD(d[6])) return 0;

    sec = BCD_TO_DEC(d[0] & 0x7F);
    min = BCD_TO_DEC(d[1] & 0x7F);
    hour = BCD_TO_DEC(d[2] & 0x3F);

    date = BCD_TO_DEC(d[4] & 0x3F);
    month = BCD_TO_DEC(d[5] & 0x1F);

    if(sec > 59 || min > 59 || hour > 23)
        return 0;

    if((d[3] & 0x07) < 1 || (d[3] & 0x07) > 7)
        return 0;

    if(date < 1 || date > 31)
        return 0;

    if(month < 1 || month > 12)
        return 0;

    return 1;
}

// ------------------------------------------------
// DISPLAY FUNCTIONS
// ------------------------------------------------

void DisplayClock(unsigned char *d)
{
    unsigned char sec, min, hour;
    unsigned char date, month, year;

    sec = BCD_TO_DEC(d[0] & 0x7F);
    min = BCD_TO_DEC(d[1] & 0x7F);
    hour = BCD_TO_DEC(d[2] & 0x3F);

    date = BCD_TO_DEC(d[4] & 0x3F);
    month = BCD_TO_DEC(d[5] & 0x1F);
    year = BCD_TO_DEC(d[6]);

    LCD_Command(0x80);

    LCD_String("TIME:");

    LCD_Data((hour / 10) + '0');
    LCD_Data((hour % 10) + '0');
    LCD_Data(':');

    LCD_Data((min / 10) + '0');
    LCD_Data((min % 10) + '0');
    LCD_Data(':');

    LCD_Data((sec / 10) + '0');
    LCD_Data((sec % 10) + '0');

    LCD_String("   ");

    LCD_Command(0xC0);

    LCD_String("DATE:");

    LCD_Data((date / 10) + '0');
    LCD_Data((date % 10) + '0');
    LCD_Data('/');

    LCD_Data((month / 10) + '0');
    LCD_Data((month % 10) + '0');
    LCD_Data('/');

    LCD_Data((year / 10) + '0');
    LCD_Data((year % 10) + '0');
}

// ------------------------------------------------
// MAIN PROGRAM
// ------------------------------------------------

void main(void)
{
    unsigned char rtc[7];

    ADCON1 = 0x06;

    LCD_Init();
    I2C_Init();

    LCD_Command(0x80);
    LCD_String("PIC DIGITAL CLK");
    LCD_Command(0xC0);
    LCD_String("DS1307 RTC");
    __delay_ms(1500);

    // Set initial time and date
    LCD_Command(0x01);
    LCD_String("SETTING RTC...");

    if(!RTC_SetTime())
    {
        LCD_Command(0x01);
        LCD_String("RTC WRITE ERROR");
        while(1);
    }

    __delay_ms(100);

    while(1)
    {
        if(!RTC_ReadTime(rtc))
        {
            LCD_Command(0x80);
            LCD_String("I2C READ ERROR  ");
            LCD_Command(0xC0);
            LCD_String("CHECK RTC WIRING");
        }
        else if(!RTC_Valid(rtc))
        {
            LCD_Command(0x80);
            LCD_String("INVALID RTC DATA");
            LCD_Command(0xC0);
            LCD_String("CHECK RTC CLOCK ");
        }
        else
        {
            DisplayClock(rtc);
        }

        __delay_ms(300);
    }
}
