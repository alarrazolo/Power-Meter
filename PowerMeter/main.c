/*
 * PowerMeter.c
 *
 * Created: 10/14/2018 2:08:19 PM
 * Author : alarr
 */ 

#include "defines.h" // defined F_CPU in this file.

#ifndef F_CPU                          /* if not defined in Makefile... */
#define F_CPU  1000000UL                     /* set a safe default baud rate */
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
//#include <util/delay.h>
#include "SPI.h"
#include "USART.h"
#include "nrf24l01.h"
#include "nrf24l01-mnemonics.h"
#include "i2c_RTC_DS1307.h"
#include "M90E26.h"
#include "M90E26_Pins.h"

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4);}

void setup_timer(void);

nRF24L01 *setup_rf(void);

m90E26 *setup_powerIC(void);

void print_time(void);

void print_RF_settings(void);

void print_power_IC_settings(void);

void print_power_data(void);

volatile bool rf_interrupt = false;
volatile bool send_message = false;
uint8_t clockAddressR = 0b11010001; // write bit for DS1307
uint8_t clockAddressW = 0b11010000; // read bit for DS1307
uint8_t to_address[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2}; // RF channel address.

int main(void)
{
	initUSART();
	initI2C();
	SPI_init();
	
	//bool on = false;
	sei();
	//nRF24L01 *rf = setup_rf();
	setup_rf();
	//m90E26 *pIC = setup_powerIC();
	
	pIC_Start();
	setup_timer();
	
	printString("Starting Program!\r\n");
	
	//print_RF_settings();
	
	print_power_IC_settings();
	//set_pIC_RegValue(SmallPMod, 0xA987);
	int counter=0;
	
	//print_power_data();
	int kWh = 0;
	
    while (1) 
    {
		
		if (send_message){
			print_power_data();
			kWh += get_pIC_RegValue(ATenergy);
			printEnergy(kWh);
			printString("\r\n");
			send_message = false;
			if (counter < 5){
				//printHexWord(get_pIC_RegValue(Pmean));
				//printString("\r\n");
				counter++;
			}
			//else set_pIC_RegValue(SmallPMod, 0x0000);
		}
		
		/*
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
			//print_time();
			//print_power_data();
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
		*/
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

void print_RF_settings(void){
	
	printString("Transmitting on Channel: 0x");
	printHexByte(getRFRegValue(RF_CH));
	printString("H\r\n");
	
	printString("RF Set up Value: ");
	printHexByte(getRFRegValue(RF_SETUP));
	printString("H\r\n");
	
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

void print_power_IC_settings(void){
	printString("Power IC Settings:");
	printString("\r\n");
	
	printString("System Status: ");
	printHexWord(get_pIC_RegValue(SysStatus));
	printString("\r\n");
	
	printString("Metering Status: ");
	printHexWord(get_pIC_RegValue(EnStatus));
	printString("\r\n");
	
	printString("Metering Mode: ");
	printHexWord(get_pIC_RegValue(MMode));
	printString("\r\n");
	
	printString("Checksum 1: ");
	printHexWord(get_pIC_RegValue(CS1));
	printString("\r\n");
	
	printString("Checksum 2: ");
	printHexWord((get_pIC_RegValue(CS2)));
	printString("\r\n");
	
	printString("Measurement Calibration Start Command: ");
	printHexWord((get_pIC_RegValue(AdjStart)));
	printString("\r\n");
	
	printString("Voltage RMS Gain: ");
	printHexWord((get_pIC_RegValue(Ugain)));
	printString("\r\n");
	
	printString("L Line Current RMS Gain: ");
	printHexWord((get_pIC_RegValue(IgainL)));
	printString("\r\n");
	
}

void print_power_data(void){
	
	//printString("Measurement Calibration start: \r\n");
	//printWord(get_pIC_RegValue(AdjStart));
	printString("Voltage: ");
	printVoltage(get_pIC_RegValue(Urms));
	printString("\t\tCurrent: ");
	printCurrent(get_pIC_RegValue(Irms));
	printString("\r\n");
	printString("Frequency: ");
	printFrequency(get_pIC_RegValue(Freq));
	printString("\tPower Factor: ");
	printPowerFactor(get_pIC_RegValue(PowerF));
	printString("\tPhase Angle: ");
	printPhaseAngle(get_pIC_RegValue(Pangle));
	printString("\n\rActive Power: ");
	printPower(get_pIC_RegValue(Pmean));
	printString("\tReactive Power: ");
	printPower(get_pIC_RegValue(Qmean));
	printString("\tAbsolute Power: ");
	printPower(get_pIC_RegValue(Smean));
	printString("\r\n\n");
	
}

// each one second interrupt
ISR(TIMER1_COMPA_vect) {
	send_message = true;
}

// nRF24L01 interrupt
ISR(INT0_vect) {
	rf_interrupt = true;
}