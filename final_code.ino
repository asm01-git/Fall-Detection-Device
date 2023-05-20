#include <Wire.h>
#include <ESP8266WiFi.h>
#include<math.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

const int roll_threshold= 30;
const int pitch_threshold= 30;
const int yaw_threshold= 45;

Adafruit_MPU6050 mpu;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
float AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ,roll,pitch,yaw;
float prev_roll,prev_pitch;
float avgCount=0;
float ax=0, ay=0, az=0, gx=0, gy=0, gz=0;
boolean fall = false; //stores if a fall has occurred
boolean trigger1=false; //stores if first trigger (lower threshold) has occurred
boolean trigger2=false; //stores if second trigger (upper threshold) has occurred
boolean trigger3=false; //stores if third trigger (orientation change) has occurred
byte trigger1count=0; //stores the counts past since trigger 1 was set true
byte trigger2count=0; //stores the counts past since trigger 2 was set true
byte trigger3count=0; //stores the counts past since trigger 3 was set true
float angleChange_roll,angleChange_pitch,avgAngleChange;

 //// Room WiFi network info.
//const char *ssid = "Not_Your_Wifi";  // Enter your Wi-Fi Name
//const char *pass = "Akg@15.8";     // Enter your Wi-Fi Password
//
////Tinkerers Lab nework info
//const char *ssid = "Tinkerers Lab";  // Enter your Wi-Fi Name
//const char *pass = "tinkerer123";     // Enter your Wi-Fi Password

const char *ssid = "Brightchamp";  // Enter your Wi-Fi Name
const char *pass = "Aswin123";


void send_event(const char *event);
const char *host = "maker.ifttt.com";
const char *privateKey = "eyEYTRctpaVcoggPr1jerYwM9W2mjk39pzBWds0WWEF";

void setup(){
 Serial.begin(115200);
 pinMode(LED_BUILTIN, OUTPUT);
 Wire.begin();
 Wire.beginTransmission(MPU_addr);
 Wire.write(0x6B);  // PWR_MGMT_1 register
 Wire.write(0);     // set to zero (wakes up the MPU-6050)
 Wire.endTransmission(true);
 Serial.println("Wrote to IMU");
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

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
     mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
      mpu.setGyroRange(MPU6050_RANGE_500_DEG);
      mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
   }
   prev_roll=0;
   prev_pitch=0;
   
 }
void loop(){
 mpu_read();
// ax = (AcX-2050)/16384.00;
// ay = (AcY-77)/16384.00;
// az = (AcZ-1947)/16384.00;
// gx = (GyX+270)/131.07;
// gy = (GyY-351)/131.07;
// gz = (GyZ+136)/131.07;
 // calculating Amplitute vactor for 3 axis
 float Raw_Amp = pow(pow(AcX,2)+pow(AcY,2)+pow(AcZ,2),0.5);
 int Amp = Raw_Amp;  // Mulitiplied by 10 bcz values are between 0 to 1
// angleChange = pow(pow(GyX,2)+pow(GyY,2),0.5);
// angleChange=angleChange*180.0/M_PI;7bhgy
angleChange_roll=(roll-prev_roll);
angleChange_pitch=(pitch-prev_pitch);
float net_angleChange=pow(pow(angleChange_roll,2)+pow(angleChange_pitch,2),0.5);
avgAngleChange+=net_angleChange/1;
avgCount++;
  
// Serial.printf("Net acceleration:%d\n",Amp);
// Serial.printf("Angle change:%d\n",angleChange);
Serial.print(roll); Serial.print(",");Serial.print(pitch); Serial.print(",");Serial.print(angleChange_roll); Serial.print(",");
Serial.println(angleChange_pitch);


if((abs(roll-80)>=roll_threshold)||(abs(pitch-90)>=pitch_threshold)){
  trigger1count++;

}
if(trigger1count>=5){
  trigger1=true;
  Serial.println("TRIGGER1_ACTIVATED");
}
if(trigger1){
  
  if(avgAngleChange>5)
   trigger2count++;
}
if(trigger2count>=3){
  trigger2=true;
  Serial.println("TRIGGER2_ACTIVATED");
  Serial.println("Fall Suspected"); 
}
if(trigger2){
  fall=true; 
}
 
if(fall){
  send_event("fall_detected");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  fall=false;
  trigger1=false;
  trigger2=false;
  trigger1count=0;
  trigger2count=0;
  digitalWrite(LED_BUILTIN, LOW);
}
 prev_roll=roll;
 prev_pitch=pitch;
 if(avgCount==1){
   avgAngleChange=0;
   avgCount=0;
  }
 delay(100);

// if (Amp<=6 && trigger2==false){ //if AM breaks lower threshold (0.4g)
//   trigger1=true;
//   Serial.println("TRIGGER 1 ACTIVATED");
////   delay(5000);
//   }
// if (trigger1==true){
//   trigger1count++;
//   if (Amp>=25){ //if AM breaks upper threshold (3g)
//     trigger2=true;
//     Serial.println("TRIGGER 2 ACTIVATED");
//     trigger1=false; trigger1count=0;
////     delay(5000);
//     }
// }
// if (trigger2==true){
//   trigger2count++;
//    Serial.println(angleChange);
//   Serial.println(Amp);
////   if(angleChange>50) 
////    delay(5000);
//   if (angleChange>=80 && angleChange<=120){ //if orientation changes by between 80-100 degrees
//     trigger3=true; trigger2=false; trigger2count=0;
//     Serial.println(angleChange);
//     Serial.println("TRIGGER 3 ACTIVATED");
////     delay(5000);
//       }
//   }
// if (trigger3==true){
//    trigger3count++;
//    if (trigger3count>=10){ 
//       angleChange = pow(pow(GyX,2)+pow(GyY,2)+pow(GyZ,2),0.5);
//       //delay(10);
//       Serial.println(angleChange); 
//       if ((angleChange>=0) && (angleChange<=10)){ //if orientation changes remains between 0-10 degrees
//           fall=true; trigger3=false; trigger3count=0;
//           
//           Serial.println(angleChange);
//             }
//       else{ //user regained normal orientation
//          trigger3=false; trigger3count=0;
//          Serial.println("TRIGGER 3 DEACTIVATED");
//       }
//     }
//  }
// if (fall==true){ //in event of a fall detection
//   Serial.println("FALL DETECTED");
//   send_event("fall_detected"); 
//   digitalWrite(LED_BUILTIN,HIGH);
//   delay(5000);
//   fall=false;
//   digitalWrite(LED_BUILTIN,LOW);
//   }
// if (trigger2count>=6){ //allow 0.5s for orientation change
//   trigger2=false; trigger2count=0;
//   Serial.println("TRIGGER 2 DECACTIVATED");
//   }
// if (trigger1count>=6){ //allow 0.5s for AM to break upper threshold
//   trigger1=false; trigger1count=0;
//   Serial.println("TRIGGER 1 DECACTIVATED");
//   }
//  
  
   }
void mpu_read(){
// Wire.beginTransmission(MPU_addr);
// Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
// Wire.endTransmission(false);
// Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
// AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
// AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
// AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
// Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
// GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
// GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
// GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

/* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

 AcX=a.acceleration.x;  
 AcY=a.acceleration.y;
 AcZ=a.acceleration.z;

 GyX=g.gyro.x;
 GyY=g.gyro.y;
 GyZ=g.gyro.z;

 roll=180.0/M_PI*acos(AcX/pow(pow(AcX,2)+pow(AcY,2)+pow(AcZ,2),0.5));
 pitch=180.0/M_PI*acos(AcY/pow(pow(AcX,2)+pow(AcY,2)+pow(AcZ,2),0.5));
 yaw=180.0/M_PI*acos(AcZ/pow(pow(AcX,2)+pow(AcY,2)+pow(AcZ,2),0.5));

// Serial.print("Acceleration X: ");
//  Serial.print(a.acceleration.x);
//  Serial.print(", Y: ");
//  Serial.print(a.acceleration.y);
//  Serial.print(", Z: ");
//  Serial.print(a.acceleration.z);
//  Serial.println(" m/s^2");
//
//  Serial.print("Rotation X: ");
//  Serial.print(g.gyro.x);
//  Serial.print(", Y: ");
//  Serial.print(g.gyro.y);
//  Serial.print(", Z: ");
//  Serial.print(g.gyro.z);
//  Serial.println(" rad/s");
 }
void send_event(const char *event)
{
  Serial.print("Connecting to "); 
  Serial.println(host);
    // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
    // We now create a URI for the request
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  while(client.connected())
  {
    if(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    } else {
      // No data yet, wait a bit
      delay(50);
    };
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}
