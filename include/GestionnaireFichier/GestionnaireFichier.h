#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Donnees.h"

class GestionnaireFichier
{
public:
    GestionnaireFichier();

    void begin();

    bool chargerConnexion();
    bool chargerConnexionIndex(int p_index, String& p_ssid, String& p_password);
    
    void chargerConfigWifi();
    bool chargerConfigMQTT();

    String getSsid() const;
    String getMotDePasse() const;

    const char* getMqttHost() const;
    const int getMqttPort() const;
    const char* getMqttUser() const;
    const char* getMqttPassword() const;

    int getBoiteId() const;
    const char* getMqttClientId() const;
    const char* getMqttTopicBase() const;

    bool sauvegarderConnexion(const String& p_ssid, const String& p_password);
    bool sauvegarderConnexion(const char* p_ssid, const char* p_password);

    bool sauvegarderConfigMQTT(const char* p_host, int p_port, const char* p_user, const char* p_password);
    bool sauvegarderConfigStation(int p_boiteId);
    bool ajouterConnexion(const String& p_ssid, const String& p_password);
    bool supprimerConnexion(int p_index);

    bool ajouterDonnees(const Donnees& p_donnees);
    bool lireDonnees(Donnees p_donnees[], int& p_nbMesures, int p_maxMesures);
    bool viderDonnees();

private:
    String m_ssid;
    String m_motDePasse;

    String m_mqttHost;
    int m_mqttPort;
    String m_mqttUser;
    String m_mqttPassword;

    int m_boiteId;
    String m_mqttClientId;
    String m_mqttTopicBase;

    void mettreAJourIdentiteStation(); 

    bool lireJson(const char* p_chemin, DynamicJsonDocument& p_doc);
};