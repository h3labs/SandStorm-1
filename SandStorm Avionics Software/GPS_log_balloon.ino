#include <SD.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

/*Declaration of variables for TX, RX pins and GPS Baudrate.

Make sure that the GPSBaud is equal to the Baudrate of the GPS used.
*/
static const int RXPin = 6, TXPin = 3;
static const int GPSBaud = 4800;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

File myFile;

// For stats that happen every 5 seconds
unsigned long last = 0UL;

// delay, in seconds
unsigned int del = 2;

char* logfile = "gpslog.csv";


void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }

  if (SD.exists(logfile)) {
    SD.remove(logfile);
  }

  myFile = SD.open(logfile, FILE_WRITE);
  myFile.println("latitude,longitude,date,altitude (feet),speed (mph),rotation");
  myFile.close();

  Serial.print("logging to ");
  Serial.println(logfile);
}

void loop()
{
  // Dispatch incoming characters
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
  
  //myFile.println("latitude,longitude,date,altitude (feet),speed (mph),rotation");

  if (gps.location.isUpdated() && 
        gps.date.isUpdated() && 
        gps.time.isUpdated() && 
        gps.speed.isUpdated() &&
        gps.course.isUpdated() &&
        gps.altitude.isUpdated() &&
        (millis() - last >= (del * 1000))
  ) {
    
    uint8_t tmp;
    double stmp;
    
    myFile = SD.open(logfile, FILE_WRITE);
    myFile.print(gps.location.lat(), 6);
    myFile.print(",");

    myFile.print(gps.location.lng(), 6);
    myFile.print(",");

    myFile.print(gps.date.year());
    myFile.print("-");
    tmp = gps.date.month();
    if (tmp < 10) {
      myFile.print("0");
    }
    myFile.print(tmp);
    myFile.print("-");
    tmp = gps.date.day();
    if (tmp < 10) {
      myFile.print("0");
    }
    myFile.print(tmp);
    myFile.print(" ");
    tmp = gps.time.hour();
    if (tmp < 10) {
      myFile.print("0");
    }
    myFile.print(tmp);
    myFile.print(":");
    tmp = gps.time.minute();
    if (tmp < 10) {
      myFile.print("0");
    }
    myFile.print(tmp);
    myFile.print(":");
    tmp = gps.time.second();
    if (tmp < 10) {
      myFile.print("0");
    }
    myFile.print(tmp);
    myFile.print(".");
    tmp = gps.time.centisecond();
    if (tmp < 10) {
      myFile.print("0");
    }
    myFile.print(tmp);
    myFile.print(",");

    myFile.print(gps.altitude.feet());
    myFile.print(",");

    stmp = gps.speed.mph();
    myFile.print(stmp);
    myFile.print(",");

    myFile.println(gps.course.deg());

    myFile.close();

    last = millis();
  }
}
