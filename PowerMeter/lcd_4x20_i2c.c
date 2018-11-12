/*
 * lcd_4x20_i2c.c
 *
 * Created: 10/28/2018 11:31:27 PM
 *  Author: alarr
 */ 

#include "defines.h"
#include "lcd_4x20_i2c.h"
#include <util/delay.h>

#define i2c_address_W 0x7E  // Define I2C Address where the PCF8574A is
#define i2c_address_R 0x7F
//#define i2c_address_W 0x4E  // Define I2C Address where the PCF8574A is
//#define i2c_address_R 0x4F
#define RS				0
#define RW				1
#define E				2
#define BACKLIGHT		3
#define D4				4
#define D5				5
#define D6				6
#define D7				7

#define delay_time 1

void initLCD(void){
	set_Up_4bit_Mode();
	configure_Control_Bytes();
	enable_lcd();
}

void send_Command(uint8_t data){
	i2cStart();
	i2cSend(i2c_address_W);
	i2cSend(data);
	i2cSend(data | (1 << E));
	i2cSend(data & ~(1 << E));
	i2cStop();
}

void checkBF(void){
	uint8_t flag;
	do{
		send_Command((1 << RW) | (1 << BACKLIGHT));
		send_Command((1 << RW) | (1 << BACKLIGHT));
		i2cStart();
		i2cSend(i2c_address_W);
		i2cSend((1 << BACKLIGHT));
		i2cStart();
		i2cSend(i2c_address_R);
		flag = i2cReadNoAck();
		i2cStop();
	}
	while((flag>>7));
}

void set_Up_4bit_Mode(void){
	//set up LCD in 4bit Mode
	_delay_ms(15);
	send_Command((1 << D4) | (1 << D5));
	_delay_ms(4.1);
	send_Command((1 << D4) | (1 << D5));
	_delay_us(100);
	send_Command((1 << D4) | (1 << D5));
	_delay_ms(4.1);
	send_Command((1 << D5));
}

void configure_Control_Bytes(void){
	//Configure control Bytes
	_delay_us(40);
	send_Command((1 << D5));
	send_Command((1 << D7));
	_delay_us(40);
	send_Command(0x00);
	send_Command((1 << D7));
	_delay_us(40);
	send_Command(0x00);
	send_Command((1 << D4));
	_delay_ms(1.64);
	send_Command(0x00);
	send_Command((1 << D5) | (1 << D6));
	_delay_us(40);
}

void enable_lcd(void){
	checkBF();
	send_Command((1 << BACKLIGHT));
	send_Command((1 << BACKLIGHT) | (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7));
}

void disable_LCD(void){
	checkBF();
	send_Command(0x00);
	send_Command((1 << D7));
}

void clear_lcd(void){
	checkBF();
	send_Command((1 << BACKLIGHT));
	send_Command((1 << BACKLIGHT) | (1 << D4));
}

void move_Cursor_Right(void){
	checkBF();
	send_Command((1 << BACKLIGHT) | (1 << D4));
	send_Command((1 << BACKLIGHT) | (1 << D6));
}

void move_Cursor_Left(void){
	checkBF();
	send_Command((1 << BACKLIGHT) | (1 << D4));
	send_Command((1 << BACKLIGHT));
}

void move_Cursor_Home(void){
	checkBF();
	send_Command((1 << BACKLIGHT));
	send_Command((1 << BACKLIGHT) | (1 << D5));
}

char nibbleToHex(uint8_t nibble) {
	/* Converts 4 bits into hexadecimal */
	if (nibble < 10) {
		return ('0' + nibble);
	}
	else {
		return ('A' + nibble - 10);
	}
}

void write(char character){
	checkBF();
	//uint8_t highNibble = ((uint8_t)character & 0xF0);
	//uint8_t lowNibble = (((uint8_t)character & 0x0F) << 4);
	uint8_t highNibble = (character & 0xF0);
	uint8_t lowNibble = ((character & 0x0F) << 4);
	send_Command((1 << BACKLIGHT) | (1 << RS) | highNibble);
	send_Command((1 << BACKLIGHT) | (1 << RS) | lowNibble);
}

void set_cursor(int line, int place){
	checkBF();
	if(line > 3) line = 3;
	if(place > 20) place = 20;
	
	if(line == 0){
		uint8_t highNibble = ((0x00 + place) & 0xF0);
		uint8_t lowNibble = (((0x00 + place) & 0x0F) << 4);
		send_Command((1 << BACKLIGHT) | (1 << D7) | highNibble);
		send_Command((1 << BACKLIGHT) | lowNibble);
	}
	else if(line == 1){
		uint8_t highNibble = ((0x40 + place) & 0xF0);
		uint8_t lowNibble = (((0x40 + place) & 0x0F) << 4);
		send_Command((1 << BACKLIGHT) | (1 << D7) | highNibble);
		send_Command((1 << BACKLIGHT) | lowNibble);
	}
	else if(line == 2){
		uint8_t highNibble = ((0x14 + place) & 0xF0);
		uint8_t lowNibble = (((0x14 + place) & 0x0F) << 4);
		send_Command((1 << BACKLIGHT) | (1 << D7) | highNibble);
		send_Command((1 << BACKLIGHT) | lowNibble);
	}
	else if(line == 3){
		uint8_t highNibble = ((0x54 + place) & 0xF0);
		uint8_t lowNibble = (((0x54 + place) & 0x0F) << 4);
		send_Command((1 << BACKLIGHT) | (1 << D7) | highNibble);
		send_Command((1 << BACKLIGHT) | lowNibble);
	}
}

void lcd_print_string(const char String[])
{
	uint8_t i = 0;
	while (String[i]) {
		write(String[i]);
		i++;
	}
}

void lcd_print_number(int number){
	checkBF();
	uint8_t highNibble = (number & 0xF0);
	uint8_t lowNibble = ((number & 0x0F) << 4);
	send_Command((1 << BACKLIGHT) | (1 << RS) | highNibble);
	send_Command((1 << BACKLIGHT) | (1 << RS) | lowNibble);
}

void lcd_print_hex(uint8_t hex){
	uint8_t highNibble = ((hex & 0xF0) >> 4);
	uint8_t lowNibble = (hex & 0x0F);
	write('0');
	write('x');
	write(nibbleToHex(highNibble));
	write(nibbleToHex(lowNibble));
	write('H');
}

void lcd_printHexWord(uint16_t word){
	//printHexByte(word>>8);
	//printHexByte(word & ~(0xff<<8));
	//printString("H");
	
	write('0');
	write('x');
	uint8_t nibble;
	nibble = (word & 0xF000) >> 12;
	write(nibbleToHex(nibble));
	nibble = (word & 0x0F00) >> 8;
	write(nibbleToHex(nibble));
	nibble = (word & 0xF0) >> 4;
	write(nibbleToHex(nibble));
	nibble = word & 0x0F;
	write(nibbleToHex(nibble));
	write('H');
	
}