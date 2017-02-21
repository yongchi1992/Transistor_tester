//******************************************************************************
//   Interfacing ADS7950 with MSP430F543xA - USCI_B1, SPI 3-Wire Master Demo
//
//   Description: SPI master talks to SPI slave using 3-wire mode. Operation
//   in Manual mode of ADS7950 and read data from channel 0~3.
//   ACLK = ~32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/4
//
//
//                   MSP430F5438A
//                 -----------------
//             /|\|                 |
//              | |                 |
//              --|RST              |
//                |                 |
//                |             P3.7|-> Data Out (UCB1SIMO)
//                |                 |
//                |             P5.4|<- Data In (UCB1SOMI)
//                |                 |
//  		\CS <-|P1.1         P5.5|-> Serial Clock Out (UCB1CLK)
//

//   July 2013
//   Built with CCE Version: 5.1.1
//******************************************************************************

#include <msp430f5438a.h>
#include "AD.h"

typedef unsigned int uint;
#define	clk1 P5OUT |=BIT5
#define clk0 P5OUT &=~BIT5
#define cs1	 P1OUT |=BIT1
#define cs0  P1OUT &=~BIT1



void adinit()
{
    GPIO_Init();
}

void GPIO_Init(void)
{

	P1DIR |= BIT1;                       // Set P1.1 to output direction
	cs1;                       		     // Set P1.1 for not(CS)=1
	P3DIR |= BIT7;                       // P3.7 OUTPUT-DATAOUT
	P5DIR |= BIT5; 						 //P5.5 OUTPUT AS CLK
	P5DIR &= ~BIT4;						 //P5.4 INPUT
	clk0;

	WriteSPI(0x1880);					 //zero order
}

uint WriteSPI(uint data)
{
	volatile unsigned int msb;
	volatile unsigned int temp;
	volatile unsigned int i;

	msb = 0x0000;

	cs0;
	P3OUT &= ~BIT7;
	P3OUT |= (data>>8) & BIT7;		//将data15给P3.7
	clk1;					//SDI15
	temp = P5IN;						//将P5.4给msb,读入SDO15
	temp =(0x0001 & (temp >> 4));
	msb |=temp;
	msb = msb << 1;

	for (i=7; i>=1; i--)
	{
		clk0;					//SDO
		P3OUT &= ~BIT7;
		P3OUT |= (data>>i) & BIT7;
		clk1;					//SDI
		temp = P5IN;
		temp =(0x0001 & (temp >> 4));
		msb |=temp;
		msb = msb << 1;
	}
	for (i=0; i<=7; i++)
	{
		clk0;
		P3OUT &= ~BIT7;
		P3OUT |= (data<<i) & BIT7;
		clk1;						//SDI7~0
		temp = P5IN;
		temp =(0x0001 & (temp >> 4));
		msb |=temp;
		msb = msb << 1;
	}

	msb = msb >> 1;

	__delay_cycles(1000);

	cs1;

	__delay_cycles(1000);

	return msb;
}


void ADS_7950(volatile int *ad,unsigned int n)
{
	volatile uint MCR;	//MCR means ModeControlReg
	if (n==3)
	{
		MCR = 0x1100;							//next order ch=ch2
		WriteSPI(MCR);
		*(ad+0) = 0x0FFF & WriteSPI(MCR);		//read from 1080 ch1
		MCR = 0x1180;							//next order ch=ch3
		WriteSPI(MCR);
		*(ad+1) = 0x0FFF & WriteSPI(MCR);		//read from 1100 ch2
		MCR = 0x1080;							//next order ch=ch1
		WriteSPI(MCR);
		*(ad+2) = 0x0FFF & WriteSPI(MCR);		//read from	1180 ch3
	}
	if (n==1)
	{
		MCR = 0x1080;						//next order
		WriteSPI(MCR);
		*ad = 0x0FFF & WriteSPI(MCR);		//read from 1080
	}
}
