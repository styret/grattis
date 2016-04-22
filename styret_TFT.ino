#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

//https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>

//https://github.com/adafruit/DHT-sensor-library
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

char hum[6];
char temp[6];

//https://github.com/sumotoy/TFT_ILI9163C/tree/Pre-Release-1.0r6
#include <SPI.h>
#include <TFT_ILI9163C.h>
#include "_fonts/unborn_small.c"

#define __CS  16  //(D0)
#define __DC  5   //(D1)
#define __RST 4   //(D2)
/*
 SCLK:D5
 MOSI:D7
*/
TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC, __RST);


void configModeCallback (WiFiManager *myWiFiManager)
{
  tft.clearScreen();
  tft.setCursor(0, 10);
  tft.print("Connect to:");
  tft.println(myWiFiManager->getConfigPortalSSID().c_str());

  tft.setCursor(0, 25);
  tft.println("and then go to");
  tft.println(WiFi.softAPIP().toString().c_str());
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting");

  tft.begin();
  tft.setRotation(2);

  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
  
  wifiManager.setTimeout(180); //so if it restarts and router is not yet online, it keeps rebooting and retrying to connect
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    tft.print("Timeout");
    delay(5000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  tft.clearScreen();
  tft.print("IP: ");
  tft.println(WiFi.localIP().toString().c_str());

  delay(5000);
  tft.clearScreen();

  tft.setCursor(43, 10);
  tft.print("Grattis pa");
  tft.setCursor(35,20);
  tft.println("Fodelsedagen");

  // Start the server
  server.on("/", [](){
    server.send(200, "text/plain", "T:" + String(temp) + "C  RH:" + String(hum) + "%");
  });
  server.begin();
}

void loop(void)
{
  delay(2000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

//  if (isnan(humidity) || isnan(temperature)) {
//    Serial.println("Failed to read from DHT sensor!");
//    return;
//  }

  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.println(temperature, 1);

  dtostrf(humidity, 3, 0, hum);
  dtostrf(temperature, 3, 0, temp);

  tft.setCursor(0, 50);
  tft.setFont(&unborn_small);//this will load the font
  tft.setTextScale(1);//smaller

  tft.setTextColor(BLUE, BLACK); //set a background inside font
  tft.println("Luftfuktighet: ");
  tft.setTextColor(RED, BLACK); //set a background inside font
  tft.print(hum);
  tft.println(" %");
  tft.println("");

  tft.setTextColor(BLUE, BLACK); //set a background inside font
  tft.println("Temp: ");
  tft.setTextColor(RED, BLACK); //set a background inside font
  tft.print(temp);
  tft.println(" *C ");
  tft.setTextColor(BLUE, BLACK); //set a background inside font


  server.handleClient();
}
