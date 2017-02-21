/*
 * LCD.c
 *
 *  Created on: 2013-7-13
 *      Author: Archimedes &Chen Yuzheng
 */
#include <msp430f5438a.h>
#include "LCD.h"


//extern const unsigned char shuzi_table[];

#define LCD_DataIn    P8DIR=0x00    //数据口方向设置为输入
#define LCD_DataOut   P8DIR=0xFF    //数据口方向设置为输出
#define LCD2MCU_Data  P8IN
#define MCU2LCD_Data  P8OUT
#define LCD_CMDOut    P3DIR|=BIT0+BIT4+BIT5
#define LCD_RS_H      P3OUT|=BIT0
#define LCD_RS_L      P3OUT&=~BIT0
#define LCD_RW_H      P3OUT|=BIT5
#define LCD_RW_L      P3OUT&=~BIT5
#define LCD_EN_H      P3OUT|=BIT4
#define LCD_EN_L      P3OUT&=~BIT4
#define LCD_RST_H     P11OUT|=BIT0
#define LCD_RST_L     P11OUT&=~BIT0
#define Point_Draw  0
#define Point_Clear 1
/************************
 *
 *
 * *******************/

/*******************************************
函数名称：delay_1us
功    能：延时约1us的时间
参    数：无
返回值  ：无
********************************************/
void delay_50us(void)
{
	unsigned int i;

	for(i = 50;i > 0;i--)  _NOP();
}

/*******************************************
函数名称：delay_1ms
功    能：延时约1ms的时间
参    数：无
返回值  ：无
********************************************/
void delay_1ms(void)
{
	unsigned int i;

	for(i = 500;i > 0;i--)  _NOP();
}
/*******************************************
函数名称：delay_nms
功    能：延时N个1ms的时间
参    数：n--延时长度
返回值  ：无
********************************************/
void delay_nms(unsigned int n)
{
    unsigned int i;

    for(i = n;i > 0;i--)    delay_1ms();
}
/*******************************************
函数名称：Write_Cmd
功    能：向液晶中写控制命令
参    数：cmd--控制命令
返回值  ：无
********************************************/
void Write_Cmd(unsigned char cmd)
{
    unsigned char lcdtemp = 0;

    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do                       //判忙
    {
        LCD_EN_H;
        delay_50us();
        lcdtemp = LCD2MCU_Data;
        LCD_EN_L;

    }
    while(lcdtemp & 0x80);

    LCD_DataOut;
    LCD_RW_L;
    MCU2LCD_Data = cmd;
    LCD_EN_H;
    delay_50us();
    LCD_EN_L;
}
/*******************************************
函数名称：Write_Data
功    能：向液晶中写显示数据
参    数：dat--显示数据
返回值  ：无
********************************************/
void  Write_Data(unsigned char dat)
{
    unsigned char lcdtemp = 0;

    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do                       //判忙
    {
        LCD_EN_H;
        delay_50us();
        lcdtemp = LCD2MCU_Data;
        LCD_EN_L;
    }
    while(lcdtemp & 0x80);

    LCD_DataOut;
    LCD_RS_H;
    LCD_RW_L;

    MCU2LCD_Data = dat;
    LCD_EN_H;
    _NOP();
    LCD_EN_L;
}
/**************************** read data ***********************/
unsigned int Read_Data(void)
{
	unsigned int mcu_data;
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do                       //判忙
    {
        LCD_EN_H;
        delay_50us();
        mcu_data = LCD2MCU_Data;
        LCD_EN_L;
    }while(mcu_data & 0x80);

	LCD_RS_H;
	delay_50us();
	LCD_EN_H;		 //dummy read
	delay_50us();
	LCD_EN_L;
	delay_50us();
	LCD_EN_H;        //E持续拉高读才有效
	delay_50us();
	mcu_data=LCD2MCU_Data;
	mcu_data<<=8;
	LCD_EN_L;
	delay_50us();
	LCD_EN_H;        //E持续拉高读才有效
	delay_50us();
	mcu_data+=LCD2MCU_Data;
	LCD_EN_L;
	return(mcu_data);
}
/*******************************************
函数名称：Ini_Lcd
功    能：初始化液晶模块
参    数：无
返回值  ：无
********************************************/
void Ini_Lcd(void)
{
///////////////////////////////////////
	 LCD_RST_L;                            //复位LCD
     delay_1ms();                     //保证复位所需要的时间
     LCD_RST_H;                            //恢复LCD正常工作
 	 _NOP();
 //LCD_PSB_H;                            //设置LCD为8位并口通信
  ////////////////////////////////////////

 	LCD_CMDOut;    //液晶控制端口设置为输出

    delay_nms(500);
    Write_Cmd(0x30);   //基本指令集
    delay_1ms();
    Write_Cmd(0x02);   // 地址归位
    delay_1ms();
 Write_Cmd(0x0c);   //整体显示打开,游标关闭
    delay_1ms();
 Write_Cmd(0x01);   //清除显示
    delay_1ms();
 Write_Cmd(0x06);   //游标右移
    delay_1ms();
 Write_Cmd(0x80);   //设定显示的起始地址
}
/*******************************************
函数名称：Disp_HZ
功    能：控制液晶显示汉字
参    数：addr--显示位置的首地址
          pt--指向显示数据的指针
          num--显示字符个数
返回值  ：无
********************************************/
void Disp_HZ(unsigned char addr,const unsigned char * pt,unsigned char num)
{
    unsigned char i;

    Write_Cmd(addr);
    for(i = 0;i < (num*2);i++)
    {
        Write_Data(*(pt++));
        delay_50us();
    }
}

//***********在任意位置开始显示图画****************/
void LCD_draw(unsigned char X,unsigned char Y,unsigned char *tab,unsigned char wide,unsigned char high)
{  //Y确定开始行，X确定列（0-7）,tab是点阵字模数据
	unsigned int hang,lie;
	Write_Cmd(0x34);//绘图显示关
	if (wide%8==0) wide=wide/8;
	else wide=wide/8+1;
	for(hang=0;hang<high;hang++)
	{
		if(Y+hang>31)       //上下屏地址转换
        {
			Write_Cmd(0x80+Y+hang-32);//先写Y轴坐标
			Write_Cmd(0x88+X);//再写X轴坐标,(0x88-0x8f)
		}            //这里X只需写（0-7）
		else
		{
			Write_Cmd(0x80+Y+hang);//先写Y轴坐标
			Write_Cmd(0x80+X);//再写X轴坐标,(0x80-0x87)
		}
		for(lie=0;lie<wide;lie++)      //根据列宽逐行写入数据，宽占几字节，就写几字节
		{
			Write_Data(tab[hang*wide+lie]); //每行写入wide个字节数据
		}
	}

	Write_Cmd(0x36);//绘图显示开
	//Write_Cmd(0x30);//回到基本指令集
}

void drawpoint(volatile unsigned int X,volatile unsigned int Y,unsigned int Type)
{ 									//X，Y化点的坐标Type画点的类型：反相点，檫除点，画点
	unsigned int lie;
	volatile unsigned int TempData = 0;
	lie =BIT0 << (15 - (X % 16));
	X/=16;
	Write_Cmd(0x34);//绘图显示关,进入基本指令

	if(Y>31)       //上下屏地址转换
    {
		Write_Cmd(0x80+Y-32);//先写Y轴坐标
		Write_Cmd(0x88+X);//再写X轴坐标,(0x88-0x8f)
	}            //这里X只需写（0-7）
	else
	{
		Write_Cmd(0x80+Y);//先写Y轴坐标
		Write_Cmd(0x80+X);//再写X轴坐标,(0x80-0x87)
	}

   TempData = Read_Data();    //读出所画点所在字节的内容(16bit)

   switch (Type)            //对该字节进行相应操作
   {
       case Point_Draw:
          					TempData &= ~lie;
         					break;
//      case Graphic_Not:
//         					TempData ^= (1 << BX);
//          					break;
      case Point_Clear:
          					TempData |= lie;
							break;
	  default:				break;
   }

	if(Y>31)       //上下屏地址转换
	{
		Write_Cmd(0x80+Y-32);//先写Y轴坐标
		Write_Cmd(0x88+X);//再写X轴坐标,(0x88-0x8f)
	}            //这里X只需写（0-7）
	else
	{
		Write_Cmd(0x80+Y);//先写Y轴坐标
		Write_Cmd(0x80+X);//再写X轴坐标,(0x80-0x87)
	}
	Write_Data(TempData>>8);
	Write_Data(TempData);		//把修改后的字节送回LCD，达到画点目的(16bit)
	Write_Cmd(0x36);//绘图显示开
}

void sel_low()
{  P8DIR = 0xff;
  P8OUT=0Xff;
  //P1DIR = 0xff;
// P1OUT =0xff;
 P8OUT=0Xff;
// P1OUT =0x00;
}



