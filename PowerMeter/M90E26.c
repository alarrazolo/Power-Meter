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
	
	set_pIC_RegValue(CalStart, 0x5678);
	set_pIC_RegValue(PLconstH, 0x0030);
	set_pIC_RegValue(PLconstL, 0xB3D3);
	set_pIC_RegValue(Lgain, 0x0000);
	set_pIC_RegValue(Lphi, 0x0000);
	set_pIC_RegValue(Ngain, 0x0000);
	set_pIC_RegValue(Nphi, 0x0000);
	set_pIC_RegValue(PStartTh, 0x08BD);
	set_pIC_RegValue(PNolTH, 0x0000);
	set_pIC_RegValue(QStartTh, 0x0AEC);
	set_pIC_RegValue(QNolTH, 0x0000);
	set_pIC_RegValue(MMode, 0x3422);
	uint16_t calibrationCS1 = get_pIC_RegValue(CS1);
	set_pIC_RegValue(CS1, calibrationCS1);
	set_pIC_RegValue(CalStart, 0x8765);
	
	
	
	set_pIC_RegValue(AdjStart, 0x5678);
	
	set_pIC_RegValue(Ugain, 0x8561);
	set_pIC_RegValue(IgainL, 0x3D1C);
	set_pIC_RegValue(IgainN, 0x7530);
	set_pIC_RegValue(Uoffset, 0x0000);
	set_pIC_RegValue(IoffsetL, 0x0000);
	set_pIC_RegValue(IoffsetN, 0x0000);
	set_pIC_RegValue(PoffsetL, 0x0000);
	set_pIC_RegValue(QoffsetL, 0x0000);
	set_pIC_RegValue(PoffsetN, 0x0000);
	set_pIC_RegValue(QoffsetN, 0x0000);
	uint16_t calibrationCS2 = get_pIC_RegValue(CS2);
	set_pIC_RegValue(CS2, 0x0000);
	set_pIC_RegValue(CS2, calibrationCS2);
	set_pIC_RegValue(AdjStart, 0x8765);
	
	//set_pIC_RegValue(CalStart, 0x5678);
	//set_pIC_RegValue(AdjStart, 0x5678);
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

void set_pIC_RegValue(uint8_t pICRegister, uint16_t byte){
	
	//send AND write Byte with register Byte to write 16 bit value into register.
	
	PORTB &= ~(1<<0); // set chip select bit low to initiate SPI communication. 
	SPI_tradeByte(WriteRegPower | pICRegister); // AND write Byte with register address and send it over SPI.
	uint8_t highByte = (byte>>8);
	uint8_t lowByte = byte & ~(0xff<<8);
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

void printVoltage(uint16_t word){
	transmitByte('0' + (word / 10000));                 /* Ten-thousands */
	transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
	transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
	printString("."); //
	transmitByte('0' + ((word / 10) % 10));                      /* Tens */
	transmitByte('0' + (word % 10));                             /* Ones */
}

void printCurrent(uint16_t word){
	transmitByte('0' + (word / 10000));                 /* Ten-thousands */
	transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
	printString("."); //
	transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
	transmitByte('0' + ((word / 10) % 10));                      /* Tens */
	transmitByte('0' + (word % 10));                             /* Ones */
}

void printPower(int16_t word){
	if(word>>15){
		//word &= ~(1UL<<15);
		printString("-");
		//word ^= (0xffff & ~(1UL<<15));
		word ^= 0xffff;
		word++;
		transmitByte('0' + (word / 10000));                 /* Ten-thousands */
		transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
		printString("."); //
		transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
		transmitByte('0' + ((word / 10) % 10));                      /* Tens */
		transmitByte('0' + (word % 10));                             /* Ones */
	}
	else{
		transmitByte('0' + (word / 10000));                 /* Ten-thousands */
		transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
		printString("."); //
		transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
		transmitByte('0' + ((word / 10) % 10));                      /* Tens */
		transmitByte('0' + (word % 10));                             /* Ones */
	}
	
}

void printFrequency(uint16_t word){
	//transmitByte('0' + (word / 10000));                 /* Ten-thousands */
	transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
	transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
	printString("."); //
	transmitByte('0' + ((word / 10) % 10));                      /* Tens */
	transmitByte('0' + (word % 10));                             /* Ones */
}

void printPowerFactor(int16_t word){
	if(word>>15){
		word &= ~(1UL<<15);
		printString("-");
		//word ^= (0xffff & ~(1UL<<16));
		//word++;
		//transmitByte('0' + (word / 10000));                 /* Ten-thousands */
		transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
		printString("."); //
		transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
		transmitByte('0' + ((word / 10) % 10));                      /* Tens */
		transmitByte('0' + (word % 10));                             /* Ones */
	}
	else{
		//transmitByte('0' + (word / 10000));                 /* Ten-thousands */
		transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
		printString("."); //
		transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
		transmitByte('0' + ((word / 10) % 10));                      /* Tens */
		transmitByte('0' + (word % 10));                             /* Ones */
	}
	
}

void printPhaseAngle(int16_t word){
	if(word>>15){
		//word &= ~(1UL<<15);
		printString("-");
		//word ^= (0xffff & ~(1UL<<15));
		word ^= 0xffff;
		word++;
		//transmitByte('0' + (word / 10000));                 /* Ten-thousands */
		transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
		transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
		transmitByte('0' + ((word / 10) % 10));                      /* Tens */
		printString("."); //
		transmitByte('0' + (word % 10));                             /* Ones */
	}
	else{
		//transmitByte('0' + (word / 10000));                 /* Ten-thousands */
		transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
		transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
		transmitByte('0' + ((word / 10) % 10));                      /* Tens */
		printString("."); //
		transmitByte('0' + (word % 10));                             /* Ones */
	}
	
}

void printEnergy(uint16_t word){
	transmitByte('0' + (word / 10000));                 /* Ten-thousands */
	transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
	transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
	transmitByte('0' + ((word / 10) % 10));                      /* Tens */
	printString("."); //
	transmitByte('0' + (word % 10));                             /* Ones */
}