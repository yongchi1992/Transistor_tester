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
	P2DIR |= 0x02;      //P2.1����Ϊ���״̬
	P4DIR |= 0xFF;		//P4.0~P4.7����Ϊ���״̬
	P7DIR |= 0x0C;		//P7.2~P7.3����Ϊ���״̬
	P9DIR |= 0x0A;		//P9.3,P9.1����Ϊ���״̬

}

void TLV_5613(volatile unsigned int data)
{
	P7OUT |= 0x04;				//SPD=1
	P9OUT &= ~0x08;				//Ƭѡ
///////////////////////////////////////////////////////
	//data &= 0x0FFF;			//����
	P9OUT &= ~0x02;				//�趨A1=0
	P7OUT &= ~0x08;				//�趨A0=0
								//A1A0=01,����LSW
	transfer(data);
///////////////////////////////////////////////////////
	data = data >> 8;
	//P9OUT &= ~0x02;			//�趨A1=0
	P7OUT |= 0x08;				//�趨A0=1
								//A1A0=01,����MSW
	transfer(data);
///////////////////////////////////////////////////////
	P9OUT |= 0x02;				//�趨A1=1
	P7OUT |= 0x08;				//�趨A0=1
								//A1A0=11,���������"xxxxx101",RLDAC,not(PWD),SPD
	transfer(0x05);
///////////////////////////////////////////////////////
	P9OUT |= 0x08;				//ȡ��Ƭѡ
}

void transfer(unsigned int val)
{
	P4OUT = val;
	P2OUT &= ~0x02;
	__no_operation();
	P2OUT |= 0x02;
}
