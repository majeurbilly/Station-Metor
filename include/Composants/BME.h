#pragma once
#include <Adafruit_BME280.h>    
#include <Arduino.h>

class BME {
public:
    BME(uint8_t adresseI2C);

    bool initialiser();
    float lireTemperature();
    float lireHumidite();
    float lirePression();


private:
    uint8_t m_adresseI2C;
    Adafruit_BME280* m_bme;
    bool m_initialise;
  
};

