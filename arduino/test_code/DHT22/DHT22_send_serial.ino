#include "DHT.h"
#include <pb.h>
#include <pb_encode.h>
#include <adcvalues.pb.h>

#define DHTPIN 9
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22

DHT dht(DHTPIN, DHTTYPE);


float humidity
float temperature
unsigned long runtime;

void setup()
{
  Serial.begin(115200);

  dht.begin();
  while(!Serial){
  }
}

void loop()
{
  humidity    = dht.readHumidity();     //Luftfeuchte auslesen
  temperature = dht.readTemperature();  //Temperatur auslesen
  runtime = millis();


  // Prüfen ob eine gültige Zahl zurückgegeben wird. Wenn NaN (not a number) zurückgegeben wird, dann Fehler ausgeben.
  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("DHT22 konnte nicht ausgelesen werden");
  }
  else
  {
    SerialData message = {runtime, temperature, humidity};

    uint8_t buffer[32];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&stream, SerialData_fields, &message);
    for(int i=0; i < stream.bytes_written; i++){
    Serial.write(buffer[i]);
    }
    Serial.write("\n");
  }

}
