#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

//void startDemo(unsigned char times, unsigned char mode);
void startDemo(unsigned char times, unsigned char mode);
unsigned char getButton();
void ledSet(unsigned char number, unsigned char state);
void randomInit();
unsigned char getAdc();
unsigned char getRandom(unsigned char bits);
void generate(unsigned char lenght);
unsigned char check(unsigned char length);
void gameOver(unsigned char score);

char *combination;

int main(void)
{
    randomInit();
    DDRB |= (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5);
    startDemo(3, 0b00000011);
    _delay_ms(500);

    unsigned char length = 1;
    while(1)
    {
        /*unsigned char buttons = getButton();
        ledSet(buttons, 1);
        ledSet(~buttons, 0);*/
        generate(length);
        if(check(length))
        {
            gameOver(length - 1);
            length = 0;
        }
        _delay_ms(500);

        length++;
    }

    return 0;
}
void gameOver(unsigned char score)
{
    startDemo(2, 0b00000001);

    //print score
    ledSet(score, 1);

    while(!getButton());
    startDemo(0, 0b00000010);
    _delay_ms(500);
}
unsigned char check(unsigned char length)
{
    unsigned char i;
    for(i = 0; i < length; i++)
    {
        unsigned char buttons;
        while(1)
        {
            buttons = getButton();
            if(buttons != 0) break;
        }
        unsigned char buttonCode = 0;
        ledSet(buttons, 1);
        if(buttons & 0b00000001)
        {
            buttonCode = 0;
        }
        else if(buttons & 0b00000010)
        {
            buttonCode = 1;
        }
        else if(buttons & 0b00000100)
        {
            buttonCode = 2;
        }
        else if(buttons & 0b00001000)
        {
            buttonCode = 3;
        }
        if(!(buttonCode == *(combination + i)))
        {
            return 1;
        }
        _delay_ms(150);
        while(getButton());
        ledSet(0xFF, 0);
    }
    free(combination);
    return 0;
}
void generate(unsigned char length)
{
    combination = malloc(sizeof(*combination) * length);

    unsigned char i;
    for(i = 0; i < length; i++)
    {
        unsigned char ran = getRandom(3);

        if(ran >= 4) ran -= 4;
        switch(ran)
        {
        case 0:
            ledSet(0b00000001, 1);
            break;
        case 1:
            ledSet(0b00000010, 1);
            break;
        case 2:
            ledSet(0b00000100, 1);
            break;
        case 3:
            ledSet(0b00001000, 1);
            break;
        }

        _delay_ms(800);
         ledSet(0xFF, 0);
        _delay_ms(400);

        *(combination + i) = ran;
    }
}

void randomInit()
{
    ADMUX = (1<<REFS0) | (1<<MUX0);
    ADCSRA = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS2) | (1<<ADSC);
}
unsigned char getRandom(unsigned char bits)
{
    unsigned char output = 0;

    unsigned char i;
    for(i = 0; i < bits; i++)
    {
        unsigned char adc = getAdc();
        output |= (adc & 0b00000001) << i;
        _delay_us(140);
    }

    return output;
}
unsigned char getAdc()
{
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC));

    return ADCW;
}

unsigned char getButton()
{
    unsigned char toReturn = 0;

    if(!(PINB & (1<<PB1))) toReturn |= 0b00000001;
    if(!(PINB & (1<<PB0))) toReturn |= 0b00000010;
    if(!(PIND & (1<<PD7))) toReturn |= 0b00000100;
    if(!(PIND & (1<<PD6))) toReturn |= 0b00001000;

    return toReturn;
}
void ledSet(unsigned char number, unsigned char state)
{
    if(number & 0b00000001)
    {
        if(state) PORTB |= (1<<PB2);
        else PORTB &= ~(1<<PB2);
    }
    if(number & 0b00000010)
    {
        if(state) PORTB |= (1<<PB3);
        else PORTB &= ~(1<<PB3);
    }
    if(number & 0b00000100)
    {
        if(state) PORTB |= (1<<PB4);
        else PORTB &= ~(1<<PB4);
    }
    if(number & 0b00001000)
    {
        if(state) PORTB |= (1<<PB5);
        else PORTB &= ~(1<<PB5);
    }
}
void startDemo(unsigned char times, unsigned char mode)
{
    unsigned char i;
    if(mode & 0b00000001)
    {
        for(i = 0; i < times; i++)
        {
            ledSet(0b00000001, 1);
            _delay_ms(100);
            ledSet(0b00000001, 0);

            ledSet(0b00000010, 1);
            _delay_ms(100);
            ledSet(0b00000010, 0);

            ledSet(0b00000100, 1);
            _delay_ms(100);
            ledSet(0b00000100, 0);

            ledSet(0b00001000, 1);
            _delay_ms(100);
            ledSet(0b00001000, 0);
        }
    }
    if(mode & 0b00000010)
    {
        _delay_ms(200);
        ledSet(0b00001111, 1);
        _delay_ms(1000);
        ledSet(0b00001111, 0);
    }
}
