/*
 * user_sntp.c
 *
 *  Created on: 2017Äê7ÔÂ7ÈÕ
 *      Author: Administrator
 */

#include "esp_common.h"


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

static os_timer_t user_sntp_timer,rtc_test_t;


void ICACHE_FLASH_ATTR rtc_count(void *arg)
{   //uint32_t current_stamp;
    uint32_t time = (uint32_t)arg;
    RTC_TIMER_DEMO rtc_time;
    uint32 rtc_t1;
    uint32 cal1;
	os_printf("time sntpt : %d \r\n",time);
    system_rtc_mem_read(64, &rtc_time, sizeof(rtc_time));
    if(rtc_time.magic!=RTC_MAGIC)
    {
        rtc_time.magic = RTC_MAGIC;
        rtc_time.time_acc= 0;
        rtc_time.time_base = system_get_rtc_time();
        os_printf("time base : %d \r\n",rtc_time.time_base);
    }
    rtc_t1 = system_get_rtc_time();
    cal1 = system_rtc_clock_cali_proc();
    os_printf("cal 1 : %d.%d \r\n", ((cal1*1000)>>12)/1000,((cal1*1000)>>12)%1000 );
    rtc_time.time_acc += ( ((uint64)(rtc_t1 - rtc_time.time_base)) *( (uint64)((cal1*1000)>>12)) ) ;
    os_printf("rtc time acc : %lld \r\n",rtc_time.time_acc);
    os_printf("power on time : %lld.%02lld S \r\n", (rtc_time.time_acc/10000000)/100, (rtc_time.time_acc/10000000)%100);
    os_printf("time: %s \n", sntp_get_real_time(time+((rtc_time.time_acc/10000000)/100)));
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
   /*Â  Â  Â  Â  
Â  Â  Â  Â  éœ€è¦åˆ¤æ–­è¯¥å‡½æ•°sntp_get_current_timestampè¿”å›å€¼
Â  Â  Â  Â  å¦‚æœè¯¥è¿”å›å€¼ä¸ºé0çš„æ•°è¡¨ç¤ºå·²ç»æ¥æ”¶åˆ°æœåŠ¡å™¨çš„æ•°æ®äº†
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

void ICACHE_FLASH_ATTR
user_sntp_init(void)
{
	

	// ÅäÖÃsntp·şÎñÆ÷
    sntp_setservername(0,"0.cn.pool.ntp.org");
    sntp_setservername(1,"1.cn.pool.ntp.org");
    sntp_setservername(2,"2.cn.pool.ntp.org");
    // sntp³õÊ¼»¯
    sntp_init();

    // ¶¨Ê±Æ÷³õÊ¼»¯
    os_timer_disarm(&user_sntp_timer);
    os_timer_setfn(&user_sntp_timer, sntp_timer_cb , NULL);
    os_timer_arm(&user_sntp_timer, SNTP_READ_INTERVAL, 1);
}
