/**
************************************************************
* @file         user_main.c
* @brief        SOC鐗� 鍏ュ彛鏂囦欢
* @author       Gizwits
* @date         2016-11-21
* @version      V03000003
* @copyright    Gizwits
* 
* @note         鏈烘櫤浜�.鍙负鏅鸿兘纭欢鑰岀敓
*               Gizwits Smart Cloud  for Smart Products
*               閾炬帴|澧炲�贾祙寮�鏀緗涓珛|瀹夊叏|鑷湁|鑷敱|鐢熸��
*               www.gizwits.com
*
***********************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gagent_soc.h"
#include "user_devicefind.h"
#include "user_webserver.h"
#include "gizwits_product.h"
#include "driver/hal_key.h"
#include "driver/hal_infrared.h"
#include "driver/hal_motor.h"
#include "driver/hal_rgb_led.h"
#include "driver/hal_temp_hum.h"


#if ESP_PLATFORM
#include "user_esp_platform.h"
#endif

#ifdef SERVER_SSL_ENABLE
#include "ssl/cert.h"
#include "ssl/private_key.h"
#else
#ifdef CLIENT_SSL_ENABLE
unsigned char *default_certificate;
unsigned int default_certificate_len = 0;
unsigned char *default_private_key;
unsigned int default_private_key_len = 0;
#endif
#endif

/**@name Gizwits妯″潡鐩稿叧绯荤粺浠诲姟鍙傛暟
* @{
*/
#define userQueueLen    200                                                 ///< 娑堟伅闃熷垪鎬婚暱搴�
LOCAL os_event_t userTaskQueue[userQueueLen];                               ///< 娑堟伅闃熷垪
/**@} */

/**@name 鐢ㄦ埛瀹氭椂鍣ㄧ浉鍏冲弬鏁�
* @{
*/
#define USER_TIME_MS 100                                                    ///< 瀹氭椂鏃堕棿锛屽崟浣嶏細姣
LOCAL os_timer_t userTimer;                                                 ///< 鐢ㄦ埛瀹氭椂鍣ㄧ粨鏋勪綋
#define TH_TIMEOUT                              (1000 / USER_TIME_MS)       ///< Temperature and humidity detection minimum time
#define INF_TIMEOUT                             (500 / USER_TIME_MS)        ///< Infrared detection minimum time

/**@} */ 

/**@name 鎸夐敭鐩稿叧瀹氫箟
* @{
*/
#define GPIO_KEY_NUM                            2                           ///< 瀹氫箟鎸夐敭鎴愬憳鎬绘暟
#define KEY_0_IO_MUX                            PERIPHS_IO_MUX_GPIO0_U      ///< ESP8266 GPIO 鍔熻兘
#define KEY_0_IO_NUM                            0                           ///< ESP8266 GPIO 缂栧彿
#define KEY_0_IO_FUNC                           FUNC_GPIO0                  ///< ESP8266 GPIO 鍚嶇О
#define KEY_1_IO_MUX                            PERIPHS_IO_MUX_MTMS_U       ///< ESP8266 GPIO 鍔熻兘
#define KEY_1_IO_NUM                            14                          ///< ESP8266 GPIO 缂栧彿
#define KEY_1_IO_FUNC                           FUNC_GPIO14                 ///< ESP8266 GPIO 鍚嶇О
LOCAL key_typedef_t * singleKey[GPIO_KEY_NUM];                              ///< 瀹氫箟鍗曚釜鎸夐敭鎴愬憳鏁扮粍鎸囬拡
LOCAL keys_typedef_t keys;                                                  ///< 瀹氫箟鎬荤殑鎸夐敭妯″潡缁撴瀯浣撴寚閽�
/**@} */

/** 鐢ㄦ埛鍖哄綋鍓嶈澶囩姸鎬佺粨鏋勪綋*/
dataPoint_t currentDataPoint;

/**
* key1鎸夐敭鐭寜澶勭悊
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR key1ShortPress(void)
{
    os_printf("#### KEY1 short press ,Production Mode\n");
   // gizMSleep();
   // gizwitsSetMode(WIFI_PRODUCTION_TEST);

    if(0x01 == currentDataPoint.valueLED_OnOff)
    	{
    	   rgbControl(0, 0, 0);
    	}
    	else
    	{
    	   rgbControl(254, 0, 0);
    	}
    	currentDataPoint.valueLED_OnOff = !currentDataPoint.valueLED_OnOff;
}

/**
* key1鎸夐敭闀挎寜澶勭悊
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR key1LongPress(void)
{
    os_printf("#### key1 long press, default setup\n");
    gizMSleep();
    gizwitsSetMode(WIFI_RESET_MODE);
}

/**
* key2鎸夐敭鐭寜澶勭悊
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR key2ShortPress(void)
{
    os_printf("#### key2 short press, soft ap mode \n");
    rgbControl(254, 0, 0);
	gizwitsSetMode(WIFI_SOFTAP_MODE);
}



/**
* key2鎸夐敭闀挎寜澶勭悊
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR key2LongPress(void)
{
    os_printf("#### key2 long press, airlink mode\n");

    rgbControl(0, 250, 0);

    gizwitsSetMode(WIFI_AIRLINK_MODE);
}

/**
* 鎸夐敭鍒濆鍖�
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR keyInit(void)
{
    singleKey[0] = keyInitOne(KEY_0_IO_NUM, KEY_0_IO_MUX, KEY_0_IO_FUNC,
                                key1LongPress, key1ShortPress);
    singleKey[1] = keyInitOne(KEY_1_IO_NUM, KEY_1_IO_MUX, KEY_1_IO_FUNC,
                                key2LongPress, key2ShortPress);
    keys.singleKey = singleKey;
    keyParaInit(&keys);
}

/**
* 鐢ㄦ埛鏁版嵁鑾峰彇

* 姝ゅ闇�瑕佺敤鎴峰疄鐜伴櫎鍙啓鏁版嵁鐐逛箣澶栨墍鏈変紶鎰熷櫒鏁版嵁鐨勯噰闆�,鍙嚜琛屽畾涔夐噰闆嗛鐜囧拰璁捐鏁版嵁杩囨护绠楁硶
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR userTimerFunc(void)
{
//	uint8_t ret = 0;
//    uint8_t curTemperature = 0;
//    uint8_t curHumidity = 0;
//    uint8_t curIr = 0;
//    static uint8_t thCtime = 0;
//	static uint8_t irCtime = 0;
//
//	thCtime++;
//	irCtime++;
//
//	if(INF_TIMEOUT < irCtime)
//    {
//        irCtime = 0;
//
//        curIr = irUpdateStatus();
//		currentDataPoint.valueInfrared = curIr;
//	}
//
//	if(TH_TIMEOUT < thCtime)
//    {
//        thCtime = 0;
//
//        ret = dh11Read(&curTemperature, &curHumidity);
//
//        if(0 == ret)
//    	{
//    		currentDataPoint.valueTemperature = curTemperature;
//        	currentDataPoint.valueHumidity = curHumidity;
//    	}
//		else
//        {
//            os_printf("@@@@ dh11Read error ! \n");
//        }
//	}

    system_os_post(USER_TASK_PRIO_0, SIG_UPGRADE_DATA, 0);
}

/**
* @brief 鐢ㄦ埛鐩稿叧绯荤粺浜嬩欢鍥炶皟鍑芥暟

* 鍦ㄨ鍑芥暟涓敤鎴峰彲娣诲姞鐩稿簲浜嬩欢鐨勫鐞�
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR gizwitsUserTask(os_event_t * events)
{
    uint8_t i = 0;
    uint8_t vchar = 0;

    if(NULL == events)
    {
        os_printf("!!! gizwitsUserTask Error \n");
    }

    vchar = (uint8)(events->par);

    switch(events->sig)
    {
    case SIG_UPGRADE_DATA:
        gizwitsHandle((dataPoint_t *)&currentDataPoint);
        break;
    default:
        os_printf("---error sig! ---\n");
        break;
    }
}

/**
* @brief 绋嬪簭鍏ュ彛鍑芥暟

* 鍦ㄨ鍑芥暟涓畬鎴愮敤鎴风浉鍏崇殑鍒濆鍖�
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR user_init(void)
{
    uint32 system_free_size = 0;
	struct devAttrs attrs;

    wifi_station_set_auto_connect(1);
    wifi_set_sleep_type(NONE_SLEEP_T);//set none sleep mode
    espconn_tcp_set_max_con(10);
    uart_init_3(9600,115200);
    UART_SetPrintPort(1);
    os_printf( "---------------SDK version:%s--------------\n", system_get_sdk_version());
    os_printf( "system_get_free_heap_size=%d\n",system_get_free_heap_size());

    struct rst_info *rtc_info = system_get_rst_info();
    os_printf( "reset reason: %x\n", rtc_info->reason);
    if (rtc_info->reason == REASON_WDT_RST ||
        rtc_info->reason == REASON_EXCEPTION_RST ||
        rtc_info->reason == REASON_SOFT_WDT_RST)
    {
        if (rtc_info->reason == REASON_EXCEPTION_RST)
        {
            os_printf("Fatal exception (%d):\n", rtc_info->exccause);
        }
        os_printf( "epc1=0x%08x, epc2=0x%08x, epc3=0x%08x, excvaddr=0x%08x, depc=0x%08x\n",
                rtc_info->epc1, rtc_info->epc2, rtc_info->epc3, rtc_info->excvaddr, rtc_info->depc);
    }

    if (system_upgrade_userbin_check() == UPGRADE_FW_BIN1)
    {
        os_printf( "---UPGRADE_FW_BIN1---\n");
    }
    else if (system_upgrade_userbin_check() == UPGRADE_FW_BIN2)
    {
        os_printf( "---UPGRADE_FW_BIN2---\n");
    }

    //user init
    //rgb led init
    rgbGpioInit();
    rgbLedInit();

	//key init
    keyInit();
    
	//motor init
    motorInit();
    motorControl(MOTOR_SPEED_DEFAULT);
    
    //temperature and humidity init
    dh11Init();

    //Infrared init
    irInit(); 
    
    //gizwits InitSIG_UPGRADE_DATA
    gizwitsInit();

    system_os_task(gizwitsUserTask, USER_TASK_PRIO_0, userTaskQueue, userQueueLen);

    //user timer 
    os_timer_disarm(&userTimer);
    os_timer_setfn(&userTimer, (os_timer_func_t *)userTimerFunc, NULL);
    os_timer_arm(&userTimer, USER_TIME_MS, 1);

    os_printf("--- system_free_size = %d ---\n", system_get_free_heap_size());
}
