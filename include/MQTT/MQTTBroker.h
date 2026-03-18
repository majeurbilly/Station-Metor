#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <config.h>


class MQTTBroker {
public:
    MQTTBroker();

    void setHost(const char* p_host);
    void setPort(int p_port);
    void setUser(const char* p_user);
    void setPassword(const char* p_password);

    bool connecter();
    bool connecter(const char* p_clientId);
    bool estConnecte() const;

    void publier(const String& p_topic, const String& p_payload);
    void publier(const String& p_topic, const String& p_payload, bool p_retain);
    void souscrire(const String& p_topic);
    void traiter();

private:
    WiFiClient* m_wifiClient;
    WiFiClientSecure* m_secureClient;  // Non-null uniquement si port 8883
    PubSubClient* m_mqtt;
    
    String m_host;
    int m_port;
    String m_user;
    String m_password;

    String m_clientId;
    String m_topic;
    int m_qos;
    bool m_retain;
};