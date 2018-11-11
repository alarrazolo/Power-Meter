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
#include "i2c.h"
#include "lcd_4x20_i2c.h"
#include "M90E26.h"
#include "M90E26_Pins.h"

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4);}

void setup_timer(void);

nRF24L01 *setup_rf(void);

m90E26 *setup_powerIC(void);

void print_time(void);

void lcd_print_Time(void);

void print_RF_settings(void);

void lcd_print_RF_settings(void);

void print_power_IC_settings(void);

void lcd_print_power_IC_settings(void);

void print_power_data();

void lcd_print_power_data();

volatile bool rf_interrupt = false;
volatile bool send_message = false;
uint8_t clockAddressR = 0b11010001; // write bit for DS1307
uint8_t clockAddressW = 0b11010000; // read bit for DS1307
uint8_t to_address[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2}; // RF channel address.

int main(void)
{
	initUSART();
	initI2C();
	initLCD();
	SPI_init();
	
	//bool on = false;
	sei();
	//nRF24L01 *rf = setup_rf();
	setup_rf();
	//m90E26 *pIC = setup_powerIC();
	
	pIC_Start();
	setup_timer();
	
	clear_lcd();
	set_cursor(0,0);
	lcd_print_string("Starting Program!");
	//printString("Starting Program!\r\n");
	
	
	
	
	//print_RF_settings();
	
	print_power_IC_settings();
	//set_pIC_RegValue(SmallPMod, 0xA987);
	int counter = 0;
	
	//print_power_data();
	int kWh = 0;
	
    while (1) 
    {
		
		if (send_message){
			print_power_data();
			kWh += get_pIC_RegValue(ATenergy);
			//printEnergy(kWh);
			//printString("\r\n");
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
	rf->ss.pin = PORTB2;
	rf->ce.port = &PORTB;
	rf->ce.pin = PORTB1;
	rf->sck.port = &PORTB;
	rf->sck.pin = PORTB5;
	rf->mosi.port = &PORTB;
	rf->mosi.pin = PORTB3;
	rf->miso.port = &PORTB;
	rf->miso.pin = PORTB4;
	// interrupt on falling edge of INT0 (PD2)
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);
	nRF24L01_begin(rf);
	return rf;
}

m90E26 *setup_powerIC(void) {
	m90E26 *pIC = powerIC_init();
	pIC->ce.port = &PORTB;
	pIC->ce.pin = PORTB0;
	pIC->sck.port = &PORTB;
	pIC->sck.pin = PORTB5;
	pIC->mosi.port = &PORTB;
	pIC->mosi.pin = PORTB3;
	pIC->miso.port = &PORTB;
	pIC->miso.pin = PORTB4;
	//nRF24L01_begin(rf);
	return pIC;
}

// setup timer to trigger interrupt every second when at 1MHz
void setup_timer(void) {
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	//OCR1A = 15624; // for 1MHz Clock
	OCR1A = 31250;  // for 8MHz Clock
	//OCR1A = 250000;  // for 16 MHz Clock
	//TCCR1B |= _BV(CS10) | _BV(CS11);
	TCCR1B |= (1 << CS12);
	//changed timer to every 4 seconds
	//TCCR1B |= _BV(CS12);
	
}

void print_RF_settings(void){
	
	printString("Transmitting on Channel: ");
	printHexByte(getRFRegValue(RF_CH));
	printString("\r\n");
	
	printString("RF Set up Value: ");
	printHexByte(getRFRegValue(RF_SETUP));
	printString("\r\n");
	
}

void lcd_print_RF_settings(void){
	
	lcd_print_string("Transmitting on Channel: ");
	lcd_print_hex(getRFRegValue(RF_CH));
	
	lcd_print_string("RF Set up Value: ");
	lcd_print_hex(getRFRegValue(RF_SETUP));
	
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

void lcd_print_Time(void){
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
	
	clear_lcd();
	set_cursor(0,0);
	lcd_print_number(hh);
	lcd_print_string(":");
	lcd_print_number(mm);
	lcd_print_string(":");
	lcd_print_number(ss);
	//printString(timeflag);
	set_cursor(1,0);
	lcd_print_number(month);
	lcd_print_string("/");
	lcd_print_number(date);
	lcd_print_string("/");
	lcd_print_number(year);
	//printString("\r\n");
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

void lcd_print_power_IC_settings(void){
	lcd_print_string("Power IC Settings:");
	lcd_print_string("\r\n");
	
	lcd_print_string("System Status: ");
	lcd_print_hex(get_pIC_RegValue(SysStatus));
	lcd_print_string("\r\n");
	
	lcd_print_string("Metering Status: ");
	lcd_print_hex(get_pIC_RegValue(EnStatus));
	lcd_print_string("\r\n");
	
	lcd_print_string("Metering Mode: ");
	lcd_print_hex(get_pIC_RegValue(MMode));
	lcd_print_string("\r\n");
	
	lcd_print_string("Checksum 1: ");
	lcd_print_hex(get_pIC_RegValue(CS1));
	lcd_print_string("\r\n");
	
	lcd_print_string("Checksum 2: ");
	lcd_print_hex((get_pIC_RegValue(CS2)));
	lcd_print_string("\r\n");
	
	lcd_print_string("Measurement Calibration Start Command: ");
	lcd_print_hex((get_pIC_RegValue(AdjStart)));
	lcd_print_string("\r\n");
	
	lcd_print_string("Voltage RMS Gain: ");
	lcd_print_hex((get_pIC_RegValue(Ugain)));
	lcd_print_string("\r\n");
	
	lcd_print_string("L Line Current RMS Gain: ");
	lcd_print_hex((get_pIC_RegValue(IgainL)));
	lcd_print_string("\r\n");
	
}

void print_power_data(){
	char data[10];
	//printString("Measurement Calibration start: \r\n");
	//printWord(get_pIC_RegValue(AdjStart));
	printString("Voltage: ");
	formatVoltage(get_pIC_RegValue(Urms), data);
	printString(data);
	
	printString("\t\tCurrent: ");
	formatCurrent(get_pIC_RegValue(Irms), data);
	printString(data);
	
	printString("\r\n");
	printString("Frequency: ");
	formatFrequency(get_pIC_RegValue(Freq), data);
	printString(data);
	
	printString("\tPower Factor: ");
	formatPowerFactor(get_pIC_RegValue(PowerF), data);
	printString(data);
	
	printString("\tPhase Angle: ");
	formatPhaseAngle(get_pIC_RegValue(Pangle), data);
	printString(data);
	
	printString("\n\rActive Power: ");
	formatPower(get_pIC_RegValue(Pmean), data);
	printString(data);
	
	printString("\tReactive Power: ");
	formatPower(get_pIC_RegValue(Qmean), data);
	printString(data);
	
	printString("\tAbsolute Power: ");
	formatPower(get_pIC_RegValue(Smean), data);
	printString(data);
	
	printString("\r\n\n");
	
}

void lcd_print_power_data(){
	char data[10];
	clear_lcd();
	set_cursor(0,0);
	lcd_print_string("V: ");
	formatVoltage(get_pIC_RegValue(Urms), data);
	lcd_print_string(data);
	
	lcd_print_string(" I: ");
	formatCurrent(get_pIC_RegValue(Irms), data);
	lcd_print_string(data);
	
	set_cursor(1,0);
	lcd_print_string("FREQ: ");
	formatFrequency(get_pIC_RegValue(Freq), data);
	lcd_print_string(data);
	
	lcd_print_string(" PF: ");
	formatPowerFactor(get_pIC_RegValue(PowerF), data);
	lcd_print_string(data);
	
	lcd_print_string(" P Angle: ");
	formatPhaseAngle(get_pIC_RegValue(Pangle), data);
	lcd_print_string(data);
	
	set_cursor(2,0);
	lcd_print_string("AP: ");
	formatPower(get_pIC_RegValue(Pmean), data);
	lcd_print_string(data);
	
	lcd_print_string(" RP: ");
	formatPower(get_pIC_RegValue(Qmean), data);
	lcd_print_string(data);
	
	lcd_print_string(" ABSP: ");
	formatPower(get_pIC_RegValue(Smean), data);
	lcd_print_string(data);
}

// each one second interrupt
ISR(TIMER1_COMPA_vect) {
	send_message = true;
}

// nRF24L01 interrupt
ISR(INT0_vect) {
	rf_interrupt = true;
}