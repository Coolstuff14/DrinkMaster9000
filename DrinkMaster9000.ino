/*
MUST USE ESP8266 BOARDS v2.3 and Fauxmo v2.3 for 2nd gen echo
Jacob Lee 2019
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


#define SERIAL_BAUDRATE                 115200
int multi=0;
int multi_amount = 500; //multiplier for half empty bottles default 500ms
fauxmoESP fauxmo;

#define pump 4 //D2 on wemo
bool pour = false;
int pour_time =0;

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------
 
void wifiSetup() {

   //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();

    //Connect
    wifiManager.autoConnect("DrinkMaster9000");
    
    //Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

 
void callback(uint8_t device_id, const char * device_name, bool state) {
  Serial.print("Device "); Serial.print(device_name); 
  Serial.print(" state: ");
  if (state) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }

  if ( (strcmp(device_name, "dm9shot2") == 0) ) {
    if (state) {
      //Pour a shot!
      Serial.println("Pour Sip");
      //Shot 0.5oz 3750ms -- 0.5oz 1250ms
        pour = true;
        pour_time =1250;
    }
  }

  if ( (strcmp(device_name, "dm9drink2") == 0) ) {
    if (state){
       //Pour a drink!
      Serial.println("Pour Drink");
       //Standard 1.5oz 3750ms
        pour = true;
        pour_time =3750;
    }
  }

  if ( (strcmp(device_name, "dm9tall2") == 0) ) {
    if (state){
    //Pour a tall drink!
      Serial.println("Pour Tall Drink");
      //Tall 2.5oz 6250ms
        pour = true;
        pour_time =6250;
    }
  }
  if ( (strcmp(device_name, "dm9half") == 0) ) {
    if (state){
    //Half bottle so add more to run time. ON
      Serial.println("Mupliplier Added! Keep drinking!");
      multi = multi_amount;
    }else{
      //OFF
      Serial.println("Mupliplier Removed! Keep drinking!");
      multi = 0;
    }
  }
}



void pumpit(){
    //Pour it!
    Serial.println("Pouring..");
    Serial.println(pour_time + multi);
    digitalWrite(pump, HIGH);
    delay(pour_time + multi);
    digitalWrite(pump, LOW);
    delay(100);
    pour_time=0;
    pour=false;
    Serial.println("Done!");
}

void calibrate(){
  //Use to calibrate ms to oz 
  if (Serial.available())
  {
   int f = Serial.parseInt();  
   if (f > 0)
   {
     Serial.println(f);
     digitalWrite(pump, HIGH);
     delay(f);
     digitalWrite(pump, LOW); 
     delay(100);
   } 
  }
}


void setup() {
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println("Drink Master 9000");
 
    // Wifi
    wifiSetup();

 
    // Fauxmo
    fauxmo.addDevice("dm9shot");
    fauxmo.addDevice("dm9drink");
    fauxmo.addDevice("dm9tall");
    fauxmo.addDevice("dm9half");
    fauxmo.onMessage(callback); 

    pinMode(pump, OUTPUT);
    digitalWrite(pump, LOW); 
     
}
 
void loop() {
  fauxmo.handle();
  //calibrate();
  if (pour){
    pumpit();
  }
}
