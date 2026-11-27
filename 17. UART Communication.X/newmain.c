
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


void UART_Init(void)
{
    TRISCbits.TRISC6 = 0;  
    TRISCbits.TRISC7 = 1;  

    SPBRG = 129;           

    TXSTAbits.BRGH = 1;    
    TXSTAbits.SYNC = 0;    

    RCSTAbits.SPEN = 1;    
    TXSTAbits.TXEN = 1;    
    RCSTAbits.CREN = 1;    
}


void UART_Tx(unsigned char data)
{
    while(PIR1bits.TXIF == 0);

    TXREG = data;
}


unsigned char UART_Rx(void)
{
    while(PIR1bits.RCIF == 0);


    if(RCSTAbits.OERR)
    {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    return RCREG;
}


void UART_String(const char *str)
{
    while(*str)
    {
        UART_Tx(*str++);
    }
}

void main(void)
{
    unsigned char received_data;

   
    ADCON1 = 0x06;

  
    TRISB = 0xF0;     
    PORTB = 0x00;

  
    UART_Init();

    __delay_ms(100);

    UART_String("PIC16F877A UART READY\r\n");
    UART_String("A=LED1 ON, a=LED1 OFF\r\n");
    UART_String("B=LED2 ON, b=LED2 OFF\r\n");
    UART_String("C=LED3 ON, c=LED3 OFF\r\n");
    UART_String("D=LED4 ON, d=LED4 OFF\r\n");
    UART_String("X=ALL ON, x=ALL OFF\r\n");

    while(1)
    {
        received_data = UART_Rx();

        switch(received_data)
        {
            case 'A':
                RB0 = 1;
                UART_String("LED1 ON\r\n");
                break;

            case 'a':
                RB0 = 0;
                UART_String("LED1 OFF\r\n");
                break;

            case 'B':
                RB1 = 1;
                UART_String("LED2 ON\r\n");
                break;

            case 'b':
                RB1 = 0;
                UART_String("LED2 OFF\r\n");
                break;

            case 'C':
                RB2 = 1;
                UART_String("LED3 ON\r\n");
                break;

            case 'c':
                RB2 = 0;
                UART_String("LED3 OFF\r\n");
                break;

            case 'D':
                RB3 = 1;
                UART_String("LED4 ON\r\n");
                break;

            case 'd':
                RB3 = 0;
                UART_String("LED4 OFF\r\n");
                break;

            case 'X':
                PORTB = (PORTB & 0xF0) | 0x0F;
                UART_String("ALL LEDs ON\r\n");
                break;

            case 'x':
                PORTB = PORTB & 0xF0;
                UART_String("ALL LEDs OFF\r\n");
                break;

            default:
                UART_String("INVALID COMMAND\r\n");
                break;
        }
    }
}
