#ifndef _USER_NTPUTC_H_
#define _USER_NTPUTC_H_
/* Includes ------------------------------------------------------------------*/
#include "esp_common.h"



/* 自定义的时间结构体 */
typedef struct
{
    unsigned short NTP_nYear;
    unsigned char NTP_nMonth;
    unsigned char NTP_nDay;
    unsigned char NTP_nHour;
    unsigned char NTP_nMin;
    unsigned char NTP_nSec;
    unsigned char NTP_DayIndex; /* 0 = Sunday */
} NTP_mytime_struct;
extern NTP_mytime_struct NTP_my_time;

extern void NTP_GMTconvert(uint32_t argc);
extern unsigned char NTP_applib_dt_dayindex(unsigned short year, unsigned char month, unsigned char day);
extern unsigned int NTP_mytime_2_utc_sec(unsigned short year, unsigned char month, unsigned char day,unsigned char hour,unsigned char min,unsigned char sec, bool daylightSaving);

#endif /* __NTP_UTC_H */



