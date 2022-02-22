//required libraries
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//setup parameters
const char* ssid = "Wifi_2ghz";
const char* password = "Voidsetup()1";
const char* auth = "EbSeLGn_3cZc8-5ZSBKVWcc6GaQ467Oj";
byte pinValue;
bool Connected2Blynk = false;

BlynkTimer timer;

void setup() {//initializing the pins and wifi connection
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  timer.setInterval(11000L, CheckConnection);//checks for connection validity every 11s
  Wifi_Connect();//starting connection to the wifi
}

void Wifi_Connect()//Wifi connection method
{
  int timeout = millis()/1000;
  WiFi.begin(ssid,password);//Connecting to the wifi
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if((millis()/1000) > timeout + 3)
    {
      break;
    }
  }

  Blynk.config(auth);//connecting to the blynk cloud
  Connected2Blynk = Blynk.connect(1000);
  timeout = millis()/1000;
  while(Blynk.connect(1000) == false)
  {
    if((millis()/1000) > timeout + 3)
    {
      break;
    }
  }
}

void CheckConnection()//connection validity check
{
  Connected2Blynk = Blynk.connected();
  if(!Connected2Blynk)
  {
    digitalWrite(BUILTIN_LED, HIGH);
    Wifi_Connect();  
  }
}

BLYNK_WRITE(V1)//getting the bulb brightness level from the blynk app
{
  pinValue = param.asInt();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Connected2Blynk)
  {
    digitalWrite(BUILTIN_LED, LOW);
    Blynk.run();
    Serial.write(pinValue);//writing the brightness level value to the serial COM
  }

  timer.run();
}
