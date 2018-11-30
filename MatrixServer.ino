  #include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Ticker.h> 
#include "Wire.h"
#include "Matrix.hpp"
 
Ticker blinker;
Matrix matrix;
//=======================================================================
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


const char* ssid = "andreyHotSpot";
const char* password = "andreygrant";

ESP8266WebServer server(80);

const int led = 13;


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
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200,"text/plain","");
    
}

void handleRoot() {
  digitalWrite(led, 1);
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", "hello \n" );
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {

  matrix = Matrix();
  matrix.turnOff();
 
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

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

  //This Section will define API that the client can use to update the screen
  server.on("/changePixel", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Max-Age", "10000");
    server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "" );
  });

  server.on("/", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Hello World" );
  });

  server.on("/changePixel",HTTP_POST, handlePixelChange);

  //In case you fucked up :)
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

   //Configure Timer that will update the screen
  timer1_attachInterrupt(renderingISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(4000); //120000 us
}

void loop(void) {
  server.handleClient();
}
