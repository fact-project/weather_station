#include "DHT.h"
#include <ArduinoJson.h>
#include <PciManager.h>
#include <PciListenerImp.h>

//DHT22 Temp/Hum Sensor
#define DHT0PIN 9
#define DHT1PIN 11
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22


//RG11 RainSensor 1 in tipping bucket mode
#define RG11_1_Pin 3
#define Bucket_Size_1 0.01

//RG11 RainSensor 2 in tipping bucket mode
#define RG11_2_Pin 4
#define Bucket_Size_1_2 0.01

//kemo M152 rainsensor in binary mode
#define M152_Pin 5


//Wind Sensor
#define WIND_SPEED_PIN 2
#define WIND_SPEED_MEASURE_TIME 10000 //ms, every 10s, should stay like this
#define WIND_DIRECTION_PIN 0

#define DATA_FREQ 2000 //ms

int cycles = 0;

volatile unsigned long tipCount;     // bucket tip counter used in interrupt routine
volatile unsigned long ContactTime;  // Timer to manage any contact bounce in interrupt routine

volatile unsigned long windCount;     // anemometer counter used in interrupt routine
volatile unsigned long windContactTime;  // Timer to manage any contact bounce in interrupt routine

volatile unsigned long M152Count;     // bucket tip counter used in interrupt routine
volatile unsigned long m152ContactTime;  // Timer to manage any contact bounce in interrupt routine

long lastCount;
float totalRainfall;

float hum0;
float temp0;

float hum1;
float temp1;

bool m152_rain = false;

float anemometer_freq = 0.;
float wind_direction = 0;
float wind_speed_value = 0.;
float windMeasureDuration = 0;

unsigned long startup_time = 0;
unsigned long last_write_time = 0;

unsigned long lastWindMeasureTime = 0;

int lastWindSpeedPinStatus = HIGH;

DHT dht0(DHT0PIN, DHTTYPE);
DHT dht1(DHT1PIN, DHTTYPE);

void onPinChange(byte changeKind) {
  if ((millis() - m152ContactTime) > 5 && changeKind == 0) {  // debounce of sensor signal
    M152Count++;
    m152ContactTime = millis(); 
    m152_rain = true;
  }
}

PciListenerImp listener(M152_Pin, onPinChange);

void setup()
{
  startup_time  = millis();
  Serial.begin(9600);

  //global constants
  cycles = 0;
  lastCount = 0;
  tipCount = 0;
  totalRainfall = 0;
  windCount = 0; 
  

  //Temperature and Humidity Sensors
  dht0.begin();
  dht1.begin();

  //Optical Rainsensor: RG11
  pinMode(RG11_1_Pin, INPUT);   // set the digital input pin to input for the RG-11 Sensor
  attachInterrupt(digitalPinToInterrupt(RG11_1_Pin), rgisr, FALLING);     // attach interrupt handler to input pin.
  // we trigger the interrupt on the voltage falling from 5V to GND

  //Wind Speed
  pinMode(WIND_SPEED_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(WIND_SPEED_PIN), windisr, FALLING);     // attach interrupt handler to input pin.

  sei();         //Enables interrupts

  //Impedance Rainsensor
  pinMode(M152_Pin, INPUT); 
  PciManager.registerListener(M152_Pin, &listener);
  m152_rain = false;
}

void measureWindSpeed(int wind_measure_time)
{
  cli();         //Disable interrupts

  if ( millis() - lastWindMeasureTime > wind_measure_time ) {
    if ( windCount > 0 ) {
      windMeasureDuration = (millis() - lastWindMeasureTime)/1000.; //in s
      anemometer_freq = float(windCount)/windMeasureDuration; //pulses per time in s
      wind_speed_value = anemometer_freq*24/10;
      windCount = 0;
    }
    lastWindMeasureTime = millis();
  }
    
  sei();         //Enables interrupts
  
  return;
}

void loop()
{
  unsigned long currentTime  = millis();
  cycles++;
  
  // ------------------------------------------------------------------
  //Read DHT22
  
  hum0  = dht0.readHumidity();     //Luftfeuchte auslesen
  temp0 = dht0.readTemperature();  //Temperatur auslesen
  hum1  = dht1.readHumidity();     //Luftfeuchte auslesen
  temp1 = dht1.readTemperature();  //Temperatur auslesen

  // ------------------------------------------------------------------
  //READ RG11
  
  cli();         //Disable interrupts
  
  if(tipCount != lastCount) {
    lastCount = tipCount;
    totalRainfall = tipCount * Bucket_Size_1;
  }
  
  sei();         //Enables interrupts

  
  // ------------------------------------------------------------------
  //Read Wind Speed  
  measureWindSpeed(WIND_SPEED_MEASURE_TIME);

  // ------------------------------------------------------------------
  //Read Wind Direction
  
  int sensorValue = analogRead(WIND_DIRECTION_PIN); 
  wind_direction = map(sensorValue, 1, 1024, 1, 5000);

  // ------------------------------------------------------------------
  //Write Data to serial every second
  
  if( millis() - last_write_time > DATA_FREQ){
    last_write_time = millis();
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
  
    root["time"]            = (millis() - startup_time)/1000; //in s
    root["cycles"]          = cycles;
    
    root["humidity_0"]      = hum0; 
    root["temperature_0"]   = temp0;  
    root["humidity_1"]      = hum1;
    root["temperature_1"]   = temp1; 
        
    root["tip_count"]       = tipCount;
    root["tot_rainfall"]    = totalRainfall;
    root["M152"]            = m152_rain;
    root["M152Count"]       = M152Count;
    root["anemometer_freq"] = anemometer_freq;
    root["wind_speed"]      = wind_speed_value;
    root["wind_direction"]  = wind_direction;
    
    root.printTo(Serial);   
    Serial.println();
    cycles = 0;
    
    // ------------------------------------------------------------------
    //READ M152 
    m152_rain = digitalRead(M152_Pin) != HIGH ? true : false;
  }
}

// Interrrupt handler routine that is triggered when the rg-11 detects rain   
void rgisr ()   { 

   if ((millis() - ContactTime) > 15 ) {  // debounce of sensor signal
      tipCount++;
      ContactTime = millis();
   } 
} 
// end of rg-11 rain detection interrupt handler 


// Interrrupt handler routine that is triggered when the anemometer is rotating   
void windisr ()   { 
  if ((millis() - windContactTime) > 5 ) {  // debounce of sensor signal
      windCount++;
      windContactTime = millis(); 
  }
} 
// end of rg-11 rain detection interrupt handler 




