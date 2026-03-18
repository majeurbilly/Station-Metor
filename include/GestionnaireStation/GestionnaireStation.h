#pragma once

#include "GestionnaireStation/EtatStation.h"
#include "GestionnaireStation/SensorConfig.h"

#include "GestionnaireFichier/GestionnaireFichier.h"
#include "WiFi/GestionnaireWiFi.h"
#include "GestionnaireModule/GestionnaireModule.h"
#include "AffichageStation/AffichageStation.h"
#include "MQTT/MQTTBroker.h"
#include "GestionnaireStation/ConfigurateurPortail.h"
#include "Composants/LCD.h"
#include "Composants/DEL.h"

class GestionnaireStation {
public:
    GestionnaireStation(GestionnaireFichier& p_gestionnaireFichier,
                       GestionnaireWiFi& p_gestionnaireWiFi,
                       GestionnaireModule& p_gestionnaireModule,
                       MQTTBroker* p_mqttBroker = nullptr);

    void initialiser();
    void tick();
    void changerMode(EtatStation p_nouveauMode);
    EtatStation getMode() const;
    AffichageStation* getAffichageStation();
    void publierAutoDiscovery();

private:
    static constexpr float BME_INVALIDE = -999.0f;

    void afficherBmeSiChangement(float p_temperature, float p_humidite, float p_pression);
    bool connecterReseau();
    bool ouvrirPortailConfiguration();
    void syncNtp();
    void connecterMQTT();
    void publierMesuresMQTT(float p_temperature, float p_humidite, float p_pression);
    void lireMesures(float& p_temperature, float& p_humidite, float& p_pression);
    void gererModeAffichage(float p_temperature, float p_humidite, float p_pression);
    void gererModeHorsConnexion(float p_temperature, float p_humidite, float p_pression);
    void sauvegarderDonneesSiNecessaire(float p_temperature, float p_humidite, float p_pression);
    void envoyerDonneesSauvegardees();

    int m_niveauTimeout;
    unsigned long m_prochaineTentativeReconnexion;
    unsigned long m_prochainePublicationMQTT;
    unsigned long m_prochaineTentativeReconnexionMQTT;
    unsigned long m_dernierEnregistrement;

    float m_derniereTemperature;
    float m_derniereHumidite;
    float m_dernierePression;

    GestionnaireFichier* m_gestionnaireFichier;
    GestionnaireWiFi* m_gestionnaireWiFi;
    GestionnaireModule* m_gestionnaireModule;
    MQTTBroker* m_mqttBroker;

    AffichageStation* m_affichageStation;
    LCD* m_lcd;
    DEL* m_del;

    ConfigurateurPortail m_configurateurPortail;
    EtatStation m_mode;
};