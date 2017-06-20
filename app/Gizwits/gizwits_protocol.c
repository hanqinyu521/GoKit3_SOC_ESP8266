/**
************************************************************
* @file         gizwits_protocol.c
* @brief        Gizwits协议相关文件 (SDK API 接口函数定义)
* @author       Gizwits
* @date         2016-11-21
* @version      V03000003
* @copyright    Gizwits
* 
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/
#include "gizwits_protocol.h"
#include "gagent_soc.h"
#include "gizwits_product.h"
#include "mem.h"
#include "driver/hal_infrared.h"
#include "driver/hal_motor.h"
#include "driver/hal_rgb_led.h"
#include "driver/hal_temp_hum.h"

/** 协议全局变量 **/
gizwitsProtocol_t gizwitsProtocol;

/**@name Gagent模块相关系统任务参数
* @{
*/
#define TaskQueueLen    200                                                 ///< 消息队列总长度
LOCAL  os_event_t   TaskQueue[TaskQueueLen];                                ///< 消息队列
/**@} */

/** 系统任务相关参数 */
#define gizwitsTaskQueueLen    200                              ///< gizwits协议消息队列
LOCAL os_event_t gizwitsTaskQueue[gizwitsTaskQueueLen];         ///< gizwits协议消息队列长度

/** 定时器相关参数 */ 
LOCAL os_timer_t gizTimer; 

/**
* @brief 读取系统时间毫秒计时数
* @param none
* @return 系统时间毫秒数
*/
uint32_t ICACHE_FLASH_ATTR gizGetTimerCount(void)
{
    return gizwitsProtocol.timerMsCount;
}

/**
* @brief 延时函数
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR gizMSleep(void)
{
    int i;
    
    for(i = 0; i < 20; i++)
    {
        os_delay_us(50000);
    }
}

/**
* @brief 查询系统时间，单位：毫秒
* @param none
* @return 系统时间，单位：毫秒
*/
uint32 ICACHE_FLASH_ATTR gizTimeMs(void)
{
    return (system_get_time() / 1000); 
}

/**
* @brief 获得与上次查询的间隔时间，单位：毫秒
* @param[in] lastRpMs 传入上次查询的时间
* @return 返回当前与上次查询之间的间隔时间，单位：毫秒
*/
uint32 ICACHE_FLASH_ATTR gizGetIntervalsMs(uint32 lastRpMs)
{

    uint32 intervals_ms = 0;

    if(0 < (gizTimeMs() - lastRpMs))
    {
        intervals_ms = gizTimeMs() - lastRpMs;
    }
    else
    {
        intervals_ms = (0xffffffff - lastRpMs) + gizTimeMs();
    }

    return (intervals_ms);
}

/**
* @brief 16位数据字节序转换
*
* @param [in] value : 需要转换的数据
*
* @return  tmp_value: 转换后的数据
*/
static uint16_t ICACHE_FLASH_ATTR gizProtocolExchangeBytes(uint16_t value)
{
    uint16_t    tmp_value;
    uint8_t     *index_1, *index_2;

    index_1 = (uint8_t *)&tmp_value;
    index_2 = (uint8_t *)&value;

    *index_1 = *(index_2+1);
    *(index_1+1) = *index_2;

    return tmp_value;
}



/**
* @brief 32位数据字节序转换
*
* @param [in] value : 需要转换的数据
*
* @return  tmp_value: 转换后的数据
*/
static uint32_t ICACHE_FLASH_ATTR gizExchangeWord(uint32_t  value)
{
    return ((value & 0x000000FF) << 24) |
        ((value & 0x0000FF00) << 8) |
        ((value & 0x00FF0000) >> 8) |
        ((value & 0xFF000000) >> 24) ;
}



/**
* @brief 数组缓冲区网络字节序转换
*
* @param [in] *buf     : buf地址
* @param [in] dataLen  : 字节长度
*
* @return 正确 : 0 
          失败 : -1
*/
static int8_t ICACHE_FLASH_ATTR gizByteOrderExchange(uint8_t *buf,uint32_t dataLen)
{
    uint32_t i = 0;
    uint8_t preData = 0;
    uint8_t aftData = 0;

    if(NULL == buf)
    {
        os_printf("gizByteOrderExchange Error , Illegal Param\n");
        return -1;
    }

    for(i = 0;i<dataLen/2;i++)
    {
        preData = buf[i];
        aftData = buf[dataLen - i - 1];
        buf[i] = aftData;
        buf[dataLen - i - 1] = preData;
    }

    return 0;
}

/**
* @brief 转化为协议中的x值及实际通讯传输的值
*
* @param [in] ratio    : 修正系数k
* @param [in] addition : 增量m
* @param [in] preValue : 作为协议中的y值, 是App UI界面的显示值
*
* @return aft_value    : 作为协议中的x值, 是实际通讯传输的值
*/
static uint32_t ICACHE_FLASH_ATTR gizY2X(uint32_t ratio, int32_t addition, int32_t preValue)
{
    uint32_t aftValue = 0;

    //x=(y - m)/k
    aftValue = ((preValue - addition) / ratio);

    return aftValue;
}


/**
* @brief 转化为协议中的y值及App UI界面的显示值
*
* @param [in] ratio    : 修正系数k
* @param [in] addition : 增量m
* @param [in] preValue : 作为协议中的x值, 是实际通讯传输的值
*
* @return aftValue : 作为协议中的y值, 是App UI界面的显示值
*/
static int32_t ICACHE_FLASH_ATTR gizX2Y(uint32_t ratio, int32_t addition, uint32_t preValue)
{
    int32_t aftValue = 0;

    //y=k * x + m
    aftValue = (preValue * ratio + addition);

    return aftValue;
}


/**
* @brief 转化为协议中的x值及实际通讯传输的值,只用作对浮点型数据做处理
*
* @param [in] ratio    : 修正系数k
* @param [in] addition : 增量m
* @param [in] preValue : 作为协议中的y值, 是App UI界面的显示值
*
* @return  aft_value : 作为协议中的x值, 是实际通讯传输的值
*/
static uint32_t ICACHE_FLASH_ATTR gizY2XFloat(float ratio, float addition, float preValue)
{
    uint32_t aftValue = 0;

    //x=(y - m)/k
    aftValue = ((preValue - addition) / ratio);

    return aftValue;
}

/**
* @brief 转化为协议中的y值及App UI界面的显示值,只用作对浮点型数据做处理
*
* @param [in] ratio : 修正系数k
* @param [in] addition : 增量m
* @param [in] preValue : 作为协议中的x值, 是实际通讯传输的值
*
* @return : 作为协议中的y值, 是App UI界面的显示值
*/
static float ICACHE_FLASH_ATTR gizX2YFloat(float ratio, float addition, uint32_t preValue)
{
    float aftValue = 0;

    //y=k * x + m
    aftValue = (preValue * ratio + addition);

    return aftValue;
}

/**
* @brief 数据点跨字节判断
*
* @param [in] bitOffset     : 位偏移
* @param [in] bitLen        : 占用位长度 
*
* @return 未跨字节 : 0 
            跨字节 : 1
*/
static uint8_t ICACHE_FLASH_ATTR gizAcrossByteJudge(uint32_t bitOffset,uint32_t bitLen)
{
    if((0 == bitOffset)||(0 == bitOffset%8))
    {
        if(bitLen <= 8)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if(8 - bitOffset%8 >= bitLen)
        {
            return 0;
        }
        else
        {
            return 1; 
        }
    }
}

/**
* @brief bool和enum类型数据点数据解压
*
* @param [in] byteOffset    : 字节偏移
* @param [in] bitOffset     : 位偏移
* @param [in] bitLen        : 占用位长度
* @param [in] *arrayAddr    : 数组地址
* @param [in] arrayLen      : 数组长度
*
* @return destValue         : 解压后结果数据,-1表示错误返回
*/
static int32_t ICACHE_FLASH_ATTR gizDecompressionValue(uint32_t byteOffset,uint32_t bitOffset,uint32_t bitLen,uint8_t *arrayAddr,uint32_t arrayLen)
{
    uint8_t ret = 0;
    uint8_t highBit = 0 ,lowBit = 0;
    uint8_t destBufTemp[COUNT_W_BIT];
    int32_t destValue = 0;
    
    if(NULL == arrayAddr || 0 == arrayLen)
    {
        os_printf("gizDecompressionValue Error , Illegal Param\n");
        return -1;
    }

    memcpy(destBufTemp,arrayAddr,arrayLen);
    if(arrayLen > 1)// Judge Byte order conversion
    {
        if(-1 == gizByteOrderExchange(destBufTemp,arrayLen))
        {
            os_printf("gizDecompressionValue gizByteOrderExchange Error \n");
            return -1;
        }
    }
    ret = gizAcrossByteJudge(bitOffset,bitLen);//Judge Byte Step
    if(0 == ret)
    {
        destValue |= ((destBufTemp[byteOffset] >> (bitOffset%8)) & (0xff >> (8 - bitLen)));
    }
    else if(1 == ret)
    {
        /* 暂时只支持最多跨2字节 */
        highBit = destBufTemp[byteOffset + 1]& (0xFF >> (8-(bitLen-(8-bitOffset%8))));
        lowBit = destBufTemp[byteOffset]>> (bitOffset%8);
        destValue |=  (highBit << (8-bitOffset%8));
        destValue |= lowBit;
    }
    //os_printf("Return gizDecompressionValue = 0x%08x \n",destValue);
    return destValue;
}

/**
* @brief bool和enum类型数据点数据压缩
*
* @param [in] byteOffset    : 字节偏移
* @param [in] bitOffset     : 位偏移
* @param [in] bitLen        : 占用位长度
* @param [out] *arrayAddr   : 数组地址
* @param [in] srcData       : 原始数据
*
* @return : 0,正确返回;-1，错误返回
*/
static int32_t ICACHE_FLASH_ATTR gizCompressValue(uint32_t byteOffset,uint32_t bitOffset,uint32_t bitLen,uint8_t *bufAddr,uint32_t srcData)
{
    uint8_t highBit = 0 ,lowBit = 0;
    uint8_t ret = 0;

    if(NULL == bufAddr)
    {
        os_printf("gizCompressValue Error , Illegal Param\n");
        return -1;
    }

    ret = gizAcrossByteJudge(bitOffset,bitLen);
    if(0 == ret)
    {
        bufAddr[byteOffset] |= (((uint8_t)srcData)<<(bitOffset%8));
    }
    else if(1 == ret)
    {
        /* 暂时支持最多跨两字节的压缩 */ 
        highBit = ((uint8_t)srcData)>>(8-bitOffset%8);
        lowBit = (uint8_t)srcData & (0xFF >> (8-bitOffset%8));
        bufAddr[byteOffset + 1] |=  highBit;
        bufAddr[byteOffset] |= (lowBit<<(bitOffset%8));
    }

    return 0;
}

/**
* @brief 用户数据点数据转换为机智云上报数据点数据
*
* @param [in]  dataPoints           : 用户数据点数据地址
* @param [out] devStatusPtr         : 机智云上报数据点数据地址
*
* @return 0,正确返回;-1，错误返回
*/
static int8_t ICACHE_FLASH_ATTR gizDataPoints2ReportData(dataPoint_t *dataPoints , devStatus_t *devStatusPtr)
{
    if((NULL == dataPoints) || (NULL == devStatusPtr))
    {
        os_printf("gizDataPoints2ReportData Error , Illegal Param\n");
        return -1;
    }

    memset((uint8_t *)devStatusPtr->wBitBuf,0,sizeof(devStatusPtr->wBitBuf));
    memset((uint8_t *)devStatusPtr->rBitBuf,0,sizeof(devStatusPtr->rBitBuf));

    gizCompressValue(LED_ONOFF_BYTEOFFSET,LED_ONOFF_BITOFFSET,LED_ONOFF_LEN,(uint8_t *)devStatusPtr,dataPoints->valueLED_OnOff);
    gizCompressValue(LED_COLOR_BYTEOFFSET,LED_COLOR_BITOFFSET,LED_COLOR_LEN,(uint8_t *)devStatusPtr,dataPoints->valueLED_Color);
    gizCompressValue(INFRARED_BYTEOFFSET,INFRARED_BITOFFSET,INFRARED_LEN,(uint8_t *)devStatusPtr,dataPoints->valueInfrared);
    gizByteOrderExchange((uint8_t *)devStatusPtr->wBitBuf,sizeof(devStatusPtr->wBitBuf));
    gizByteOrderExchange((uint8_t *)devStatusPtr->rBitBuf,sizeof(devStatusPtr->rBitBuf));

    devStatusPtr->valueAlert_1 = dataPoints->valueAlert_1;
    devStatusPtr->valueAlert_2 = dataPoints->valueAlert_2;
    devStatusPtr->valueFault_LED = dataPoints->valueFault_LED;
    devStatusPtr->valueFault_Motor = dataPoints->valueFault_Motor;
    devStatusPtr->valueFault_TemHum = dataPoints->valueFault_TemHum;
    devStatusPtr->valueFault_IR = dataPoints->valueFault_IR;

    devStatusPtr->valueLED_R = gizY2X(LED_R_RATIO,  LED_R_ADDITION, dataPoints->valueLED_R); 
    devStatusPtr->valueLED_G = gizY2X(LED_G_RATIO,  LED_G_ADDITION, dataPoints->valueLED_G); 
    devStatusPtr->valueLED_B = gizY2X(LED_B_RATIO,  LED_B_ADDITION, dataPoints->valueLED_B); 
    devStatusPtr->valueTemperature = gizY2X(TEMPERATURE_RATIO,  TEMPERATURE_ADDITION, dataPoints->valueTemperature); 
    devStatusPtr->valueHumidity = gizY2X(HUMIDITY_RATIO,  HUMIDITY_ADDITION, dataPoints->valueHumidity); 

    devStatusPtr->valueMotor_Speed = gizProtocolExchangeBytes(gizY2X(MOTOR_SPEED_RATIO,  MOTOR_SPEED_ADDITION, dataPoints->valueMotor_Speed)); 



    return 0;
}

/**
* @brief 在该函数被Gagent模块调用，当连接状态变化时会传入相应的数值
* @param[in] value WiFi连接状态数值
* @return none
*/
void ICACHE_FLASH_ATTR gizWiFiStatus(uint16_t value)
{
    uint8_t rssiValue = 0;
    wifi_status_t status;
    static wifi_status_t lastStatus;

    if(0 != value)
    {
        status.value = gizProtocolExchangeBytes(value); 

        os_printf("@@@@ GAgentStatus[hex]:%02x | [Bin]:%d,%d,%d,%d,%d,%d \r\n", status.value, status.types.con_m2m, status.types.con_route, status.types.binding, status.types.onboarding, status.types.station, status.types.softap);

        //OnBoarding mode status
        if(1 == status.types.onboarding)
        {
            if(1 == status.types.softap)
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_SOFTAP;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("OnBoarding: SoftAP or Web mode\r\n");
            }

                if(1 == status.types.station)
                {
                    gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_AIRLINK;
                    gizwitsProtocol.issuedProcessEvent.num++;
                    os_printf("OnBoarding: AirLink mode\r\n");
                }
            }
            else
            {
                if(1 == status.types.softap)
                {
                    gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_SOFTAP;
                    gizwitsProtocol.issuedProcessEvent.num++;
                    os_printf("OnBoarding: SoftAP or Web mode\r\n");
                }

                if(1 == status.types.station)
                {
                    gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_STATION;
                    gizwitsProtocol.issuedProcessEvent.num++;
                    os_printf("OnBoarding: Station mode\r\n");
            }
        }

        //binding mode status
        if(lastStatus.types.binding != status.types.binding)
        {
            lastStatus.types.binding = status.types.binding;
            if(1 == status.types.binding)
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_OPEN_BINDING;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: in binding mode\r\n");
            }
            else
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_CLOSE_BINDING;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: out binding mode\r\n");
            }
        }

        //router status
        if(lastStatus.types.con_route != status.types.con_route)
        {
            lastStatus.types.con_route = status.types.con_route;
            if(1 == status.types.con_route)
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_CON_ROUTER;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: connected router\r\n");
            }
            else
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_DISCON_ROUTER;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: disconnected router\r\n");
            }
        }

        //M2M server status
        if(lastStatus.types.con_m2m != status.types.con_m2m)
        {
            lastStatus.types.con_m2m = status.types.con_m2m;
            if(1 == status.types.con_m2m)
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_CON_M2M;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: connected m2m\r\n");
            }
            else
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_DISCON_M2M;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: disconnected m2m\r\n");
            }
        }

        //APP status
        if(lastStatus.types.app != status.types.app)
        {
            lastStatus.types.app = status.types.app;
            if(1 == status.types.app)
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_CON_APP;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: app connect\r\n");
            }
            else
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_DISCON_APP;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: no app connect\r\n");
            }
        }

        //test mode status
        if(lastStatus.types.test != status.types.test)
        {
            lastStatus.types.test = status.types.test;
            if(1 == status.types.test)
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_OPEN_TESTMODE;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: in test mode\r\n");
            }
            else
            {
                gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_CLOSE_TESTMODE;
                gizwitsProtocol.issuedProcessEvent.num++;
                os_printf("WiFi status: out test mode\r\n");
            }
        }

        rssiValue = status.types.rssi;
        gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = WIFI_RSSI;
        gizwitsProtocol.issuedProcessEvent.num++;

        gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8_t *)&rssiValue, sizeof(rssiValue));
        os_memset((uint8_t *)&gizwitsProtocol.issuedProcessEvent, 0, sizeof(eventInfo_t));

        lastStatus = status;
    }
}

/**
* @brief 根据协议生成“控制型事件”

* @param [in] issuedData  ：控制型数据
* @param [out] info       ：事件队列
* @param [out] dataPoints ：数据点数据
* @return 0，执行成功， 非 0，失败
*/
static int8_t ICACHE_FLASH_ATTR gizDataPoint2Event(gizwitsIssued_t *issuedData, eventInfo_t *info, dataPoint_t *dataPoints)
{
    if((NULL == issuedData) || (NULL == info) ||(NULL == dataPoints))
    {
        os_printf("gizDataPoint2Event Error , Illegal Param\n");
        return -1;
    }

    /** 大于1字节做位序转换 **/
    if(sizeof(issuedData->attrFlags) > 1)
    {
        if(-1 == gizByteOrderExchange((uint8_t *)&issuedData->attrFlags,sizeof(attrFlags_t)))
        {
            os_printf("gizByteOrderExchange Error\n");
            return -1;
        }
    }
    if(0x01 == issuedData->attrFlags.flagLED_OnOff)
    {
        info->event[info->num] = EVENT_LED_ONOFF;
        info->num++;
        dataPoints->valueLED_OnOff = gizDecompressionValue(LED_ONOFF_BYTEOFFSET,LED_ONOFF_BITOFFSET,LED_ONOFF_LEN,(uint8_t *)&issuedData->attrVals.wBitBuf,sizeof(issuedData->attrVals.wBitBuf));
    }



    if(0x01 == issuedData->attrFlags.flagLED_Color)
    {
        info->event[info->num] = EVENT_LED_COLOR;
        info->num++;
        dataPoints->valueLED_Color = gizDecompressionValue(LED_COLOR_BYTEOFFSET,LED_COLOR_BITOFFSET,LED_COLOR_LEN,(uint8_t *)&issuedData->attrVals.wBitBuf,sizeof(issuedData->attrVals.wBitBuf));
    }



    if(0x01 == issuedData->attrFlags.flagLED_R)
    {
        info->event[info->num] = EVENT_LED_R;
        info->num++;
        dataPoints->valueLED_R = gizX2Y(LED_R_RATIO,  LED_R_ADDITION, issuedData->attrVals.valueLED_R); 
    }



    if(0x01 == issuedData->attrFlags.flagLED_G)
    {
        info->event[info->num] = EVENT_LED_G;
        info->num++;
        dataPoints->valueLED_G = gizX2Y(LED_G_RATIO,  LED_G_ADDITION, issuedData->attrVals.valueLED_G); 
    }



    if(0x01 == issuedData->attrFlags.flagLED_B)
    {
        info->event[info->num] = EVENT_LED_B;
        info->num++;
        dataPoints->valueLED_B = gizX2Y(LED_B_RATIO,  LED_B_ADDITION, issuedData->attrVals.valueLED_B); 
    }



    if(0x01 == issuedData->attrFlags.flagMotor_Speed)
    {
        info->event[info->num] = EVENT_MOTOR_SPEED;
        info->num++;
        dataPoints->valueMotor_Speed = gizX2Y(MOTOR_SPEED_RATIO,  MOTOR_SPEED_ADDITION, gizProtocolExchangeBytes(issuedData->attrVals.valueMotor_Speed));
    }



    return 0;
}

/**
* @brief 对比当前数据和上次数据
*
* @param [in] *cur        : 当前数据点数据
* @param [in] *last       : 上次数据点数据
*
* @return : 0,数据无变化;1，数据有变化
*/
static int8_t ICACHE_FLASH_ATTR gizCheckReport(dataPoint_t *cur, dataPoint_t *last)
{
    int8_t ret = 0;
    static uint32_t lastReportTime = 0;

    if((NULL == cur) || (NULL == last))
    {
        os_printf("gizCheckReport Error , Illegal Param\n");
        return -1;
    }
    
    if(last->valueLED_OnOff != cur->valueLED_OnOff)
    {
        os_printf("valueLED_OnOff Changed\n");
        ret = 1;
    }
    if(last->valueLED_Color != cur->valueLED_Color)
    {
        os_printf("valueLED_Color Changed\n");
        ret = 1;
    }
    if(last->valueLED_R != cur->valueLED_R)
    {
        os_printf("valueLED_R Changed\n");
        ret = 1;
    }
    if(last->valueLED_G != cur->valueLED_G)
    {
        os_printf("valueLED_G Changed\n");
        ret = 1;
    }
    if(last->valueLED_B != cur->valueLED_B)
    {
        os_printf("valueLED_B Changed\n");
        ret = 1;
    }
    if(last->valueMotor_Speed != cur->valueMotor_Speed)
    {
        os_printf("valueMotor_Speed Changed\n");
        ret = 1;
    }
    if(last->valueInfrared != cur->valueInfrared)
    {
        os_printf("valueInfrared Changed\n");
        ret = 1;
    }
    if(last->valueAlert_1 != cur->valueAlert_1)
    {
        os_printf("valueAlert_1 Changed\n");
        ret = 1;
    }
    if(last->valueAlert_2 != cur->valueAlert_2)
    {
        os_printf("valueAlert_2 Changed\n");
        ret = 1;
    }
    if(last->valueFault_LED != cur->valueFault_LED)
    {
        os_printf("valueFault_LED Changed\n");
        ret = 1;
    }
    if(last->valueFault_Motor != cur->valueFault_Motor)
    {
        os_printf("valueFault_Motor Changed\n");
        ret = 1;
    }
    if(last->valueFault_TemHum != cur->valueFault_TemHum)
    {
        os_printf("valueFault_TemHum Changed\n");
        ret = 1;
    }
    if(last->valueFault_IR != cur->valueFault_IR)
    {
        os_printf("valueFault_IR Changed\n");
        ret = 1;
    }

    if(last->valueTemperature != cur->valueTemperature)
    {
        if(gizGetTimerCount()-lastReportTime >= REPORT_TIME_MAX)
        {
            os_printf("Temperature Changed\n");
            lastReportTime = gizGetTimerCount();
            ret = 1;
        }
    }
    if(last->valueHumidity != cur->valueHumidity)
    {
        if(gizGetTimerCount()-lastReportTime >= REPORT_TIME_MAX)
        {
            os_printf("Humidity Changed\n");
            lastReportTime = gizGetTimerCount();
            ret = 1;
        }
    }

    return ret;
}

/**
* @brief 该函数被Gagent模块调用，接收来自云端或APP端下发的相关协议数据
* @param[in] inData 输入的协议数据
* @param[in] inLen 输入数据的长度
* @param[out] outData 输出的协议数据
* @param[out] outLen 输出数据的长度
* @return 0， 执行成功， 非 0， 失败
*/
int32_t ICACHE_FLASH_ATTR gizIssuedProcess(uint8_t *inData, uint32_t inLen,uint8_t *outData,int32_t *outLen)
{
    gizwitsIssued_t *gizIssuedData= (gizwitsIssued_t *)&inData[1];
    if(NULL == gizIssuedData)
    {
        os_printf("!!! IssuedProcess Error \n"); 
        return -1; 
    }
 
    if((NULL == outData) || (NULL == outLen)) 
    {
        os_printf("!!! IssuedProcess Error \n"); 

        return -1; 
    }

    switch(inData[0]) 
    {
        case ACTION_CONTROL_DEVICE:
            gizDataPoint2Event(gizIssuedData, &gizwitsProtocol.issuedProcessEvent,&gizwitsProtocol.gizCurrentDataPoint);
            
            system_os_post(USER_TASK_PRIO_2, SIG_ISSUED_DATA, 0); 
            *outLen = 0; 
            break;
            
        case ACTION_READ_DEV_STATUS:
            gizDataPoints2ReportData(&gizwitsProtocol.gizLastDataPoint,&gizwitsProtocol.reportData.devStatus);
            gizwitsProtocol.reportData.action = ACTION_READ_DEV_STATUS_ACK;
            os_memcpy(outData, (uint8_t *)&gizwitsProtocol.reportData, sizeof(gizwitsReport_t));
            *outLen = sizeof(gizwitsReport_t);
            break;
            
        case ACTION_W2D_TRANSPARENT_DATA: //透传
            os_memcpy(gizwitsProtocol.transparentBuff, &inData[1], inLen-1);
            gizwitsProtocol.transparentLen = inLen-1;

            system_os_post(USER_TASK_PRIO_2, SIG_PASSTHROUGH, 0);
            *outLen = 0;

            break;
            
        default:
        
            break;
    }

    return 0;
}

/**
* @brief 定时器回调函数，在该函数中完成10分钟的定时上报
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR gizTimerFunc(void)
{
    static uint32 repCtime = 0;

    //600s Regularly report
    if(TIM_REP_TIMOUT < repCtime)
    {
        repCtime = 0;

        os_printf("@@@@ gokit_timing report! \n");

        system_os_post(USER_TASK_PRIO_2, SIG_IMM_REPORT, 0);
    }
    gizwitsProtocol.timerMsCount++;
    repCtime++;
}

/**
* @brief Gizwits协议相关系统事件回调函数
* @param none
* @return none
* @note 在该函数中完成了Gizwits协议相关的系统事件的处理:
*/
void ICACHE_FLASH_ATTR gizTask(os_event_t * events)
{
    uint8_t i = 0;
    uint8 vchar = 0;

    if(NULL == events)
    {
        os_printf("!!! gizTask Error \n");
    }

    vchar = (uint8)(events->par);

    switch(events->sig)
    {
    case SIG_ISSUED_DATA:
        gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8_t *)&gizwitsProtocol.gizCurrentDataPoint, sizeof(dataPoint_t));
        //clean event info
        os_memset((uint8_t *)&gizwitsProtocol.issuedProcessEvent, 0, sizeof(eventInfo_t));
        break;
    case SIG_PASSTHROUGH:
        gizwitsProtocol.issuedProcessEvent.event[0] = TRANSPARENT_DATA;
        gizwitsProtocol.issuedProcessEvent.num = 1;
        gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8_t *)gizwitsProtocol.transparentBuff, gizwitsProtocol.transparentLen);

        os_memset((uint8_t *)&gizwitsProtocol.issuedProcessEvent, 0, sizeof(eventInfo_t));
        os_memset((uint8_t *)gizwitsProtocol.transparentBuff, 0, BUFFER_LEN_MAX);
        gizwitsProtocol.transparentLen = 0;
        break;
    case SIG_IMM_REPORT:
        gizDataPoints2ReportData(&gizwitsProtocol.gizLastDataPoint, (devStatus_t *)&gizwitsProtocol.reportData.devStatus);
        gizwitsProtocol.reportData.action = ACTION_REPORT_DEV_STATUS;
        gagentUploadData((uint8_t *)&gizwitsProtocol.reportData, sizeof(gizwitsReport_t));
        break;
    default:
        os_printf("---error sig! ---\n");
        break;
    }
}

/**@name Gizwits 用户API接口
* @{
*/

/**
* @brief WiFi配置接口

* 用户可以调用该接口使WiFi模组进入相应的配置模式或者复位模组

* @param[in] mode 配置模式选择：0x0， 模组复位 ;0x01， SoftAp模式 ;0x02， AirLink模式; 0x03 , 模组进入产测; 0x04:允许用户绑定设备

* @return 错误命令码
*/
void ICACHE_FLASH_ATTR gizwitsSetMode(uint8_t mode)
{
    switch(mode)
    {
        case WIFI_RESET_MODE:
            gagentReset();
            break;
        case WIFI_SOFTAP_MODE:
        case WIFI_AIRLINK_MODE:
        case WIFI_PRODUCTION_TEST:
            gagentConfig(mode);
            break;
        case WIFI_NINABLE_MODE:
            GAgentEnableBind();
            break;
        default :
            break;
    }
}

/**
* @brief 获取网络时间戳接口
* @param[in] none
* @return none
*/
uint32_t ICACHE_FLASH_ATTR gizwitsGetTimeStamp(void)
{
    _tm tmValue;

    gagentGetNTP(&tmValue);

    return tmValue.ntp;
}

/**
* @brief gizwits协议初始化接口

* 用户调用该接口可以完成Gizwits协议相关初始化（包括协议相关定时器、串口的初始）

* 用户可在在此接口内完成数据点的初始化状态设置

* @param none
* @return none
*/
void ICACHE_FLASH_ATTR gizwitsInit(void)
{
    int16_t value = 0;
    struct devAttrs attrs;
    os_memset((uint8_t *)&gizwitsProtocol, 0, sizeof(gizwitsProtocol_t));

    system_os_task(gizTask, USER_TASK_PRIO_2, gizwitsTaskQueue, gizwitsTaskQueueLen);

    //gokit timer start
    os_timer_disarm(&gizTimer);
    os_timer_setfn(&gizTimer, (os_timer_func_t *)gizTimerFunc, NULL);
    os_timer_arm(&gizTimer, MAX_SOC_TIMOUT, 1);

    //Gagent
    system_os_task(gagentProcessRun, USER_TASK_PRIO_1, TaskQueue, TaskQueueLen);

    attrs.mBindEnableTime = gizProtocolExchangeBytes(NINABLETIME);
    memset((uint8_t *)attrs.mDevAttr, 0, 8);
    attrs.mDevAttr[7] |= DEV_IS_GATEWAY<<0;
    attrs.mDevAttr[7] |= (0x01<<1);
    os_memcpy(attrs.mstrDevHV, HARDWARE_VERSION, os_strlen(HARDWARE_VERSION));
    os_memcpy(attrs.mstrDevSV, SOFTWARE_VERSION, os_strlen(SOFTWARE_VERSION));
    os_memcpy(attrs.mstrP0Ver, P0_VERSION, os_strlen(P0_VERSION));
    os_memcpy(attrs.mstrProductKey, PRODUCT_KEY, os_strlen(PRODUCT_KEY));
    os_memcpy(attrs.mstrProtocolVer, PROTOCOL_VERSION, os_strlen(PROTOCOL_VERSION));
    os_memcpy(attrs.mstrSdkVerLow, SDK_VERSION, os_strlen(SDK_VERSION));
    gagentInit(attrs);

    os_printf("gizwitsInit OK \r\n");
}

/**
* @brief gizwits数据点更新上报处理

* 用户调用该接口可以完成设备数据的变化上报

* @param [in] dataPoint 用户设备数据点
* @return none
*/
int8_t ICACHE_FLASH_ATTR gizwitsHandle(dataPoint_t *dataPoint)
{
    if(NULL == dataPoint)
    {
        os_printf("!!! gizReportData Error \n");

        return (-1);
    }

    //Regularly report conditional
    if((1 == gizCheckReport(dataPoint, (dataPoint_t *)&gizwitsProtocol.gizLastDataPoint)))
    {
        gizDataPoints2ReportData(dataPoint, &gizwitsProtocol.reportData.devStatus);

        gizwitsProtocol.reportData.action = ACTION_REPORT_DEV_STATUS;
        gagentUploadData((uint8_t *)&gizwitsProtocol.reportData, sizeof(gizwitsReport_t));

        memcpy((uint8_t *)&gizwitsProtocol.gizLastDataPoint, (uint8_t *)dataPoint, sizeof(dataPoint_t));
    }

	return 0;
}

/**
* @brief gizwits上报透传数据接口

* 用户调用该接口可以完成私有协议数据的上报

* @param [in] data 输入的私有协议数据
* @param [in] len 输入的私有协议数据长度
* @return -1 , 错误返回;0,正确返回
*/
int32_t ICACHE_FLASH_ATTR gizwitsPassthroughData(uint8_t * data, uint32_t len)
{
    uint8_t *passReportBuf = NULL;
    uint32_t passReportBufLen = len + sizeof(uint8_t);

    if(NULL == data)
    {
        os_printf("!!! gizReportData Error \n");

        return (-1);
    }

    passReportBuf = (uint8 * )os_malloc(passReportBufLen);
    if(NULL != passReportBuf)
    {
        os_printf("Failed to malloc buffer\n");
        return (-1);
    }

    passReportBuf[0] = ACTION_D2W_TRANSPARENT_DATA;
    os_memcpy((uint8_t *)&passReportBuf[1], data, len);

    gagentUploadData(passReportBuf, passReportBufLen);

    os_free(passReportBuf);
    passReportBuf = NULL;

    return 0;
}
/**@} */

