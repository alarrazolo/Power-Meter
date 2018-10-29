/*
 * lcd_4x20_i2c.c
 *
 * Created: 10/28/2018 11:31:27 PM
 *  Author: alarr
 */ 

#include "lcd_4x20_i2c.h"


//#define i2c_address_W 0x7E  // Define I2C Address where the PCF8574A is
//#define i2c_address_R 0x7F
#define i2c_address_W 0x4E  // Define I2C Address where the PCF8574A is
#define i2c_address_R 0x4F
#define RS				0
#define RW				1
#define E				2
#define BACKLIGHT		3
#define D4				4
#define D5				5
#define D6				6
#define D7				7

void initLCD(){
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

void set_Up_4bit_Mode(void){
	//set up LCD in 4bit Mode
	send_Command((1 << D4) | (1 << D5));
	
	send_Command((1 << D4) | (1 << D5));
	
	send_Command((1 << D4) | (1 << D5));
	
	send_Command((1 << D5));
}

void configure_Control_Bytes(void){
	//Configure control Bytes
	
	send_Command((1 << D5));
	send_Command((1 << D7));
	
	send_Command(0x00);
	send_Command((1 << D7));
	
	send_Command(0x00);
	send_Command((1 << D4));
	
	send_Command(0x00);
	send_Command((1 << D5) | (1 << D6));
}

void enable_lcd(void){
	send_Command((1 << BACKLIGHT));
	send_Command((1 << BACKLIGHT) | (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7));
}

void disable_LCD(void){
	send_Command(0x00);
	send_Command((1 << D7));
}

void clear_lcd(void){
	send_Command((1 << BACKLIGHT));
	send_Command((1 << BACKLIGHT) | (1 << D4));
}

void move_Cursor_Right(void){
	send_Command((1 << BACKLIGHT) | (1 << D4));
	send_Command((1 << BACKLIGHT) | (1 << D6));
}

void move_Cursor_Left(void){
	send_Command((1 << BACKLIGHT) | (1 << D4));
	send_Command((1 << BACKLIGHT));
}

void move_Cursor_Home(void){
	send_Command((1 << BACKLIGHT));
	send_Command((1 << BACKLIGHT) | (1 << D5));
}

void write(char character){
	//uint8_t highNibble = ((uint8_t)character & 0xF0);
	//uint8_t lowNibble = (((uint8_t)character & 0x0F) << 4);
	uint8_t highNibble = (character & 0xF0);
	uint8_t lowNibble = ((character & 0x0F) << 4);
	send_Command((1 << BACKLIGHT) | (1 << RS) | highNibble);
	send_Command((1 << BACKLIGHT) | (1 << RS) | lowNibble);
}

void set_cursor(int line, int place){
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
	uint8_t highNibble = (number & 0xF0);
	uint8_t lowNibble = ((number & 0x0F) << 4);
	send_Command((1 << BACKLIGHT) | (1 << RS) | highNibble);
	send_Command((1 << BACKLIGHT) | (1 << RS) | lowNibble);
}