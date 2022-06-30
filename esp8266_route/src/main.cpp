#include <Arduino.h>

/*WIFI*/
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>


/*WIFI参数*/
#define WLAN_SSID "JDCwifi_B89E"
#define WLAN_PASS "199709183614"

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

    if(digitalRead(5) == LOW)      /*微风模式*/
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