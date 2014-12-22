/****************************************************************
WiFi Weather Station

Sparkfun Wifi shield on an Arduino Uno: Weather station that sends data to wunderground.com

Tacked together by Dan Fein, danielfein.com

Sparkfun hardware library by Shawn Hymel, https://github.com/sparkfun/SFE_CC3000_Library
Sensors from Adafruit, see their product pages for those libraries.

Notes:
This project assumes battery power and to save power it will rest for awhile, wake, send data and rest again.

You will need to sign up for a user account at wunderground.com, to get your pass
When you register a station you will get an ID

Wunderground wants UTC Zulu, not local time, if your RTC is local, offset it in code.
Wunderground Upload guidelines: http://wiki.wunderground.com/index.php/PWS_-_Upload_Protocol

This code assumes your wifi credentials are already saved in the CC3000's non-volitile memory
****************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#include "DHT.h"
#include <Adafruit_BMP085.h>
#include "Adafruit_SI1145.h"
#include "RTClib.h"

#include <io.h> //for the sleepies

// Pins
#define CC3000_INT      2   // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7   // Can be any digital pin
#define CC3000_CS       10  // Preferred is pin 10 on Uno
#define DHTPIN          3   // DHT 22  (AM2302)

// Constants
unsigned int timeout = 30000;             // Milliseconds
#define DHTTYPE DHT22                                  // DHT 22 (AM2302)
char SERVER[] = "rtupdate.wunderground.com";           // Realtime update server - RapidFire
//char SERVER [] = "weatherstation.wunderground.com";  //standard server
char WEBPAGE [] = "GET /weatherstation/updateweatherstation.php?";
char ID [] = "xxxxxx";
char PASSWORD [] = "xxxxxxxx";

// Global Variables
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client client = SFE_CC3000_Client(wifi);
RTC_DS1307 rtc;                         // Hardware RTC time
DHT dht(DHTPIN, DHTTYPE);               // DHT 22  (AM2302)
Adafruit_BMP085 bmp;                    // BMP Pressure Sensor
Adafruit_SI1145 uv = Adafruit_SI1145(); // UV Sensor
unsigned int connections = 0;           // number of connections



void setup(void){
  //Turn everything on
  Serial.begin(38400);
  Wire.begin();    //Rtc
  rtc.begin();     //Hardware rtc
  bmp.begin();     //Pressure sensor
  dht.begin();     //Humidity Sensor
  
  // Turn the Wifi on
  Serial.print(F("\nInitializing..."));
   // Initialize CC3000 (configure SPI communications)
  if ( wifi.init() ) {
    Serial.println("CC3000 initialization complete");
  } else {
    Serial.println("Something went wrong during CC3000 init!");
  } 
  // Connect to WiFi network stored in non-volatile memory
  Serial.println("Connecting to network stored in profile...");
  if ( !wifi.fastConnect(timeout) ) {
    Serial.println("Error: Could not connect to network");
  }
}

void loop(void){  
  // Lets see what time the RTC is set at! 
  DateTime now = rtc.now();

  //Get sensor data
  float tempc = bmp.readTemperature(); //Can read temp from bmp or dht sensors
  float tempf =  (tempc * 9.0)/ 5.0 + 32.0; //was dht.readTemperature, need to convert native C to F
  float humidity = dht.readHumidity(); 
  float baromin = bmp.readPressure()* 0.0002953;// Calc for converting Pa to inHg (wunderground)
  float dewptf = (dewPoint(tempf, dht.readHumidity())); //Dew point calc(wunderground) //replaced dht.readtemp with converted temp
  float UVindex = uv.readUV();
  // the index is multiplied by 100 so to get the integer index, divide by 100!
        UVindex /= 100.0; 
              
               
  //*
  // Debug, or you can sit up all night watching it.
  Serial.println("+++++++++++++++++++++++++");
  Serial.println("RTC TIME ");
  Serial.print("&dateutc=");
  Serial.print(now.year());
  Serial.print("-");
  Serial.print(now.month());
  Serial.print("-");
  Serial.print(now.day());
  Serial.println("+");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
  
  Serial.print("temp= ");
  Serial.print(tempf);
  Serial.println(" *F");
  Serial.print("baro= ");
  Serial.print(baromin);
  Serial.println(" inHg");
  Serial.print("dew point= ");
  Serial.println(dewptf);
  Serial.print("humidity= ");
  Serial.println(humidity);
  Serial.print("UV: ");  
  Serial.println(UVindex);
 //*/
 
 //Send data to Weather Underground
 if (client.connect(SERVER, 80)) { 
    Serial.println("Sending DATA ");
    // Ship it!
    client.print(WEBPAGE); 
    client.print("ID=");
    client.print(ID);
    client.print("&PASSWORD=");
    client.print(PASSWORD);
    client.print("&dateutc=");
    client.print(now.year());
    client.print("-");
    client.print(now.month());
    client.print("-");
    client.print(now.day());
    client.print("+");
    client.print(now.hour()+8);// YOU MUST Add 8 hours -for pacific time- to get back to UTC or Wunderground wont show RAPID FIRE
    client.print("%3A");
    client.print(now.minute());
    client.print("%3A");
    client.print(now.second());
    client.print("&tempf=");
    client.print(tempf);
    client.print("&baromin=");
    client.print(baromin);
    client.print("&dewptf=");
    client.print(dewptf);
    client.print("&humidity=");
    client.print(humidity);
    client.print("&uv=");
    client.print(UVindex);
    //client.print("&action=updateraw");//Standard update
    client.print("&softwaretype=Arduino%20UNO%20version1&action=updateraw&realtime=1&rtfreq=2.5");//Rapid Fire
    client.println();
    Serial.println("Upload complete");
    } 
    else {
      Serial.println(F("Connection failed"));                 
      return;
      }
    
    lpDelay(1200); // Low Power Delay. Value of 4=1sec, 40=10sec, 1200=5min   
 
}

/****************************************************************

  Function Fun Time

****************************************************************/

double dewPoint(double tempf, double humidity)
{
  double A0= 373.15/(273.15 + tempf);
  double SUM = -7.90298 * (A0-1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
  SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM-3) * humidity;
  double T = log(VP/0.61078);   
  return (241.88 * T) / (17.558-T);
}

 // Low Power Delay.  Drops the system clock to its lowest setting and sleeps for 256*quarterSeconds milliseconds.
 // ie: value of 4=1sec    20=5sec          
int lpDelay(int quarterSeconds) {
  int oldClkPr = CLKPR;   // save old system clock prescale
  CLKPR = 0x80;           // Tell the AtMega we want to change the system clock
  CLKPR = 0x08;           // 1/256 prescaler = 60KHz for a 16MHz crystal
  delay(quarterSeconds);  // since the clock is slowed way down, delay(n) now acts like delay(n*256)
  CLKPR = 0x80;           // Tell the AtMega we want to change the system clock
  CLKPR = oldClkPr;       // Restore old system clock prescale
}

/****************************************************************

  End of Program

****************************************************************/