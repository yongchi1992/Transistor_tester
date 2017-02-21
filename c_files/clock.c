/*
 * clock.c
 *
 *  Created on: 2013-7-18
 *      Author: Archimedes
 */
#include "rtc.h"
#include "sfr_sys.h"
#include "hw_memmap.h"
#include "wdt.h"
#include "gpio.h"
#include "ucs.h"
#include "clock.h"

void time_init (void)
{
    Calendar currentTime;

    GPIO_setAsPeripheralModuleFunctionInputPin(__MSP430_BASEADDRESS_PORT7_R__,
        GPIO_PORT_P7,
        GPIO_PIN0 + GPIO_PIN1
        );

    //Initialize LFXT1
    UCS_LFXT1Start(__MSP430_BASEADDRESS_UCS__,
        UCS_XT1_DRIVE3,
        UCS_XCAP_3
        );

    //Setup Current Time for Calendar
    currentTime.Seconds    = 0x56;
    currentTime.Minutes    = 0x16;
    currentTime.Hours      = 0x09;
    currentTime.DayOfWeek  = 0x07;
    currentTime.DayOfMonth = 0x21;
    currentTime.Month      = 0x07;
    currentTime.Year       = 0x2013;

    //Initialize Calendar Mode of RTC
    /*
     * Base Address of the RTC_A
     * Pass in current time, intialized above
     * Use BCD as Calendar Register Format
     */
    RTC_calendarInit(__MSP430_BASEADDRESS_RTC__,
        currentTime,
        RTC_FORMAT_BCD);

    //Specify an interrupt to assert every minute
    RTC_setCalendarEvent(__MSP430_BASEADDRESS_RTC__,
        RTC_CALENDAREVENT_MINUTECHANGE);

    //Enable interrupt for RTC Ready Status, which asserts when the RTC
    //Calendar registers are ready to read.
    //Also, enable interrupts for the Calendar alarm and Calendar event.
    RTC_enableInterrupt(__MSP430_BASEADDRESS_RTC__,
        RTCRDYIE + RTCTEVIE);

    //Start RTC Clock
    RTC_startClock(__MSP430_BASEADDRESS_RTC__);

//    //Enter LPM3 mode with interrupts enabled
//    __bis_SR_register(LPM3_bits + GIE);
//    __no_operation();
    _EINT();
}



