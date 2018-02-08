// Wireless RFID Door Lock Using NodeMCU
// Created by LUIS SANTOS & RICARDO VEIGA
// 7th of June, 2017


#include <Wire.h>
//#include "SSD1306.h"/
#include <MFRC522.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#define RST_PIN 20 // RST-PIN for RC522 - RFID - SPI - Module GPIO15 
#define SS_PIN  2  // SDA-PIN for RC522 - RFID - SPI - Module GPIO2
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

#define Relay 4

#define Button 5

#define BlueLed 0
#define GreenLed 16
#define RedLed 15

//SSD1306  display(0x3c, 4, 5);/

//Wireless name and password
//const char* ssid     = "TP-LINK_65B2"; // replace with you wireless network name
//const char* password = "igrik7735"; //replace with you wireless network password

const char* ssid     = "ChatOS"; // replace with you wireless network name
const char* password = "chatoptimizer"; //replace with you wireless network password

// Remote site information
const String host = "94.130.131.170"; // IP address of your local server
String url = "http://test-account.chatoptimizer.com/media/card.txt"; // folder location of the txt file with the RFID cards identification, p.e. "/test.txt" if on the root of the server

int time_buffer = 500; // amount of time in miliseconds that the relay will remain open

void setup() {
  setColor(255, 0, 0);  // red
  pinMode(Relay, OUTPUT);
  pinMode(Button, INPUT);
  digitalWrite(Relay,1);
    
//  display.init();/
//  display.flipScreenVertically();
//  display.set/Contrast(255);
  
  Serial.begin(115200);    // Initialize serial communications
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522

  // We start by connecting to a WiFi network

  Serial.println("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  wifi_connected();
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  leds_off();
  delay(3000);
}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(RedLed, red);
  analogWrite(GreenLed, green);
  analogWrite(BlueLed, blue);  
}

void leds_off() {
  setColor(0, 0, 0);
  /*analogWrite(BlueLed, 0);   // turn the LED off
  analogWrite(GreenLed, 0);   // turn the LED off
  analogWrite(RedLed, 0);   // turn the LED off
  */
//  display.clear();
//  display.display()/;
}

void reject() {
  // Align text vertical/horizontal center
//  display.clear();/
//  display.setTextAlig/nment(TEXT_ALIGN_CENTER_BOTH);
//  display.setFont(ArialM/T_Plain_16);
//  display.drawString(DISPLA/Y_WIDTH/2, 10+(DISPLAY_HEIGHT/4), "NOT");
//  display.drawString(DISPLAY_W/IDTH/2, 30+(DISPLAY_HEIGHT/4), "AUTHORIZED");
//  display.display();/

  setColor(255, 0, 0);
  delay(200);
  leds_off(); 
}

void authorize() {
  // Align text vertical/horizontal center
//  display.clear();/
//  display.setTextAlig/nment(TEXT_ALIGN_CENTER_BOTH);
//  display.setFont(ArialM/T_Plain_16);
//  display.drawString(DISPLA/Y_WIDTH/2, DISPLAY_HEIGHT/2, "AUTHORIZED");
//  display.display();/
  
  /*analogWrite(GreenLed, 255);   // turn the Green LED on
  analogWrite(RedLed, 0);*/
  setColor(0, 0, 0);
  digitalWrite(Relay,0);
  delay(time_buffer);              // wait for a second 
  digitalWrite(Relay,1);
  leds_off(); 
}

void wifi_connected () {
  // Align text vertical/horizontal center
//  display.clear();/
//  display.setTextAlig/nment(TEXT_ALIGN_CENTER_BOTH);
//  display.setFont(ArialM/T_Plain_16);
//  display.drawString(DISPLA/Y_WIDTH/2, 10+(DISPLAY_HEIGHT/4), "WI-FI");
//  display.drawString(DI/SPLAY_WIDTH/2, 20+(DISPLAY_HEIGHT/2), "CONNECTED");
//  display.display();/
  delay(3000);  
}

void connection_failed() {
  // Align text vertical/horizontal center
//  display.clear();/
//  display.setTextAlig/nment(TEXT_ALIGN_CENTER_BOTH);
//  display.setFont(ArialM/T_Plain_16);
//  display.drawString(DISPLA/Y_WIDTH/2, 10+(DISPLAY_HEIGHT/4), "CONNECTION");
//  display.drawString(DISPLAY_W/IDTH/2, 20+(DISPLAY_HEIGHT/2), "FAILED");
//  display.display();/
  delay(3000);
  leds_off();   
}

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void loop() {
  /*analogWrite(RedLed, 255);
  analogWrite(GreenLed, 0);
  analogWrite(BlueLed, 0);
  */
  if(digitalRead(Button)==HIGH) {
     authorize();
  }
  
  int authorized_flag = 0;
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {   
    delay(50);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {   
    delay(50);
    return;
  }

////-------------------------------------------------RFID----------------------------------------------


  // Shows the card ID on the serial console
  String content= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  Serial.println("Cart read:" + content);

////-------------------------------------------------SERVER----------------------------------------------


   Serial.println("HTTP");
HTTPClient http;    //Declare object of class HTTPClient
    Serial.println("HTTP begin");
   http.begin("http://test-account.chatoptimizer.com/teststr/");      //Specify request destination
   http.addHeader("Content-Type", "text/plain");  //Specify content-type header
 
   int httpCode = http.POST("Message from ESP8266");   //Send the request
   String payload = http.getString();                  //Get the response payload

   Serial.println("httpCode" + httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection



  
  // Use WiFiClient class to create TCP connections
/*  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    connection_failed();
    return;
  }*/
  
  // This will send the request to the server
  client.print(url);

  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  String line;
   Serial.println("11");
//  while(client.a/vailable()){
    Serial.println( client.read());
     line = client.readStringUntil('\n');
     Serial.println("line" + line);
     Serial.println("content" + content);
    if(line==content){
      authorized_flag=1;
    }
//  /}
  
  if(authorized_flag==1){
    Serial.println("AUTHORIZED");
    authorize();
  }
  else{
    Serial.println("NOT AUTHORIZED");
    reject();
  }
}
