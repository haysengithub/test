/*
 * user_sntp.c
 *
 *  Created on: 2017年7月7日
 *      Author: Administrator
 */

#include "esp_common.h"
#include "user_ntputc.h"


//#include "ets_sys.h"
//#include "osapi.h"
//#include "user_interface.h"

#define SNTP_READ_INTERVAL	10000
#define RTC_MAGIC 126

typedef	struct	{
    uint64	time_acc;
	uint32	magic	;
	uint32	time_base;
}	RTC_TIMER_DEMO;
#if 0
static os_timer_t user_sntp_timer,rtc_test_t;


void ICACHE_FLASH_ATTR rtc_count(uint32_t time)
{   //uint32_t current_stamp;
    RTC_TIMER_DEMO rtc_time;
    uint32 rtc_t1;
    uint32 cal1;
	//os_printf("time sntpt : %d \r\n",time);
    system_rtc_mem_read(64, &rtc_time, sizeof(rtc_time));
    if(rtc_time.magic!=RTC_MAGIC)
    {
        rtc_time.magic = RTC_MAGIC;
        rtc_time.time_acc= 0;
        rtc_time.time_base = system_get_rtc_time();
        //os_printf("time base : %d \r\n",rtc_time.time_base);
    }
    rtc_t1 = system_get_rtc_time();
    cal1 = system_rtc_clock_cali_proc();
    //os_printf("cal 1 : %d.%d \r\n", ((cal1*1000)>>12)/1000,((cal1*1000)>>12)%1000 );
    rtc_time.time_acc += ( ((uint64)(rtc_t1 - rtc_time.time_base)) *( (uint64)((cal1*1000)>>12)) ) ;
    //os_printf("rtc time acc : %lld \r\n",rtc_time.time_acc);
    //os_printf("power on time : %lld.%02lld S \r\n", (rtc_time.time_acc/10000000)/100, (rtc_time.time_acc/10000000)%100);
    //os_printf("time: %s \n", sntp_get_real_time(time+((rtc_time.time_acc/10000000)/100)));
	rtc_time.time_base = rtc_t1;
    system_rtc_mem_write(64, &rtc_time, sizeof(rtc_time));

    //OLED_clear();
	OLED_show_str(0, 0, sntp_get_real_time(time+((rtc_time.time_acc/10000000)/100)), 2);     //such as   :20.08
	//current_stamp = sntp_get_current_timestamp();
	//OLED_show_str(0,4,sntp_get_real_time(current_stamp),2);
    
}


static void ICACHE_FLASH_ATTR
sntp_timer_cb(void *arg)
{
    uint32 current_stamp;
#if 1//for sntp test
   /*聽 聽 聽 聽 
聽 聽 聽 聽 闇�瑕佸垽鏂鍑芥暟sntp_get_current_timestamp杩斿洖鍊?聽 聽 聽 聽 濡傛灉璇ヨ繑鍥炲�间负闈?鐨勬暟琛ㄧず宸茬粡鎺ユ敹鍒版湇鍔″櫒鐨勬暟鎹簡
    */
    current_stamp = sntp_get_current_timestamp();
    os_printf("###sntp: %d, %s \n",current_stamp, sntp_get_real_time(current_stamp));
    if(current_stamp)
    {
        os_printf("###sntp:timezone=%d\n",sntp_get_timezone());
        os_timer_disarm(&user_sntp_timer);
        os_timer_disarm(&rtc_test_t);
        os_timer_setfn(&rtc_test_t,rtc_count,(void *)current_stamp);
        os_timer_arm(&rtc_test_t,10000,1);
		OLED_clear();
    }
#endif
}

#endif


void user_sntp_task(void *pvParameters)
{
    bool sntpGetFlag = FALSE;
	uint32 current_stamp;

	RTC_TIMER_DEMO rtc_time;
    uint32 rtc_t1;
    uint32 cal1;
	
	while(1)
	{
        if(FALSE==sntpGetFlag)
        {
            current_stamp = sntp_get_current_timestamp();
			if(0==current_stamp)
		    {
		        taskYIELD();
		        continue;
		    }

			sntpGetFlag =TRUE;
			OLED_clear();
		}

		system_rtc_mem_read(64, &rtc_time, sizeof(rtc_time));
	    if(rtc_time.magic!=RTC_MAGIC)
	    {
	        rtc_time.magic = RTC_MAGIC;
	        rtc_time.time_acc= 0;
	        rtc_time.time_base = system_get_rtc_time();
	        //os_printf("time base : %d \r\n",rtc_time.time_base);
	    }
	    rtc_t1 = system_get_rtc_time();
	    cal1 = system_rtc_clock_cali_proc();
	    //os_printf("cal 1 : %d.%d \r\n", ((cal1*1000)>>12)/1000,((cal1*1000)>>12)%1000 );
	    rtc_time.time_acc += ( ((uint64)(rtc_t1 - rtc_time.time_base)) *( (uint64)((cal1*1000)>>12)) ) ;
	    //os_printf("rtc time acc : %lld \r\n",rtc_time.time_acc);
	    //os_printf("power on time : %lld.%02lld S \r\n", (rtc_time.time_acc/10000000)/100, (rtc_time.time_acc/10000000)%100);
	    os_printf("time: %s \n", sntp_get_real_time(current_stamp+((rtc_time.time_acc/10000000)/100)));
		rtc_time.time_base = rtc_t1;
	    system_rtc_mem_write(64, &rtc_time, sizeof(rtc_time));

	    OLED_clear();
		//OLED_show_str(0, 0, sntp_get_real_time(current_stamp+((rtc_time.time_acc/10000000)/100)), 2);     //such as   :20.08
		//current_stamp = sntp_get_current_timestamp();
		//OLED_show_str(0,4,sntp_get_real_time(current_stamp),2);

        //os_printf("time base : %d \r\n",current_stamp+(uint32)((rtc_time.time_acc/10000000)/100));
		NTP_GMTconvert(current_stamp+(uint32)((rtc_time.time_acc/10000000)/100));

		unsigned char display_temp[24]={0};
		//2017-11-26 23:08:12 7
		sprintf(display_temp, "%d-%d-%d %d:%d:%d %d", 
			NTP_my_time.NTP_nYear,NTP_my_time.NTP_nMonth,NTP_my_time.NTP_nDay,
			NTP_my_time.NTP_nHour,NTP_my_time.NTP_nMin,NTP_my_time.NTP_nSec,
			NTP_my_time.NTP_DayIndex);
		OLED_show_str(0,0,display_temp,2);

		
        vTaskDelay(5000 / portTICK_RATE_MS);
		
	
		//taskYIELD();
	}
	
	vTaskDelete(NULL);	
}



void ICACHE_FLASH_ATTR
user_sntp_init(void)
{
	

	// 配置sntp服务器
    sntp_setservername(0,"0.cn.pool.ntp.org");
    sntp_setservername(1,"1.cn.pool.ntp.org");
    sntp_setservername(2,"2.cn.pool.ntp.org");
    // sntp初始化
    sntp_init();

    // 定时器初始化
    //os_timer_disarm(&user_sntp_timer);
    //os_timer_setfn(&user_sntp_timer, sntp_timer_cb , NULL);
    //os_timer_arm(&user_sntp_timer, SNTP_READ_INTERVAL, 1);

	if(pdPASS != xTaskCreate(user_sntp_task, "sntp_task", 512, NULL, 2, NULL))
	{
		os_printf("%s user_sntp_init failed.\n", __FUNCTION__);
	}
}
