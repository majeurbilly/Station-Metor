#include "Program.h"
#include "GestionnaireFichier/GestionnaireFichier.h"
#include "WiFi/GestionnaireWiFi.h"
#include "GestionnaireStation/GestionnaireStation.h"
#include "GestionnaireModule/GestionnaireModule.h"
#include "MQTT/MQTTBroker.h"
#include "Composants/BME.h"
#include <Wire.h>

Program::Program()
{
    this->m_gestionnaireFichier = nullptr;
    this->m_gestionnaireStation = nullptr;
    this->m_affichageStation = nullptr;
    this->m_mqttBroker = nullptr;
    this->m_gestionnaireWiFi = nullptr;
    this->m_gestionnaireModule = nullptr;
    this->m_bme = nullptr;
    this->m_lcd = nullptr;
    this->m_del = nullptr;
    this->m_mode = EtatStation::CONNEXION;
}

void Program::setup() {
    Wire.begin(21, 22);

    this->m_gestionnaireFichier = new GestionnaireFichier();
    this->m_gestionnaireFichier->begin();
    this->m_gestionnaireWiFi = new GestionnaireWiFi();
    this->m_bme = new BME(BME_ADRESSE_I2C);
    this->m_gestionnaireModule = new GestionnaireModule(*this->m_bme);
    this->m_mqttBroker = new MQTTBroker();

    this->m_gestionnaireStation = new GestionnaireStation(*this->m_gestionnaireFichier, *this->m_gestionnaireWiFi, *this->m_gestionnaireModule, this->m_mqttBroker);
    this->m_gestionnaireStation->initialiser();
}

void Program::loop() {
    if (this->m_gestionnaireStation != nullptr) {
        this->m_gestionnaireStation->tick();
    }
}

