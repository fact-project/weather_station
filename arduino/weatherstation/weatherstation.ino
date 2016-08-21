#include "DHT.h"
#include <ArduinoJson.h>
#include <PciManager.h>
#include <PciListenerImp.h>


// Pin Definitions =============================================================

//DHT22 Temp/Hum Sensor
#define DHT0PIN 8
#define DHT1PIN 9
#define DHT2PIN 10
#define DHT3PIN 11
#define DHT4PIN 12
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22

//RG11 RainSensor 1 in drop detection mode mode
#define RG11_1_Pin 3
#define Bucket_Size_1 0.01

//RG11 RainSensor 2 in tipping bucket mode
#define RG11_2_Pin 4
#define Bucket_Size_2 0.001

//kemo M152 rainsensor in binary mode
#define M152_Pin 5

//Wind Sensor
#define WIND_SPEED_PIN 2
#define WIND_SPEED_MEASURE_TIME 10000 //ms, every 10s, should stay like this
#define WIND_DIRECTION_PIN 0//Analog Pin

#define DATA_FREQ 1000 //ms
#define COUNTER_RESET_DELAY 700 //ms

// Global variables ============================================================

int cycles = 0;

volatile unsigned long rg1State;
volatile unsigned long dropCounterRg1;     // bucket tip counter
volatile unsigned long dropPulseLengthRg1;  // drop size determined as pulse length
volatile unsigned long dropContactTimeRg1;  // Timer to manage any contact bounce in interrupt routine
volatile unsigned long dropStartTimeRg1 = 0;  // Timer to manage any contact bounce in interrupt routine


volatile unsigned long tipCounterRg2;     // bucket tip counter
volatile unsigned long tipContactTimeRg2;  // Timer to manage any contact

volatile unsigned long windCount;     // anemometer counter
volatile unsigned long windContactTime;  // Timer to manage any contact bounce in interrupt routine

volatile unsigned long m152Counter;     // contact counter for interrupt routine
volatile unsigned long m152ContactTime;  // Timer to manage any contact bounce in interrupt routine

//Rain Sensor variables
long lastCountRg1      = 0;
long lastCountRg2      = 0;
float averagedDropSizeRg1 = 0.0;
float totalRainfallRg1 = 0.0;
float totalRainfallRg2 = 0.0;
bool m152State         = false;

//Humidity variables
float hum0 = 0.0;
float hum1 = 0.0;
float hum2 = 0.0;
float hum3 = 0.0;
float hum4 = 0.0;

//Temperatur variables
float temp0 = 0.0;
float temp1 = 0.0;
float temp2 = 0.0;
float temp3 = 0.0;
float temp4 = 0.0;


//Windsensor variables
float anemometerFrequency = 0.0;
float windDirection       = 0.0;
float windSpeed           = 0.0;
float windMeasureDuration = 0.0;

//Time variables
unsigned long startupTime         = 0;
unsigned long lastWriteTime       = 0;
unsigned long lastWindMeasureTime = 0;
unsigned long lastCounterReset    = 0;

int lastWindSpeedPinStatus = HIGH;

// Global Functions ============================================================

DHT dht0(DHT0PIN, DHTTYPE);
DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);
DHT dht3(DHT3PIN, DHTTYPE);
DHT dht4(DHT4PIN, DHTTYPE);

void measureWindSpeed(int wind_measure_time)
{

  cli();         //Disable interrupts

  if ( millis() - lastWindMeasureTime > wind_measure_time ) {
    if ( windCount > 0 ) {
      windMeasureDuration = (millis() - lastWindMeasureTime)/1000.; //in s
      anemometerFrequency = float(windCount)/windMeasureDuration; //pulses per time in s
      windCount = 0;
    }
    else {
      anemometerFrequency = 0;
    }
    windSpeed = anemometerFrequency*24/10;
    lastWindMeasureTime = millis();
  }

  sei();         //Enables interrupts

  return;
}

void countM152PulseChange(byte changeKind) {
  if ((millis() - m152ContactTime) > 5 && changeKind == 0) {  // debounce of sensor signal
    m152Counter++;
    m152ContactTime = millis();
    m152State = true;
  }
}

// Interrrupt handler: rg-11_2 detects rain
void countRg2PciTips (byte changeKind)   {

  if ((millis() - tipContactTimeRg2) > 15 && changeKind == 0) {  // debounce of sensor signal
    tipCounterRg2++;
    tipContactTimeRg2 = millis();
  }
}

PciListenerImp listener1(M152_Pin, countM152PulseChange);
PciListenerImp listener2(RG11_2_Pin, countRg2PciTips);

// Interrupt Functions ========================================================

// Interrrupt handler: rg-11_1 detects rain in drop detection mode
void countRg1Drops ()   {

  rg1State = digitalRead(RG11_1_Pin);
  if (rg1State == LOW) {
    dropStartTimeRg1 = millis();
  }
  else if (rg1State == HIGH) {
    dropPulseLengthRg1 += millis() - dropStartTimeRg1;
    dropCounterRg1++;
  }
  dropContactTimeRg1 = millis();
}

// Interrrupt handler: rg-11_2 detects rain
void countRg2Tips ()   {

  if ((millis() - tipContactTimeRg2) > 15 ) {  // debounce of sensor signal
    tipCounterRg2++;
    tipContactTimeRg2 = millis();
  }
}

// Interrrupt handler: routine that is triggered when the anemometer is rotating
void countWindRotations ()   {
  if ((millis() - windContactTime) > 5 ) {  // debounce of sensor signal
      windCount++;
      windContactTime = millis();
  }
}


// Setup =======================================================================

void setup()
{
  startupTime  = millis();
  Serial.begin(9600);

  //global constants
  cycles = 0;
  lastCountRg1 = 0;
  dropCounterRg1 = 0;
  totalRainfallRg1 = 0;
  windCount = 0;


  //Temperature and Humidity Sensors
  dht0.begin();
  dht1.begin();
  // dht2.begin();
  // dht3.begin();
  // dht4.begin();

  //Optical Rainsensor: RG11
  pinMode(RG11_1_Pin, INPUT);   // set the digital input pin to input for the RG-11 Sensor
  attachInterrupt(digitalPinToInterrupt(RG11_1_Pin), countRg1Drops, CHANGE);     // attach interrupt handler to input pin.
  // we trigger the interrupt on the voltage falling from 5V to GND

//  pinMode(RG11_2_Pin, INPUT);   // set the digital input pin to input for the
//  attachInterrupt(digitalPinToInterrupt(RG11_2_Pin), countRg2Tips, FALLING);
  pinMode(RG11_2_Pin, INPUT);
  PciManager.registerListener(RG11_2_Pin, &listener2);


  //Wind Speed
  pinMode(WIND_SPEED_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(WIND_SPEED_PIN), countWindRotations, FALLING);     // attach interrupt handler to input pin.

  sei();         //Enables interrupts

  //Impedance Rainsensor
  pinMode(M152_Pin, INPUT);
  PciManager.registerListener(M152_Pin, &listener1);


  m152State = false;
}

// Main Loop ===================================================================

void loop()
{
  unsigned long currentTime  = millis();
  cycles++;

  // ------------------------------------------------------------------
  //Read DHT22

  hum0  += dht0.readHumidity();     //Luftfeuchte auslesen
  hum1  += dht1.readHumidity();     //Luftfeuchte auslesen
  hum2  += dht2.readHumidity();     //Luftfeuchte auslesen
  hum3  += dht3.readHumidity();     //Luftfeuchte auslesen
  hum4  += dht4.readHumidity();     //Luftfeuchte auslesen

  temp0 += dht0.readTemperature();  //Temperatur auslesen
  temp1 += dht1.readTemperature();  //Temperatur auslesen
  temp2 += dht2.readTemperature();  //Temperatur auslesen
  temp3 += dht3.readTemperature();  //Temperatur auslesen
  temp4 += dht4.readTemperature();  //Temperatur auslesen

  // ------------------------------------------------------------------

  cli();         //Disable interrupts

  //READ RG11_1
  if(dropCounterRg1 != lastCountRg1) {
    lastCountRg1 = dropCounterRg1;
    if (dropCounterRg1 > 0){
      averagedDropSizeRg1 = float(dropPulseLengthRg1)/dropCounterRg1;
    } else {
      averagedDropSizeRg1 = 0;
    }
    totalRainfallRg1 = dropCounterRg1 * Bucket_Size_1;
  }

  //READ RG11_2
  if(tipCounterRg2 != lastCountRg2) {
    lastCountRg2 = tipCounterRg2;
    totalRainfallRg2 = tipCounterRg2 * Bucket_Size_2;
  }

  sei();         //Enables interrupts


  // ------------------------------------------------------------------
  //Read Wind Speed
  measureWindSpeed(WIND_SPEED_MEASURE_TIME);

  // ------------------------------------------------------------------
  //Read Wind Direction

  int sensorValue = analogRead(WIND_DIRECTION_PIN);
  windDirection += map(sensorValue, 1, 1024, 1, 5000);

  // ------------------------------------------------------------------
  //Write Data to serial every second

  if( millis() - lastWriteTime > DATA_FREQ){
    lastWriteTime = millis();

    StaticJsonBuffer<300> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["time"]    = (millis() - startupTime)/1000; //in s
    root["cycles"]  = cycles;

    root["humidity0"] = hum0/cycles;
    root["humidity1"] = hum1/cycles;
    root["humidity2"] = hum2/cycles;
    root["humidity3"] = hum3/cycles;
    root["humidity4"] = hum4/cycles;

    root["temperature0"] = temp0/cycles;
    root["temperature1"] = temp1/cycles;
    root["temperature2"] = temp2/cycles;
    root["temperature3"] = temp3/cycles;
    root["temperature4"] = temp4/cycles;

    root["rg1DropCount"]        = dropCounterRg1;
    root["rg1meanDropSize"]     = averagedDropSizeRg1;
    root["rg1TotRainfall"]      = totalRainfallRg1/cycles;
    root["rg2TipCount"]         = tipCounterRg2;
    root["rg2TotRainfall"]      = totalRainfallRg2/cycles;
    root["m152State"]           = m152State;
    root["m152Counter"]         = m152Counter;
    root["windAnemometerFrequency"] = anemometerFrequency;
    root["windSpeed"]           = windSpeed;
    root["windDirection"]       = windDirection/cycles;

    root.printTo(Serial);
    Serial.println();

    //Reset values to make sure the output is not comulative:
    cycles  = 0;

    hum0    = 0.0;
    hum1    = 0.0;
    hum2    = 0.0;
    hum3    = 0.0;
    hum4    = 0.0;

    temp0   = 0.0;
    temp1   = 0.0;
    temp2   = 0.0;
    temp3   = 0.0;
    temp4   = 0.0;

    windDirection  = 0.0;

  }
  // ------------------------------------------------------------------
  // Reset Dropcounters
  if( millis() - lastCounterReset > COUNTER_RESET_DELAY+DATA_FREQ){
    lastCounterReset = millis();

    if ( digitalRead(RG11_1_Pin) == HIGH ){
      dropCounterRg1      = 0;
      dropPulseLengthRg1  = 0;
    }

    tipCounterRg2  = 0;


        //READ M152
    m152State   = digitalRead(M152_Pin) != HIGH ? true : false;
    if (!m152State){
      m152Counter = 0;
    }
  }
  // ------------------------------------------------------------------
}
