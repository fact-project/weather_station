#include "DHT.h"
#include <ArduinoJson.h>

#define DHTPIN 9
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22

DHT dht(DHTPIN, DHTTYPE);



void send_float (float arg)
{
  // get access to the float as a byte-array:
  byte * data = (byte *) &arg; 

  // write the data to the serial
  Serial.write (data, sizeof (arg));
  Serial.println();
}

void setup()
{
  Serial.begin(9600);
  //Serial.println("DHT22 - Test!");

  dht.begin();
}

void loop()
{
  float h = dht.readHumidity();     //Luftfeuchte auslesen
  float t = dht.readTemperature();  //Temperatur auslesen


  // Prüfen ob eine gültige Zahl zurückgegeben wird. Wenn NaN (not a number) zurückgegeben wird, dann Fehler ausgeben.
  if (isnan(t) || isnan(h))
  {
    Serial.println("DHT22 konnte nicht ausgelesen werden");
  }
  else
  {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["humidity"] = h;
    root["temperature"] = t;
    root.printTo(Serial);
//    Serial.write((const char *)&h, sizeof(float)/*4*/);
//    Serial.write((const char *)&t, sizeof(float)/*4*/);   
    Serial.println();

    
  }
}
