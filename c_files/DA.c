/*
 * DA.c
 *
 *  Created on: 2013-7-14
 *      Author: Archimedes
 */

#include <msp430f5438a.h>
#include "DA.h"

void dainit()
{
	P2DIR |= 0x02;      //P2.1设置为输出状态
	P4DIR |= 0xFF;		//P4.0~P4.7设置为输出状态
	P7DIR |= 0x0C;		//P7.2~P7.3设置为输出状态
	P9DIR |= 0x0A;		//P9.3,P9.1设置为输出状态

}

void TLV_5613(volatile unsigned int data)
{
	P7OUT |= 0x04;				//SPD=1
	P9OUT &= ~0x08;				//片选
///////////////////////////////////////////////////////
	//data &= 0x0FFF;			//置数
	P9OUT &= ~0x02;				//设定A1=0
	P7OUT &= ~0x08;				//设定A0=0
								//A1A0=01,传输LSW
	transfer(data);
///////////////////////////////////////////////////////
	data = data >> 8;
	//P9OUT &= ~0x02;			//设定A1=0
	P7OUT |= 0x08;				//设定A0=1
								//A1A0=01,传输MSW
	transfer(data);
///////////////////////////////////////////////////////
	P9OUT |= 0x02;				//设定A1=1
	P7OUT |= 0x08;				//设定A0=1
								//A1A0=11,传输控制字"xxxxx101",RLDAC,not(PWD),SPD
	transfer(0x05);
///////////////////////////////////////////////////////
	P9OUT |= 0x08;				//取消片选
}

void transfer(unsigned int val)
{
	P4OUT = val;
	P2OUT &= ~0x02;
	__no_operation();
	P2OUT |= 0x02;
}
