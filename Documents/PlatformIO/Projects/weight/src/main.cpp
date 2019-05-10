
#include <Arduino.h>
#include <DNSServer.h>
#include <ESPUI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "HX711.h"

#define calibration_factor -386.45 // obtain from Calibration code
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 0;

LiquidCrystal lcd(22,23,5,18,19,21);


HX711 scale;

long reading;
int weight;

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;

#if defined(ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

const char *ssid = "ESPUI";
const char *password = "";

long oldTime = 0;
bool switchi = false;



void setup(void) {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Weight in gm=");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the Calibration sketch
  scale.tare();
  /*
    #if defined(ESP32)
    WiFi.setHostname(ssid);
    #else
    WiFi.hostname(ssid);
    #endif
  */

  WiFi.softAP(ssid);
  // WiFi.softAP(ssid, password);
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // change the beginning to this if you want to join an existing network
  /*
     Serial.begin(115200);
     WiFi.begin(ssid, password);
     Serial.println("");
     // Wait for connection
     while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
     }
     Serial.println("");
     Serial.print("IP address: ");
     Serial.println(WiFi.localIP());
  */

  ESPUI.label("Weight in gms:", COLOR_TURQUOISE, "Stop");
  //  ESPUI.label("Millis:", COLOR_EMERALD, "0");
  //  ESPUI.button("Push Button", &buttonCallback, COLOR_PETERRIVER);
  //  ESPUI.button("Other Button", &buttonExample, COLOR_WETASPHALT, "Press");
  //  ESPUI.pad("Pad with center", true, &padExample, COLOR_SUNFLOWER);
  //  ESPUI.pad("Pad without center", false, &padExample, COLOR_CARROT);
  //  ESPUI.switcher("Switch one", false, &switchExample, COLOR_ALIZARIN);
  //  ESPUI.switcher("Switch two", true, &otherSwitchExample, COLOR_NONE);
  //  ESPUI.slider("Slider one", &slider, COLOR_ALIZARIN, "30");
  //  ESPUI.slider("Slider two", &slider, COLOR_NONE, "100");
  //  ESPUI.text("Text Test:", &textCall, COLOR_ALIZARIN, "a Text Field");
  //  ESPUI.number("Numbertest", &numberCall, COLOR_ALIZARIN, 5, 0, 10);

  /*
     .begin loads and serves all files from PROGMEM directly.
     If you want to serve the files from SPIFFS use ESPUI.beginSPIFFS
     (.prepareFileSystem has to be run in an empty sketch before)
  */

  dnsServer.start(DNS_PORT, "*", apIP);

  /*
     Optionally you can use HTTP BasicAuth. Keep in mind that this is NOT a
    SECURE way of limiting access.
     Anyone who is able to sniff traffic will be able to intercept your password
    since it is transmitted in cleartext ESPUI.begin("ESPUI Control", "myuser",
    "mypassword");
  */
  ESPUI.begin("Artiosys IOT Dashboard");
}

void loop(void) {
  dnsServer.processNextRequest();

  if (scale.is_ready()) {
    reading = scale.read();
    weight = scale.get_units(10);
    Serial.print("HX711 reading: ");
    Serial.println(reading);

    Serial.print("Weight in gms: "); // Use the same unit used in Calibration code
    Serial.println(weight);
  }

  ESPUI.print("Weight in gms:", String(weight));
  
//  lcd.clear();
  lcd.setCursor(0,1);

  lcd.print(String(weight));
//  lcd.setCursor(0,1);
//  lcd.println(String(weight));

delay(500);
  


}
