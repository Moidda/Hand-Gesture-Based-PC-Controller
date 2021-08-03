/*
 Polling single sonar 
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

#include "USART_RS232_H_file.h"

#define  Trigger_pin0	PA0	/* Trigger pin */
#define  Trigger_pin1	PA1	/* Trigger pin */

#define INF 5
#define DEL 0.2
#define DELAY 100
#define GARBAGE 20.0

char str[10] = {};

volatile int TimerOverflow = 0;

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}

void turn_led_on() {
	PORTB |= (1<<PINB0);
}

void turn_led_off() {
	PORTB &= (~(1<<PINB0));
}



double clip(double distance){
	if(distance < INF)
		return distance;
	return INF;
}

int clk = 0;

void sendData(double distance){
	if(clk == 2){
		dtostrf(distance, 2, 3, str);
		USART_SendString(str);
		clk = 0;
	}
	else
		clk++;
	
}

long count;
double getDistance(int trigger) {
	double distance = 100.0;
	while(distance > GARBAGE){
		PORTA |= (1 << trigger);
		_delay_us(10);
		PORTA &= (~(1 << trigger));
		
		TCNT1 = 0;			/* Clear Timer counter */
		TCCR1B = 0x41;		/* Capture on rising edge, No prescaler*/
		TIFR = 1<<ICF1;		/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;		/* Clear Timer Overflow flag */

		/*Calculate width of Echo by Input Capture (ICP) */
		
		while ((TIFR & (1 << ICF1)) == 0);		/* Wait for rising edge */
		TCNT1 = 0;								/* Clear Timer counter */
		TCCR1B = 0x01;							/* Capture on falling edge, No prescaler */
		TIFR = 1<<ICF1;							/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;							/* Clear Timer Overflow flag */
		TimerOverflow = 0;						/* Clear Timer overflow count */

		while ((TIFR & (1 << ICF1)) == 0);		/* Wait for falling edge */
		count = ICR1 + (65535 * TimerOverflow);	/* Take count */
		/* 8MHz Timer freq, sound speed =343 m/s */
		distance = (double)count / 466.47;
	}
	/* Give 10us trigger pulse on trig. pin to HC-SR04 */
	distance = clip(distance);
	sendData(distance);
	return distance;
}

int inActiveRange(double distance){
	if(1.0 < distance && distance < 5.0)
	return 1;
	return 0;
}

int main(void)
{	
	double distance, temp;
	
	DDRA = 0x01;		/* Make trigger pin as output */
	PORTD = 0xFF;		/* Turn on Pull-up */

	USART_init(9600);
	
	sei();					/* Enable global interrupt */
	TIMSK = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR1A = 0;				/* Set all bit to zero Normal operation */

	turn_led_on();
	int pauseCounter = 0;

	while(1)
	{		
		// sonar 1
		 distance = getDistance(Trigger_pin0);
		 //dtostrf(distance, 2, 3, str);
		 //USART_SendString(str);
		 
		 if(inActiveRange(distance)){
			 USART_SendString("H");
			 _delay_ms(DELAY);
			 distance = getDistance(Trigger_pin0);
			 if(inActiveRange(distance)){
				 USART_SendString("C");
				 temp = distance;
				 pauseCounter = 0;
				 
				 while(1){
					 distance = getDistance(Trigger_pin0);
					 if(distance >= INF)
						break;
					 if(temp + DEL < distance) {
						USART_SendString("D");
						temp = distance;
						pauseCounter = 0;
					 }
					 else if(temp - DEL> distance) {
						USART_SendString("U");
						temp = distance;
						pauseCounter = 0;
					 }
					 else {
						 pauseCounter++;
						 if(pauseCounter >= 3) {
							USART_SendString("P");
							pauseCounter = 0;
						 }
					 }
					_delay_ms(50);
				 }
				 USART_SendString("R");
			 }
			 else{
				 if (pauseCounter == -1) {
					 pauseCounter = 0;
				 }
				 else {
					USART_SendString("S"); 
				 }
			 }
		 }

		PORTB ^= (1<<PINB0);
		_delay_ms(50);
	}	
}