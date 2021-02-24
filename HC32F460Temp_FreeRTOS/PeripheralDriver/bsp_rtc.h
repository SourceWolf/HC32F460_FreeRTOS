#ifndef BSP_RTC_H
#define BSP_RTC_H
#include "hc32_ddl.h"
uint8_t bsp_rtc_init(void);
uint8_t bsp_rtc_set_time(uint8_t hour, uint8_t minute, uint8_t second);
uint8_t bsp_rtc_set_date(uint8_t year, uint8_t month, uint8_t day);
#endif
