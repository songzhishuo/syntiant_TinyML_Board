![FujSQTmhWsvAm4RLNF4C6EjvyJlo](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/FujSQTmhWsvAm4RLNF4C6EjvyJlo-16566023755721.png)

# 板卡介绍

 配备超低功耗 Syntiant ® NDP101神经决策处理器™，可以使语音和传感器应用程序分别在 140 和 100 微瓦以下运行，与基于 MCU 的传统 MCU 相比，吞吐量提高 20 倍，系统效率提高 200 倍。Syntiant TinyML 板的尺寸为 24 毫米 x 28 毫米，是一个小型的独立系统，通过微型 USB 连接通过 Edge Impulse 轻松下载经过训练的模型，而无需任何专用硬件。

主要特点：

- 神经决策处理器使用NDP101，连接了两个传感器

  - BMI160 6轴运动传感器
  - SPH0641LM4H麦克风

- 主机处理器：

  SAMD21 Cortex-M0+ 32位低功耗48MHz ARM MCU

  - 内置256KB FLASH和32KB SRAM，
  - 5个数字I/O，与Arduino MKR系列板兼容
  - 其中包含1路UART接口和1路I2C接口（包括在数字I/O引脚中）
  - 2MB板载串行闪存
  - 一个用户定义的RGB LED
  - uSD卡插槽

- 电路板可使用5V micro-USB或3.7V LiPo电池供电

![img](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/project-2-165660256464817.png)



# 任务目标：

基于**Syntiant TinyML board** 设计一款可以和**Raspberry 4**进行联动控制小米和涂鸦设备的语音助手。



# 设计思路

## 系统拓扑：

![photo.drawio](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/photo.drawio.png)

## 详细描述：

不管是tuya云还是米家云都是私有的，我们无法直接将自定义设备进行接入并进行交互。因此我们需要借助一个工具----`Home Assistant`来进行消息的转发。并且将树莓派作为其软件运行的载体。当TinyML开发板识别到对应语音指令之后通过IO状态上报给ESP8266芯片，ESP8266通过局域网MQTT服务上报给树莓派上的EMQ服务器，HASS去自动解析MQTT报文并且将相应指令上报给不同的云平台。





# 详细实施：

## 1.Docker安装

为了便于管理树莓派上的软件包我们统一使用docker进行软件的安装，这样可以保证每个软件都是独立运行在虚拟环境中的不会因为运行出错或者错删文件导致系统无法启动。

下面提供两种安装方式：

### 脚本安装方式

只需要输入命令就可以安装好（省事 方便，初次学习可以试试第二种方法）

```shell
sudo curl -sSL https://get.docker.com | sh
```

![在这里插入图片描述](C:\Users\songz\Pictures\博客截图\20200913223141976.png%23pic_center)



### apt 安装方式
由于 Raspbian 基于 Debian，我们还可以使用 apt安装（其他发行版可做参考）

1.添加使用 https 传输的软件包以及 CA 证书。
由于 apt 源使用 https 以确保软件下载过程中不被篡改。因此，我们首先需要添加使用 https 传输的软件包以及 CA 证书。

```shell
$ sudo apt-get install \
     apt-transport-https \
     ca-certificates \
     curl \
     gnupg2 \
     lsb-release \
     software-properties-common
```

2.添加 Docker 的 GPG key

为了确认所下载软件包的合法性，需要添加软件源的 GPG 密钥。

```shell
curl -fsSL https://yum.dockerproject.org/gpg | sudo apt-key add -

# 官方源
# $ curl -fsSL https://download.docker.com/linux/raspbian/gpg | sudo apt-key add -
```

3.设置稳定的 repository

```shell
sudo add-apt-repository \
      "deb https://apt.dockerproject.org/repo/ \
      raspbian-$(lsb_release -cs) \
      stable"


# 官方源
#$ sudo add-apt-repository \
#"deb [arch=armhf] https://download.docker.com/linux/raspbian \
#$(lsb_release -cs) \
#stable"
```

4.安装 Docker

更新 apt 软件包缓存，并安装 docker

```shell
$ sudo apt-get update
$ sudo apt-get -y install docker-engine
```

5.测试 Docker 是否安装正确

```shell
sudo docker run hello-world
```



## 2.HomeAssisatant 的安装

整个安装过程十分的简单这里不再赘述可以直接参考此贴：

[树莓派4+ docker+ homeassistant_停止测试的博客-CSDN博客_树莓派跑docker](https://blog.csdn.net/weixin_43990954/article/details/107330798)



## 3.tuya设备接入



1. 首先登陆[涂鸦 IoT 平台 (tuya.com)](https://iot.tuya.com/) 官网，在云开发->云管理->我的云项目中 创建一个新的云项目。在创建过程中开放方式选择`自定义开发`即可。

![image-20220627222017214](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220627222017214-16566024192942.png)

![image-20220630232227545](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220630232227545.png)

2. 在HASS的配置->设备与服务中添加新的集成，在搜索框搜索涂鸦即可。

![image-20220627222357791](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220627222357791-16566024404764.png)

在配置框中的名称和tuya云平台上的名称对应关系如下图所示：

| 编号 | 云平台名                     | HASS配置名             |
| ---- | ---------------------------- | ---------------------- |
| 1    | Access ID/Client ID          | 涂鸦物联网设备接入ID   |
| 2    | Access Secret/Client Secret: | 涂鸦物联网设备接入密码 |

其中的用户名和密码对应Tuya 手机APP上登陆的用户名和密码，不是IOT平台这里不要混淆。

![image-20220627222613700](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220627222613700-16566024495005.png)

注：Access ID和Secret可以在概况中查询到。

![image-20220627222251482](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220627222251482-16566024532156.png)

3. 在手机APP上根据提示将设备添加到关联，之后在云平台的设备中手动去关联APP账户并且添加关联的设备到此云服务，对于设备较多的用户建议手动添加所需设备，不要全部导入否则云平台在检索设备时会非常卡。

![image-20220627223843391](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220627223843391-16566024566847.png)

![img](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220627224039238-16565087561781-16566024602338.png)



4. 添加成功狗手动刷新HASS面板即可看到新添加的设备

![image-20220627224357478](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220627224357478-16566024647279.png)



## 4. 米家设备接入

可以直接参考此篇博客：

[米家设备接入 Home Assistant - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/400997679)



## 5. EMQ安装和关联HASS

EMQ是个轻量级的MQTT服务器，将其直接运行在树莓派上可以作为ESP8266和HASS沟通的桥梁

1. 安装

详细的安装教程可以参考我的博文：

[树莓派安装配置使用EMQ教程_Argon_Ghost的博客-CSDN博客](https://blog.csdn.net/Argon_Ghost/article/details/106033912)

2. 集成

直接在集成选项中添加MQTT组件即可，然后输入对应的IP和端口（一般情况下MQTT EMQ 的端口为1883）

![image-20220627231041051](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220627231041051-165660247110410.png)



## 6.数据转发

为使ESP8266可以控制米家设备和涂鸦设备，可以通过HASS平台的自动化功能来完成，自动化类似于Iphone上的自动化功能，用户可以自定义触发源触发信号，在触发信号到来的时候将触发信号触发起到控制硬件的目的，这里以ESP8266通过MQTT触发米家空气净化器开机为例来做讲解，其余设备类似：

### 1.配置

在 `配置`->`场景自动化`-> `新建自动化` 来创建一个新的`BluePrint`自动化场景

### 2.配置触发条件

因为我们需要在收到MQTT特定报文时候进行触发，我配套代码中的触发条件配置为如下图所示即可：

![image-20220629211116613](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220629211116613-165660247506511.png)


### 3.配置动作

配置动作就是在条件触发后的相应，这里我们需要相应的是开机，相应配置如下图所示：

![image-20220629211800201](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220629211800201-165660247776612.png)

### 4.注
配套代码实现中的其他触发条件均通过`office/sensor1` topic进行，详细的匹配关系参数如下表所示：

| 编号 | 触发目的 | 参数 |
| ---- | -------- | ---- |
| 1    | 关机     | 0    |
| 2    | 开机     | 1    |
| 3    | 自动模式 | 2    |
| 4    | 睡眠模式 | 3    |



## 7.语音数据训练

**syntiant** 提供了完整的从训练到部署的一站式支持，可以通过如下链接打开此平台：

[在线训练平台]: https://studio.edgeimpulse.com

![image-20220629212409859](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220629212409859-165660248152513.png)

整个训练过程分为以下几个部分，详细的使用方法可以在EDGE IMPULSE网站的wiki上查看。

1. 通过麦克风将数据集录入
2. 新建识别内容和工程
3. 设置采用方式、采样率、采样窗口等参数
4. 配置数据分类方式（这边我们直接使用NN 经典模型）
5. 数据训练

训练完毕之后直接点击Deployment即可对数据进行导出，我项目中训练的模型如下图所示：

![image-20220629213706942](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220629213706942-165660248442614.png)

## 8. 模型下载

对于音频模型可以直接将模型烧录在板卡的内置Flash中，在官方提供的Demo程序中已经集成了Flash的烧录功能，通过tool中的

flash_windows.bat工具即可将模型和程序烧录到板卡中。

## 9.程序编译

在生成好的模型中可以得到如下三个文件，`model-parameters`是我们新生成模型的参数和匹配关键字名称的C函数，需要替换代码工程src目录下的`model-parameters`,`ei_model.bin`就是NDP101芯片所需要的模型数据了。

![image-20220629220035106](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/image-20220629220035106-165660248865415.png)

在替换完成之后使用Arduino IDE从新编译工程并且在on_classification_changed函数中填充自己的应用代码即可,如下所示。

```c
/**
 * @brief      Called when a inference matches 1 of the features
 *
 * @param[in]  event          The event
 * @param[in]  confidence     The confidence
 * @param[in]  anomaly_score  The anomaly score
 */
void on_classification_changed(const char *event, float confidence, float anomaly_score) {
    Serial.println(event);
    Serial2.println(event);

    
    // here you can write application code, e.g. to toggle LEDs based on keywords
    if (strcmp(event, "source_close") == 0) {
        cmd_data = CLOSE_CMD;
        // Serial.println("source_close lable");
    }

    if (strcmp(event, "source_open") == 0) {
        cmd_data = OPEN_CMD;
        // Serial.println("source_open lable");
    }

    if (strcmp(event, "source_auto") == 0) {
        cmd_data = AUTO_CMD;
        // Serial.println("source_auto lable");
    }

    if (strcmp(event, "source_sleep") == 0) {
        cmd_data = SLEEP_CMD;
        // Serial.println("source_sleep lable");
    }

   /*PRINTF*/
    Serial.println("---------------->");
    switch (cmd_data)
    {
    case OPEN_CMD:
        Serial.println("source_open lable");
        break;
    case CLOSE_CMD:
        Serial.println("source_close lable");
        break;
    case AUTO_CMD:
        Serial.println("source_auto lable");
        break;
    case SLEEP_CMD:
        Serial.println("source_sleep lable");
        break;
    default:
        break;
    }
    cmd_data =  NULL_CMD;
}

```

注：

在首次导入模型代码时候编译可能会报错，这是因为EDGE平台生成的代码`model_variables.h`文件中有两段代码未定义屏蔽报错的代码段或者修改为如下格式即可：

```c
/* Generated by Edge Impulse
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _EI_CLASSIFIER_MODEL_VARIABLES_H_
#define _EI_CLASSIFIER_MODEL_VARIABLES_H_

#include <stdint.h>
#include "model_metadata.h"
// #include "edge-impulse-sdk/classifier/ei_model_types.h"

const char* ei_classifier_inferencing_categories[] = { "source_auto", "source_close", "source_open", "source_sleep", "z_openset" };

uint8_t ei_dsp_config_66_axes[] = { 0 };
const uint32_t ei_dsp_config_66_axes_size = 1;
ei_dsp_config_audio_syntiant_t ei_dsp_config_66 = {
    1,
    1,
    0.032f,
    0.024f,
    40,
    512,
    0,
    0,
    0.96875f
};
// const ei_model_performance_calibration_t ei_calibration = {
//     1, /* integer version number */
//     (int32_t)(EI_CLASSIFIER_RAW_SAMPLE_COUNT / ((EI_CLASSIFIER_FREQUENCY > 0) ? EI_CLASSIFIER_FREQUENCY : 1)) * 1000, /* Model window */
//     0.8f, /* Default threshold */
//     (int32_t)(EI_CLASSIFIER_RAW_SAMPLE_COUNT / ((EI_CLASSIFIER_FREQUENCY > 0) ? EI_CLASSIFIER_FREQUENCY : 1)) * 500, /* Half of model window */
//     0   /* Don't use flags */
// };

#endif // _EI_CLASSIFIER_MODEL_METADATA_H_

```





## 10.板卡下载过程分析

官方Demo中模型下载是个十分有意思的过程，我特地分析了他的代码并且绘制了下面的流程图。整个流程分为了两个部分，通过串口RST板子让板子引导进入程序下载模式下载我们的代码程序，在程序下载成功并且成功运行之后延时一段时间并且发送模型下载串口指令，串口相应之后就开始分块去搬移模型数据（每次搬移1K），在搬移的过程中不断的通过SPI将数据写入Flash中。搬移成功之后通过串口回复给上位机，上位机即显示下载成功。如果有有兴趣可以自己去了解下`syntiant.cpp`文件中的`runManagementCommand()`函数，系统就是调用这个函数完成了模型的下载。

![分析.drawio](https://raw.githubusercontent.com/songzhishuo/blog-img/main/img/%E5%88%86%E6%9E%90.drawio-165660249447716.png)



## 11.ESP8266 部分代码

ESP82666 主要是MQTT通信功能的代码，这里直接将代码副出：

```cpp
#include <Arduino.h>

/*WIFI*/
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

/*WIFI参数*/
#define WLAN_SSID "JDCxxxxx89E"
#define WLAN_PASS "199xxxxx614"

/*服务器参数*/
#define AIO_SERVER "192.168.68.210"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME ""
#define AIO_KEY ""
#define AIO_CID "ESP"

WiFiClient client;
/*实例化mqtt设备*/
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_CID, AIO_USERNAME, AIO_KEY);
/*发布topic报文*/
Adafruit_MQTT_Publish mqtt_pub1 = Adafruit_MQTT_Publish(&mqtt, "office/sensor1");
/*订阅topic报文*/
Adafruit_MQTT_Subscribe mqtt_sub1 = Adafruit_MQTT_Subscribe(&mqtt, "/feeds", MQTT_QOS_0);

static void mqttcallback(char *data, uint16_t len);
static void wifi_connect();
static void mqtt_init();
static void MQTT_connect();

void setup()
{
    Serial.begin(115200);       // 设置串口波特率为115200
    wifi_connect();             //wifi连接
    //smartConfig();
    mqtt_init();                //mqtt连接初始化

}

void loop()
{
    /*MQTT 连接*/
    MQTT_connect();
    mqtt.processPackets(1000); //获取主题


    if(digitalRead(14) == LOW)      /*关闭模式*/
    {
      mqtt_pub1.publish("0");
      Serial.println("0");
    }

    if(digitalRead(16) == LOW)      /*开启模式*/
    {
      mqtt_pub1.publish("1");
      Serial.println("1");
    }    

    if(digitalRead(15) == LOW)      /*微风模式*/
    {
      mqtt_pub1.publish("2");
      Serial.println("2");
    }

    if(digitalRead(13) == LOW)      /*大风模式*/
    {
      mqtt_pub1.publish("3");
      Serial.println("3");
    }

    delay(200);
}

void smartConfig()
{
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig");
  delay(2000);
  // 等待配网
  WiFi.beginSmartConfig();
 
 while (1)
  {
    Serial.print(".");
    delay(500);
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      WiFi.setAutoConnect(true);  // 设置自动连接
      break;
    }
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void wifi_connect()
{
  Serial.println(F("Adafruit MQTT demo"));
  // Connect to WiFi access point.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    mqtt.disconnect();
    delay(10000); // wait 10 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}

void mqtt_init()
{
  mqtt_sub1.setCallback(mqttcallback);
  // Setup MQTT subscription for time feed.
  mqtt.subscribe(&mqtt_sub1);
  // function definition further below.
  MQTT_connect();
  mqtt.processPackets(1000);
}

/**
 * @brief mqtt接收回调
 * 
 * @param data 
 * @param len 
 */
void mqttcallback(char *data, uint16_t len)
{
  Serial.print("Hey we're in a onoff callback, the button value is: ");
  Serial.println(data);
}
```

# 总结

之前一直觉得TinyML是一个神秘莫测十分高大上的东西，通过这次活动对深度学习、TinyML等有了更深刻的了解。恰巧最近公司项目中有一个需要场景识别的需求，后续我也将采用ESP32作为主控尝试引入TinyML。syntiant NDP101 板卡虽然功能强大，但是受限于资料不完全开源，目前我们还是不能了解到其完整的使用方法。希望后续能开放其低层资料，让大家来学习和使用，毕竟这么强一块板卡让更多Geek用起来才能发挥其强劲的性能。目前来看syntiant 的Arduino Demo程序仅仅是一个框架性质的Demo，没有办法真的用于项目，math匹配函数在实际运行时候，作为死循环运行在低层，这点对新手来说十分的不友好，希望后续官方更进一步优化Arduino下的代码框架。

# 视频

https://www.bilibili.com/video/BV1KB4y1B7fb?share_source=copy_web

# 参考

[Harrypotter-zhs/Syntiant_Tinyml_test: Learning Arduino, Deep learning (github.com)](https://github.com/Harrypotter-zhs/Syntiant_Tinyml_test)

[Syntiant Tiny ML Board - Edge Impulse Documentation](https://docs.edgeimpulse.com/docs/development-platforms/officially-supported-mcu-targets/syntiant-tinyml-board)
