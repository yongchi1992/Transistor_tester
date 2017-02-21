/*
 * AD.h
 *
 *  Created on: 2013-7-14
 *      Author: Archimedes
 */

#ifndef AD_H_
#define AD_H_

void adinit(void);
void GPIO_Init(void);
unsigned int WriteSPI(unsigned int data);
void ADS_7950(volatile int *ad,unsigned int n);

#endif /* AD_H_ */
