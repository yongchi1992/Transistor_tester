/* --COPYRIGHT--,BSD
 * Copyright (c) 2011, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//
//debug.h - Macros for assisting debug of the driver library.
//
//Copyright (c) 2011 Texas Instruments Incorporated.  All rights reserved.
//Software License Agreement
//
//Texas Instruments (TI) is supplying this software for use solely and
//exclusively on TI's microcontroller products. The software is owned by
//TI and/or its suppliers, and is protected under applicable copyright
//laws. You may not combine this software with "viral" open-source
//software in order to form a larger program.
//
//THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
//NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
//NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
//CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
//DAMAGES, FOR ANY REASON WHATSOEVER.
//
//This is part of revision 0001 of the MSP430 Peripheral Driver Library.
//
//*****************************************************************************

#ifndef __DEBUG_H__
#define __DEBUG_H__

//*****************************************************************************
//
//Prototype for the function that is called when an invalid argument is passed
//to an API.  This is only used when doing a DEBUG build.
//
//*****************************************************************************
extern void __error__ (char *pcFilename, unsigned long ulLine);

//*****************************************************************************
//
//The ASSERT macro, which does the actual assertion checking.  Typically, this
//will be for procedure arguments.
//
//*****************************************************************************
#ifdef DEBUG
#define ASSERT(expr) {                                      \
        if (!(expr))                        \
        {                                  \
            __error__(__FILE__, __LINE__); \
        }                                  \
}
#else
#define ASSERT(expr)
#endif

#endif //__DEBUG_H__

