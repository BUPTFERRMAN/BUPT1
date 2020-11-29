//先引入ESP8266和dht11的头文件，前者为了WiFi模块后者为了温湿度传感
//引入ESP8266.h头文件，建议使用教程中修改后的文件
#include "ESP8266.h"
#include "dht11.h"
#include "SoftwareSerial.h"


#define SSID "gzj"    //填写2.4GHz的WIFI名称，不要使用校园网
#define PASSWORD "12345678"//填写自己的WIFI密码
#define HOST_NAME "api.heclouds.com"  //API主机名称，连接到OneNET平台，无需修改
#define DEVICE_ID "642788433"       //填写自己的OneNet设备ID
#define HOST_PORT (80)                //API端口，连接到OneNET平台，无需修改
String APIKey = "64XMYp0Sf2gd8yIxMrGASfzwscQ="; //与设备绑定的APIKey

#define INTERVAL_SENSOR 5000 //定义传感器采样及发送时间间隔

//配置oled显示所需头文件
#include <IRremote.h>
#include<Wire.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(128,64,&Wire,OLED_RESET);

//定义超声波所需变量和引脚
const int TrigPin = 2; //发送超声波
const int EchoPin = 3; //接收超声波
unsigned int DisMin = 6.5;    //亮灯距离
unsigned int DisMax = 4.5;   //报警距离
int RECV_PIN = 11;//红外接收器的引脚
double DisBey=500;//过近距离
float cm;//距离变量

dht11 DHT11;
//定义DHT11接入Arduino的管脚
#define DHT11PIN 26

//定义ESP8266所连接的软串口
/*********************
 * 该实验需要使用软串口
 * Arduino上的软串口RX定义为D3,
 * 接ESP8266上的TX口,
 * Arduino上的软串口TX定义为D2,
 * 接ESP8266上的RX口.
 * D3和D2可以自定义,
 * 但接ESP8266时必须恰好相反
 *********************/
SoftwareSerial mySerial3(53,52);
ESP8266 wifi(mySerial3);

//定义红外遥控器按键相对应
long okey = 0x9716BE3F;
long tkey = 0x3D9AE3F7;
long skey = 0x6182021B;
long fkey = 0x488F3CBB;
long fikey = 0x499E79F;
long sikey = 0x449E79F;
long sekey = 0x32C6FDF7;
long eikey = 0x1BC0157B;
long nikey = 0x3EC3FC1B;
IRrecv irrecv(RECV_PIN);
decode_results results;

//定义有关按钮的变量和引脚，实现按下亮灯，再按关灯
const int buttonPin = 24;
const int ledPin = 22;
// 按键前一个状态
int oldButtonState = HIGH;
// 按键状态
int buttonState = HIGH;
// led灯状态，false->没亮，true->亮
boolean ledState = false;



void setup()//只进行一次的函数
{
  //超声波
Serial.begin(115200);
pinMode(TrigPin, OUTPUT);//发射超声波
pinMode(EchoPin, INPUT);//接受超声波
 pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
 //超声波控制led和蜂鸣器
 //彩色led灯的管脚
  pinMode(10,OUTPUT);//fengmingqi
  pinMode(6,OUTPUT);//GREEN
  pinMode(5,OUTPUT);//BLUE
  pinMode(8,OUTPUT);//red
 //红外函数
  irrecv.enableIRIn(); // 启动红外解码
//按钮函数
    pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
//WiFi连接
   Serial.print("setup begin\r\n");
mySerial3.begin(115200); //初始化软串口
  //以下为ESP8266初始化的代码
  //接入WiFi实现上传数据
  Serial.print("FW Version: ");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStation()) {
    Serial.print("to station ok\r\n");
  } else {
    Serial.print("to station err\r\n");
  }

  //ESP8266接入WIFI
  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  Serial.println("");
  Serial.print("DHT11 LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);

  mySerial3.println("AT+UART_CUR=9600,8,1,0,0");
  mySerial3.begin(9600);
  Serial.println("setup end\r\n");
}
unsigned long net_time1 = millis(); //数据上传服务器时间

//通过红外用遥控器控制oled显示不同内容
void hongwai(){
    if (irrecv.decode(&results))
   {if (results.value==okey)
    { 
    oled(1);
    }
    if (results.value==tkey)
    { 
   oled(2);
    }
    irrecv.resume(); 
   }
  }
//oled屏要显示的内容
void oled(int m){
  Adafruit_SSD1306 display(128,64,&Wire,OLED_RESET);
  static const unsigned char PROGMEM str1[]=
{0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x20,0x0E,0x60,0x11,0xA0,0x09,0x28,0x11,0x30,
0x3F,0x20,0x25,0x10,0x65,0x10,0x59,0xE8,0x01,0x16,0x01,0x06,0x00,0x01,0x00,0x00/*"砥",0*/};
static const unsigned char PROGMEM str2[]=
{0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xB8,0x1C,0xC0,0x08,0x80,0x10,0xFC,0x1E,0xA0,
0x15,0x20,0x35,0x3C,0x59,0x44,0x02,0x44,0x02,0x88,0x04,0x18,0x00,0x00,0x00,0x00/*"砺",1*/};
static const unsigned char PROGMEM str3[]=
{0x00,0x00,0x00,0x20,0x04,0x20,0x02,0x40,0x00,0x7E,0x7F,0x90,0x00,0x10,0x1E,0x50,
0x12,0x50,0x1E,0x50,0x12,0x50,0x1E,0x50,0x12,0x08,0x06,0x38,0x02,0x10,0x00,0x00/*"前",2*/};
static const unsigned char PROGMEM str4[]=
{0x00,0x00,0x00,0x00,0x04,0x00,0x08,0x38,0x10,0x40,0x24,0x00,0x08,0x1E,0x0B,0xE0,
0x18,0x20,0x28,0x20,0x48,0x20,0x08,0x20,0x08,0x20,0x08,0x60,0x00,0x20,0x00,0x00/*"行",3*/};
  static const unsigned char PROGMEM str5[]=
{0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x98,0x1F,0x6C,0x20,0x08,0x20,0x00,0x01,0xF0,
0x0E,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x03,0x80,0x01,0x00,0x00,0x00/*"宁",0*/};
static const unsigned char PROGMEM str6[]=
{0x00,0x00,0x08,0x40,0x04,0x40,0x07,0x78,0x08,0x90,0x1D,0x28,0x0A,0xFC,0x75,0x2E,
0x1B,0xF8,0x1A,0x28,0x12,0xF0,0x1A,0x20,0x22,0x20,0x24,0x20,0x00,0x60,0x00,0x00/*"静",1*/};
static const unsigned char PROGMEM str7[]=
{0x00,0x00,0x00,0x00,0x00,0x20,0x07,0x20,0x1C,0x40,0x08,0x40,0x0A,0xF8,0x1E,0xA0,
0x29,0x20,0x0A,0xA0,0x1C,0x60,0x08,0x20,0x0C,0x50,0x71,0x9C,0x00,0x00,0x00,0x00/*"致",2*/};
static const unsigned char PROGMEM str8[]=
{0x00,0x00,0x00,0x00,0x10,0x20,0x09,0xC0,0x00,0x00,0x00,0x78,0x13,0xC0,0x11,0x40,
0x11,0x40,0x11,0x48,0x12,0x3C,0x38,0x00,0x07,0x80,0x00,0x7E,0x00,0x00,0x00,0x00/*"远",3*/};
static const unsigned char PROGMEM str9[]=
{0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xB0,0x07,0xC0,0x08,0x80,
0x05,0x00,0x03,0x00,0x01,0x00,0x02,0x80,0x04,0x40,0x18,0x30,0x00,0x1C,0x00,0x00/*"丈",0*/};
  static const unsigned char PROGMEM str10[]=
{0x00,0x00,0x00,0x00,0x07,0xF0,0x07,0xA0,0x04,0x40,0x07,0x80,0x07,0xFE,0x38,0x60,
0x0F,0xB0,0x0B,0x20,0x05,0xC0,0x03,0x00,0x07,0xC0,0x01,0x00,0x3F,0xF8,0x00,0x00/*"量",1*/};
static const unsigned char PROGMEM str11[]=
{0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x01,0x0C,
0x21,0x0C,0x11,0x08,0x11,0x78,0x1E,0x88,0x30,0x08,0x00,0x08,0x00,0x00,0x00,0x00/*"山",2*/};
static const unsigned char PROGMEM str12[]=
{0x00,0x00,0x00,0x00,0x18,0x00,0x08,0x1E,0x07,0xF0,0x00,0x10,0x31,0xD0,0x02,0x50,
0x02,0xD0,0x0B,0x10,0x10,0x10,0x10,0x10,0x20,0x10,0x00,0x30,0x00,0x00,0x00,0x00/*"河",3*/};

   display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(35,5);
  if(m==1){
   
    display.drawBitmap(32,32,str1,16,16,1);
    display.drawBitmap(48,32,str2,16,16,1);
      display.drawBitmap(64,32,str3,16,16,1);
        display.drawBitmap(80,32,str4,16,16,1);
        display.display();}//砥砺前行
     if(m==2){
   display.drawBitmap(32,32,str5,16,16,1);
    display.drawBitmap(48,32,str6,16,16,1);
      display.drawBitmap(64,32,str7,16,16,1);
        display.drawBitmap(80,32,str8,16,16,1);
        display.display();//宁静致远
          }
          if(m==3){
               display.drawBitmap(32,32,str9,16,16,1);
    display.drawBitmap(48,32,str10,16,16,1);
      display.drawBitmap(64,32,str11,16,16,1);
        display.drawBitmap(80,32,str12,16,16,1);
         display.display();//丈量山河
            }
}
//定义按钮函数，控制led灯的亮灭
void button(){ oldButtonState = buttonState;
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW && oldButtonState == HIGH) {
    if(ledState == true) {
      digitalWrite(ledPin, LOW);
 
    } else {
      digitalWrite(ledPin, HIGH);
  
    }
    ledState = !ledState;
  }}
  //定义超声波函数，不同距离不同反应
void distance(){
  // 产生一个10us的高电平脉冲去触发TrigPin
digitalWrite(TrigPin, LOW);
delayMicroseconds(2);
digitalWrite(TrigPin, HIGH);
delayMicroseconds(10);
digitalWrite(TrigPin, LOW);
// 检测脉冲宽度，并计算出距离
cm = pulseIn(EchoPin, HIGH) / 58.0;
 if (cm >= DisMin) {         //小于亮灯门限值
   analogWrite(5,20);
   digitalWrite(6,LOW);
   digitalWrite(10,LOW);
digitalWrite(8,LOW);
      }
    else if (cm > DisMax) {   //小于报警门限
   digitalWrite(5,LOW);
   analogWrite(6,20);
   digitalWrite(10,LOW); 
  digitalWrite(8,LOW);
       }
    else{           //发光二极管亮度最大值，并启动声音报警
   digitalWrite(6,LOW);
   digitalWrite(5,LOW);
   digitalWrite(10,HIGH);
   digitalWrite(8,HIGH);
        }
        if(cm>=DisBey){
          digitalWrite(6,LOW);
   digitalWrite(5,LOW);
   digitalWrite(10,HIGH);
   digitalWrite(8,HIGH);
   }
//通过串口监视器输出
Serial.print("Distance:"); Serial.println(cm);
delay(1000);}


void loop()
{
  //运行红外，超声波与按钮三个函数
hongwai();
distance();
button();

//以下为上传数据至onenet
 if (net_time1 > millis())
    net_time1 = millis();

  if (millis() - net_time1 > INTERVAL_SENSOR) //发送数据时间间隔
  {

    int chk = DHT11.read(DHT11PIN);

    Serial.print("Read sensor: ");
    switch (chk) {
      case DHTLIB_OK:
        Serial.println("OK");
        break;
      case DHTLIB_ERROR_CHECKSUM:
        Serial.println("Checksum error");
        break;
      case DHTLIB_ERROR_TIMEOUT:
        Serial.println("Time out error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
    float sensor_dis=cm;
    float sensor_hum = (float)DHT11.humidity;
    float sensor_tem = (float)DHT11.temperature;
    Serial.print("Humidity (%): ");
    Serial.println(sensor_hum, 2);
Serial.print("Distance(%):");
Serial.println(sensor_dis,2);
    Serial.print("Temperature (oC): ");
    Serial.println(sensor_tem, 2);
    Serial.println("");

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
      Serial.print("create tcp ok\r\n");
      char buf[10];
      //拼接发送data字段字符串
      String jsonToSend = "{\"Temperature\":";
      dtostrf(sensor_tem, 1, 2, buf);
      jsonToSend += "\"" + String(buf) + "\"";
      jsonToSend += ",\"Humidity\":";
      dtostrf(sensor_hum, 1, 2, buf);
      jsonToSend += "\"" + String(buf) + "\"";
        jsonToSend += ",\"Distance\":";
      dtostrf(sensor_dis, 1, 2, buf);
      jsonToSend += "\"" + String(buf) + "\"";
      jsonToSend += "}";

      //拼接POST请求字符串
      String postString = "POST /devices/";
      postString += DEVICE_ID;
      postString += "/datapoints?type=3 HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "Content-Length:";
      postString += jsonToSend.length();
      postString += "\r\n";
      postString += "\r\n";
      postString += jsonToSend;
      postString += "\r\n";
      postString += "\r\n";
      postString += "\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
      Serial.println("send success");
      if (wifi.releaseTCP()) { //释放TCP连接
        Serial.print("release tcp ok\r\n");
      } else {
        Serial.print("release tcp err\r\n");
      }
      postArray = NULL; //清空数组，等待下次传输数据
    } else {
      Serial.print("create tcp err\r\n");
    }

    Serial.println("");

    net_time1 = millis();
  }

}
