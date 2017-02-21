/*
 * KeyBoardDriver.h
 *
 *  Created on: 2013-7-13
 *      Author: Archimedes & Chen Yuzheng
 */

#ifndef KEYBOARDDRIVER_H_
#define KEYBOARDDRIVER_H_

void Init_Keypad(void);
void Check_Key(void);
void Key_Event(volatile unsigned int *key,volatile unsigned int *key_long);
void delay_15ms(void);

#endif /* KEYBOARDDRIVER_H_ */
