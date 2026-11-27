
#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 20000000

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF


#define EEPROM_WREN  0x06
#define EEPROM_WRITE 0x02
#define EEPROM_READ  0x03
#define EEPROM_RDSR  0x05


#define EEPROM_CS RD0


#define LCD_RS RD1
#define LCD_EN RD2


void SPI_Init(void)
{
    TRISCbits.TRISC3 = 0;  
    TRISCbits.TRISC4 = 1;  
    TRISCbits.TRISC5 = 0;  

    TRISDbits.TRISD0 = 0;  

    SSPSTAT = 0x40;       
    SSPCON = 0x21;        

    EEPROM_CS = 1;
}

unsigned char SPI_Transfer(unsigned char data)
{
    SSPBUF = data;

    while(SSPSTATbits.BF == 0);

    return SSPBUF;
}



void EEPROM_WriteEnable(void)
{
    EEPROM_CS = 0;

    SPI_Transfer(EEPROM_WREN);

    EEPROM_CS = 1;
}

unsigned char EEPROM_ReadStatus(void)
{
    unsigned char status;

    EEPROM_CS = 0;

    SPI_Transfer(EEPROM_RDSR);

    status = SPI_Transfer(0xFF);

    EEPROM_CS = 1;

    return status;
}

void EEPROM_Wait(void)
{
    while(EEPROM_ReadStatus() & 0x01)
    {
        __delay_ms(1);
    }
}

void EEPROM_WriteByte(unsigned int address,
                      unsigned char data)
{
    EEPROM_WriteEnable();

    EEPROM_CS = 0;

    SPI_Transfer(EEPROM_WRITE);

    SPI_Transfer((unsigned char)(address >> 8));

    SPI_Transfer((unsigned char)address);

    SPI_Transfer(data);

    EEPROM_CS = 1;

    EEPROM_Wait();
}

unsigned char EEPROM_ReadByte(unsigned int address)
{
    unsigned char data;

    EEPROM_CS = 0;

    SPI_Transfer(EEPROM_READ);

    SPI_Transfer((unsigned char)(address >> 8));

    SPI_Transfer((unsigned char)address);

    data = SPI_Transfer(0xFF);

    EEPROM_CS = 1;

    return data;
}


void LCD_Enable(void)
{
    LCD_EN = 1;
    __delay_us(2);

    LCD_EN = 0;
    __delay_us(100);
}

void LCD_SendNibble(unsigned char data)
{
    PORTD = (PORTD & 0x0F) | (data & 0xF0);

    LCD_Enable();
}

void LCD_Command(unsigned char command)
{
    LCD_RS = 0;

    LCD_SendNibble(command & 0xF0);

    LCD_SendNibble((command << 4) & 0xF0);

    if(command == 0x01 || command == 0x02)
        __delay_ms(2);
    else
        __delay_us(50);
}

void LCD_Char(unsigned char data)
{
    LCD_RS = 1;

    LCD_SendNibble(data & 0xF0);

    LCD_SendNibble((data << 4) & 0xF0);

    __delay_us(50);
}

void LCD_String(const char *text)
{
    while(*text)
    {
        LCD_Char(*text++);
    }
}

void LCD_Init(void)
{
    __delay_ms(20);

    LCD_RS = 0;
    LCD_EN = 0;

    LCD_SendNibble(0x30);
    __delay_ms(5);

    LCD_SendNibble(0x30);
    __delay_us(150);

    LCD_SendNibble(0x30);
    __delay_us(150);

    LCD_SendNibble(0x20);

    LCD_Command(0x28);  // 4-bit, 2-line LCD
    LCD_Command(0x0C);  // Display ON
    LCD_Command(0x06);  // Cursor increment
    LCD_Command(0x01);  // Clear display
}

void LCD_SetCursor(unsigned char row,
                   unsigned char column)
{
    if(row == 1)
        LCD_Command(0x80 + column - 1);
    else
        LCD_Command(0xC0 + column - 1);
}

void main(void)
{
    unsigned char stored_data;
    char display_text[17];

    ADCON1 = 0x06;  

    TRISD = 0x00;
    PORTD = 0x00;

    SPI_Init();
    LCD_Init();

    LCD_SetCursor(1,1);
    LCD_String("SPI EEPROM TEST");

    __delay_ms(1500);

  
    stored_data = EEPROM_ReadByte(0x0000);

    
    if(stored_data != 100)
    {
        EEPROM_WriteByte(0x0000, 100);
    }

  
    stored_data = EEPROM_ReadByte(0x0000);

   
    LCD_Command(0x01);

    LCD_SetCursor(1,1);
    LCD_String("EEPROM DATA:");

    LCD_SetCursor(2,1);

    sprintf(display_text, "Value = %u", stored_data);

    LCD_String(display_text);

    while(1)
    {
        
    }
}
