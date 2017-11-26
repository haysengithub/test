#ifndef _USER_LUNAR_H
#define _USER_LUNAR_H

#include "esp_common.h"


extern unsigned int LunarCalendarDay;

int LunarCalendar(int year,int month,int day); //公历转农历函数

#endif
