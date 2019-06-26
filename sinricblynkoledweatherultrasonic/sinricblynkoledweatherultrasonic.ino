#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  get it from https://github.com/Links2004/arduinoWebSockets/releases 
#include <ArduinoJson.h> // get it from https://arduinojson.org/ or install via Arduino library manager
#include <StreamString.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <OneWire.h>
#include<DallasTemperature.h> 
#include <ESP8266HTTPClient.h>


ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "xxxxx" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "xxxx" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "xxxx" // TODO: Change to your Wifi network password
#define MYPIN1 14 //D5
#define MYPIN2 2 //D4
#define MYPIN3 3 //RX
#define MYPIN4 0  //D3
#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BLYNK_PRINT Serial
const int pingPin = 12; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 13; // Echo Pin of Ultrasonic Sensor
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//For Weather
String Location = "Mumbai,IN";
String API_Key  = "xxxx";
int count =1;

/* Blynk credentials */
char auth[] = "xxxx";
/* TIMER */

SimpleTimer timer;


#define ONE_WIRE_BUS 15   // DS18B20 on arduino pin2 corresponds to D7 on physical board
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temp;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;
int x,y;
void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void turnOn(String deviceId) {
  if (deviceId == "xxxx") // Device ID of first device
  {  
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    pinMode(MYPIN1, HIGH);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 0);
  // Display static text
    display.println("Light 1 is : ON     ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display(); 
  } 
  else if (deviceId == "xxxx") // Device ID of second device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    pinMode(MYPIN2, HIGH);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 10);
  // Display static text
    display.println("Light 2 is : ON     ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display(); 
  }
  else if (deviceId == "xxx") // Device ID of second device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    pinMode(MYPIN3, HIGH);
    //display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 20);    
  // Display static text
    display.println("Light 3 is : ON      ");
    display.setCursor(0, 40);
    display.println("                    ");    
    display.display(); 
  }
  else if (deviceId == "xxxx") // Device ID of second device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    pinMode(MYPIN4, HIGH);
   // display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 30);
  // Display static text
    display.println("Light 4 is : ON     ");
    display.setCursor(0, 40);
    display.println("                    ");    
    display.display(); 
  }
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == "xxxx") // Device ID of first device
   {  
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     pinMode(MYPIN1, LOW);
    display.setTextSize(1);  
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 0);
  // Display static text
    display.println("Light 1 is : OFF    ");
    display.setCursor(0, 40);
    display.println("                    ");    
    display.display();      
   }
   else if (deviceId == "xxxx") // Device ID of second device
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     pinMode(MYPIN2, LOW);
   // display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 10);
  // Display static text
    display.println("Light 2 is : OFF    ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display();      

  }
  else if (deviceId == "xxxx") // Device ID of second device
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     pinMode(MYPIN3, LOW);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 20);
  // Display static text
    display.println("Light 3 is : OFF     ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display(); 
  }
  else if (deviceId == "xxxx") // Device ID of second device
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     pinMode(MYPIN4, LOW);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 30);
  // Display static text
    display.println("Light 4 is : OFF    ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display(); 
  }
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId);    
  }
}
void ultra() {
   long duration, inches, cm;
   pinMode(pingPin, OUTPUT);
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   pinMode(echoPin, INPUT);
   duration = pulseIn(echoPin, HIGH);
   inches = microsecondsToInches(duration);
   cm = microsecondsToCentimeters(duration);
   Serial.print(inches);
   Serial.print("in, ");
   Serial.print(cm);
   Serial.print("cm");
   Serial.println();
   delay(100);

  if(cm<=20 &&  count==0 && cm>=3){
    ultraturnon();
    Serial.print("Turning ON");
    delay(200);
    count=1;
  }
  else if(cm<=20 && count==1 && cm>=3) {
    ultraturnoff();    
    Serial.print("Turning OFF");
    delay(200);
    count=0;
  }
}

long microsecondsToInches(long microseconds) {
   return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch or Light device types
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

        // For Light device type
        // Look at the light example in github
          
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if (action == "SetTargetTemperature") {
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            String value = json ["value"];
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}
void ultraturnon() {
    setPowerStateOnServer("xxxx", "ON");           
    Serial.print("Turn on device id: ");
    Serial.println("xxxx");
    pinMode(MYPIN1, HIGH);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 0);
  // Display static text
    display.println("Light 1 is : ON     ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display(); 
    
    setPowerStateOnServer("xxxx", "ON");           
    Serial.print("Turn on device id: ");
    Serial.println("xxxx");
    pinMode(MYPIN2, HIGH);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 10);
  // Display static text
    display.println("Light 2 is : ON     ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display(); 
  
    setPowerStateOnServer("xxxx", "ON");         
    Serial.print("Turn on device id: ");
    Serial.println("xxxx");
    pinMode(MYPIN3, HIGH);
    //display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 20);    
  // Display static text
    display.println("Light 3 is : ON      ");
    display.setCursor(0, 40);
    display.println("                    ");    
    display.display(); 
  
    setPowerStateOnServer("xxxx", "ON");       
    Serial.print("Turn on device id: ");
    Serial.println("xxxx");
    pinMode(MYPIN4, HIGH);
   // display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 30);
  // Display static text
    display.println("Light 4 is : ON     ");
    display.setCursor(0, 40);
    display.println("                    ");    
    display.display(); 
    
  
}

void ultraturnoff() {
    setPowerStateOnServer("xxxx", "OFF");     
     Serial.print("Turn off Device ID: ");
     Serial.println("xxxx");
     pinMode(MYPIN1, LOW);
    display.setTextSize(1);  
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 0);
  // Display static text
    display.println("Light 1 is : OFF    ");
    display.setCursor(0, 40);
    display.println("                    ");    
    display.display();
       
    setPowerStateOnServer("xxxx", "OFF");   
     Serial.print("Turn off Device ID: ");
     Serial.println("xxxx");
     pinMode(MYPIN2, LOW);
   // display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 10);
  // Display static text
    display.println("Light 2 is : OFF    ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display();      

  
    setPowerStateOnServer("xxxx", "OFF");  
     Serial.print("Turn off Device ID: ");
     Serial.println("xxxx");
     pinMode(MYPIN3, LOW);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 20);
  // Display static text
    display.println("Light 3 is : OFF     ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display(); 
  
    setPowerStateOnServer("xxxx", "OFF");
     Serial.print("Turn off Device ID: ");
     Serial.println("xxxx");
     pinMode(MYPIN4, LOW);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 30);
  // Display static text
    display.println("Light 4 is : OFF    ");
    display.setCursor(0, 40);
    display.println("                    ");
    display.display();    
  
}
void setup() {
  Serial.begin(9600);
 if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
 }
  delay(2000);
    display.clearDisplay();
  Blynk.begin(auth, MySSID, MyWifiPassword);
  DS18B20.begin();
  timer.setInterval(1000L, getSendData);
  Serial.println(" ");
  Serial.println("Testing  Sensor data");
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  
timer.setInterval(60000, repeatMe);
  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  timer.run(); // Initiates SimpleTimer
  Blynk.run();
  webSocket.loop();
  delay(100);
  ultra(); 
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");
                  
      }
      
  }   
}
void weather()
{
if (isConnected = true)  //Check WiFi connection status
          {
      HTTPClient http;  //Declare an object of class HTTPClient
 
    // specify request destination
    http.begin("http://api.openweathermap.org/data/2.5/weather?q=" + Location + "&APPID=" + API_Key);  // !!
 
    int httpCode = http.GET();  // send the request
 
    if (httpCode > 0)  // check the returning code
    {
      String payload = http.getString();   //Get the request response payload
 
      DynamicJsonBuffer jsonBuffer(512);
 
      // Parse JSON object
      JsonObject& root = jsonBuffer.parseObject(payload);
      if (!root.success()) {
        Serial.println(F("Parsing failed!"));
        return;
      }
 
      float temp = (float)(root["main"]["temp"]) - 273.15;        // get temperature in °C
      int   humidity = root["main"]["humidity"];                  // get humidity in %
      float pressure = (float)(root["main"]["pressure"]) / 1000;  // get pressure in bar
      float wind_speed = root["wind"]["speed"];                   // get wind speed in m/s
      int  wind_degree = root["wind"]["deg"];                     // get wind degree in °
 
      // print data
      Serial.printf("Out Temp    = %.2f°C\r\n", temp);
      Serial.printf("Humidity    = %d %%\r\n", humidity);
      Serial.printf("Pressure    = %.3f bar\r\n", pressure);
      Serial.printf("Wind speed  = %.1f m/s\r\n", wind_speed);
      Serial.printf("Wind degree = %d°\r\n\r\n", wind_degree);
      display.setTextColor(WHITE, BLACK);
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.printf("Out Temp   : %5.2f C\r\n", temp);
      display.setCursor(0, 10);
      display.printf("Humidity   : %d %%\r\n", humidity);
      display.setCursor(0, 20);
      display.printf("Pressure   : %.3fbar\r\n", pressure);
      display.setCursor(0, 30);
      display.printf("Wind speed : %.1f m/s\r\n", wind_speed);
      display.setCursor(0, 40);
      display.printf("Wind degree: %d", wind_degree);
      display.drawRect(91, 0, 3, 3, WHITE);     // put degree symbol ( ° )
      display.drawRect(97, 40, 3, 3, WHITE);
      display.display();
 
    }
 
    http.end();   //Close connection
 
  }  
  
  }


// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")
void setPowerStateOnServer(String deviceId, String value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}
void repeatMe() {
    display.clearDisplay();
    weather();
}
//eg: setPowerStateOnServer("deviceid", "CELSIUS", "25.0")
void setTargetTemperatureOnServer(String deviceId, String value, String scale) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["action"] = "SetTargetTemperature";
  root["deviceId"] = deviceId;
  
  JsonObject& valueObj = root.createNestedObject("value");
  JsonObject& targetSetpoint = valueObj.createNestedObject("targetSetpoint");
  targetSetpoint["value"] = value;
  targetSetpoint["scale"] = scale;
   
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}
void getSendData()
{
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);
  Serial.println(temp);
  display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 50);
  // Display static text
String StrUno = "Room Temp:  ";
 String StrDos = StrUno + temp ; 
    display.println(StrDos);
    display.display();      
  Blynk.virtualWrite(10, temp); //virtual pin V10
}
