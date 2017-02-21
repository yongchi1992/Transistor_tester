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
	        );     //！！太特么难找了！！Set VCore to highest level for supporting 24MHz

	P5SEL |= BIT2 + BIT3;    //P5.2和P5.3选择为晶振XT2输入
	UCSCTL6 &= ~XT2OFF;
	UCSCTL3 |= SELREF_2;
	UCSCTL4 |= SELA_2;
	do
	{
	  UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
	  SFRIFG1 &= ~OFIFG;                          // 清除振荡器失效标志
	  for (i = 0xFF; i > 0; i--);                 // 延时，等待XT2起振
	} while (SFRIFG1 & OFIFG);                    // 判断XT2是否起振
	UCSCTL6 &= ~XT2DRIVE0;
	UCSCTL4 |= SELS_5 + SELM_5;                   //选择MCLK、SMCLK为XT2,
}

