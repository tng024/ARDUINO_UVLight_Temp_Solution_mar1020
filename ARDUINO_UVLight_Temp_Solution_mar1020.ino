#include <ESP8266WiFi.h>        //I can connect to a Wifi
#include <ESP8266WebServer.h>   //I can be a server 'cos I have the class ESP8266WebServer available
#include <WiFiClient.h>

#include "Relay.hpp"
#include "PIR.hpp"

const char *ssid = "ESPap";  
const char *password = "yourpassword"; 
ESP8266WebServer server(80);    

Relay relay(12);
PIR PIR_sensor(1,3,15,13,12,14);


// Variable declaration
const int dirPin = 5; //This pin corresponds to GPIO5 (D1) (Yellow wire) https://nodemcu.readthedocs.io/en/latest/en/modules/gpio/
const int stepPin = 4; //This pin corresponds to GPIO4 (D2) (Orange wire)
int steps = 0; //This variable is related to the number of turns. If microstepping is disabled, 200 corresponds to a complete turn.
int stepDelay = 0; //This variable is the pulse duration in milliseconds and it is related to the rotation speed. Without microstepping, 1.8º are stepDelay ms.
bool dir = HIGH; //Rotation direction. HIGH is clockwise.

String relayCmd;
unsigned long timerDuration;


void setup() {
  // Instantiate objects
  PIR_sensor.configuration();
  PIR_sensor.pirCheck();

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
}

void loop() {
  server.handleClient();          //To handle the actual incoming of HTTP requests, we need to call the handleClient method on the server object, on the main loop function.
//  if(PIR_sensor.pirCheck()){
//    Serial.println("Human detected! Shutting down...");
//    }
//  else{
//    Serial.println("No human detected!");
//    }
    //if(PIR_sensor.pirCheck())
}

void handleRootPath() {
  server.send(200, "text/plain", "Connected!");
}

void handleInit() {
  steps = 0;  //Motor stopped if the arguments are wrong.
  stepDelay = 0;
  String message = "Initialization with: ";

  if (server.hasArg("Dir")) {
    digitalWrite(dirPin, server.arg("Dir") == "HIGH"); //This is a cunning way of checking the value of the argument Dir.
    message += "Direction: ";
    message += server.arg("Dir");
  }
  if (server.hasArg("Delay")) {
    stepDelay = (server.arg("Delay")).toInt(); //Converts the string to integer.
    message += " Delay: ";
    message += server.arg("Delay");
  }
  if (server.hasArg("Steps")) {
    steps = (server.arg("Steps")).toInt();
    message += " Steps: ";
    message += server.arg("Steps");
  }

  server.send(200, "text/plain", message); 
  
  for (int i = 0; i < steps; i++) { 
    digitalWrite(stepPin, HIGH);
    delay(stepDelay);
    digitalWrite(stepPin, LOW);
    delay(stepDelay);
  }
  Serial.println(steps); 
}

void handleRelayControl(){
    String message = "Initialization with: ";
    if (server.hasArg("State")) {
    relayCmd = server.arg("State");
    Serial.println(relayCmd);
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
    String temp = server.arg("Duration");
    Serial.println(temp);
    message += "Duration: ";
    message += server.arg("Duration");
  }

    if (server.hasArg("Cancel")) {
    String temp = server.arg("Cancel");
    Serial.println(temp);
    message += "Cancel: ";
    message += server.arg("Cancel");
  }
  server.send(200, "text/plain", message); 
  }
