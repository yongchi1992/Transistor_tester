/*
 * KeyBoardDriver.c
 *
 *  Created on: 2013-7-13
 *      Author: Archimedes & Chen Yuzheng
 */
#include <msp430f5438a.h>
#include "KeyBoardDriver.h"

/***************����***************/
unsigned int Key_Pressed;      //�����Ƿ񱻰���:1--�ǣ�0--��
unsigned int Key_Val;          //��ż�ֵ
unsigned int Key_Long;			//�����Ƿ����
unsigned int Key_Map[] = {1 ,2 ,3 ,4 ,
						  5 ,6 ,7 ,8 ,
						  9 ,10,11,12,
						  13,14,15,16 }; //���ü����߼���ֵ���������ֵ��ӳ��
extern unsigned int daiji;
/***********************************/

/*******************************************
�������ƣ�Init_Keypad
���ܣ���ʼ��ɨ����̵�IO�˿�
��������
����ֵ����
********************************************/
void Init_Keypad(void)
{
    P7DIR |= 0xF0;       //P7.4~P7.7����Ϊ���״̬
    P2DIR &= 0x0F;		 //P2.4~P2.7����Ϊ����״̬
    P7OUT |= 0xF0;       // P7.4~P7.7����ߵ�ƽ
    Key_Pressed = 0;
    Key_Val = 0;
}
/*******************************************
�������ƣ�Check_Key
���ܣ�ɨ����̵�IO�˿ڣ���ü�ֵ
��������
����ֵ����
********************************************/
void Check_Key(void)
{
    unsigned int row,col,tmp1,tmp2;

    tmp1 = 0x80;
    for(row = 0;row < 4;row++)              //��ɨ��
    {
        P7OUT = 0xf0;                      //P7.4~P7.7���ȫ1
        P7OUT -= tmp1;                      //P7.4~p7.7�����λ����һ��Ϊ0
        tmp1 >>=1;
        if ((P2IN & 0xf0) < 0xf0)           //�Ƿ�P2IN��P2.4~P2.7����һλΪ0
        {
            tmp2 = 0x10;                         // tmp2���ڼ�����һλΪ0
            for(col = 0;col < 4;col++)              // �м��
            {
                if((P2IN & tmp2) == 0x00)           // �Ƿ��Ǹ���,����0Ϊ��
                {
                    Key_Val = Key_Map[row * 4 + col];  // ��ȡ��ֵ
                    return;                         // �˳�ѭ��
                }
                tmp2 <<= 1;                        // tmp2����1λ
            }
        }
    }
}
/*******************************************
�������ƣ�delay_15ms
���ܣ���ʱԼ15ms�������������
��������
����ֵ����
********************************************/
void delay_15ms()
{
    volatile unsigned int tmp;
    for(tmp = 60000;tmp > 0;tmp--) _NOP();
    for(tmp = 30000;tmp > 0;tmp--) _NOP();
}
/*******************************************
�������ƣ�Key_Event
��    �ܣ���ⰴ��������ȡ��ֵ
��    ������
����ֵ  ����
********************************************/
void Key_Event(volatile unsigned int *key,volatile unsigned int *key_long)
{
    volatile unsigned int tmp;

    P7OUT &= 0x00;              // ����P1OUTȫΪ0���ȴ���������
    tmp = P2IN;                 // ��ȡP2IN
    if ((tmp & 0xf0) < 0xf0) 	//����м�����
    {
    	delay_15ms();   //��������
        Check_Key();            // ����check_Key(),��ȡ��ֵ
        daiji=0;
        P3OUT&=~BIT0;
        		P3OUT&=~BIT5;

        Write_Cmd(0x0C);
    }
    else if ((tmp & 0xf0) == 0xf0) //��������Ѿ��ͷ�
    {
        Key_Val = 0;
        delay_15ms();   //ƽ��ʱ��
    }
    else delay_15ms();  //ƽ��ʱ��

    if (*key == Key_Val)
    	{
    	*key_long += 1;

    	}
    else *key_long = 0;
    *key = Key_Val;
}



