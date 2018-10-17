/*
 * M90E26.h
 *
 * Created: 10/16/2018 4:14:46 PM
 *  Author: alarr
 */ 

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "nrf24l01.h"
#include "SPI.h"
#include "M90E26_Pins.h"
#include "USART.h"

#ifndef M90E26_H_
#define M90E26_H_

#define ReadRegPower 0x80
#define WriteRegPower 0x00

/*
typedef struct {
	volatile uint8_t *port;
	uint8_t pin;
} gpio_pin;
*/

typedef struct {
	gpio_pin ss; // slave select
	gpio_pin ce; // chip enabled
	gpio_pin sck; // serial clock
	gpio_pin mosi; // master out slave in
	gpio_pin miso; // master in slave out
	uint8_t status;
} m90E26;


m90E26 *powerIC_init(void);

void pIC_Start(void);

uint16_t get_pIC_RegValue(uint8_t pICRegister);

void set_pIC_RegValue(uint8_t pICRegister, uint16_t byte);

void printVoltage(uint16_t word);

void printCurrent(uint16_t word);

void printPower(int16_t word);

void printFrequency(uint16_t word);

void printPowerFactor(int16_t word);

void printPhaseAngle(int16_t word);

#endif /* M90E26_H_ */