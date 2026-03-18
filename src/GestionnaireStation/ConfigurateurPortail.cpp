#include "GestionnaireStation/ConfigurateurPortail.h"
#include "config.h"
#include <WiFiManager.h>
#include <WiFi.h>

ConfigurateurPortail::ConfigurateurPortail(GestionnaireFichier& p_gestionnaireFichier)
    : m_gestionnaireFichier(p_gestionnaireFichier)
{
}

bool ConfigurateurPortail::ouvrir()
{
    m_gestionnaireFichier.chargerConfigMQTT();

    String mqttHost = m_gestionnaireFichier.getMqttHost();
    String mqttPort = String(m_gestionnaireFichier.getMqttPort());
    String mqttUser = m_gestionnaireFichier.getMqttUser();
    String mqttPassword = m_gestionnaireFichier.getMqttPassword();
    String boiteIdStr = String(m_gestionnaireFichier.getBoiteId());

    WiFiManager wifiManager;
    WiFiManagerParameter paramBoiteId("boite_id", "Numéro de station (1, 2, 3...)", boiteIdStr.c_str(), 5);
    WiFiManagerParameter paramMqttHost("mqtt_host", "Serveur MQTT", mqttHost.c_str(), 63);
    WiFiManagerParameter paramMqttPort("mqtt_port", "Port MQTT", mqttPort.c_str(), 7);
    WiFiManagerParameter paramMqttUser("mqtt_user", "Utilisateur MQTT", mqttUser.c_str(), 47);
    WiFiManagerParameter paramMqttPassword("mqtt_password", "Mot de passe MQTT", mqttPassword.c_str(), 47);
    wifiManager.addParameter(&paramBoiteId);
    wifiManager.addParameter(&paramMqttHost);
    wifiManager.addParameter(&paramMqttPort);
    wifiManager.addParameter(&paramMqttUser);
    wifiManager.addParameter(&paramMqttPassword);

    wifiManager.setConfigPortalTimeout(PORTAL_TIMEOUT_SEC);
    wifiManager.startConfigPortal("StationMeteo");

    if (WiFi.status() != WL_CONNECTED)
    {
        return false;
    }

    String portStr = paramMqttPort.getValue();
    int port = portStr.toInt();
    if (port <= 0 || port > 65535)
    {
        port = MQTT_PORT;
    }
    m_gestionnaireFichier.sauvegarderConfigMQTT(
        paramMqttHost.getValue(),
        port,
        paramMqttUser.getValue(),
        paramMqttPassword.getValue()
    );

    String valeurBoiteId = paramBoiteId.getValue();
    int boiteId = valeurBoiteId.toInt();
    if (boiteId >= 1)
    {
        m_gestionnaireFichier.sauvegarderConfigStation(boiteId);
    }

    m_gestionnaireFichier.ajouterConnexion(WiFi.SSID(), WiFi.psk());
    // https://avantmaker.com/references/esp32-arduino-core-index/esp32-arduino-core-wifi/esp32-wifi-library-station-class/esp32-wifi-library-wifi-psk/
    return true;
}
