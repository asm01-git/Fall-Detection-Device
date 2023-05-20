//#include <WiFi.h>
#include <ESP8266WiFi.h>

//Comment out wifi creds based on your location

// Room WiFi network info.
const char *ssid = "Not_Your_Wifi";  // Enter your Wi-Fi Name
const char *pass = "Akg@15.8";     // Enter your Wi-Fi Password
//
////Tinkerers Lab nework info
//const char *ssid = "Tinkerers Lab";  // Enter your Wi-Fi Name
//const char *pass = "tinkerer123";     // Enter your Wi-Fi Password

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");              // print ... till not connected
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  
//  digitalWrite(13,HIGH);
//  pinMode(13,OUTPUT);
//  Serial.println(digitalRead(13));
}

void loop() {
  // put your main code here, to run repeatedly:

}
