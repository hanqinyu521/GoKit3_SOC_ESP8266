# GoKit3_SOC_ESP8266
（环境+源码改写+编译+烧录+iOS+Android+按键实时反馈）

从接触机智云Gokit一步一坑，大坑套小坑也算挺过来了,把遇到的问题和经验分享给大家，帮助新人少走一些弯路。

####一、环境搭建
1.下载安装eclipse一体化开发环境
下载地址：[http://pan.baidu.com/s/1skRvR1j](http://pan.baidu.com/s/1skRvR1j) 

安装完毕之后去官方下载微信宠物屋for GoKit3(S) ESP8266源码

![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3094797-12413e51c7bab78d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

Import导入soc源码

![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3094797-afeefb329b902ad7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3094797-a2390d78406331e8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3094797-5bfbf69c2ad2ccdf.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

需要修改makefile文件配置才能编译成功
```
BOOT?=new
APP?=1
SPI_SPEED?=40
SPI_MODE?=QIO
SPI_SIZE_MAP?=6
```
![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3094797-6b7c92b55fc33846.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

####二、GoKit3开发套件
1、GoKit3(S) - SoC版（乐鑫模组(GoKit3转接板)+底板+GoKit3功能板）
![](http://upload-images.jianshu.io/upload_images/3094797-e628e098456f2839.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![](http://upload-images.jianshu.io/upload_images/3094797-c5c712c61ca79204.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

2、GoKit3玩法
根据使用方式不同可分MCU版和SOC版两大类
([了解SoC与MCU的区别](http://club.gizwits.com/#SOC版与MCU版的区别))，每个类型又分不同方式的玩法。

2.1、MCU版
![](http://upload-images.jianshu.io/upload_images/3094797-6d5dcc0ba3cdd473.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

MCU版的玩法与GoKit2一样，采用底板+扩展板+模组的方式(如上图所示)。

2.2、SOC版
模组+扩展版为SOC版本不需要底板。

![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3094797-1bef80382ea65dc2.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


2.3、模组组装时分为俩种模式（有坑不要插错了）
```

MCU模式
SOC模式
```

####三、烧录选项配置
```

1. 前三个 ‘.bin’ 文件都在\bin 目录下，选择对应的名称的 ‘.bin’文件即可。
最后一个.bin 文件默认为SOC版固件：user1.4096.new.6.bin
位置：\bin\upgrade\user1.4096.new.6.bin
```

![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3094797-3b11338f8f97ca58.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

2.GoKit套件SOC版烧录模式接法（有坑不要接错了）
```

需要注意RX TX接法
GND接GND
RX接TX
TX接RX
```

![IMG_20170615_173535.jpg](http://upload-images.jianshu.io/upload_images/3094797-fd5007eb572cded4.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

将拨码开关设为程序烧录模式（**下拨位置**）
![](http://upload-images.jianshu.io/upload_images/3094797-b27ba842bc1199ad.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


设备管理器选择com口 设置波特率

![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3094797-9ac9fba698361229.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3094797-2700369f2734cf4b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)





相继按下烧录工具的“START”键与WiFi模组的“Reset”键，开始烧录程序：
![](http://upload-images.jianshu.io/upload_images/3094797-733a904d7bbc8825.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/3094797-65e796b7db42512e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

若显示“完成”及烧录成功


####四、源码改写
```

只需要关心文件路径为“GizLamp\app”下面的几个地方：
如果你需要添加8266的外设，只需要在
“GizLamp\app\driver”文件目录下添加相应外设的驱动的.c文件
“GizLamp\app\include\driver”文件目录下添加相应外设的驱动的.h文件
App通过云端下发控制事件处理，可以在
“GizLamp\app\Gizwits”文件目录下“gizwits_product.c”文件里面的
“gizwitsEventProcess（）函数里添加驱动外设执行事件函数即可实现控制设备
上报云端状态事件处理，可以在
“GizLamp\app\user”文件目录下“user_main.c”文件里面的“userTimerFunc（）”函数里添加数据状态上报函数即可以实现状态上报。
```

![](http://upload-images.jianshu.io/upload_images/3094797-c9cc5733fe8ff47f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


第一步先修改Gizwits目录下的gizwits_protocol.h里的PRODUCT_KEY
```
/**@name Product Key (产品标识码)
* @{
*/
#define PRODUCT_KEY "xxxxxxxxxxxxxxxxxxxxxxx"
```
然后我们要实现按键APP实时反馈状态
我们以key1键为例 在user_main.c文件修改key1ShortPress
```

LOCAL void ICACHE_FLASH_ATTR key1ShortPress(void)
{
    os_printf("#### KEY1 short press ,Production Mode\n");
//判断当前LED状态取反赋值
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
```

gizwitsEventProcess（）驱动外设执行事件函数
根据数据点执行相应逻辑
```

int8_t ICACHE_FLASH_ATTR gizwitsEventProcess(eventInfo_t *info, uint8_t *data, uint32_t len)
{
  uint8_t i = 0;
  dataPoint_t *dataPointPtr = (dataPoint_t *)data;
  moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)data;

  if((NULL == info) || (NULL == data))
  {
    return -1;
  }

  for(i=0; i<info->num; i++)
  {
    switch(info->event[i])
    {
      case EVENT_LED_ONOFF:
        currentDataPoint.valueLED_OnOff = dataPointPtr->valueLED_OnOff;
        os_printf("Evt: EVENT_LED_ONOFF %d \n", currentDataPoint.valueLED_OnOff);
        if(0x01 == currentDataPoint.valueLED_OnOff)
        {
            rgbControl(254, 0, 0);
        }
        else
        {  
            rgbControl(0, 0, 0);   
        }
        break;

      case EVENT_LED_COLOR:
        currentDataPoint.valueLED_Color = dataPointPtr->valueLED_Color;
        os_printf("Evt: EVENT_LED_COLOR %d\n", currentDataPoint.valueLED_Color);
        switch(currentDataPoint.valueLED_Color)
        {
          case LED_COLOR_VALUE0:
            rgbControl(currentDataPoint.valueLED_R,currentDataPoint.valueLED_G,currentDataPoint.valueLED_B);
            break;
          case LED_COLOR_VALUE1:
            rgbControl(254, 254, 0);
            break;
          case LED_COLOR_VALUE2:
            rgbControl(254, 0, 70);
            break;
          case LED_COLOR_VALUE3:
            rgbControl(238, 30, 30); 
            break;
          default:
            break;
        }
        break;

      case EVENT_LED_R:
        currentDataPoint.valueLED_R = dataPointPtr->valueLED_R;
        os_printf("Evt:EVENT_LED_R %d\n",currentDataPoint.valueLED_R);
        rgbControl(currentDataPoint.valueLED_R,currentDataPoint.valueLED_G,currentDataPoint.valueLED_B); 
        break;
      case EVENT_LED_G:
        currentDataPoint.valueLED_G = dataPointPtr->valueLED_G;
        os_printf("Evt:EVENT_LED_G %d\n",currentDataPoint.valueLED_G);
        rgbControl(currentDataPoint.valueLED_R,currentDataPoint.valueLED_G,currentDataPoint.valueLED_B);
        break;
      case EVENT_LED_B:
        currentDataPoint.valueLED_B = dataPointPtr->valueLED_B;
        os_printf("Evt:EVENT_LED_B %d\n",currentDataPoint.valueLED_B);
        rgbControl(currentDataPoint.valueLED_R,currentDataPoint.valueLED_G,currentDataPoint.valueLED_B);
        break;
      case EVENT_MOTOR_SPEED:
        currentDataPoint.valueMotor_Speed = dataPointPtr->valueMotor_Speed;
        os_printf("Evt:EVENT_MOTOR_SPEED %d\n",currentDataPoint.valueMotor_Speed);
        motorControl(currentDataPoint.valueMotor_Speed);
        break;

      case WIFI_SOFTAP:
        break;
      case WIFI_AIRLINK:
        break;
      case WIFI_STATION:
        break;
      case WIFI_CON_ROUTER:
        rgbControl(0, 0, 0);
        break;
      case WIFI_DISCON_ROUTER:
        break;
      case WIFI_CON_M2M:
        break;
      case WIFI_DISCON_M2M:
        break;
      case WIFI_RSSI:
        os_printf("RSSI %d\n", wifiData->rssi);
        break;
      case TRANSPARENT_DATA:
        os_printf("TRANSPARENT_DATA \n");
        //user handle , Fetch data from [data] , size is [len]
        break;
      default:
        break;
    }
  }
  system_os_post(USER_TASK_PRIO_0, SIG_UPGRADE_DATA, 0);
  return 0;
}
```

###实际效果：



![soc.gif](http://upload-images.jianshu.io/upload_images/3094797-c1835749076c4bdf.gif?imageMogr2/auto-orient/strip)
