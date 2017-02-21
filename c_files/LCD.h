/*
 * LCD.h
 *
 *  Created on: 2013-7-13
 *      Author: Archimedes
 */

#ifndef LCD_H_
#define LCD_H_

void sel_low(void);
void delay_50us(void);
void delay_1ms(void);
void delay_nms(unsigned int n);
void Write_Cmd(unsigned char cod);
void Write_Data(unsigned char dat);
void Ini_Lcd(void);
void Disp_HZ(unsigned char addr,const unsigned char * pt,unsigned char num);
void LCD_draw(unsigned char X,unsigned char Y,unsigned char *tab,unsigned char wide,unsigned char high);
void drawpoint(volatile unsigned int X,volatile unsigned int Y,unsigned int Type);
//void Disp_ND(unsigned char addr,unsigned int  thickness);
//void Draw_TX(unsigned char Yaddr,unsigned char Xaddr,const unsigned char * dp) ;
//void Draw_PM(const unsigned char *ptr);

#endif /* LCD_H_ */
