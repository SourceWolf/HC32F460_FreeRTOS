#include "bsp_rtc.h"

uint8_t bsp_rtc_init(void)
{
    stc_rtc_init_t stc_rtc_cfg;
    MEM_ZERO_STRUCT(stc_rtc_cfg);
    stc_rtc_cfg.enClkSource = RtcClkXtal32;//时钟源外部
    stc_rtc_cfg.enCompenEn = Disable;//补偿关闭
    stc_rtc_cfg.enCompenWay = RtcOutputCompenDistributed;
    stc_rtc_cfg.enPeriodInt = RtcPeriodIntOneSec;
    stc_rtc_cfg.enTimeFormat = RtcTimeFormat24Hour;
    RTC_Init(&stc_rtc_cfg);
    RTC_Cmd(Enable);
    return Ok;
}
uint8_t bsp_rtc_set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
    stc_rtc_date_time_t time; 
    time.u8Hour = hour;
    time.u8Minute = minute;
    time.u8Second = second;
    RTC_SetDateTime(RtcDataFormatDec,&time,Disable,Enable);
    return Ok;
}
uint8_t bsp_rtc_set_date(uint8_t year, uint8_t month, uint8_t day)
{
    stc_rtc_date_time_t time; 
    time.u8Year = year;
    time.u8Month = month;
    time.u8Day = day;
    RTC_SetDateTime(RtcDataFormatDec,&time,Enable,Disable);
    return Ok;
}

