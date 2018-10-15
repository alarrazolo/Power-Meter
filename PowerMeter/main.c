/*
 * PowerMeter.c
 *
 * Created: 10/14/2018 2:08:19 PM
 * Author : alarr
 */ 

//#define F_CPU 1000000UL
#include "defines.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
//#include <util/delay.h>
#include "nrf24l01.h"
#include "nrf24l01-mnemonics.h"
#include "USART.h"
#include "i2c_RTC_DS1307.h"


#define ReadRegPower 0x80
#define WriteRegPower 0x00

typedef struct {
	gpio_pin ss; // slave select
	gpio_pin ce; // chip enabled
	gpio_pin sck; // serial clock
	gpio_pin mosi; // master out slave in
	gpio_pin miso; // master in slave out
	uint8_t status;
} m90E26;
m90E26 *powerIC_init(void);
m90E26 *setup_powerIC(void);


static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4);}
	
uint8_t SPI_tradeByte(uint8_t byte);

void setup_timer(void);

nRF24L01 *setup_rf(void);

void print_time(void);

void printRFRegValue(uint8_t rfRegister);

void printpICRegValue(uint8_t pICRegister);

volatile bool rf_interrupt = false;
volatile bool send_message = false;
uint8_t clockAddressR = 0b11010001;
uint8_t clockAddressW = 0b11010000;
uint8_t to_address[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};

int main(void)
{
	initUSART();
	initI2C();
	DDRB |= (1<<0);
	bool on = false;
	sei();
	nRF24L01 *rf = setup_rf();
	m90E26 *pIC = setup_powerIC();
	setup_timer();
	
	//set Chip select on power meter IC high
	PORTB |= (1<<0);
	
	printString("Starting Program!\r\n");
	
	
	printString("Transmitting on Channel: 0x");
	printRFRegValue(RF_CH);
	
	printString("RF Set up Value: ");
	printRFRegValue(RF_SETUP);
	
	printString("pIC Reg Value 01H: ");
	printpICRegValue(0x01);
	printString("pIC Reg Value 03H: ");
	printpICRegValue(0x03);
	
    while (1) 
    {
		
		
		
		if (rf_interrupt) {
			rf_interrupt = false;
			int success = nRF24L01_transmit_success(rf);
			if (success == 0){
				
			}
			else{
				nRF24L01_flush_transmit_message(rf);
				//printString("Message Sent Failed\r\n");
			}
			
		}

		if (send_message) {
			send_message = false;
			on = !on;
			nRF24L01Message msg;
			char* message;
			if (on){
				message = "Holo Bibis";
				memcpy(msg.data, message, strlen(message) + 1);
			}
			else {
				message = "Te Amo!";
				memcpy(msg.data, message, strlen(message) + 1);
			}
			msg.length = strlen((char *)msg.data) + 1;
			nRF24L01_transmit(rf, to_address, &msg);
			
		}
		
    }
}


nRF24L01 *setup_rf(void) {
	nRF24L01 *rf = nRF24L01_init();
	rf->ss.port = &PORTB;
	rf->ss.pin = PB2;
	rf->ce.port = &PORTB;
	rf->ce.pin = PB1;
	rf->sck.port = &PORTB;
	rf->sck.pin = PB5;
	rf->mosi.port = &PORTB;
	rf->mosi.pin = PB3;
	rf->miso.port = &PORTB;
	rf->miso.pin = PB4;
	// interrupt on falling edge of INT0 (PD2)
	EICRA |= _BV(ISC01);
	EIMSK |= _BV(INT0);
	nRF24L01_begin(rf);
	return rf;
}

m90E26 *powerIC_init(void) {
	m90E26 *pIC = malloc(sizeof(m90E26));
	memset(pIC, 0, sizeof(m90E26));
	return pIC;
}

m90E26 *setup_powerIC(void) {
	m90E26 *pIC = powerIC_init();
	pIC->ce.port = &PORTB;
	pIC->ce.pin = PB0;
	pIC->sck.port = &PORTB;
	pIC->sck.pin = PB5;
	pIC->mosi.port = &PORTB;
	pIC->mosi.pin = PB3;
	pIC->miso.port = &PORTB;
	pIC->miso.pin = PB4;
	//nRF24L01_begin(rf);
	return pIC;
}

// setup timer to trigger interrupt every second when at 1MHz
void setup_timer(void) {
	TCCR1B |= _BV(WGM12);
	TIMSK1 |= _BV(OCIE1A);
	//OCR1A = 15624; // for 1MHz Clock
	OCR1A = 31250;  // for 8MHz Clock
	//OCR1A = 250000;  // for 16 MHz Clock
	//TCCR1B |= _BV(CS10) | _BV(CS11);
	TCCR1B |= _BV(CS12);
	//changed timer to every 4 seconds
	//TCCR1B |= _BV(CS12);
	
}

uint8_t SPI_tradeByte(uint8_t byte) {
	SPDR = byte;                       /* SPI starts sending immediately */
	loop_until_bit_is_set(SPSR, SPIF);                /* wait until done */
	/* SPDR now contains the received byte */
	return SPDR;
}

void print_time(void){
	i2cStart();
	i2cSend(clockAddressW);
	i2cSend(0x00);
	i2cStart();
	i2cSend(clockAddressR);
	uint8_t ss = bcd2bin(i2cReadAck() & 0x7F);		//seconds
	uint8_t mm = bcd2bin(i2cReadAck());		//minutes
	uint8_t hh = bcd2bin(i2cReadAck());		//hours
	bcd2bin(i2cReadAck());		//day of the week
	uint8_t date = bcd2bin(i2cReadAck());		//day of the month
	uint8_t month = bcd2bin(i2cReadAck());		//month
	uint16_t year = bcd2bin(i2cReadNoAck()) + 2000;		//year
	i2cStop();
	
	if (hh > 12){
		hh = hh - 12;
		//char* timeflag = "PM";
	}
	else {
		//char* timeflag = "AM";
	}
	printByte(hh);
	printString(":");
	printByte(mm);
	printString(":");
	printByte(ss);
	//printString(timeflag);
	printString("   ");
	printByte(month);
	printString("/");
	printByte(date);
	printString("/");
	printWord(year);
	printString("\r\n");
}

void printRFRegValue(uint8_t rfRegister){
	
	PORTB &= ~(1<<2);
	SPI_tradeByte(R_REGISTER | rfRegister);
	uint8_t rf_d1bit = SPI_tradeByte(0);
	PORTB |= (1<<2);
	printHexByte(rf_d1bit);
	printString("H\r\n");
	
}

void printpICRegValue(uint8_t pICRegister){
	
	PORTB &= ~(1<<0);
	SPI_tradeByte(ReadRegPower | pICRegister);
	uint8_t pIC_d1bitHigh = SPI_tradeByte(0);
	uint8_t pIC_d1bitLow = SPI_tradeByte(0);
	PORTB |= (1<<0);
	printHexByte(pIC_d1bitHigh);
	printHexByte(pIC_d1bitLow);
	printString("H\r\n");
	
}

// each one second interrupt
ISR(TIMER1_COMPA_vect) {
	send_message = true;
	print_time();
}

// nRF24L01 interrupt
ISR(INT0_vect) {
	rf_interrupt = true;
}