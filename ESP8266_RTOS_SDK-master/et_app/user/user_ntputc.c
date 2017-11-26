#include "esp_common.h"
#include "user_ntputc.h"


#define NTP_UTC_BASE_YEAR 1970
#define NTP_MONTH_PER_YEAR 12
#define NTP_DAY_PER_YEAR 365
#define NTP_SEC_PER_DAY 86400
#define NTP_SEC_PER_HOUR 3600
#define NTP_SEC_PER_MIN 60

/* ÿ���µ����� */
const unsigned char NTP_g_day_per_mon[NTP_MONTH_PER_YEAR] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int day_diff(int year_start, int month_start, int day_start  
            , int year_end, int month_end, int day_end)  
{  
    int y2, m2, d2;  
    int y1, m1, d1;  
      
    m1 = (month_start + 9) % 12;  
    y1 = year_start - m1/10;  
    d1 = 365*y1 + y1/4 - y1/100 + y1/400 + (m1*306 + 5)/10 + (day_start - 1);  
  
    m2 = (month_end + 9) % 12;  
    y2 = year_end - m2/10;  
    d2 = 365*y2 + y2/4 - y2/100 + y2/400 + (m2*306 + 5)/10 + (day_end - 1);  
      
    return (d2 - d1);  
}

/*
 * ���ܣ�
 *     �ж��Ƿ�������
 * ������
 *     year����Ҫ�жϵ������
 *
 * ����ֵ��
 *     ���귵��1�����򷵻�0
 */
unsigned char NTP_applib_dt_is_leap_year(unsigned short year)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((year % 400) == 0) {
        return 1;
    } else if ((year % 100) == 0) {
        return 0;
    } else if ((year % 4) == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * ���ܣ�
 *     �õ�ÿ�����ж�����
 * ������
 *     month����Ҫ�õ��������·���
 *     year����������Ӧ�������
 *
 * ����ֵ��
 *     �����ж�����
 *
 */
unsigned char NTP_applib_dt_last_day_of_mon(unsigned char month, unsigned short year)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((month == 0) || (month > 12)) {
        return NTP_g_day_per_mon[1] + NTP_applib_dt_is_leap_year(year);
    }

    if (month != 2) {
        return NTP_g_day_per_mon[month - 1];
    } else {
        return NTP_g_day_per_mon[1] + NTP_applib_dt_is_leap_year(year);
    }
}

/*
 * ���ܣ�
 *     ���ݸ��������ڵõ���Ӧ������
 * ������
 *     year�����������
 *     month���������·�
 *     day������������
 *
 * ����ֵ��
 *     ��Ӧ����������0 - ������ ... 6 - ������
 */
unsigned char NTP_applib_dt_dayindex(unsigned short year, unsigned char month, unsigned char day)
{
    char century_code, year_code, month_code, day_code;
    int week = 0;

    century_code = year_code = month_code = day_code = 0;

    if (month == 1 || month == 2) {
        century_code = (year - 1) / 100;
        year_code = (year - 1) % 100;
        month_code = month + 12;
        day_code = day;
    } else {
        century_code = year / 100;
        year_code = year % 100;
        month_code = month;
        day_code = day;
    }

    /* ���ݲ��չ�ʽ�������� */
    week = year_code + year_code / 4 + century_code / 4 - 2 * century_code + 26 * ( month_code + 1 ) / 10 + day_code - 1;
    week = week > 0 ? (week % 7) : ((week % 7) + 7);

    return week;
}

/*
 * ���ܣ�
 *     ����UTCʱ����õ���Ӧ������
 * ������
 *     utc_sec��������UTCʱ���
 *     result��������Ľ��
 *     daylightSaving���Ƿ�������ʱ
 *
 * ����ֵ��
 *     ��
 */
void NTP_utc_sec_2_mytime(uint32_t utc_sec, NTP_mytime_struct *result, bool daylightSaving)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int sec, day;
    unsigned short y;
    unsigned char m;
    unsigned short d;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (daylightSaving) {
        utc_sec += NTP_SEC_PER_HOUR;
    }

    /* hour, min, sec */
    /* hour */
    sec = utc_sec % NTP_SEC_PER_DAY;
    result->NTP_nHour = sec / NTP_SEC_PER_HOUR;

    /* min */
    sec %= NTP_SEC_PER_HOUR;
    result->NTP_nMin = sec / NTP_SEC_PER_MIN;

    /* sec */
    result->NTP_nSec = sec % NTP_SEC_PER_MIN;

    /* year, month, day */
    /* year */
    /* year */
    day = utc_sec / NTP_SEC_PER_DAY;
    for (y = NTP_UTC_BASE_YEAR; day > 0; y++) {
        d = (NTP_DAY_PER_YEAR + NTP_applib_dt_is_leap_year(y));
        if (day >= d)
        {
            day -= d;
        }
        else
        {
            break;
        }
    }

    result->NTP_nYear = y;

    for (m = 1; m < NTP_MONTH_PER_YEAR; m++) {
        d = NTP_applib_dt_last_day_of_mon(m, y);
        if (day >= d) {
            day -= d;
        } else {
            break;
        }
    }

    result->NTP_nMonth = m;
    result->NTP_nDay = (unsigned char) (day + 1);
    /* ���ݸ��������ڵõ���Ӧ������ */
    result->NTP_DayIndex = NTP_applib_dt_dayindex(result->NTP_nYear, result->NTP_nMonth, result->NTP_nDay);
}



 NTP_mytime_struct NTP_my_time;
void NTP_GMTconvert(uint32_t argc)
{
   

		
		
    /////* �������UTCʱ������������ʱ������ʱ����ʱ�䣬�������Ҫת���ɱ���ʱ�����Ҫ���8Сʱ */
    ////NTP_utc_sec_2_mytime(argc + 8 * NTP_SEC_PER_HOUR, &NTP_my_time, false);
	NTP_utc_sec_2_mytime(argc , &NTP_my_time, false);	



}


