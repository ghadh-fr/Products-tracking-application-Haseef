#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <HX711.h>  // HX711 library for the scale
//#include <HX711_ADC.h>
//***********************************************

#define FIREBASE_HOST "haseef-34e27-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "0YO15bKjYPUMep3gjJK4RKSvNrPwIrfRhzvrwe6X"
char ssid[] = "ALFRIDI";
char pass[] = "Alking66";

#define calibration_factor -242750  


#define DOUT D1  // Pin connected to HX711 (serial data output pin) / gnd-vcc
#define CLK  D2 // Pin connected to HX711 clk pin(serial clock input)


#define num_measurements 10 // Number of measurements
#define threshold 0      // Measures only if the weight is greater than 0 g
#define threshold1 3  // Restart averaging if the weight changes more than 3 g



HX711 scale1;    //(D1, D2); method to takes 2 pin 



//----------------------------------------


long weight1 = 0.0;



void setup() {
  
scale1.begin(D1, D2);
  // setup code, to run once:
  Serial.begin(115200);
  delay(10);
 
  Serial.print("Connecting");
   WiFi.disconnect();
  Serial.println("disconnected");
    WiFi.begin(ssid, pass); // Connect to WiFi router
  
  // Wait for connection
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
   // ESP.wdtFeed(); //will ensure the reset of the watchdog.
    delay(10); //milisecond 

  }
  //If successfully connected to the wifi router, the name of router will be displayed in the serial monitor.

  Serial.print("Successfully connected to : ");
  Serial.println("WIFI_SSID");
  Serial.println(WiFi.SSID());

  //Firebase Realtime Database Configuration.
  
 
  scale1.set_scale(calibration_factor);//Adjust to this calibration factor
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  scale1.tare(); //Reset the scale to 0
}
//main code, to run repeatedly:
void loop() {
  
  weight1 = scale1.get_units();
  Serial.println(weight1);
  
  long avgweight1 = 0; 
if (weight1 > threshold) { // Takes measures if the weight is greater than the threshold //0
    long weight01 = scale1.get_units();
    for (int i = 0; i < num_measurements; i++) {  //10  Takes several measurements
      delay(100);
      weight1 = scale1.get_units();
      avgweight1 = avgweight1 + weight1;
      if (abs(weight1 - weight01) > threshold1)//3 {
        avgweight1 = 0;
        i = 0;
      }
      weight01 = weight1;
    }
    avgweight1 = avgweight1 / num_measurements; // 10, Calculate average weight
    
    Serial.print("Measured weight for Sensor 1: ");
    Serial.printf("%.2f",avgweight1, 1);
    Serial.println(" kg");//170g - 0.17kg 

 Firebase.setFloat("productArduino/dairyProducts/D1/weight2", round(avgweight1)); //code to send or update data (Float data type)

  
}}

  
