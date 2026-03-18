#ifndef CONFIG_H
#define CONFIG_H

// Identifiant de la boîte / station (pour les données sauvegardées)
#define BOITE_ID 1

// Sauvegarde locale
#define INTERVALLE_SAUVEGARDE_MS 60000UL

// Fichiers sur LittleFS
#define FICHIER_DONNEES_PATH    "/donnees.json"
#define FICHIER_CONNEXION_PATH  "/connexion.json"
#define FICHIER_CONFIG_PATH     "/config.json"

// JSON
#define MAX_JSON_SIZE 1024

// MQTT (valeurs par défaut, peuvent être surchargées par config.json)
#define MQTT_HOST "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "station_meteo_1"
#define MQTT_TOPIC "station/donnees"
#define MQTT_TOPIC_BASE "station/1"
#define MQTT_QOS 0
#define MQTT_RETAIN false
#define MQTT_PUBLICATION_INTERVALLE_MS 30000UL

// WiFi
#define TIMEOUT_MS 20000UL
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Reconnexion et portail de configuration
#define RECONNEXION_INTERVALLE_MS 120000UL // 2 min
#define PORTAL_TIMEOUT_SEC 120

// NTP (obligatoire pour SSL/TLS port 8883 - HiveMQ Cloud)
#define NTP_SERVER      "time.google.com"
#define NTP_GMT_OFFSET  -18000   // Québec (EST)
#define NTP_DST_OFFSET  3600     // Heure d'été (HAE)

// Matériel (BME280, LCD I2C, DEL)
#define BME_ADRESSE_I2C 0x76
#define LCD_ADRESSE_I2C 0x27 
#define LCD_COLONNES 16
#define LCD_LIGNES 2
#define DEL_PIN 2

#endif
