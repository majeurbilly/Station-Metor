#pragma once
#include <Arduino.h>
#include <config.h> 
#include "GestionnaireFichier/GestionnaireFichier.h"
#include "GestionnaireStation/GestionnaireStation.h"
#include "GestionnaireStation/EtatStation.h"
#include "AffichageStation/AffichageStation.h"
#include "MQTT/MQTTBroker.h"
#include "WiFi/GestionnaireWiFi.h"
#include "GestionnaireModule/GestionnaireModule.h"
#include "Composants/BME.h"
#include "Composants/LCD.h"
 #include "Composants/DEL.h"

class Program {
public:
    Program();
    void loop();
    void setup();

private:
    GestionnaireFichier* m_gestionnaireFichier;
    GestionnaireStation* m_gestionnaireStation;
    AffichageStation* m_affichageStation;
    MQTTBroker* m_mqttBroker;
    GestionnaireWiFi* m_gestionnaireWiFi;
    GestionnaireModule* m_gestionnaireModule;
    BME* m_bme;
    LCD* m_lcd;
    DEL* m_del;
    EtatStation m_mode;
};
