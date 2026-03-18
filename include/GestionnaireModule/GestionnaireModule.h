#pragma once

#include <Arduino.h>

class BME;

class GestionnaireModule {
public:
    GestionnaireModule(BME& bme);

    void initialiser();
    float lireTemperature();
    float lireHumidite();
    float lirePression();

private:
    BME* m_bme;
};