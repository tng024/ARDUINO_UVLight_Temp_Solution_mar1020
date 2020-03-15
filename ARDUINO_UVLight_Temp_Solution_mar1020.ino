#include <ESP8266WiFi.h>        
#include <ESP8266WebServer.h>   
#include <WiFiClient.h>

#include "Relay.hpp"
#include "PIR.hpp"
#include "millisDelay.h"

const char *ssid = "UVLightDevice";  
const char *password = "yourpassword"; 
ESP8266WebServer server(80);    

// Instantiate objects
Relay relay(4);
PIR pirSensor(5,3,15,13,12,14);
millisDelay timer;
millisDelay timer2;


// Variable declaration
String relayControlCmd;
unsigned long timerDurationCmd;
int timerCancelCmd;
bool waitTimeFinished;



void setup() {
  // PIR sensors
  pirSensor.configuration();
  pirSensor.pirCheck();

  // Relay
  relay.initialize();

  // Start serial comm. - Debug tool
  Serial.begin(115200);           

  // Configure NODEMCU as Access Point
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid); //Password is not necessary
  IPAddress myIP = WiFi.softAPIP(); //Get the IP assigned to itself.
  Serial.print("AP IP address: "); //This is written in the PC console.
  Serial.println(myIP);

  // Handler association
  server.on("/", handleRootPath); 
  server.on("/Init", handleInit);   
  server.on("/RelayControl", handleRelayControl);     
  server.on("/Timer", handleTimer);   

  // Start server
  server.begin();                 
  Serial.println("Server started!");

  // Test timer lib
  Serial.println(timer.toMillisec(2));
  delay(3000);

  relay.ON();
  delay(2000);
  relay.OFF();
}

void loop() {
  server.handleClient();          //To handle the actual incoming of HTTP requests, we need to call the handleClient method on the server object, on the main loop function.
  Serial.println(relayControlCmd);
  Serial.println(timerDurationCmd);
  Serial.println(timerCancelCmd);
  delay(200);
  if(relayControlCmd == "ON"){
    timer2.start(1); // Delay 1 second (to test)
    if(timer2.justFinished()){
      Serial.println("Safe to turn on!");
      timer.start(timerDurationCmd);  // Start timer of timerDuration
      relay.ON();
      Serial.println("Relay is ON");  
      if(timer.justFinished()){
        relay.OFF();
        Serial.println("Relay is OFF");        
        }
      }
    }
  else{
    relay.OFF();
    Serial.println("Relay is OFF");  
    delay(1000);
    }
    
//  if(pirSensor.pirCheck()){
//    Serial.println("Human detected! Shutting down...");
//    }
//  else{
//    Serial.println("No human detected!");
//    }
//  if(pirSensor.pirCheck())
}

void handleRootPath() {
  server.send(200, "text/plain", "Connected!");
}

void handleInit() {

}

void handleRelayControl(){
    String message = "Initialization with: ";
    if (server.hasArg("State")) {
    relayControlCmd = server.arg("State");
    //Serial.println(relayCmd);
    //String temp = server.arg("State");
    //Serial.println(temp);
    message += "State: ";
    message += server.arg("State");
  }
  server.send(200, "text/plain", message); 
  }

void handleTimer(){
    String message = "Initialization with: ";
    if (server.hasArg("Duration")) {
    timerDurationCmd = (server.arg("Duration").toInt());
    //Serial.println(temp);
    message += "Duration: ";
    message += server.arg("Duration");
  }

    if (server.hasArg("Cancel")) {
    timerCancelCmd = (server.arg("Cancel").toInt());
    //Serial.println(temp);
    message += "Cancel: ";
    message += server.arg("Cancel");
  }
  server.send(200, "text/plain", message); 
  }
