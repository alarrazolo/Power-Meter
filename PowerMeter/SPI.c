/*
 * SPI.c
 *
 * Created: 10/16/2018 4:11:50 PM
 *  Author: alarr
 */ 

#include "SPI.h"

void SPI_init(void){
	// set as master
	SPCR |= _BV(MSTR);
	// enable SPI
	SPCR |= _BV(SPE);
	// SPI mode 0: Clock Polarity CPOL = 0, Clock Phase CPHA = 0
	SPCR &= ~_BV(CPOL);
	SPCR &= ~_BV(CPHA);
	// Clock 2X speed
	//SPCR &= ~_BV(SPR0);
	SPCR |=_BV(SPR0);
	SPCR &= ~_BV(SPR1);
	//SPSR |= _BV(SPI2X);
	SPSR &= ~_BV(SPI2X);
	// most significant first (MSB)
	SPCR &= ~_BV(DORD);
}

uint8_t SPI_tradeByte(uint8_t byte) {
	SPDR = byte;                       /* SPI starts sending immediately */
	//loop_until_bit_is_set(SPSR, SPIF);                /* wait until done */
	while (!(SPSR & _BV(SPIF)));
	/* SPDR now contains the received byte */
	return SPDR;
}