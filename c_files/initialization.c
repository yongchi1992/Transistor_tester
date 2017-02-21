/*
 * initialization.c
 *
 *  Created on: 2013-7-16
 *      Author: Archimedes
 */
#include <msp430f5438a.h>
#include "power.h"
#include "KeyBoardDriver.h"
#include "LCD.h"
#include "AD.h"
#include "DA.h"
#include "rtc.h"
#include "sfr_sys.h"
#include "hw_memmap.h"
#include "wdt.h"
#include "gpio.h"
#include "ucs.h"
#include "clock.h"

extern unsigned char Image_zju[512];
extern unsigned char Image_ti[512];
extern unsigned char Image_name[1024];


void initialize(void)
{
	unsigned int i,j;

	WDTCTL = WDTPW+WDTHOLD; //关闭看门狗
	SPEED_UP();

	sel_low();
	Ini_Lcd();              //初始化液晶
	Write_Cmd(0x34);		//打开扩展指令集
	for(i=0;i<64;i++)		//清屏
	{
		if(i>31)       //上下屏地址转换
		{
			Write_Cmd(0x80+i-32);//先写Y轴坐标
			Write_Cmd(0x88);//再写X轴坐标,(0x88-0x8F)
		}
		else
		{
			Write_Cmd(0x80+i);//先写Y轴坐标
			Write_Cmd(0x80);//再写X轴坐标,(0x80-0x87)
		}
		for(j=0;j<16;j++)
		{
			Write_Data(0x00); //每行写入16个空格
		}
	}
	for (i=0;i<64;i++)
	{
		LCD_draw(0,0,&Image_zju[512-(i+1)*8],64,i+1);
		LCD_draw(4,63-i,Image_ti,64,i+1);
		delay_nms(300);
	}
	delay_nms(15000);

	LCD_draw(0,0,Image_name,128,64);
	delay_nms(30000);
	Ini_Lcd();				//初始化液晶
	Init_Keypad();
	adinit();				//初始化AD
	dainit();				//初始化DA
	time_init();
    P9DIR |= BIT6;		 	//蜂鸣器脚设定为输出
    P9OUT &= ~BIT6;
}
