/*
 * SPI.h
 *
 * Created: 10/16/2018 4:10:07 PM
 *  Author: alarr
 */ 

#include <avr/io.h>

#ifndef SPI_H_
#define SPI_H_

void SPI_init(void);

uint8_t SPI_tradeByte(uint8_t byte);

#endif /* SPI_H_ */