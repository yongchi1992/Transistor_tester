/*
 * LCD.c
 *
 *  Created on: 2013-7-13
 *      Author: Archimedes &Chen Yuzheng
 */
#include <msp430f5438a.h>
#include "LCD.h"


//extern const unsigned char shuzi_table[];

#define LCD_DataIn    P8DIR=0x00    //���ݿڷ�������Ϊ����
#define LCD_DataOut   P8DIR=0xFF    //���ݿڷ�������Ϊ���
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
�������ƣ�delay_1us
��    �ܣ���ʱԼ1us��ʱ��
��    ������
����ֵ  ����
********************************************/
void delay_50us(void)
{
	unsigned int i;

	for(i = 50;i > 0;i--)  _NOP();
}

/*******************************************
�������ƣ�delay_1ms
��    �ܣ���ʱԼ1ms��ʱ��
��    ������
����ֵ  ����
********************************************/
void delay_1ms(void)
{
	unsigned int i;

	for(i = 500;i > 0;i--)  _NOP();
}
/*******************************************
�������ƣ�delay_nms
��    �ܣ���ʱN��1ms��ʱ��
��    ����n--��ʱ����
����ֵ  ����
********************************************/
void delay_nms(unsigned int n)
{
    unsigned int i;

    for(i = n;i > 0;i--)    delay_1ms();
}
/*******************************************
�������ƣ�Write_Cmd
��    �ܣ���Һ����д��������
��    ����cmd--��������
����ֵ  ����
********************************************/
void Write_Cmd(unsigned char cmd)
{
    unsigned char lcdtemp = 0;

    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do                       //��æ
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
�������ƣ�Write_Data
��    �ܣ���Һ����д��ʾ����
��    ����dat--��ʾ����
����ֵ  ����
********************************************/
void  Write_Data(unsigned char dat)
{
    unsigned char lcdtemp = 0;

    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do                       //��æ
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
    do                       //��æ
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
	LCD_EN_H;        //E�������߶�����Ч
	delay_50us();
	mcu_data=LCD2MCU_Data;
	mcu_data<<=8;
	LCD_EN_L;
	delay_50us();
	LCD_EN_H;        //E�������߶�����Ч
	delay_50us();
	mcu_data+=LCD2MCU_Data;
	LCD_EN_L;
	return(mcu_data);
}
/*******************************************
�������ƣ�Ini_Lcd
��    �ܣ���ʼ��Һ��ģ��
��    ������
����ֵ  ����
********************************************/
void Ini_Lcd(void)
{
///////////////////////////////////////
	 LCD_RST_L;                            //��λLCD
     delay_1ms();                     //��֤��λ����Ҫ��ʱ��
     LCD_RST_H;                            //�ָ�LCD��������
 	 _NOP();
 //LCD_PSB_H;                            //����LCDΪ8λ����ͨ��
  ////////////////////////////////////////

 	LCD_CMDOut;    //Һ�����ƶ˿�����Ϊ���

    delay_nms(500);
    Write_Cmd(0x30);   //����ָ�
    delay_1ms();
    Write_Cmd(0x02);   // ��ַ��λ
    delay_1ms();
 Write_Cmd(0x0c);   //������ʾ��,�α�ر�
    delay_1ms();
 Write_Cmd(0x01);   //�����ʾ
    delay_1ms();
 Write_Cmd(0x06);   //�α�����
    delay_1ms();
 Write_Cmd(0x80);   //�趨��ʾ����ʼ��ַ
}
/*******************************************
�������ƣ�Disp_HZ
��    �ܣ�����Һ����ʾ����
��    ����addr--��ʾλ�õ��׵�ַ
          pt--ָ����ʾ���ݵ�ָ��
          num--��ʾ�ַ�����
����ֵ  ����
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

//***********������λ�ÿ�ʼ��ʾͼ��****************/
void LCD_draw(unsigned char X,unsigned char Y,unsigned char *tab,unsigned char wide,unsigned char high)
{  //Yȷ����ʼ�У�Xȷ���У�0-7��,tab�ǵ�����ģ����
	unsigned int hang,lie;
	Write_Cmd(0x34);//��ͼ��ʾ��
	if (wide%8==0) wide=wide/8;
	else wide=wide/8+1;
	for(hang=0;hang<high;hang++)
	{
		if(Y+hang>31)       //��������ַת��
        {
			Write_Cmd(0x80+Y+hang-32);//��дY������
			Write_Cmd(0x88+X);//��дX������,(0x88-0x8f)
		}            //����Xֻ��д��0-7��
		else
		{
			Write_Cmd(0x80+Y+hang);//��дY������
			Write_Cmd(0x80+X);//��дX������,(0x80-0x87)
		}
		for(lie=0;lie<wide;lie++)      //�����п�����д�����ݣ���ռ���ֽڣ���д���ֽ�
		{
			Write_Data(tab[hang*wide+lie]); //ÿ��д��wide���ֽ�����
		}
	}

	Write_Cmd(0x36);//��ͼ��ʾ��
	//Write_Cmd(0x30);//�ص�����ָ�
}

void drawpoint(volatile unsigned int X,volatile unsigned int Y,unsigned int Type)
{ 									//X��Y���������Type��������ͣ�����㣬�߳��㣬����
	unsigned int lie;
	volatile unsigned int TempData = 0;
	lie =BIT0 << (15 - (X % 16));
	X/=16;
	Write_Cmd(0x34);//��ͼ��ʾ��,�������ָ��

	if(Y>31)       //��������ַת��
    {
		Write_Cmd(0x80+Y-32);//��дY������
		Write_Cmd(0x88+X);//��дX������,(0x88-0x8f)
	}            //����Xֻ��д��0-7��
	else
	{
		Write_Cmd(0x80+Y);//��дY������
		Write_Cmd(0x80+X);//��дX������,(0x80-0x87)
	}

   TempData = Read_Data();    //���������������ֽڵ�����(16bit)

   switch (Type)            //�Ը��ֽڽ�����Ӧ����
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

	if(Y>31)       //��������ַת��
	{
		Write_Cmd(0x80+Y-32);//��дY������
		Write_Cmd(0x88+X);//��дX������,(0x88-0x8f)
	}            //����Xֻ��д��0-7��
	else
	{
		Write_Cmd(0x80+Y);//��дY������
		Write_Cmd(0x80+X);//��дX������,(0x80-0x87)
	}
	Write_Data(TempData>>8);
	Write_Data(TempData);		//���޸ĺ���ֽ��ͻ�LCD���ﵽ����Ŀ��(16bit)
	Write_Cmd(0x36);//��ͼ��ʾ��
}

void sel_low()
{  P8DIR = 0xff;
  P8OUT=0Xff;
  //P1DIR = 0xff;
// P1OUT =0xff;
 P8OUT=0Xff;
// P1OUT =0x00;
}



