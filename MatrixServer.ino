#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <WebSockets.h>
#include <Ticker.h> 
#include "Wire.h"
#include "Matrix.hpp"
 
ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

Matrix matrix;
//======================================================================
//Helper functions
void ICACHE_RAM_ATTR renderingISR();
bool inputIsSingleDigit(String &input);
void handlePixelChange();
void setUpRoutes();
void handleNotFound();
void handleRoot();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);
//=======================================================================

const char* ssid = "andreyHotSpot";
const char* password = "andreygrant";

void setup(void) {

  matrix = Matrix();

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

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

  webSocket.begin();
  // if there's an incomming websocket message, go to function 'webSocketEvent'
  webSocket.onEvent(webSocketEvent);          
  Serial.println("WebSocket server started.");

  //Configure Timer that will update the screen
  timer1_attachInterrupt(renderingISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(4000); //120000 us
}

void loop(void) {
  webSocket.loop();       
}

bool inputIsSingleDigit(String &input){
  if(input == "")               {}
    else if(input.length() > 1) {} 
    else if (!isDigit(input[0])){}
    else if (input.toInt() > 7) {}
    else {
      return true;
      }

    return false;
  }

void handlePixelChange() {
  String message = "";
  String row = server.arg("row");
  String col = server.arg("col");

  if( !inputIsSingleDigit (row) || !inputIsSingleDigit(col) ){
    message += "Params are not in correct format! \n";
    }
  else {
    message += "Inverted row:"+row+" and col:" + col + "\n";
    int rowInt = row.toInt();
    int colInt = col.toInt();  
    matrix.changeSingleLED(rowInt, colInt);
    }
  
  message += '\n';
}

void ICACHE_RAM_ATTR renderingISR(){

    //Turn LED OFF
    matrix.turnOff();
    
    //Update
    matrix.update();
    
    //Turn LED ON
    matrix.turnOn();

    //Reset timer
    timer1_write(4000);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type){
    case WStype_DISCONNECTED: 
      Serial.println("Disconnected"); 
      break;
      
    case WStype_CONNECTED:    
      Serial.println("Connected");
    
    break;
    
    
    case WStype_TEXT: 
      Serial.printf("[%u] get Text: %s\n", num, payload); 
      delay(500);
      webSocket.sendTXT(num, "Got MSG");
      break;
  }
}