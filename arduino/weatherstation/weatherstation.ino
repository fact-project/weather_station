#include "DHT.h"
#include <ArduinoJson.h>
#include <PciManager.h>
#include <PciListenerImp.h>


// Pin Definitions =============================================================

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

// Global variables ============================================================

int cycles = 0;

volatile unsigned long tipCount;     // bucket tip counter used in interrupt routine
volatile unsigned long tipContactTime;  // Timer to manage any contact bounce in interrupt routine

volatile unsigned long windCount;     // anemometer counter used in interrupt routine
volatile unsigned long windContactTime;  // Timer to manage any contact bounce in interrupt routine

volatile unsigned long m152Count;     // contact counter used in interrupt routine
volatile unsigned long m152ContactTime;  // Timer to manage any contact bounce in interrupt routine

//Rain Sensor variables
long lastCount;
float totalRainfall;
bool m152State = false;

//Temperatur Sensor variables
float hum0;
float temp0;
float hum1;
float temp1;

//Windsensor variables
float anemometerFrequency = 0.;
float windDirection = 0;
float windSpeed = 0.;
float windMeasureDuration = 0;

//Time variables
unsigned long startupTime = 0;
unsigned long lastWriteTime = 0;
unsigned long lastWindMeasureTime = 0;

int lastWindSpeedPinStatus = HIGH;

// Global Functions ============================================================

DHT dht0(DHT0PIN, DHTTYPE);
DHT dht1(DHT1PIN, DHTTYPE);

void countM152PulseChange(byte changeKind) {
  if ((millis() - m152ContactTime) > 5 && changeKind == 0) {  // debounce of sensor signal
    m152Count++;
    m152ContactTime = millis();
    m152State = true;
  }
}

PciListenerImp listener(M152_Pin, countM152PulseChange);

// Interrupt Functions ========================================================

// Interrrupt handler: rg-11 detects rain
void rgisr ()   {

   if ((millis() - tipContactTime) > 15 ) {  // debounce of sensor signal
      tipCount++;
      tipContactTime = millis();
   }
}
// end of rg-11 rain detection interrupt handler

// Interrrupt handler: routine that is triggered when the anemometer is rotating
void windisr ()   {
  if ((millis() - windContactTime) > 5 ) {  // debounce of sensor signal
      windCount++;
      windContactTime = millis();
  }
}
// end of anemometer interrupt handler

// Setup =======================================================================

void setup()
{
  startupTime  = millis();
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
  m152State = false;
}

void measureWindSpeed(int wind_measure_time)
{
  cli();         //Disable interrupts

  if ( millis() - lastWindMeasureTime > wind_measure_time ) {
    if ( windCount > 0 ) {
      windMeasureDuration = (millis() - lastWindMeasureTime)/1000.; //in s
      anemometerFrequency = float(windCount)/windMeasureDuration; //pulses per time in s
      windSpeed = anemometerFrequency*24/10;
      windCount = 0;
    }
    lastWindMeasureTime = millis();
  }

  sei();         //Enables interrupts

  return;
}


// Main Loop ===================================================================

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
  windDirection = map(sensorValue, 1, 1024, 1, 5000);

  // ------------------------------------------------------------------
  //Write Data to serial every second

  if( millis() - lastWriteTime > DATA_FREQ){
    lastWriteTime = millis();

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["time"]            = (millis() - startupTime)/1000; //in s
    root["cycles"]          = cycles;

    root["humidity_0"]      = hum0;
    root["temperature_0"]   = temp0;
    root["humidity_1"]      = hum1;
    root["temperature_1"]   = temp1;

    root["tipCount"]       = tipCount;
    root["tot_rainfall"]    = totalRainfall;
    root["M152_state"]      = m152State;
    root["m152Count"]      = m152Count;
    root["anemometerFrequency"] = anemometerFrequency;
    root["wind_speed"]      = windSpeed;
    root["windDirection"]  = windDirection;

    root.printTo(Serial);
    Serial.println();
    cycles = 0;

    // ------------------------------------------------------------------
    //READ M152
    m152State = digitalRead(M152_Pin) != HIGH ? true : false;
  }
}
