#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "GestionnaireFichier/GestionnaireFichier.h"
#include "config.h"

const char* FICHIER_DONNEES = FICHIER_DONNEES_PATH;
const char* FICHIER_CONNEXION = FICHIER_CONNEXION_PATH;
const char* FICHIER_CONFIG = FICHIER_CONFIG_PATH;

GestionnaireFichier::GestionnaireFichier()
{
    m_ssid = "";
    m_motDePasse = "";

    m_mqttHost = MQTT_HOST;
    m_mqttPort = MQTT_PORT;
    m_mqttUser = "";
    m_mqttPassword = "";

    m_boiteId = BOITE_ID;
    mettreAJourIdentiteStation();
}

void GestionnaireFichier::mettreAJourIdentiteStation()
{
    m_mqttClientId = "ESP32_Bateau_" + String(m_boiteId < 10 ? "0" : "") + String(m_boiteId);
    m_mqttTopicBase = "meteor/bateau_" + String(m_boiteId < 10 ? "0" : "") + String(m_boiteId);
}

void GestionnaireFichier::begin()
{
    if (!LittleFS.begin(true)) {
        return;
    }

    Serial.println("LittleFS initialise");

    if (!LittleFS.exists(FICHIER_DONNEES_PATH)) {
        File fichier = LittleFS.open(FICHIER_DONNEES_PATH, FILE_WRITE);
        if (fichier) {
            fichier.close();
            Serial.println("donnees.json cree");
        } else {
            Serial.println("Impossible de creer donnees.json");
        }
    }
}

bool GestionnaireFichier::chargerConnexion()
{
    DynamicJsonDocument doc(MAX_JSON_SIZE);

    if (!lireJson(FICHIER_CONNEXION, doc))
        return false;

    JsonArray arr = doc.as<JsonArray>();

    if (arr.size() == 0)
        return false;

    JsonObject obj = arr[0];

    m_ssid = obj["ssid"] | "";
    m_motDePasse = obj["password"] | "";

    return true;
}

bool GestionnaireFichier::chargerConnexionIndex(int p_index, String& p_ssid, String& p_password)
{
    DynamicJsonDocument doc(MAX_JSON_SIZE);

    if (!lireJson(FICHIER_CONNEXION, doc))
    {
        return false;
    }

    JsonArray arr = doc.as<JsonArray>();

    if (p_index < 0 || p_index >= arr.size())
    {
        return false;
    }

    JsonObject obj = arr[p_index];

    p_ssid = obj["ssid"] | "";
    p_password = obj["password"] | "";

    return true;
}

void GestionnaireFichier::chargerConfigWifi()
{
    m_ssid = "";
    m_motDePasse = "";

    if (!chargerConnexion())
    { 
        return; 
    }

    if (m_ssid.isEmpty())
    {
        m_ssid = WIFI_SSID;
    }

    if (m_motDePasse.isEmpty())
    {
        m_motDePasse = WIFI_PASSWORD;
    }
}

bool GestionnaireFichier::chargerConfigMQTT()
{
    DynamicJsonDocument doc(MAX_JSON_SIZE);

    m_mqttHost = MQTT_HOST;
    m_mqttPort = MQTT_PORT;

    if (!lireJson(FICHIER_CONFIG, doc)){ 
        return true; 
    }

    m_mqttHost = doc["mqtt_host"] | String(MQTT_HOST);
    m_mqttPort = doc["mqtt_port"] | MQTT_PORT;
    m_mqttUser = doc["mqtt_user"] | "";
    m_mqttPassword = doc["mqtt_password"] | "";

    if (doc["boite_id"].is<int>()) 
    {
        m_boiteId = doc["boite_id"].as<int>();
        if (m_boiteId < 1) 
        {
            m_boiteId = BOITE_ID;
        }
    } else {
        m_boiteId = BOITE_ID;
    }
    mettreAJourIdentiteStation();

    return true;
}

bool GestionnaireFichier::sauvegarderConfigMQTT(const char* p_host, int p_port, const char* p_user, const char* p_password)
{
    DynamicJsonDocument doc(MAX_JSON_SIZE);

    if (lireJson(FICHIER_CONFIG, doc) && doc.is<JsonObject>()) {
    } else {
        doc.clear();
        doc.to<JsonObject>(); 
    }

    JsonObject obj = doc.as<JsonObject>();
    obj["mqtt_host"] = p_host;
    obj["mqtt_port"] = p_port;
    obj["mqtt_user"] = p_user;
    obj["mqtt_password"] = p_password;

    File fichier = LittleFS.open(FICHIER_CONFIG, FILE_WRITE);
    if (!fichier) {
        return false;
    }
    if (serializeJson(doc, fichier) == 0) {
        fichier.close();
        return false;
    }
    fichier.close();

    m_mqttHost = p_host;
    m_mqttPort = p_port;
    m_mqttUser = p_user;
    m_mqttPassword = p_password;
    return true;
}

bool GestionnaireFichier::sauvegarderConfigStation(int p_boiteId)
{
    if (p_boiteId < 1) {
        return false;
    }

    DynamicJsonDocument doc(MAX_JSON_SIZE);
    if (lireJson(FICHIER_CONFIG, doc) && doc.is<JsonObject>()) {
    } else {
        doc.clear();
        doc.to<JsonObject>();
    }

    JsonObject obj = doc.as<JsonObject>();
    obj["boite_id"] = p_boiteId;

    File fichier = LittleFS.open(FICHIER_CONFIG, FILE_WRITE);
    if (!fichier) {
        return false;
    }
    if (serializeJson(doc, fichier) == 0) {
        fichier.close();
        return false;
    }
    fichier.close();

    m_boiteId = p_boiteId;
    mettreAJourIdentiteStation();
    return true;
}

String GestionnaireFichier::getSsid() const
{
    return m_ssid;
}

String GestionnaireFichier::getMotDePasse() const
{
    return m_motDePasse;
}

const char* GestionnaireFichier::getMqttHost() const
{
    return m_mqttHost.c_str();
}

const int GestionnaireFichier::getMqttPort() const
{
    return m_mqttPort;
}

const char* GestionnaireFichier::getMqttUser() const
{
    return m_mqttUser.c_str();
}

const char* GestionnaireFichier::getMqttPassword() const
{
    return m_mqttPassword.c_str();
}

int GestionnaireFichier::getBoiteId() const
{
    return m_boiteId;
}

const char* GestionnaireFichier::getMqttClientId() const
{
    return m_mqttClientId.c_str();
}

const char* GestionnaireFichier::getMqttTopicBase() const
{
    return m_mqttTopicBase.c_str();
}

bool GestionnaireFichier::sauvegarderConnexion(const String& p_ssid, const String& p_password)
{
    return sauvegarderConnexion(p_ssid.c_str(), p_password.c_str());
}

bool GestionnaireFichier::sauvegarderConnexion(const char* p_ssid, const char* p_password)
{
    DynamicJsonDocument doc(MAX_JSON_SIZE);
    JsonArray arr = doc.to<JsonArray>();

    JsonObject reseau = arr.createNestedObject();
    reseau["name"] = p_ssid;
    reseau["ssid"] = p_ssid;
    reseau["password"] = p_password;

    File fichier = LittleFS.open(FICHIER_CONNEXION, FILE_WRITE);
    if (!fichier) {
        return false;
    }

    if (serializeJson(doc, fichier) == 0) {
        fichier.close();
        return false;
    }

    fichier.close();

    m_ssid = p_ssid;
    m_motDePasse = p_password;

    return true;
}

// https://randomnerdtutorials.com/esp32-write-data-littlefs-arduino/
bool GestionnaireFichier::ajouterDonnees(const Donnees& p_donnees)
{
    DynamicJsonDocument doc(384);

    doc["id"] = p_donnees.id;
    doc["temperature"] = p_donnees.temperature;
    doc["humidite"] = p_donnees.humidite;
    doc["pression"] = p_donnees.pression;

    File fichier = LittleFS.open(FICHIER_DONNEES, FILE_APPEND);
    if (!fichier) {
        fichier = LittleFS.open(FICHIER_DONNEES, FILE_WRITE);
    }

    if (!fichier) {
        return false;
    }

    if (serializeJson(doc, fichier) == 0) {
        fichier.close();
        return false;
    }

    fichier.println();
    fichier.close();

    return true;
}

bool GestionnaireFichier::ajouterConnexion(const String& ssid, const String& password)
{
    DynamicJsonDocument doc(1024);

    if (LittleFS.exists(FICHIER_CONNEXION)) 
    {
        File f = LittleFS.open(FICHIER_CONNEXION, FILE_READ);
        deserializeJson(doc, f);
        f.close();
    }

    JsonArray arr;

    if (doc.is<JsonArray>())
    {
        arr = doc.as<JsonArray>();
    }
    else
    {
        arr = doc.to<JsonArray>();
    }

    for (int i = 0; i < (int)arr.size(); i++)
    {
        JsonObject item = arr[i].as<JsonObject>();
        if (item["ssid"].as<String>() == ssid)
        {
            item["name"] = ssid;
            item["password"] = password;
            File f = LittleFS.open(FICHIER_CONNEXION, FILE_WRITE);
            if (!f) return false;
            serializeJson(doc, f);
            f.close();
            return true;
        }
    }

    JsonObject obj = arr.createNestedObject();
    obj["name"] = ssid;
    obj["ssid"] = ssid;
    obj["password"] = password;

    File f = LittleFS.open(FICHIER_CONNEXION, FILE_WRITE);
    serializeJson(doc, f);
    f.close();

    return true;
}

bool GestionnaireFichier::supprimerConnexion(int p_index)
{
    DynamicJsonDocument doc(1024);

    if (!LittleFS.exists(FICHIER_CONNEXION))
        return false;

    if (!lireJson(FICHIER_CONNEXION, doc))
        return false;

    if (!doc.is<JsonArray>())
        return false;

    JsonArray arr = doc.as<JsonArray>();
    if (p_index < 0 || p_index >= (int)arr.size())
        return false;

    arr.remove(p_index);

    File fichier = LittleFS.open(FICHIER_CONNEXION, FILE_WRITE);
    if (!fichier)
    {
        return false;
    }
    if (serializeJson(doc, fichier) == 0)
    {
        fichier.close();
        return false;
    }
    fichier.close();

    return true;
}

// https://docs.arduino.cc/language-reference/en/functions/communication/serial/readStringUntil/
bool GestionnaireFichier::lireDonnees(Donnees p_donnees[], int& p_nbMesures, int p_maxMesures)
{
    p_nbMesures = 0;

    if (!LittleFS.exists(FICHIER_DONNEES)) {
        return true;
    }

    File fichier = LittleFS.open(FICHIER_DONNEES, FILE_READ);
    if (!fichier) {
        return false;
    }

    while (fichier.available() && p_nbMesures < p_maxMesures) {
        String ligne = fichier.readStringUntil('\n');
        ligne.trim();

        if (ligne.isEmpty()) {
            continue;
        }

        DynamicJsonDocument doc(384);
        DeserializationError erreur = deserializeJson(doc, ligne);

        if (!erreur) {
            p_donnees[p_nbMesures].id = doc["id"] | 0;
            p_donnees[p_nbMesures].temperature = doc["temperature"] | 0.0f;
            p_donnees[p_nbMesures].humidite = doc["humidite"] | 0.0f;
            p_donnees[p_nbMesures].pression = doc["pression"] | 0.0f;
            p_nbMesures++;
        } else {
            Serial.println(erreur.c_str());
        }
    }

    fichier.close();
    return true;
}

bool GestionnaireFichier::viderDonnees()
{
    if (LittleFS.exists(FICHIER_DONNEES)) 
    {
        LittleFS.remove(FICHIER_DONNEES);
    }

    return true;
}

bool GestionnaireFichier::lireJson(const char* p_chemin, DynamicJsonDocument& doc)
{
    if (!LittleFS.exists(p_chemin))
    {
        return false;
    }

    File fichier = LittleFS.open(p_chemin, FILE_READ);
    if (!fichier)
    {
        return false;
    }

    DeserializationError erreur = deserializeJson(doc, fichier);
    fichier.close();

    return !erreur;
}