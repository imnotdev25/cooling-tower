// Thanks https://github.com/electronicsguy/HTTPSRedirect For his Httpsredirect Lib

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>
#include "HTTPSRedirect.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D6
// DHT dht(DHTPin, DHTTYPE); //--> Initialize DHT sensor, DHT dht(Pin_used, Type_of_DHT_Sensor);
#define SENSOR D8
#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router


DHT dht1(D4, DHT11);
DHT dht2(D3, DHT11);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float t_3, t_4, t_1, h_1, t_2, h_2;
String string_temperaturein, string_humidityin, string_temperatureout, string_humidityout;
//t_1 , h_1 , t_2 , h_2 ;    // hot & cool water temp , (t_1 , h_1 , t_2 , h_2 ) defined as global variable
uint8_t sensor1[8] = { 0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC };  // 64-bit addresses
uint8_t sensor2[8] = { 0x28, 0x61, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27 };

const char *ssid = "wifi";
const char *password = "11111111";
const char *host = "script.google.com";
const int httpsPort = 443;

long currentMillis = 0;  // Flow Sensor
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres;
float totalLitres;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}
//----------------------------------------

WiFiClientSecure client;  //--> Create a WiFiClientSecure object.

String GAS_ID = " ";  //--> spreadsheet script ID

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);
  pinMode(SENSOR, INPUT_PULLUP);
  sensors.begin();
  dht1.begin();
  dht2.begin();  // Start reading DHT11 sensors

  pulseCount = 0;  // Start Reading Flow Sensors
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
  delay(500);

  WiFi.begin(ssid, password);  //--> Connect to your WiFi router
  Serial.println("");

  pinMode(ON_Board_LED, OUTPUT);  //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH);
  //Status LED
  pinMode(ON_Board_LED, OUTPUT);  //--> On Board LED port Direction output
  //pinMode(16, OUTPUT);            //D0 White
  //pinMode(5, OUTPUT);             //D1 RED
  //pinMode(4, OUTPUT);             //D2 GREEN
  //digitalWrite(ON_Board_LED, HIGH);  //--> Turn off Led On Board
  //--> Turn off Led On Board

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH);  //--> Turn off the On Board LED when it is connected to the wifi router.
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  client.setInsecure();
}

void loop() {
  //                                               Reading temperature or humidity takes about 250 milliseconds!
  //                                               Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h_1 = dht1.readHumidity();
  //                                               Read temperature as Celsius (the default)
  float t_1 = dht1.readTemperature();
  float h_2 = dht2.readHumidity();
  float t_2 = dht2.readTemperature();
  //sensors.requestTemperatures();
  t_3 = sensors.getTempC(sensor1);
  t_4 = sensors.getTempC(sensor2);
  // string_temperaturein = String(t_1);
  digitalWrite(16, LOW);
  delay(1000);

  // flow meter
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {

    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    flowMilliLitres = (flowRate / 60) * 1000;
    flowLitres = (flowRate / 60);

    totalMilliLitres += flowMilliLitres;
    totalLitres += flowLitres;
    // float flow = flowRate;
    // int vol = totalLitres;
  }

  // Check if any reads failed and exit early (to try again).
  if (isnan(h_1) || isnan(t_1)) {
    Serial.println("Failed to read from DHT in sensor (IN)!");
    delay(1500);
    return;
  }
  if (isnan(h_2) || isnan(t_2)) {
    Serial.println("Failed to read from DHT Out sensor (OUT)!");
    delay(1500);
    return;
  }
  String Temp_1 = "Temperature Inlet : " + String(t_1) + " °C";
  String Humi_1 = "Humidity Inlet : " + String(h_1) + " %";
  String Temp_2 = "Temperature Outlet : " + String(t_2) + " °C";
  String Humi_2 = "Humidity Outlet : " + String(h_2) + " %";
  String Temp_3 = "Temperature Hot water :" + String(t_3) + " °C";
  String Temp_4 = "Temperature Cold water :" + String(t_4) + " °C";
  String Flow = "Flowrate: " + String(flowRate) + " L/Hr ";
  String Vol = "Volume:" + String(totalLitres) + "L";
  Serial.println(Temp_1);
  Serial.println(Humi_1);
  Serial.println(Temp_2);
  Serial.println(Humi_2);
  Serial.println(Temp_3);
  Serial.println(Temp_4);
  Serial.println(Flow);
  Serial.println(Vol);
  // String tem_2 = t_2
  //sendData();
  // sendData(t_2, h_2);
  //\//--> Calls the sendData Subroutine
  ///}

  // Subroutine for sending data to Google Sheets
  //void sendData()
  //{
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);

  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  //----------------------------------------Processing data and sending data

  //String string_temperaturein = String(t_1);
  //String string_humidityin = String(h_1, DEC);
  //String string_temperatureout = String(t_2);
  //String string_humidityout = String(h_2, DEC);
  //String string_flow = String(flowRate);
  //String string_vol = String(totalLitres);
  String url = "/macros/s/" + GAS_ID + "/exec?temperaturein=" + String(t_1) + "&humidityin=" + String(h_1, DEC) + "&temperatureout=" + String(t_2) + "&humidityout=" + String(h_2, DEC) + "&temphot=" + String(t_4) + "&tempcold=" + String(t_3);
  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n" + "Connection: close\r\n\r\n");
  Serial.println("request sent");
  //----------------------------------------

  //---------------------------------------->>>>=========Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
    digitalWrite(4, LOW);
    delay(250);
    digitalWrite(4, HIGH);
    delay(250);
  } else {
    Serial.println("esp8266/Arduino CI has failed");
    digitalWrite(5, LOW);
    delay(250);
    digitalWrite(5, HIGH);
    delay(250);
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
}
