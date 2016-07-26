#include "DHT.h"
#include <ArduinoJson.h>

//DHT22 Temp/Hum Sensor
#define DHT0PIN 9
#define DHT1PIN 11
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22


//RG11 RainSensor in tipping bucket mode
#define RG11_Pin 2
#define Bucket_Size 0.01

//kemo M152 rainsensor in binary mode
#define M152_Pin 10


//Wind Sensor
#define WIND_SPEED_PIN 8
#define WIND_DIRECTION_PIN 0

volatile unsigned long tipCount;     // bucket tip counter used in interrupt routine
volatile unsigned long ContactTime;  // Timer to manage any contact bounce in interrupt routine

long lastCount;
float totalRainfall;

float hum0;
float temp0;

float hum1;
float temp1;

bool m152_rain;
float wind_direction = 0;
int wind_speed_value = 0;

DHT dht0(DHT0PIN, DHTTYPE);
DHT dht1(DHT1PIN, DHTTYPE);



void setup()
{
  Serial.begin(9600);
  //Serial.println("DHT22 - Test!");
  //Serial.print("Bucket Size: "); Serial.print(Bucket_Size); Serial.println(" mm");

  dht0.begin();
  dht1.begin();

  lastCount = 0;
  tipCount = 0;
  totalRainfall = 0;

  pinMode(RG11_Pin, INPUT);   // set the digital input pin to input for the RG-11 Sensor
  attachInterrupt(digitalPinToInterrupt(RG11_Pin), rgisr, FALLING);     // attach interrupt handler to input pin.
  // we trigger the interrupt on the voltage falling from 5V to GND

  sei();         //Enables interrupts

  pinMode(M152_Pin, INPUT); 
  m152_rain = 0;

  pinMode(WIND_SPEED_PIN, INPUT);
}

void loop()
{
  //Read DHT22
  hum0 = dht0.readHumidity();     //Luftfeuchte auslesen
  temp0 = dht0.readTemperature();  //Temperatur auslesen
  hum1 = dht1.readHumidity();     //Luftfeuchte auslesen
  temp1 = dht1.readTemperature();  //Temperatur auslesen

  //READ RG11
  cli();         //Disable interrupts
  
  if(tipCount != lastCount) {
    lastCount = tipCount;
    totalRainfall = tipCount * Bucket_Size;
  }
  
  sei();         //Enables interrupts

  //READ M152 
  m152_rain = digitalRead(M152_Pin) < 1 ? true : false;

  //Read Wind Speed
  digitalWrite(WIND_SPEED_PIN, HIGH);
  wind_speed_value = digitalRead(WIND_SPEED_PIN);

  //Read Wind Direction
  int sensorValue = analogRead(WIND_DIRECTION_PIN); 
  wind_direction = map(sensorValue, 1, 1024, 1, 5000);


  // Prüfen ob eine gültige Zahl zurückgegeben wird. Wenn NaN (not a number) zurückgegeben wird, dann Fehler ausgeben.
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  root["humidity_0"]      = hum0;
  root["temperature_0"]   = temp0;  
  root["humidity_1"]      = hum1;
  root["temperature_1"]   = temp1;  
  
  root["tip_count"]     = tipCount;
  root["tot_rainfall"]  = totalRainfall;
  root["M152"]          = m152_rain;
  root["wind_speed"]    = wind_speed_value;
  root["wind_direction"]= wind_direction;
  
  root.printTo(Serial);
//    Serial.write((const char *)&h, sizeof(float)/*4*/);
//    Serial.write((const char *)&t, sizeof(float)/*4*/);   
  Serial.println();
}

// Interrrupt handler routine that is triggered when the rg-11 detects rain   
void rgisr ()   { 

   if ((millis() - ContactTime) > 15 ) {  // debounce of sensor signal
      tipCount++;
      ContactTime = millis();
   } 
} 
// end of rg-11 rain detection interrupt handler 

