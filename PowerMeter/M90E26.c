/*
 * M90E26.c
 *
 * Created: 10/16/2018 4:19:36 PM
 *  Author: alarr
 */ 

//#include <string.h>
#include "M90E26.h"
//#include "M90E26_Pins.h"
//#include "SPI.h"

m90E26 *powerIC_init(void) {
	m90E26 *pIC = malloc(sizeof(m90E26));
	memset(pIC, 0, sizeof(m90E26));
	return pIC;
}

void pIC_Start(void){
	
	// set chip select port to output.
	DDRB |= (1<<0);
	//set chip select on power meter IC high
	PORTB |= (1<<0);
	
	if(!((SPCR >> MSTR)&0x01)) SPI_init();
	set_pIC_RegValue(AdjStart, 0x56, 0x78);
	
}


uint16_t get_pIC_RegValue(uint8_t pICRegister){
	
	//send AND read Byte with register address Byte to read back 16 bit value.
	
	PORTB &= ~(1<<0); // set chip select bit low to initiate SPI communication. 
	SPI_tradeByte(ReadRegPower | pICRegister); // AND read Byte with register address and send it over SPI
	uint8_t highByte = SPI_tradeByte(0); // Read MSD Byte first.
	uint8_t lowByte = SPI_tradeByte(0); // Read LSD Byte second.
	PORTB |= (1<<0); // set chip select bit high to terminate SPI communication.
	uint16_t full16Byte = (highByte << 8) | lowByte; // Combine MSB and LSB together into 16 bit Byte.
	return full16Byte; // return Byte to function call. 
	
	//printHexByte(pIC_d1bitHigh);
	//printHexByte(pIC_d1bitLow);
	//printString("H\r\n");
	
}

void set_pIC_RegValue(uint8_t pICRegister, uint8_t highByte, uint8_t lowByte){
	
	//send AND write Byte with register Byte to write 16 bit value into register.
	
	PORTB &= ~(1<<0); // set chip select bit low to initiate SPI communication. 
	SPI_tradeByte(WriteRegPower | pICRegister); // AND write Byte with register address and send it over SPI.
	SPI_tradeByte(highByte); // Write MSD Byte first.
	SPI_tradeByte(lowByte); // Write LSD Byte second.
	PORTB |= (1<<0); // set chip select bit high to terminate SPI communication.
	
	//printString("Set Reg: ");
	//printHexByte(pICRegister);
	//printString(" to value: ");
	//printHexByte(highBit);
	//printHexByte(lowBit);
	//printString("H\r\n");
	
}
