
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define N_SENSORES 24

// Pin donde se conecta el bus 1-Wire
const int pinDatosDQ = 9;

int i;

// Instancia a las clases OneWire y DallasTemperature
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);

void setup()
{
  // Iniciamos la comunicación serie
  Serial.begin(9600);
  // Iniciamos el bus 1-Wire
  sensorDS18B20.begin();
}

void loop()
{
  // Mandamos comandos para toma de temperatura a los sensores
  //Serial.println("Mandando comandos a los sensores");
  sensorDS18B20.requestTemperatures();

  for (i = 0; i < N_SENSORES; i++)
  {
    // Leemos y mostramos los datos de los sensores DS18B20
    Serial.print("S");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(sensorDS18B20.getTempCByIndex(i));
    Serial.println(" ºC");
  }

  delay(1000);
}