#include <math.h>
#include "UbidotsMicroESP8266.h"

#define TOKEN  "Your_token_here"  // Put here your Ubidots TOKEN
#define WIFISSID "Your_WiFi_SSID" // Put here your Wi-Fi SSID
#define PASSWORD "Your_WiFi_Password" // Put here your Wi-Fi password

Ubidots client(TOKEN);

//*************************************************
// Global variables
//*************************************************
int Trig = D0;    // Trigger pin - NodeMCU
int Echo = D1;    // Echo pin - NodeMCU
int infrared = A0;
float ultrasonic_cm, infrared_cm;
float max_value, min_value;
//---------------------------------------

//*************************************************
// Functions declaration
//*************************************************
void calibration_infra (void);
float read_infrared (void);
float read_ultrasonic (void);
//---------------------------------------

//*************************************************
// Setup
//*************************************************
void setup() 
{
  Serial.begin (9600);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  client.wifiConnection(WIFISSID, PASSWORD);

  calibration_infra(); // Infrared sensor calibration  
}

//*************************************************
// Main
//*************************************************
void loop() 
{  
  infrared_cm = read_infrared(); // Infrared sensor measurement
  ultrasonic_cm = read_ultrasonic(); // Ultrasonic sensor measurement

  /*
  if(infrared_cm < min_value || infrared_cm > max_value)
  {
    infrared_cm = 0;
  }
  */
  
  Serial.print("Ultrasonic sensor distance: ");
  Serial.print(ultrasonic_cm);
  Serial.println(" cm");
  
  Serial.print("Infrared sensor distance: ");
  Serial.print(infrared_cm);
  Serial.println(" cm");
  Serial.println(" ");
  
  // send_ubidots(); // Build and send Ubidots payload
  
  delay(2000);
}

//*************************************************
// Functions definition
//*************************************************
void calibration_infra (void)
{
  max_value, min_value = 0;
  float val;
  int times = 500;
  int del = 100;
  
  for (int i=0; i<times; i++)
  {
    val = read_infrared();

    if(val > max_value)
    {
      max_value = val;
    } else if (val < min_value)
    {
      min_value = val;
    }
    delay(del);
  }

  Serial.print("Min value = ");
  Serial.println(min_value);
  Serial.print("Max value = ");
  Serial.println(max_value);
}
//---------------------------------------
float read_infrared ()
{
  int infra_val = 0;
  float infra_cm;
  double pot;

  infra_val = analogRead(infrared);
  pot = pow(infra_val, 1.2);
  infra_cm = 29284*(1/pot);

  return infra_cm;
}
//---------------------------------------
float read_ultrasonic ()
{
  float duration, ultra_cm;

  // Trigger: HIGH pulse of 10 or more us. Recommended short LOW pulse before.
  digitalWrite(Trig, LOW);
  delayMicroseconds(5);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
 
  pinMode(Echo, INPUT);
  duration = pulseIn(Echo, HIGH);
 
  // Time into distance conversion
  ultra_cm = (duration/2) / 29.1;

  return ultra_cm;
}
//---------------------------------------
void send_ubidots()
{ 
  client.setDataSourceName("parking_test_1_s3_s4"); // Change device name
  client.setDataSourceLabel("parking_test_1_s3_s4"); // Change device label
  client.add("ultrasonic_cm", ultrasonic_cm);
  client.add("infrared_cm", infrared_cm);
  client.sendAll(true);
}
