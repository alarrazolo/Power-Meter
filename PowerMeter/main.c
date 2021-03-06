/*
 * PowerMeter.c
 *
 * Created: 10/14/2018 2:08:19 PM
 * Author : alarr
 */ 

#include "defines.h" // defined F_CPU in this file.

#ifndef F_CPU
#define F_CPU  1000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
#include "SPI.h"
#include "USART.h"
#include "nrf24l01.h"
#include "nrf24l01-mnemonics.h"
#include "i2c.h"
#include "lcd_4x20_i2c.h"
#include "M90E26.h"
#include "M90E26_Pins.h"
#include "i2c_RTC_DS1307.h"

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
uint8_t to_address[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2}; // RF channel address.

int main(void)
{
	initUSART();
	initI2C();
	initLCD();
	//SPI_init();
	
	//bool on = false;
	sei();
	//nRF24L01 *rf = setup_rf();
	
	//setup_rf();
	
	//m90E26 *pIC = setup_powerIC();
	
	//pIC_Start();
	setup_timer();
	
	clear_lcd();
	set_cursor(0,0);
	lcd_print_string("Starting Program!");
	//printString("Starting Program!\r\n");
	
	//print_RF_settings();
	
	//print_power_IC_settings();
	//lcd_print_power_IC_settings();
	
	//set_pIC_RegValue(SmallPMod, 0xA987);
	int counter = 0;
	
	//print_power_data();
	//int kWh = 0;
	
    while (1) 
    {
		
		if (send_message){
			//lcd_print_power_data();
			//print_power_data();
			print_time();
			lcd_print_Time();
			//printString("Hello");
			//kWh += get_pIC_RegValue(ATenergy);
			//printEnergy(kWh);
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
	//TCCR1B |= (1 << CS10) | (1 << CS11); // 1MHz
	TCCR1B |= (1 << CS12); // 8MHz
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
	
	clear_lcd();
	lcd_print_string("RF Channel: ");
	lcd_print_hex(getRFRegValue(RF_CH));
	
	set_cursor(2,0);
	lcd_print_string("Set up Value: ");
	lcd_print_hex(getRFRegValue(RF_SETUP));
	
}

void print_time(void){
	char time[30];
	get_time(time);
	printString(time);
}

void lcd_print_Time(void){
	char time[30];
	get_time(time);
	clear_lcd();
	lcd_print_string(time);
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
	clear_lcd();
	//lcd_print_string("pIC Settings:");
	//lcd_print_string("\r\n");
	
	lcd_print_string("SS: ");
	lcd_printHexWord(get_pIC_RegValue(SysStatus));
	
	set_cursor(1,0);
	lcd_print_string("MS: ");
	lcd_printHexWord(get_pIC_RegValue(EnStatus));
	
	set_cursor(2,0);
	lcd_print_string("MM: ");
	lcd_printHexWord(get_pIC_RegValue(MMode));
	
	//set_cursor(1,0);
	//lcd_print_string("CS 1: ");
	//lcd_printHexWord(get_pIC_RegValue(CS1));
	
	//lcd_print_string("CS 2: ");
	//lcd_printHexWord((get_pIC_RegValue(CS2)));
	
	//set_cursor(2,0);
	//lcd_print_string("M Cal Start: ");
	//lcd_printHexWord((get_pIC_RegValue(AdjStart)));
	
	//set_cursor(3,0);
	//lcd_print_string("VRMS Gain: ");
	//lcd_printHexWord((get_pIC_RegValue(Ugain)));
	
	//lcd_print_string("Lc RMS Gain: ");
	//lcd_printHexWord((get_pIC_RegValue(IgainL)));
	
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
	clear_lcd();
	char data[10];
	clear_lcd();
	set_cursor(0,0);
	lcd_print_string("V:    ");
	formatVoltage(get_pIC_RegValue(Urms), data);
	lcd_print_string(data);
	
	set_cursor(1,0);
	lcd_print_string("I:    ");
	formatCurrent(get_pIC_RegValue(Irms), data);
	lcd_print_string(data);
	
	set_cursor(2,0);
	lcd_print_string("RP:  ");
	formatPower(get_pIC_RegValue(Qmean), data);
	lcd_print_string(data);
	
	set_cursor(3,0);
	lcd_print_string("P Angle: ");
	formatPhaseAngle(get_pIC_RegValue(Pangle), data);
	lcd_print_string(data);
	
	/*set_cursor(4,0);
	lcd_print_string("PF: ");
	formatPowerFactor(get_pIC_RegValue(PowerF), data);
	lcd_print_string(data);
	
	set_cursor(4,0);
	lcd_print_string("FREQ: ");
	formatFrequency(get_pIC_RegValue(Freq), data);
	lcd_print_string(data);
	
	set_cursor(2,0);
	lcd_print_string(" P Angle: ");
	formatPhaseAngle(get_pIC_RegValue(Pangle), data);
	lcd_print_string(data);
	
	lcd_print_string("AP: ");
	formatPower(get_pIC_RegValue(Pmean), data);
	lcd_print_string(data);
	
	set_cursor(3,0);
	lcd_print_string(" RP: ");
	formatPower(get_pIC_RegValue(Qmean), data);
	lcd_print_string(data);
	
	lcd_print_string(" ABSP: ");
	formatPower(get_pIC_RegValue(Smean), data);
	lcd_print_string(data);
	
	lcd_print_string("FREQ: ");
	formatFrequency(get_pIC_RegValue(Freq), data);
	lcd_print_string(data);*/
}

// each one second interrupt
ISR(TIMER1_COMPA_vect) {
	send_message = true;
}

// nRF24L01 interrupt
ISR(INT0_vect) {
	rf_interrupt = true;
}