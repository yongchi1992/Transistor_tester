/*
 * KeyBoardDriver.c
 *
 *  Created on: 2013-7-13
 *      Author: Archimedes & Chen Yuzheng
 */
#include <msp430f5438a.h>
#include "KeyBoardDriver.h"

/***************变量***************/
unsigned int Key_Pressed;      //按键是否被按下:1--是，0--否
unsigned int Key_Val;          //存放键值
unsigned int Key_Long;			//按键是否持续
unsigned int Key_Map[] = {1 ,2 ,3 ,4 ,
						  5 ,6 ,7 ,8 ,
						  9 ,10,11,12,
						  13,14,15,16 }; //设置键盘逻辑键值与程序计算键值的映射
extern unsigned int daiji;
/***********************************/

/*******************************************
函数名称：Init_Keypad
功能：初始化扫描键盘的IO端口
参数：无
返回值：无
********************************************/
void Init_Keypad(void)
{
    P7DIR |= 0xF0;       //P7.4~P7.7设置为输出状态
    P2DIR &= 0x0F;		 //P2.4~P2.7设置为输入状态
    P7OUT |= 0xF0;       // P7.4~P7.7输出高电平
    Key_Pressed = 0;
    Key_Val = 0;
}
/*******************************************
函数名称：Check_Key
功能：扫描键盘的IO端口，获得键值
参数：无
返回值：无
********************************************/
void Check_Key(void)
{
    unsigned int row,col,tmp1,tmp2;

    tmp1 = 0x80;
    for(row = 0;row < 4;row++)              //行扫描
    {
        P7OUT = 0xf0;                      //P7.4~P7.7输出全1
        P7OUT -= tmp1;                      //P7.4~p7.7输出四位中有一个为0
        tmp1 >>=1;
        if ((P2IN & 0xf0) < 0xf0)           //是否P2IN的P2.4~P2.7中有一位为0
        {
            tmp2 = 0x10;                         // tmp2用于检测出那一位为0
            for(col = 0;col < 4;col++)              // 列检测
            {
                if((P2IN & tmp2) == 0x00)           // 是否是该列,等于0为是
                {
                    Key_Val = Key_Map[row * 4 + col];  // 获取键值
                    return;                         // 退出循环
                }
                tmp2 <<= 1;                        // tmp2左移1位
            }
        }
    }
}
/*******************************************
函数名称：delay_15ms
功能：延时约15ms，完成消抖功能
参数：无
返回值：无
********************************************/
void delay_15ms()
{
    volatile unsigned int tmp;
    for(tmp = 60000;tmp > 0;tmp--) _NOP();
    for(tmp = 30000;tmp > 0;tmp--) _NOP();
}
/*******************************************
函数名称：Key_Event
功    能：检测按键，并获取键值
参    数：无
返回值  ：无
********************************************/
void Key_Event(volatile unsigned int *key,volatile unsigned int *key_long)
{
    volatile unsigned int tmp;

    P7OUT &= 0x00;              // 设置P1OUT全为0，等待按键输入
    tmp = P2IN;                 // 获取P2IN
    if ((tmp & 0xf0) < 0xf0) 	//如果有键按下
    {
    	delay_15ms();   //消除抖动
        Check_Key();            // 调用check_Key(),获取键值
        daiji=0;
        P3OUT&=~BIT0;
        		P3OUT&=~BIT5;

        Write_Cmd(0x0C);
    }
    else if ((tmp & 0xf0) == 0xf0) //如果按键已经释放
    {
        Key_Val = 0;
        delay_15ms();   //平衡时间
    }
    else delay_15ms();  //平衡时间

    if (*key == Key_Val)
    	{
    	*key_long += 1;

    	}
    else *key_long = 0;
    *key = Key_Val;
}



