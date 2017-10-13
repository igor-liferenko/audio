/* The PWM sample rate is 64kHz, we just update the
   output every 8kHz.
*/

/*
    NOTE: a lot of code was removed in commit e579fe82396a6622b560bc81eae45100cafa51e1
    read this article and understand for what it was needed and whether it should be
    restored back:
    https://digibird1.wordpress.com/arduino-as-a-soundcard/
 */

/* see
  http://www.trackuino.org/2010/04/audio-with-arduino-and-pwm.html
  http://wiki.openmusiclabs.com/wiki/PWMDAC
  http://web.archive.org/web/20151112214041/http://madmanmarkau.net/codingblog/?p=17
*/

#define F_CPU 16000000UL
#include <inttypes.h>
#include <avr/io.h>

int main (void)
{
    unsigned char *pin, *pout, count;
    int      nbuf, pwm_cycles;
    unsigned char buf[BUF_SZ];
	
    /* 16 bit Timer/Counter 1, in 8 bit fast PWM mode */
    /* 64kHz PWM with a 16 MHz clock */

    TCCR1A = 0xc1; 
    TCCR1B = 0x9;
    OCR1A = 128;

    /* set up RS232 UART -------------------------------*/

         UBRR0H = 0;
         UBRR0L = 16; /* 115,200 baud with 16 MHz clock +2% error */
         UCSR0A = (1<<U2X0); /* double speed mode */
         UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
         UCSR0B = (1<<TXEN0)|(1<<RXEN0);


    DDRB  |= (1<<PB1); 
    DDRB  |= (1<<PB5); 
	
    nbuf = 0;
    pin = pout = buf;
    count = 0;

    pwm_cycles = 0;

    while(1) {

	/* wait for current PWM cycle to complete */

	while(!(TIFR1 & 0x1)) { /* !pwm_complete */
	    /* receive bytes from host via RS232 UART */

	    if ((UCSR0A & 1<<RXC0)) {
		if (nbuf <= BUF_SZ) {
		    *pin++ = UDR0;
		    if (pin == (buf + BUF_SZ))
			pin = buf;
		    nbuf++;
		}
//else

//		    PORTB |= 1<<PB5;   
//	      PORTB &= ~(1<<PB5);
            }	    
	}

	TIFR1 |= 0x1; /* reset PWM-cycle-complete flag */

	/* update PWM output sample every 7th cycle */

//	if (pwm_cycles == 7) {

	    if (nbuf) {
		OCR1A = *pout++;
		nbuf--;
	    }
	    else
		OCR1A = 128; /* send 0 if FIFO empty */

	    if (pout == (buf + BUF_SZ))
		pout = buf;
		
//	}
	    
	/* modulo 8 counter */

//	pwm_cycles++;
//	pwm_cycles &= 7;
    }
}

