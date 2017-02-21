/*
 * power.c
 *
 *  Created on: 2013-7-15
 *      Author: Archimedes
 */

#include "hw_memmap.h"
#include "pmm.h"
#include "wdt.h"
#include "gpio.h"

unsigned int status = STATUS_SUCCESS ;

void SPEED_UP(void)
{
	unsigned char i;
	unsigned int status = STATUS_SUCCESS ;

	status = PMM_setVCore(__MSP430_BASEADDRESS_PMM__,
	        PMMCOREV_3
	        );     //����̫��ô�����ˣ���Set VCore to highest level for supporting 24MHz

	P5SEL |= BIT2 + BIT3;    //P5.2��P5.3ѡ��Ϊ����XT2����
	UCSCTL6 &= ~XT2OFF;
	UCSCTL3 |= SELREF_2;
	UCSCTL4 |= SELA_2;
	do
	{
	  UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
	  SFRIFG1 &= ~OFIFG;                          // �������ʧЧ��־
	  for (i = 0xFF; i > 0; i--);                 // ��ʱ���ȴ�XT2����
	} while (SFRIFG1 & OFIFG);                    // �ж�XT2�Ƿ�����
	UCSCTL6 &= ~XT2DRIVE0;
	UCSCTL4 |= SELS_5 + SELM_5;                   //ѡ��MCLK��SMCLKΪXT2,
}

