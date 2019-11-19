/*
   Copyright (c) 2019, Nuvem, Inc.
   All rights reserved.
   MIT License
   
   Permission is hereby granted, free of charge, to any person 
   obtaining a copy of this software and associated documentation 
   files (the "Software"), to deal in the Software without restriction, 
   including without limitation the rights to use, copy, modify, 
   merge, publish, distribute, sublicense, and/or sell copies of 
   the Software, and to permit persons to whom the Software is 
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall 
   be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "DHT.h"

// DHT11 Humidity/Temp Sensor
#define DHTTYPE DHT11
// set the DHT11 pin
#define DHTPIN D2               
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

#define LED D5

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "password"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

// server port
ESP8266WebServer server(80);

// flag for device "power"
boolean deviceEnabled = true;
// initial default value of threshold for thermostat 
float tempThreshold = 65.0;

void setup(void) {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(DHTPIN, INPUT);
  
  // initialize LED off
  digitalWrite(LED, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // initialize the DHT-11 sensor
  dht.begin();
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // root url (/)
  server.on("/", handleRoot);
  // handle request to turn on heater
  server.on("/heater/on", actuatorOn);
  // handle request to turn off heater
  server.on("/heater/off", actuatorOff);

  // handle request to turn power on
  server.on("/power/on", actuatorOn);
  // handle request to turn power off
  server.on("/power/off", actuatorOff);

  // request for temperature - GET & POST
  // GET request returns temperature from DHT-11
  server.on("/heater/temp", HTTP_GET, getTemp);

  // POST request sets the value for the temp on/off threshold
  server.on("/heater/temp", HTTP_POST, setTempThreshold);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}

String getHtml() {
  String html = "<!DOCTYPE html><html lang='en'><meta \
    charset='UTF-8'><meta name='viewport'\
    content='width=device-width,initial-scale=1'>\
    <meta http-equiv='X-UA-Compatible'\
    content='ie=edge'><link rel='stylesheet'\
    href='https://cdnjs.cloudflare.com/ajax/libs/roundSlider/1.3.3/roundslider.min.css'>\
    <link \
    href='https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css'\
    rel='stylesheet'\
    integrity='sha384-wvfXpqpZZVQGK6TAh5PVlGOfQNHSoD2xbE+QkPxCAFlNEevoEH3Sl0sibVcOQVnN'\
    crossorigin='anonymous'><style>\
    .rs-tooltip-text{font-size:3em;margin-left:-.9em;margin-top:-.5em;font-weight:700}.icon{font-size:3em;cursor:pointer}.btn-power--on{color:green}.btn-power--off{color:red}\
    </style><title>Thermostat</title><div \
    style='display:flex;justify-content:center'>\
    <div style='text-align:center'><h3>\
    Thermostat</h3><div>Temp: <span \
    id='temp'>0</span>°</div><div \
    id='slider'></div><div \
    style='display:flex;justify-content:center;margin-top:1em'>\
    <div style='margin-right:.5em'><i \
    id='power'class='fa fa-power-off icon'>\
    </i><p>power</div><div><i id='heater'\
    class='fas fa-fire-alt icon'></i><p>\
    heater</div></div></div></div><script \
    src='https://code.jquery.com/jquery-3.4.1.min.js'\
    integrity='sha256-CSXorXvZcTkaix6Yvo6HppcZGetbYMGWSFlBw8HfCJo='\
    crossorigin='anonymous'></script><script src='https://cdnjs.cloudflare.com/ajax/libs/roundSlider/1.3.3/roundslider.min.js'>\
    </script><script \
    src='https://kit.fontawesome.com/7e847f21bf.js'\
    crossorigin='anonymous'></script><script src='https://cdnjs.cloudflare.com/ajax/libs/axios/0.19.0/axios.min.js'\
    integrity='sha256-S1J4GVHHDMiirir9qsXWc8ZWw74PHHafpsHp5PXtjTs='\
    crossorigin='anonymous'></script>\
    <script>\
    const baseUrl='';let temp=68,thermo=65;$('#slider').roundSlider({min:50,max:90,radius:85,sliderType:'default',value:thermo,startAngle:270,editableTooltip:!1,stop:showValue});\
    let pwrState=!0,htrState=!1,tempF=0;const clsPwrOn='btn-power--on',clsPwrOff='btn-power--off',evt='click',tempEl=$('#temp'),power=$('#power');power.addClass(clsPwrOn),power.on(evt,togglePwr);\
    const heater=$('#heater');function togglePwr(){pwrState?axios.get(baseUrl+'/power/off').then(\
    function(t){swapClass(power,clsPwrOff,clsPwrOn),swapClass(heater,clsPwrOff,clsPwrOn)}.bind(this)):axios.get(baseUrl+'/power/on').then(function(t){swapClass(power,clsPwrOn,clsPwrOff)}.bind(this)),\
    pwrState=!pwrState}function toggleHtr(t){htrState?axios.get(baseUrl+'/heater/off').then(function(){swapClass(heater,clsPwrOff,clsPwrOn)}.bind(this)):pwrState&&axios.get(baseUrl+'/heater/on').then(\
    function(){swapClass(heater,clsPwrOn,clsPwrOff)}.bind(this)),htrState=pwrState?!htrState:htrState}function swapClass(t,e,s){t.addClass(e),t.removeClass(s)}\
    function showValue(t){axios.post(baseUrl+`/heater/temp?temp=${t.value}`).then(function(){t.value<tempF?swapClass(heater,clsPwrOn,clsPwrOff):swapClass(heater,clsPwrOff,clsPwrOn)}.bind(this))}\
    heater.addClass(clsPwrOff),heater.on(evt,toggleHtr),axios.get(baseUrl+'/heater/temp').then((function(t){tempEl.text(t.data.temp),tempF=t.data.temp}));</script>";
    return html;
}

void handleRoot() {
  // get the html to return in response
  toggleLED();
  String html = getHtml();
  server.send(200, "text/html", html);
}

void handleNotFound() {
  digitalWrite(LED, 1);
  String message = "File Not Found\n\n";
}

void powerOn() {
  togglePower(true);
  sendHeader();
  server.send(200, "text/plain", "power: on");  
}

void powerOff() {
  togglePower(false);
  toggleActuator(LOW);
  sendHeader();
  server.send(200, "text/plain", "power: off");  
}

void togglePower(boolean state) {
  deviceEnabled = state;
}

void actuatorOn() {
  toggleActuator(HIGH);
  sendHeader();
  server.send(200, "text/plain", "actuator: on");
}

void actuatorOff() {
  toggleActuator(LOW);
  sendHeader();
  server.send(200, "text/plain", "actuator: off");
}

void toggleActuator(uint8_t state) {
  if (deviceEnabled) {
    digitalWrite(LED, state);    
  } else {
    digitalWrite(LED, LOW);
  }
}

int getTemp() {
  tempThreshold = readTemp();
  String message = "{\"temp\":" + String(tempThreshold) + "}";
  sendHeader();
  server.send(200, "application/json", message);  
}

float readTemp() {
  float c = dht.readTemperature();
  float f = dht.convertCtoF(c); 
  return f;
}

void setTempThreshold() {
  Serial.print("set temperature: ");
  Serial.println(server.arg(0));
  tempThreshold = server.arg(0).toFloat();
  toggleLED();
  sendHeader();
  server.send(200, "application/json", "{}");  
}

void toggleLED() {
  // if the current temp is less than
  // the threshold, turn the heater on
  Serial.print(readTemp());
  Serial.print(":");
  Serial.println(tempThreshold);
  // if current temp is less than threshold
  if (readTemp() < tempThreshold) {
    digitalWrite(LED, HIGH);   
  } else {
    // temp is less, turn heater off
    digitalWrite(LED, LOW);
  }
}

void sendHeader() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
}
