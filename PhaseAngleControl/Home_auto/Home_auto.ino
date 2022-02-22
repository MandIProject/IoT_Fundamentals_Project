//required libraries
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ThingSpeak.h>
#include <DHT.h>

//connected pins
const uint8_t bulb_relay = D5, DHT11_pin = D3, flame_sens = D6, motor_pwm = D1, motor_enable = D2, motor_relay = D0;

DHT dht(DHT11_pin, DHT11);
WidgetLED led1(V8);
WidgetLCD lcd(V7);
BlynkTimer timer, timer1;
WiFiClient client;

//all the setup parameters
const char* ssid = "Wifi_2ghz";
const char* password = "Voidsetup()1";
const char* auth = "EbSeLGn_3cZc8-5ZSBKVWcc6GaQ467Oj";
const char* WriteAPI = "QHSWWY6XX89FDHXN";
const uint16_t ChannelNumber = 1525831;
bool Connected2Blynk = false;

//some required variables
float humidity, temp_f, temp_c, ppm;
unsigned long previousMillis = 0;
const long interval = 2000;

void setup()//initializing all the pins, the sensors and the timer event
{
  Serial.begin(115200);
  pinMode(motor_enable, OUTPUT);
  digitalWrite(motor_enable, HIGH);
  pinMode(flame_sens, INPUT);
  pinMode(bulb_relay, OUTPUT);
  pinMode(motor_relay, OUTPUT);
  pinMode(DHT11_pin, INPUT);
  dht.begin();
  digitalWrite(bulb_relay, HIGH);
  digitalWrite(motor_relay, HIGH);
  timer.setInterval(2000L, gettemperature_AirQ);//runs the function every 2s to get the sensor values
  timer1.setInterval(11000L, CheckConnection);//checks for connection validity every 11s
  Wifi_Connect();//connecting to the wifi network
  lcd.clear();
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

  ThingSpeak.begin(client);//connecting to the thingspeak client
}

void CheckConnection()//connection validity check
{
  Connected2Blynk = Blynk.connected();
  if(!Connected2Blynk)
  {
    Wifi_Connect();  
  }
}

BLYNK_WRITE(V5)//getting the motor speed level from the blynk app
{
  uint8_t slider_value = param.asInt();
  analogWrite(motor_pwm, slider_value);
}

void loop()
{
  if(Connected2Blynk)
  {
    Blynk.run();
    timer.run();
  }

  timer1.run();
}

void gettemperature_AirQ() { //method to read the sensor values and write them to the blynk cloud and thingspeak cloud
  // Wait at least 2 seconds seconds between measurements.
  temp_c = dht.readTemperature(false); // Read temperature as Celsius
  humidity = dht.readHumidity(); // Read humidity (percent)
  ppm = analogRead(A0);
  ThingSpeak.setField(1, (int)temp_c);
  ThingSpeak.setField(2, (int)humidity);
  ThingSpeak.setField(3, ppm);
  int x = ThingSpeak.writeFields(ChannelNumber, WriteAPI);
  Serial.println(temp_c);
  Serial.println(humidity);
  Serial.println(ppm);
  Blynk.virtualWrite(V2, (int)temp_c);
  Blynk.virtualWrite(V3, (int)humidity);
  Blynk.virtualWrite(V4, ppm);
  if(ppm >=400){
    led1.on();
    lcd.print(0,0,"Smoke or leakage!");
  }
  if(digitalRead(flame_sens) == LOW){//checking for fire, if there is a fire then shuts off all fans and lights, cuts off the power
    led1.on();
    lcd.print(0,0,"FIRE!");
    digitalWrite(bulb_relay, HIGH);
    digitalWrite(motor_relay, HIGH);
    analogWrite(motor_pwm, 0);
  }
  else{
    led1.off();
    lcd.clear();
  }
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp_c) || isnan(ppm)) {
  Serial.println("Failed to read from sensor!");
  return;
  }
}
