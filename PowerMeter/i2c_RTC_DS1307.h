// Functions for i2c communication
#include <avr/io.h>
#include "i2c.h"
#include "USART.h"

void formatTime(uint8_t byte, char *buf, int i);
void formatYear(uint16_t word, char *buf, int i);
void get_time(char *buf);