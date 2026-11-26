
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

void pwm_init(void);
void pwm_set_duty(unsigned int duty);

void main(void)
{
    unsigned int i;

    TRISCbits.TRISC2 = 0;

    pwm_init();

    while(1)
    {
        // Increase brightness
        for(i = 0; i <= 1000; i += 10)
        {
            pwm_set_duty(i);
            __delay_ms(20);
        }

        // Decrease brightness
        for(i = 1000; i > 0; i -= 10)
        {
            pwm_set_duty(i);
            __delay_ms(20);
        }
    }
}

// PWM initialization
void pwm_init(void)
{
    PR2 = 249;

    CCP1CON = 0x0C;

    T2CON = 0x05;

    pwm_set_duty(0);
}

// PWM duty cycle
void pwm_set_duty(unsigned int duty)
{
    if(duty > 1000)
        duty = 1000;

    CCPR1L = (unsigned char)(duty >> 2);

    CCP1CON = (CCP1CON & 0xCF) | ((duty & 0x03) << 4);
}
