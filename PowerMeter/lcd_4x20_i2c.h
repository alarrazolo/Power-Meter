/*
 * lcd_4x20_i2c.h
 *
 * Created: 10/28/2018 11:33:52 PM
 *  Author: alarr
 */ 


#ifndef LCD_4X20_I2C_H_
#define LCD_4X20_I2C_H_

#include <avr/io.h>
#include "i2c.h"

void initLCD(void);
void send_Command(uint8_t data);
void set_Up_4bit_Mode(void);
void configure_Control_Bytes(void);
void enable_lcd(void);
void disable_LCD(void);
void clear_lcd(void);
void move_Cursor_Right(void);
void move_Cursor_Left(void);
void move_Cursor_Home(void);
void write(char character);
char nibbleToHex(uint8_t nibble);
void set_cursor(int line, int place);
void lcd_print_string(const char String[]);
void lcd_print_number(int number);
void lcd_print_hex(uint8_t hex);


#endif /* LCD_4X20_I2C_H_ */