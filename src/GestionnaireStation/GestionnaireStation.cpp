#include "GestionnaireStation/GestionnaireStation.h"
#include "GestionnaireFichier/GestionnaireFichier.h"
#include "WiFi/GestionnaireWiFi.h"
#include "GestionnaireModule/GestionnaireModule.h"
#include "AffichageStation/AffichageStation.h"
#include "MQTT/MQTTBroker.h"
#include "Composants/LCD.h"
#include "Composants/DEL.h"
#include "config.h"
#include <WiFi.h>
#include <time.h>
#include <ArduinoJson.h>

GestionnaireStation::GestionnaireStation(GestionnaireFichier& gestionnaireFichier,
                                         GestionnaireWiFi& gestionnaireWiFi,
                                         GestionnaireModule& gestionnaireModule,
                                         MQTTBroker* mqttBroker)
    : m_configurateurPortail(gestionnaireFichier)
{
    m_gestionnaireFichier = &gestionnaireFichier;
    m_gestionnaireWiFi = &gestionnaireWiFi;
    m_gestionnaireModule = &gestionnaireModule;
    m_mqttBroker = mqttBroker;

    m_affichageStation = nullptr;
    m_lcd = nullptr;
    m_del = nullptr;

    m_mode = EtatStation::CONNEXION;
    m_niveauTimeout = 0;
    m_prochaineTentativeReconnexion = 0;
    m_prochainePublicationMQTT = 0;
    m_prochaineTentativeReconnexionMQTT = 0;
    m_dernierEnregistrement = 0;

    m_derniereTemperature = BME_INVALIDE;
    m_derniereHumidite = BME_INVALIDE;
    m_dernierePression = BME_INVALIDE;
}

void GestionnaireStation::afficherBmeSiChangement(float p_temperature, float p_humidite, float p_pression) 
{
    bool changement = (m_derniereTemperature <= BME_INVALIDE) ||
                      (p_temperature >= m_derniereTemperature + 0.05f || p_temperature <= m_derniereTemperature - 0.05f) ||
                      (p_humidite >= m_derniereHumidite + 0.5f || p_humidite <= m_derniereHumidite - 0.5f) ||
                      (p_pression >= m_dernierePression + 0.5f || p_pression <= m_dernierePression - 0.5f);
    if (changement) 
    {
        m_affichageStation->afficherDonnees(p_temperature, p_humidite, p_pression);
        m_derniereTemperature = p_temperature;
        m_derniereHumidite = p_humidite;
        m_dernierePression = p_pression;
    }
}

void GestionnaireStation::initialiser()     
{
    m_gestionnaireFichier->chargerConfigMQTT();

    m_lcd = new LCD(LCD_ADRESSE_I2C, LCD_COLONNES, LCD_LIGNES);
    m_del = new DEL(DEL_PIN);
    m_lcd->initialiser();
    m_affichageStation = new AffichageStation(m_lcd, m_del);

    m_gestionnaireModule->initialiser();

    float temperature;
    float humidite;
    float pression;
    lireMesures(temperature, humidite, pression);
    m_affichageStation->afficherDonnees(temperature, humidite, pression);

    if (connecterReseau()) 
    {
        m_mode = EtatStation::AFFICHAGE;
        return;
    }
    
    m_niveauTimeout = 1;

    if (ouvrirPortailConfiguration()) 
    {
        m_mode = EtatStation::AFFICHAGE;
        return;
    }

    m_niveauTimeout = 2;
    m_mode = EtatStation::HORS_CONNEXION;
    m_prochaineTentativeReconnexion = millis() + RECONNEXION_INTERVALLE_MS;
}

void GestionnaireStation::tick() 
{
    float temperature;
    float humidite;
    float pression;
    lireMesures(temperature, humidite, pression);

    if (m_del != nullptr) 
    {
        m_del->tick();
    }

    if (m_affichageStation != nullptr)
    {
        m_affichageStation->afficherEtat(static_cast<int>(m_mode));
    }

    switch (m_mode) 
    {
        case EtatStation::AFFICHAGE:
            gererModeAffichage(temperature, humidite, pression);
            break;

        case EtatStation::HORS_CONNEXION:
            gererModeHorsConnexion(temperature, humidite, pression);
            break;

        case EtatStation::ERREUR:
            afficherBmeSiChangement(temperature, humidite, pression);
            break;

        default:
            m_derniereTemperature = BME_INVALIDE;
            m_derniereHumidite = BME_INVALIDE;
            m_dernierePression = BME_INVALIDE;
            m_affichageStation->afficherMessage("Connexion...");
            break;
    }
}

void GestionnaireStation::gererModeAffichage(float p_temperature, float p_humidite, float p_pression)
{
    if (WiFi.status() != WL_CONNECTED) 
    {
        m_mode = EtatStation::HORS_CONNEXION;
        m_prochaineTentativeReconnexion = millis() + RECONNEXION_INTERVALLE_MS;
        return;
    }

    if (m_mqttBroker != nullptr) {
        m_mqttBroker->traiter();
    }

    if (m_mqttBroker != nullptr && !m_mqttBroker->estConnecte()) 
    {
        if (millis() >= m_prochaineTentativeReconnexionMQTT) {
            connecterMQTT();
            m_prochaineTentativeReconnexionMQTT = millis() + RECONNEXION_INTERVALLE_MS;
        }
    }

    afficherBmeSiChangement(p_temperature, p_humidite, p_pression);

    if (m_mqttBroker != nullptr && m_mqttBroker->estConnecte()) 
    {
        publierMesuresMQTT(p_temperature, p_humidite, p_pression);
    } 
    else 
    {
        sauvegarderDonneesSiNecessaire(p_temperature, p_humidite, p_pression);
    }
}

void GestionnaireStation::gererModeHorsConnexion(float p_temperature, float p_humidite, float p_pression)
{
    afficherBmeSiChangement(p_temperature, p_humidite, p_pression);
    sauvegarderDonneesSiNecessaire(p_temperature, p_humidite, p_pression);

    if (millis() < m_prochaineTentativeReconnexion) 
    {
        return;
    }

    m_derniereTemperature = BME_INVALIDE;
    m_derniereHumidite = BME_INVALIDE;
    m_dernierePression = BME_INVALIDE;
    m_affichageStation->afficherMessage("Connexion...");

    if (connecterReseau()) 
    {
        m_mode = EtatStation::AFFICHAGE;
    } 
    else if (ouvrirPortailConfiguration()) 
    {
        m_mode = EtatStation::AFFICHAGE;
    }

    m_prochaineTentativeReconnexion = millis() + RECONNEXION_INTERVALLE_MS;
}

bool GestionnaireStation::connecterReseau()
{
    m_gestionnaireWiFi->configurerDepuisFichier(m_gestionnaireFichier);

    if (!m_gestionnaireWiFi->connecter()) 
    {
        return false;
    }

    delay(2000);  
    syncNtp();    
    connecterMQTT();
    return true;
}

bool GestionnaireStation::ouvrirPortailConfiguration()
{
    m_derniereTemperature = BME_INVALIDE;
    m_derniereHumidite = BME_INVALIDE;
    m_dernierePression = BME_INVALIDE;
    m_affichageStation->afficherMessage("Portail ouvert");

    if (!m_configurateurPortail.ouvrir())
    {
        return false;
    }

    delay(2000);
    syncNtp();
    connecterMQTT();
    return true;
}

// https://ebokify.com/esp32-ntp-time-in-arduino-ide/
void GestionnaireStation::syncNtp()
{
    configTime(NTP_GMT_OFFSET, NTP_DST_OFFSET, NTP_SERVER);
    struct tm timeinfo;
    int tentatives = 0;
    while (!getLocalTime(&timeinfo) && tentatives < 20) {
        delay(500);
        tentatives++;
    }
}

void GestionnaireStation::connecterMQTT()
{
    if (m_mqttBroker == nullptr) 
    {
        return;
    }

    if (!m_gestionnaireFichier->chargerConfigMQTT())
     {
        return;
    }

    m_mqttBroker->setHost(m_gestionnaireFichier->getMqttHost());
    m_mqttBroker->setPort(m_gestionnaireFichier->getMqttPort());
    m_mqttBroker->setUser(m_gestionnaireFichier->getMqttUser());
    m_mqttBroker->setPassword(m_gestionnaireFichier->getMqttPassword());    

    if (m_mqttBroker->connecter(m_gestionnaireFichier->getMqttClientId())) 
    {
        m_prochainePublicationMQTT = millis();  
        publierAutoDiscovery();
        envoyerDonneesSauvegardees();
    }
}

void GestionnaireStation::publierMesuresMQTT(float temperature, float humidite, float pression)
{
    if (m_mqttBroker == nullptr || !m_mqttBroker->estConnecte()) 
    {
        return;
    }

    if (millis() < m_prochainePublicationMQTT)
    {
        return;
    }

    m_prochainePublicationMQTT = millis() + MQTT_PUBLICATION_INTERVALLE_MS;

    StaticJsonDocument<256> doc;
    doc["temperature"] = round(temperature * 10.0f) / 10.0f;
    doc["humidite"] = round(humidite * 10.0f) / 10.0f;
    doc["pression"] = round(pression * 10.0f) / 10.0f;

    char buffer[256];
    serializeJson(doc, buffer);

    String topic = m_gestionnaireFichier->getMqttTopicBase();
    m_mqttBroker->publier(topic, String(buffer));
}

void GestionnaireStation::lireMesures(float& temperature, float& humidite, float& pression)
{
    temperature = m_gestionnaireModule->lireTemperature();
    humidite = m_gestionnaireModule->lireHumidite();
    pression = m_gestionnaireModule->lirePression();
}

void GestionnaireStation::changerMode(EtatStation nouveauMode)
{
    m_mode = nouveauMode;
}

EtatStation GestionnaireStation::getMode() const
{
    return m_mode;
}

void GestionnaireStation::publierAutoDiscovery()
{
    if (m_mqttBroker == nullptr || !m_mqttBroker->estConnecte())
    {
        return;
    }

    int boiteId = m_gestionnaireFichier->getBoiteId();
    String clientId = m_gestionnaireFichier->getMqttClientId();
    String topicBase = m_gestionnaireFichier->getMqttTopicBase();
    String bateauIdStr = (boiteId < 10 ? "0" : "") + String(boiteId);
    String deviceName = "Bateau " + bateauIdStr;

    static const SensorConfig sensors[] = {
        { "Temperature", "temperature", "temperature", "°C", "temperature" },
        { "Humidite", "humidite", "humidite", "%", "humidity" },
        { "Pression", "pression", "pression", "hPa", "pressure" }
    };

    for (const SensorConfig& config : sensors)
    {
        StaticJsonDocument<512> doc;

        doc["name"] = config.name;
        doc["unique_id"] = ("esp32_bateau_" + bateauIdStr + "_" + config.uniqueIdSuffix).c_str();
        doc["state_topic"] = topicBase.c_str();
        doc["value_template"] = ("{{ value_json." + String(config.valueKey) + " }}").c_str();
        doc["unit_of_measurement"] = config.unit;
        doc["device_class"] = config.deviceClass;

        JsonObject device = doc.createNestedObject("device");
        JsonArray ids = device.createNestedArray("identifiers");
        ids.add(clientId);
        device["name"] = deviceName.c_str();

        char buffer[512];
        serializeJson(doc, buffer);

        String discoveryTopic = "homeassistant/sensor/" + clientId + "/" + String(config.uniqueIdSuffix) + "/config";
        m_mqttBroker->publier(discoveryTopic, String(buffer), true);
    }
}

void GestionnaireStation::sauvegarderDonneesSiNecessaire(float temperature, float humidite, float pression)
{
    if (millis() - m_dernierEnregistrement < INTERVALLE_SAUVEGARDE_MS) {
        return;
    }

    Donnees donnee;
    donnee.id = m_gestionnaireFichier->getBoiteId();
    donnee.temperature = temperature;
    donnee.humidite = humidite;
    donnee.pression = pression;

    if (m_gestionnaireFichier->ajouterDonnees(donnee)) {
        m_dernierEnregistrement = millis();
    }
}

void GestionnaireStation::envoyerDonneesSauvegardees()
{
    if (m_mqttBroker == nullptr || !m_mqttBroker->estConnecte()) {
        return;
    }

    Donnees donnees[50];
    int nbMesures = 0;

    if (!m_gestionnaireFichier->lireDonnees(donnees, nbMesures, 50)) {
        return;
    }

    if (nbMesures == 0) {
        return;
    }

    String topic = m_gestionnaireFichier->getMqttTopicBase();
    for (int i = 0; i < nbMesures; i++) {
        StaticJsonDocument<256> doc;
        doc["temperature"] = round(donnees[i].temperature * 10.0f) / 10.0f;
        doc["humidite"] = round(donnees[i].humidite * 10.0f) / 10.0f;
        doc["pression"] = round(donnees[i].pression * 10.0f) / 10.0f;

        char buffer[256];
        serializeJson(doc, buffer);
        m_mqttBroker->publier(topic, String(buffer));
        m_mqttBroker->traiter();
    }

    m_gestionnaireFichier->viderDonnees();
}
