/*
 * M90E26_Pins.h
 *
 * Created: 10/16/2018 8:27:39 PM
 *  Author: alarr
 */ 


#ifndef M90E26_PINS_H_
#define M90E26_PINS_H_

//Register Map

//Status and Special Register
#define SoftRest	0x00
#define SysStatus	0x01
#define FuncEn		0x02
#define SagTh		0x03
#define SmallPMod	0x04
#define LastData	0x06

//Metering Calibration and Configuration Register
#define LSB			0x08
#define CalStart	0x20
#define PLconstH	0x21
#define PLconstL	0x22
#define Lgain		0x23
#define Lphi		0x24
#define Ngain		0x25
#define Nphi		0x26
#define PStartTh	0x27
#define PNolTH		0x28
#define QStartTh	0x29
#define QNolTH		0x2A
#define MMode		0x2B
#define CS1			0x2C

//Measurement Calibration Register
#define AdjStart	0x30
#define Ugain		0x31
#define IgainL		0x32
#define IgainN		0x33
#define Uoffset		0x34
#define IoffsetL	0x35
#define IoffsetN	0x36
#define PoffsetL	0x37
#define QoffsetL	0x38
#define PoffsetN	0x39
#define QoffsetN	0x3A
#define CS2			0x3B

//Energy Register
#define APenergy	0x40
#define ANenergy	0x41
#define ATenergy	0x42
#define RPenergy	0x43
#define RNenergy	0x44
#define RTenergy	0x45
#define EnStatus	0x46

//Measurement Register
#define Irms		0x48
#define Urms		0x49
#define Pmean		0x4A
#define Qmean		0x4B
#define Freq		0x4C
#define PowerF		0x4D
#define Pangle		0x4E
#define Smean		0x4F
#define Irms2		0x68
#define Pmean2		0x6A
#define Qmean2		0x6B
#define PowerF2		0x6D
#define Pangle2		0x6E
#define Smean2		0x6F

#endif /* M90E26_PINS_H_ */