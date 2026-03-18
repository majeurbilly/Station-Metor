#include "MQTT/MQTTBroker.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Taille buffer pour payload JSON (temperature, humidite, pression)
#define MQTT_BUFFER_SIZE 512

MQTTBroker::MQTTBroker()
{
    this->m_wifiClient = nullptr;
    this->m_mqtt = nullptr;
    this->m_secureClient = nullptr;

    this->m_host = MQTT_HOST;
    this->m_port = MQTT_PORT;
    this->m_user = "";
    this->m_password = "";

    this->m_clientId = MQTT_CLIENT_ID;
    this->m_topic = MQTT_TOPIC;
    this->m_qos = MQTT_QOS;
    this->m_retain = MQTT_RETAIN;
}

void MQTTBroker::setHost(const char* p_host) 
{ 
    if (p_host != nullptr) 
    {
        m_host = p_host;
    }
}

void MQTTBroker::setPort(int p_port)
{
    m_port = p_port;
}

void MQTTBroker::setUser(const char* p_user)
{
    if (p_user != nullptr)
    {
        m_user = p_user;
    } 
    else 
    {
        m_user = "";
    }
}

void MQTTBroker::setPassword(const char* p_password)
{
    if (p_password != nullptr) 
    {
        m_password = p_password;
    } 
    else 
    {
        m_password = "";
    }
}

bool MQTTBroker::connecter(const char* p_clientId)
{
    if (p_clientId != nullptr) 
    {
        m_clientId = p_clientId;
    }

    return connecter();
}

bool MQTTBroker::connecter()
{
    if (WiFi.status() != WL_CONNECTED) 
    {
        return false;
    }

    if (m_mqtt != nullptr) 
    {
        delete m_mqtt;
        m_mqtt = nullptr;
    }
    if (m_secureClient != nullptr) 
    {
        delete m_secureClient;
        m_secureClient = nullptr;
        m_wifiClient = nullptr;
    }
    else if (m_wifiClient != nullptr) 
    {
        delete m_wifiClient;
        m_wifiClient = nullptr;
    }

    if (m_port == 8883) 
    {
        WiFiClientSecure* secureClient = new WiFiClientSecure();
        secureClient->setInsecure();  // Pour tests ; en prod : setCACert()
        m_secureClient = secureClient;
        m_wifiClient = secureClient;
    } 
    else 
    {
        m_wifiClient = new WiFiClient();
    }

    m_mqtt = new PubSubClient(*m_wifiClient);
    m_mqtt->setBufferSize(MQTT_BUFFER_SIZE);
    m_mqtt->setServer(m_host.c_str(), m_port);

    bool estConnecte = false;

    if (!m_user.isEmpty()) 
    {
        estConnecte = m_mqtt->connect(
            m_clientId.c_str(),
            m_user.c_str(),
            m_password.c_str()
        );
    } 
    else 
    {
        estConnecte = m_mqtt->connect(m_clientId.c_str());
    }

    if (!estConnecte) 
    {
        Serial.println(m_mqtt->state());
        return false;
    }

    return true;
}

bool MQTTBroker::estConnecte() const
{
    if (m_mqtt == nullptr) 
    {
        return false;
    }

    return m_mqtt->connected();
}

void MQTTBroker::publier(const String& p_topic, const String& p_payload)
{
    if (m_mqtt != nullptr && estConnecte())
    {
        m_mqtt->publish(p_topic.c_str(), p_payload.c_str());
    }
}

void MQTTBroker::publier(const String& p_topic, const String& p_payload, bool p_retain)
{
    if (m_mqtt != nullptr && estConnecte())
    {
        m_mqtt->publish(p_topic.c_str(), p_payload.c_str(), p_retain);
    }
}

void MQTTBroker::souscrire(const String& p_topic)
{
    if (m_mqtt != nullptr && estConnecte()) 
    {
        m_mqtt->subscribe(p_topic.c_str());
    }
}

void MQTTBroker::traiter()
{
    if (m_mqtt != nullptr) 
    {
        m_mqtt->loop();
    }
}