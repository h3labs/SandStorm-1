#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int GPS_RXPin = 6, GPS_TXPin = 3;
static const int GPSBaud = 4800;
static const int CamPin = 8;


// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss_gps(GPS_RXPin, GPS_TXPin);

// delay, in seconds
unsigned int del = 2;

unsigned long last = 0UL;
unsigned long current_time = 0;
unsigned long last_photo = 0;
unsigned long last_photo_end = 0;

unsigned int photo_interval = 30; // seconds
unsigned int photo_duration = 1000; // milliseconds
bool photo_state = false;

void setup()
{
  Serial.begin(115200);
  ss_gps.begin(GPSBaud);
  current_time = millis();
  pinMode(CamPin, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(CamPin, LOW);
  digitalWrite(13, LOW);
}

void loop()
{

  current_time = millis();
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss_gps.available() > 0)
  {
    gps.encode(ss_gps.read());
  }
    
  if (gps.location.isUpdated() && 
        gps.date.isUpdated() && 
        gps.time.isUpdated() && 
        gps.speed.isUpdated() &&
        gps.course.isUpdated() &&
        gps.altitude.isUpdated() &&
        (millis() - last >= (del * 1000))
  ) {
    displayInfo();
    last = current_time;
  }
  
  if ((current_time - last_photo) >= (photo_interval * 1000)) {
  	digitalWrite(CamPin, HIGH);
  	digitalWrite(13, HIGH);
  	last_photo = current_time;
  	last_photo_end = current_time + photo_duration;
  	photo_state = true;
  }
  if ((current_time >= last_photo_end) && photo_state == true) {
 	digitalWrite(CamPin, LOW);
 	digitalWrite(13, LOW);
 	photo_state = false;
  }
}

void displayInfo()
{
  if (gps.location.isValid()
      &&
      gps.date.isValid()
      &&
      gps.time.isValid()
      &&
      gps.speed.isValid()
      &&
      gps.altitude.isValid()
  )
  {
    // holder for various values that require 0 padding
    uint8_t tmp;

    // format: lat,long,altitude (m),m/s,date+time
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    Serial.print(F(","));
    Serial.print(gps.altitude.meters(), 4);
    Serial.print(F(","));
    Serial.print(gps.speed.kmph(), 4);
    Serial.print(F(","));

    Serial.print(gps.date.year());
    Serial.print("-");
    tmp = gps.date.month();
    if (tmp < 10) {
      Serial.print("0");
    }
    Serial.print(tmp);
    Serial.print("-");
    tmp = gps.date.day();
    if (tmp < 10) {
      Serial.print("0");
    }
    Serial.print(tmp);
    Serial.print(" ");
    tmp = gps.time.hour();
    if (tmp < 10) {
      Serial.print("0");
    }
    Serial.print(tmp);
    Serial.print(":");
    tmp = gps.time.minute();
    if (tmp < 10) {
      Serial.print("0");
    }
    Serial.print(tmp);
    Serial.print(":");
    tmp = gps.time.second();
    if (tmp < 10) {
      Serial.print("0");
    }
    Serial.print(tmp);
    Serial.print(".");
    tmp = gps.time.centisecond();
    if (tmp < 10) {
      Serial.print("0");
    }
    Serial.println(tmp);
  }
}

