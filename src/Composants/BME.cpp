#include "Composants/BME.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

BME::BME(uint8_t adresseI2C) {
    m_adresseI2C = adresseI2C;
    m_bme = nullptr;
    m_initialise = false;
}

bool BME::initialiser() {
    Adafruit_BME280* bme = new Adafruit_BME280();
    m_initialise = bme->begin(m_adresseI2C);

    if (!m_initialise) {
        Serial.println("Erreur d'initialisation BME");
        delete bme;
        return false;
    }
    m_bme = bme;
    Serial.println("BME initialisee");
    return true;
}

float BME::lireTemperature() {
    if (m_bme == nullptr)
    {
        return 0.0f;
    }
    return m_bme->readTemperature();
}

float BME::lireHumidite() {
    if (m_bme == nullptr){
        return 0.0f;
    }
    return m_bme->readHumidity();
}

float BME::lirePression() {
    if (m_bme == nullptr){
        return 0.0f;
    }
    return m_bme->readPressure() / 100.0f;
}
