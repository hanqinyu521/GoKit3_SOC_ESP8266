/**
************************************************************
* @file         gizwits_protocol.h
* @brief        瀵瑰簲gizwits_protocol.c鐨勫ご鏂囦欢 (鍖呭惈浜у搧鐩稿叧瀹氫箟)
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
#ifndef _GIZWITS_PROTOCOL_H
#define _GIZWITS_PROTOCOL_H
#include <stdint.h>
#include "osapi.h"
#include "user_interface.h"
#include "user_webserver.h"

/**
* @name 鍗忚鐗堟湰鍙�
* @{
*/
#define PROTOCOL_VERSION              "00000004"
/**@} */

/**@name P0鍗忚鐗堟湰鍙�
* @{
*/
#define P0_VERSION                    "00000002"
/**@} */

/**@name Product Key (浜у搧鏍囪瘑鐮�)
* @{
*/
#define PRODUCT_KEY                   "xxxxxxxxxxxxxxxxxxxxxxxxxxx"
/**@} */

#define BUFFER_LEN_MAX                900
#define REPORT_TIME_MAX               6000          ///< 6s


/**@name 瀹氭椂鍣ㄧ浉鍏崇姸鎬�
* @{
*/
#define SOC_TIME_OUT                  10
#define MIN_INTERVAL_TIME             2000          ///< The minimum interval for sending
#define MAX_SOC_TIMOUT                1             ///< 1ms
#define TIM_REP_TIMOUT                600000        ///< 600s Regularly report
/**@} */

/**@name task鐩稿叧瀹氫箟
* @{
*/
#define SIG_ISSUED_DATA               0x01          ///< 鍗忚澶勭悊浠诲姟
#define SIG_PASSTHROUGH               0x02          ///< 鍗忚閫忎紶浠诲姟
#define SIG_IMM_REPORT                0x09          ///< 鍗忚绔嬪嵆涓婃姤浠诲姟
/**@} */

/** WiFi妯＄粍閰嶇疆鍙傛暟*/
typedef enum
{
  WIFI_RESET_MODE = 0x00,                           ///< WIFI妯＄粍澶嶄綅
  WIFI_SOFTAP_MODE,                                 ///< WIFI妯＄粍softAP妯″紡
  WIFI_AIRLINK_MODE,                                ///< WIFI妯＄粍AirLink妯″紡
  WIFI_PRODUCTION_TEST,                             ///< MCU璇锋眰WiFi妯＄粍杩涘叆浜ф祴妯″紡
  WIFI_NINABLE_MODE,                                ///< MCU璇锋眰妯＄粍杩涘叆鍙粦瀹氭ā寮�
}WIFI_MODE_TYPE_T;

/**@name 鏁版嵁鐐圭浉鍏冲畾涔�
* @{
*/
#define LED_ONOFF_BYTEOFFSET          0             ///< 鏁版嵁鐐筁ED_OnOff 鑺傚簭
#define LED_ONOFF_BITOFFSET           0             ///< 鏁版嵁鐐筁ED_OnOff bit搴�
#define LED_ONOFF_LEN                 1             ///< 鏁版嵁鐐筁ED_OnOff 瀛楁鍊�
                                                    
#define LED_COLOR_BYTEOFFSET          0             ///< 鏁版嵁鐐筁ED_Color 鑺傚簭
#define LED_COLOR_BITOFFSET           1             ///< 鏁版嵁鐐筁ED_Color bit搴�
#define LED_COLOR_LEN                 2             ///< 鏁版嵁鐐筁ED_Color 瀛楁鍊�
                                                     
#define INFRARED_BYTEOFFSET           6             ///< 鏁版嵁鐐笽nfrared 鑺傚簭
#define INFRARED_BITOFFSET            0             ///< 鏁版嵁鐐笽nfrared bit搴�
#define INFRARED_LEN                  1             ///< 鏁版嵁鐐笽nfrared 瀛楁鍊�
                                                    
#define ALERT_1_BYTEOFFSET            9             ///< 鏁版嵁鐐笰lert_1 鑺傚簭
#define ALERT_1_BITOFFSET             0             ///< 鏁版嵁鐐笰lert_1 bit搴�
#define ALERT_1_LEN                   1             ///< 鏁版嵁鐐笰lert_1 瀛楁鍊�
                                                    
#define ALERT_2_BYTEOFFSET            9             ///< 鏁版嵁鐐笰lert_2 鑺傚簭
#define ALERT_2_BITOFFSET             1             ///< 鏁版嵁鐐笰lert_2 bit搴�
#define ALERT_2_LEN                   1             ///< 鏁版嵁鐐笰lert_2 瀛楁鍊�
                                                    
#define FAULT_LED_BYTEOFFSET          10            ///< 鏁版嵁鐐笷ault_LED 鑺傚簭
#define FAULT_LED_BITOFFSET           0             ///< 鏁版嵁鐐笷ault_LED bit搴�
#define FAULT_LED_LEN                 1             ///< 鏁版嵁鐐笷ault_LED 瀛楁鍊�
                                                    
#define FAULT_MOTOR_BYTEOFFSET        10            ///< 鏁版嵁鐐笷ault_Motor 鑺傚簭
#define FAULT_MOTOR_BITOFFSET         1             ///< 鏁版嵁鐐笷ault_Motor bit搴�
#define FAULT_MOTOR_LEN               1             ///< 鏁版嵁鐐笷ault_Motor 瀛楁鍊�
                                                    
#define FAULT_TEMHUM_BYTEOFFSET       10            ///< 鏁版嵁鐐笷ault_TemHum 鑺傚簭
#define FAULT_TEMHUM_BITOFFSET        2             ///< 鏁版嵁鐐笷ault_TemHum bit搴�
#define FAULT_TEMHUM_LEN              1             ///< 鏁版嵁鐐笷ault_TemHum 瀛楁鍊�
                                                    
#define FAULT_IR_BYTEOFFSET           10            ///< 鏁版嵁鐐笷ault_IR 鑺傚簭
#define FAULT_IR_BITOFFSET            3             ///< 鏁版嵁鐐笷ault_IR bit搴�
#define FAULT_IR_LEN                  1             ///< 鏁版嵁鐐笷ault_IR 瀛楁鍊�


#define LED_R_RATIO                   1             ///< LED绾㈣壊鍊煎垎杈ㄧ巼
#define LED_R_ADDITION                0             ///< LED绾㈣壊鍊煎閲�
#define LED_R_MIN                     0             ///< LED绾㈣壊鍊兼渶灏忓��
#define LED_R_MAX                     254           ///< LED绾㈣壊鍊兼渶灏忓��
                                                     
#define LED_G_RATIO                   1             ///< LED缁胯壊鍊煎垎杈ㄧ巼
#define LED_G_ADDITION                0             ///< LED缁胯壊鍊煎閲�
#define LED_G_MIN                     0             ///< LED缁胯壊鍊兼渶灏忓��
#define LED_G_MAX                     254           ///< LED缁胯壊鍊兼渶澶у��
                                                     
#define LED_B_RATIO                   1             ///< LED钃濊壊鍊煎垎杈ㄧ巼
#define LED_B_ADDITION                0             ///< LED钃濊壊鍊煎閲�
#define LED_B_MIN                     0             ///< LED钃濊壊鍊兼渶灏忓��
#define LED_B_MAX                     254           ///< LED钃濊壊鍊兼渶澶у��
                                                    
#define MOTOR_SPEED_RATIO             1             ///< 鐢垫満杞�熷垎杈ㄧ巼
#define MOTOR_SPEED_ADDITION          -5            ///< 鐢垫満杞�熷閲�
#define MOTOR_SPEED_MIN               0             ///< 鐢垫満杞�熸渶灏忓��
#define MOTOR_SPEED_MAX               10            ///< 鐢垫満杞�熸渶澶у��
#define MOTOR_SPEED_DEFAULT           0             ///<  鐢垫満杞�熼粯璁ゅ��
                                                    
#define TEMPERATURE_RATIO             1             ///< 鐜娓╁害鍒嗚鲸鐜�
#define TEMPERATURE_ADDITION          -13           ///< 鐜娓╁害澧為噺
#define TEMPERATURE_MIN               0             ///< 鐜娓╁害鏈�灏忓��
#define TEMPERATURE_MAX               200           ///< 鐜娓╁害鏈�澶у��
                                                         
#define HUMIDITY_RATIO                1             ///< 鐜婀垮害鍒嗚鲸鐜�
#define HUMIDITY_ADDITION             0             ///< 鐜婀垮害澧為噺
#define HUMIDITY_MIN                  0             ///< 鐜婀垮害鏈�灏忓��
#define HUMIDITY_MAX                  100           ///< 鐜婀垮害鏈�澶у��

/**@} */

/** 鍙啓鍨嬫暟鎹偣 甯冨皵鍜屾灇涓惧彉閲� 鎵�鍗犲瓧鑺傚ぇ灏�*/

#define COUNT_W_BIT 1


/** 鍙鍨嬫暟鎹偣 甯冨皵鍜屾灇涓惧彉閲� 鎵�鍗犲瓧鑺傚ぇ灏�*/
#define COUNT_R_BIT 1

typedef enum
{
    LED_COLOR_VALUE0 = 0,                           ///< 鑷畾涔�
    LED_COLOR_VALUE1 = 1,                           ///< 榛勮壊
    LED_COLOR_VALUE2 = 2,                           ///< 绱壊
    LED_COLOR_VALUE3 = 3,                           ///< 绮夎壊
} LED_COLOR_ENUM_T;




/** 浜嬩欢缁撴瀯浣�*/
typedef enum
{
  WIFI_SOFTAP = 0x00,                               ///< WiFi SOFTAP 閰嶇疆浜嬩欢
  WIFI_AIRLINK,                                     ///< WiFi妯″潡 AIRLINK 閰嶇疆浜嬩欢
  WIFI_STATION,                                     ///< WiFi妯″潡 STATION 閰嶇疆浜嬩欢
  WIFI_OPEN_BINDING,                                ///< WiFi妯″潡寮�鍚粦瀹氫簨浠�
  WIFI_CLOSE_BINDING,                               ///< WiFi妯″潡鍏抽棴缁戝畾浜嬩欢
  WIFI_CON_ROUTER,                                  ///< WiFi妯″潡宸茶繛鎺ヨ矾鐢变簨浠�
  WIFI_DISCON_ROUTER,                               ///< WiFi妯″潡宸叉柇寮�杩炴帴璺敱浜嬩欢
  WIFI_CON_M2M,                                     ///< WiFi妯″潡宸茶繛鏈嶅姟鍣∕2M浜嬩欢
  WIFI_DISCON_M2M,                                  ///< WiFi妯″潡宸叉柇寮�杩炴湇鍔″櫒M2M浜嬩欢
  WIFI_OPEN_TESTMODE,                               ///< WiFi妯″潡寮�鍚祴璇曟ā寮忎簨浠�
  WIFI_CLOSE_TESTMODE,                              ///< WiFi妯″潡鍏抽棴娴嬭瘯妯″紡浜嬩欢
  WIFI_CON_APP,                                     ///< WiFi妯″潡杩炴帴APP浜嬩欢
  WIFI_DISCON_APP,                                  ///< WiFi妯″潡鏂紑APP浜嬩欢
  WIFI_RSSI,                                        ///< WiFi妯″潡淇″彿浜嬩欢
  TRANSPARENT_DATA,                                 ///< 閫忎紶浜嬩欢
  EVENT_LED_ONOFF,                                  ///< 绾㈢伅寮�鍏虫帶鍒朵簨浠�
  EVENT_LED_COLOR,                                  ///< LED缁勫悎棰滆壊鎺у埗浜嬩欢
  EVENT_LED_R,                                      ///< LED绾㈣壊鍊兼帶鍒朵簨浠�
  EVENT_LED_G,                                      ///< LED缁胯壊鍊兼帶鍒朵簨浠�
  EVENT_LED_B,                                      ///< LED钃濊壊鍊兼帶鍒朵簨浠�
  EVENT_MOTOR_SPEED,                                ///< 鐢垫満杞�熸帶鍒朵簨浠�
  EVENT_TYPE_MAX                                    ///< 鏋氫妇鎴愬憳鏁伴噺璁＄畻 (鐢ㄦ埛璇垹)
} EVENT_TYPE_T;

/** 鐢ㄦ埛鍖鸿澶囩姸鎬佺粨鏋勪綋*/
#pragma pack(1)
typedef struct {
  bool valueLED_OnOff;                              ///< 瀵瑰簲鏁版嵁鐐癸細LED_OnOff 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氬竷灏斿��
  LED_COLOR_ENUM_T valueLED_Color;                  ///< 瀵瑰簲鏁版嵁鐐癸細LED_Color 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭灇涓炬灇
  uint32_t valueLED_R;                              ///< 瀵瑰簲鏁版嵁鐐癸細LED_R 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint32_t valueLED_G;                              ///< 瀵瑰簲鏁版嵁鐐癸細LED_G 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint32_t valueLED_B;                              ///< 瀵瑰簲鏁版嵁鐐癸細LED_B 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  int32_t valueMotor_Speed;                         ///< 瀵瑰簲鏁版嵁鐐癸細Motor_Speed 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  bool valueInfrared;                               ///< 瀵瑰簲鏁版嵁鐐癸細Infrared 璇诲啓绫诲瀷锛氬彧璇� 鏁版嵁绫诲瀷锛氬竷灏斿��
  int32_t valueTemperature;                         ///< 瀵瑰簲鏁版嵁鐐癸細Temperature 璇诲啓绫诲瀷锛氬彧璇� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint32_t valueHumidity;                           ///< 瀵瑰簲鏁版嵁鐐癸細Humidity 璇诲啓绫诲瀷锛氬彧璇� 鏁版嵁绫诲瀷锛氭暟鍊�
  bool valueAlert_1;                                ///< 瀵瑰簲鏁版嵁鐐癸細Alert_1 璇诲啓绫诲瀷锛氭姤璀� 鏁版嵁绫诲瀷锛氬竷灏斿��
  bool valueAlert_2;                                ///< 瀵瑰簲鏁版嵁鐐癸細Alert_1 璇诲啓绫诲瀷锛氭姤璀� 鏁版嵁绫诲瀷锛氬竷灏斿��
  bool valueFault_LED;                              ///< 瀵瑰簲鏁版嵁鐐癸細Fault_LED 璇诲啓绫诲瀷锛氭晠闅� 鏁版嵁绫诲瀷锛氬竷灏斿��
  bool valueFault_Motor;                            ///< 瀵瑰簲鏁版嵁鐐癸細Fault_Motor 璇诲啓绫诲瀷锛氭晠闅� 鏁版嵁绫诲瀷锛氬竷灏斿��
  bool valueFault_TemHum;                           ///< 瀵瑰簲鏁版嵁鐐癸細Fault_TemHum 璇诲啓绫诲瀷锛氭晠闅� 鏁版嵁绫诲瀷锛氬竷灏斿��
  bool valueFault_IR;                               ///< 瀵瑰簲鏁版嵁鐐癸細Fault_IR 璇诲啓绫诲瀷锛氭晠闅� 鏁版嵁绫诲瀷锛氬竷灏斿��
} dataPoint_t;

/** 瀵瑰簲鍗忚鈥�4.10 WiFi妯＄粍鎺у埗璁惧鈥濅腑鐨勬爣蹇椾綅"attr_flags" */
typedef struct {
  uint8_t flagLED_OnOff:1;                          ///< 瀵瑰簲鏁版嵁鐐癸細LED_OnOff 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氬竷灏斿��
  uint8_t flagLED_Color:1;                          ///< 瀵瑰簲鏁版嵁鐐癸細LED_Color 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭灇涓炬灇
  uint8_t flagLED_R:1;                              ///< 瀵瑰簲鏁版嵁鐐癸細LED_R 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint8_t flagLED_G:1;                              ///< 瀵瑰簲鏁版嵁鐐癸細LED_G 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint8_t flagLED_B:1;                              ///< 瀵瑰簲鏁版嵁鐐癸細LED_B 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint8_t flagMotor_Speed:1;                        ///< 瀵瑰簲鏁版嵁鐐癸細Motor_Speed 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
} attrFlags_t;

/** 瀵瑰簲鍗忚鈥�4.10 WiFi妯＄粍鎺у埗璁惧鈥濅腑鐨勬暟鎹��"attr_vals" */
typedef struct {
  uint8_t wBitBuf[COUNT_W_BIT];                     ///< 鍙啓鍨嬫暟鎹偣 甯冨皵鍜屾灇涓惧彉閲� 鎵�鍗犲瓧鑺傚ぇ灏�
  uint8_t valueLED_R;                               ///< 瀵瑰簲鏁版嵁鐐癸細LED_R 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint8_t valueLED_G;                               ///< 瀵瑰簲鏁版嵁鐐癸細LED_G 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint8_t valueLED_B;                               ///< 瀵瑰簲鏁版嵁鐐癸細LED_B 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint16_t valueMotor_Speed;                        ///< 瀵瑰簲鏁版嵁鐐癸細Motor_Speed 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
} attrVals_t;

/** 瀵瑰簲鍗忚鈥�4.10 WiFi妯＄粍鎺у埗璁惧鈥濅腑鈥淧0鍗忚鍖衡�濈殑鏍囧織浣�"attr_flags" + 鏁版嵁鍊�"attr_vals" */
typedef struct {
    attrFlags_t attrFlags;                    
    attrVals_t  attrVals;
}gizwitsIssued_t;

/** 瀵瑰簲鍗忚鈥�4.9 璁惧MCU鍚慦iFi妯＄粍涓诲姩涓婃姤褰撳墠鐘舵�佲�濅腑鐨勮澶囩姸鎬�"dev_status" */
typedef struct {
  uint8_t wBitBuf[COUNT_W_BIT];                     ///< 鍙啓鍨嬫暟鎹偣 甯冨皵鍜屾灇涓惧彉閲� 鎵�鍗犲瓧鑺傚ぇ灏�
                                                    
  uint8_t valueLED_R;                               ///< 瀵瑰簲鏁版嵁鐐癸細LED_R 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint8_t valueLED_G;                               ///< 瀵瑰簲鏁版嵁鐐癸細LED_G 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint8_t valueLED_B;                               ///< 瀵瑰簲鏁版嵁鐐癸細LED_B 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint16_t valueMotor_Speed;                        ///< 瀵瑰簲鏁版嵁鐐癸細Motor_Speed 璇诲啓绫诲瀷锛氬彲鍐� 鏁版嵁绫诲瀷锛氭暟鍊�
                                                    
  uint8_t rBitBuf[COUNT_R_BIT];                     ///< 鍙鍨嬫暟鎹偣 甯冨皵鍜屾灇涓惧彉閲� 鎵�鍗犲瓧鑺傚ぇ灏�
                                                    
  uint8_t valueTemperature;                         ///< 瀵瑰簲鏁版嵁鐐癸細Temperature 璇诲啓绫诲瀷锛氬彧璇� 鏁版嵁绫诲瀷锛氭暟鍊�
  uint8_t valueHumidity;                            ///< 瀵瑰簲鏁版嵁鐐癸細Humidity 璇诲啓绫诲瀷锛氬彧璇� 鏁版嵁绫诲瀷锛氭暟鍊�
                                                    
  uint8_t valueAlert_1:1;                           ///< 瀵瑰簲鏁版嵁鐐癸細Alert_1 璇诲啓绫诲瀷锛氭姤璀� 鏁版嵁绫诲瀷锛氬竷灏斿��
  uint8_t valueAlert_2:1;                           ///< 瀵瑰簲鏁版嵁鐐癸細Alert_1 璇诲啓绫诲瀷锛氭姤璀� 鏁版嵁绫诲瀷锛氬竷灏斿��
                                                    
  uint8_t valuereserve_2:6;                         ///< 鏁版嵁浣嶈ˉ榻�
                                                    
  uint8_t valueFault_LED:1;                         ///< 瀵瑰簲鏁版嵁鐐癸細Fault_LED 璇诲啓绫诲瀷锛氭晠闅� 鏁版嵁绫诲瀷锛氬竷灏斿��
  uint8_t valueFault_Motor:1;                       ///< 瀵瑰簲鏁版嵁鐐癸細Fault_Motor 璇诲啓绫诲瀷锛氭晠闅� 鏁版嵁绫诲瀷锛氬竷灏斿��
  uint8_t valueFault_TemHum:1;                      ///< 瀵瑰簲鏁版嵁鐐癸細Fault_TemHum 璇诲啓绫诲瀷锛氭晠闅� 鏁版嵁绫诲瀷锛氬竷灏斿��
  uint8_t valueFault_IR:1;                          ///< 瀵瑰簲鏁版嵁鐐癸細Fault_IR 璇诲啓绫诲瀷锛氭晠闅� 鏁版嵁绫诲瀷锛氬竷灏斿��
                                                    
  uint8_t valuereserve_3:4;                         ///< 鏁版嵁浣嶈ˉ榻�
} devStatus_t;

/** 瀵瑰簲鍗忚鈥�4.9 璁惧MCU鍚慦iFi妯＄粍涓诲姩涓婃姤褰撳墠鐘舵�佲�濅腑鐨� 璁惧鐘舵�佷綅"dev_status"  */
typedef struct
{
    uint8_t action;
    devStatus_t devStatus;                          ///< 浣滅敤锛氱敤鏉ュ瓨鏀惧緟涓婃姤鐨勮澶囩姸鎬佹暟鎹�
}gizwitsReport_t;

/** P0 command 鍛戒护鐮�*/
typedef enum
{
    ACTION_CONTROL_DEVICE       = 0x01,             ///< 鍗忚4.10 WiFi妯＄粍鎺у埗璁惧 WiFi妯＄粍鍙戦��
    ACTION_READ_DEV_STATUS      = 0x02,             ///< 鍗忚4.8 WiFi妯＄粍璇诲彇璁惧鐨勫綋鍓嶇姸鎬� WiFi妯＄粍鍙戦��
    ACTION_READ_DEV_STATUS_ACK  = 0x03,             ///< 鍗忚4.8 WiFi妯＄粍璇诲彇璁惧鐨勫綋鍓嶇姸鎬� 璁惧MCU鍥炲
    ACTION_REPORT_DEV_STATUS    = 0X04,             ///< 鍗忚4.9 璁惧MCU鍚慦iFi妯＄粍涓诲姩涓婃姤褰撳墠鐘舵�� 璁惧MCU鍙戦��
    ACTION_W2D_TRANSPARENT_DATA = 0x05,             ///< WiFi鍒拌澶嘙CU閫忎紶
    ACTION_D2W_TRANSPARENT_DATA = 0x06,             ///< 璁惧MCU鍒癢iFi閫忎紶
    CMD_PRODUCTION_TEST         = 0x13,             ///< 鍛戒护瀛楋紝瀵瑰簲鍗忚锛�4.11 MCU璇锋眰WiFi妯＄粍杩涘叆浜ф祴妯″紡 涓� 璁惧MCU鍙戦��
    ACK_PRODUCTION_TEST         = 0x14,             ///< 鍛戒护瀛楋紝瀵瑰簲鍗忚锛�4.11 MCU璇锋眰WiFi妯＄粍杩涘叆浜ф祴妯″紡 涓� WiFi妯＄粍鍥炲
} action_type_t;

/** WiFi妯＄粍宸ヤ綔鐘舵��*/
typedef union
{
    uint16_t value;
    struct
    {
        uint16_t            softap:1;               ///< 琛ㄧずWiFi妯＄粍鎵�澶勭殑SOFTAP妯″紡鐘舵�侊紝绫诲瀷涓篵ool
        uint16_t            station:1;              ///< 琛ㄧずWiFi妯＄粍鎵�澶勭殑STATION妯″紡鐘舵�侊紝绫诲瀷涓篵ool
        uint16_t            onboarding:1;           ///< 琛ㄧずWiFi妯＄粍鎵�澶勭殑閰嶇疆鐘舵�侊紝绫诲瀷涓篵ool
        uint16_t            binding:1;              ///< 琛ㄧずWiFi妯＄粍鎵�澶勭殑缁戝畾鐘舵�侊紝绫诲瀷涓篵ool
        uint16_t            con_route:1;            ///< 琛ㄧずWiFi妯＄粍涓庤矾鐢卞櫒鐨勮繛鎺ョ姸鎬侊紝绫诲瀷涓篵ool
        uint16_t            con_m2m:1;              ///< 琛ㄧずWiFi妯＄粍涓庝簯绔痬2m鐨勭姸鎬侊紝绫诲瀷涓篵ool
        uint16_t            reserve1:2;             ///< 鏁版嵁浣嶈ˉ榻�
        uint16_t            rssi:3;                 ///< 琛ㄧず璺敱鐨勪俊鍙峰己搴︼紝绫诲瀷涓烘暟鍊�
        uint16_t            app:1;                  ///< 琛ㄧずWiFi妯＄粍涓嶢PP绔殑杩炴帴鐘舵�侊紝绫诲瀷涓篵ool
        uint16_t            test:1;                 ///< 琛ㄧずWiFi妯＄粍鎵�澶勭殑鍦烘祴妯″紡鐘舵�侊紝绫诲瀷涓篵ool
        uint16_t            reserve2:3;             ///< 鏁版嵁浣嶈ˉ榻�
    }types;
} wifi_status_t;

/** 浜嬩欢闃熷垪缁撴瀯浣� */
typedef struct {
    uint8_t num;                                    ///< 闃熷垪鎴愬憳涓暟
    uint8_t event[EVENT_TYPE_MAX];                  ///< 闃熷垪鎴愬憳浜嬩欢鍐呭
}eventInfo_t;

/** wifi淇″彿寮哄害鏁板�肩粨鏋勪綋 */
typedef struct {
    uint8_t rssi;                                   ///< WIFI淇″彿寮哄害
}moduleStatusInfo_t;

/** 鍗忚澶勭悊鐩稿叧缁撴瀯浣� */
typedef struct
{
    uint32_t timerMsCount;                          ///< 鏃堕棿璁℃暟(Ms)
    uint8_t transparentBuff[BUFFER_LEN_MAX];        ///< 閫忎紶鏁版嵁瀛樺偍鍖�
    uint32_t transparentLen;                        ///< 閫忎紶鏁版嵁闀垮害
    dataPoint_t gizCurrentDataPoint;                ///< 褰撳墠璁惧鐘舵�佹暟鎹�
    dataPoint_t gizLastDataPoint;                   ///< 涓婃涓婃姤鐨勮澶囩姸鎬佹暟鎹�
    gizwitsReport_t reportData;                     ///< 鍗忚涓婃姤瀹為檯鏁版嵁
    eventInfo_t issuedProcessEvent;                 ///< 鎺у埗浜嬩欢
}gizwitsProtocol_t;
#pragma pack()

/**@name Gizwits 鐢ㄦ埛API鎺ュ彛
* @{
*/
void gizwitsSetMode(uint8_t mode);
uint32_t gizwitsGetTimeStamp(void);
void gizwitsInit(void);
int8_t gizwitsHandle(dataPoint_t *dataPoint);
int32_t gizwitsPassthroughData(uint8_t * data, uint32_t len);
/**@} */

#endif
