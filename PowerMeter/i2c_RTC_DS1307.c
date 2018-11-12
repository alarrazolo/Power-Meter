#include "i2c_RTC_DS1307.h"

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4);}
	
uint8_t clockAddressR = 0b11010001; // write bit for DS1307
uint8_t clockAddressW = 0b11010000; // read bit for DS1307

void clearBuff(char *buf){
	uint8_t i = 0;
	while (buf[i]) {
		buf[i] = 0;
		i++;
	}
}

void formatTime(uint8_t byte, char *buf, int i){
	buf[i] = ('0' + ((byte / 10) % 10));                      // Tens
	buf[++i] = ('0' + (byte % 10));                             // Ones
}

void formatYear(uint16_t word, char *buf, int i){
	buf[i] = ('0' + ((word / 1000) % 10));               // Thousands
	buf[i] = ('0' + ((word / 100) % 10));                 // Hundreds
	buf[i] = ('0' + ((word / 10) % 10));                      // Tens
	buf[++i] = ('0' + (word % 10));                             // Ones
}

void get_time(char *buf){
	clearBuff(buf);
	int i = 0;
	char* timeflag;
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
		timeflag = "PM";
	}
	else {
		timeflag = "AM";
	}
	
	formatTime(hh, buf, i);
	i += 2;
	buf[i++] = ':';
	formatTime(mm, buf, i);
	i += 2;
	buf[i++] = ':';
	formatTime(ss, buf, i);
	i += 2;
	buf[i++] = ' ';
	
	buf[i++] = timeflag[0];
	buf[i++] = timeflag[1];
	
	buf[i++] = ' ';
	formatTime(month, buf, i);
	i += 2;
	buf[i++] = '/';
	formatTime(date, buf, i);
	i += 2;
	buf[i++] = '/';
	formatYear(year, buf, i);
}