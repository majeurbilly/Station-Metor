#include "WiFi/GestionnaireWiFi.h"
#include "GestionnaireFichier/GestionnaireFichier.h"
#include <config.h>

GestionnaireWiFi::GestionnaireWiFi()
    : m_timeoutMs(TIMEOUT_MS),
      m_gestionnaireFichier(nullptr) 
{
    this->m_connecte = false;
    this->m_timeOut = false;
    this->m_tempsDebutConnexion = 0;
}

void GestionnaireWiFi::configurerDepuisFichier(GestionnaireFichier* p_gestionnaireFichier) 
{
    if (p_gestionnaireFichier != nullptr) 
    {
        m_gestionnaireFichier = p_gestionnaireFichier;
        p_gestionnaireFichier->chargerConfigWifi();
        this->m_ssid = p_gestionnaireFichier->getSsid();
        this->m_motDePasse = p_gestionnaireFichier->getMotDePasse();
    }
}

bool GestionnaireWiFi::connecter()
{
    if (m_gestionnaireFichier != nullptr) 
    {
        return connecterDepuisListe();
    }

    if (m_ssid.isEmpty()) 
    {
        return false;
    }

    return connecterReseau(m_ssid, m_motDePasse);
}

void GestionnaireWiFi::deconnecter() 
{
    WiFi.disconnect();
    this->m_connecte = false;
}

bool GestionnaireWiFi::estConnecte() const
{
    return WiFi.status() == WL_CONNECTED;
}

bool GestionnaireWiFi::timeOutAtteint() const
{
    return m_timeOut;
}

IPAddress GestionnaireWiFi::obtenirAdresseIP() const
{
    if (WiFi.status() != WL_CONNECTED) 
    {
        return IPAddress(0, 0, 0, 0);
    }

    return WiFi.localIP();
}

String GestionnaireWiFi::obtenirInformationsReseau() const
{
    if (WiFi.status() != WL_CONNECTED) 
    {
        return "";
    }

    return WiFi.SSID() + " @ " + WiFi.localIP().toString();
}

bool GestionnaireWiFi::connecterDepuisListe()
{
    int index = 0;
    bool auMoinsUnReseau = false;

    while (true) 
    {
        String ssidCourant;
        String motDePasseCourant;

        if (!m_gestionnaireFichier->chargerConnexionIndex(index, ssidCourant, motDePasseCourant)) {
            break;
        }

        auMoinsUnReseau = true;

        if (ssidCourant.isEmpty()) {
            index++;
            continue;
        }

        if (connecterReseau(ssidCourant, motDePasseCourant)) {
            return true;
        }

        index++;
    }

    return false;
}

bool GestionnaireWiFi::connecterReseau(const String& p_ssid, const String& p_motDePasse)
{
    m_ssid = p_ssid;
    m_motDePasse = p_motDePasse;
    m_timeOut = false;
    m_connecte = false;
    m_tempsDebutConnexion = millis();

    WiFi.mode(WIFI_STA);
    WiFi.begin(m_ssid.c_str(), m_motDePasse.c_str());

    while (WiFi.status() != WL_CONNECTED) 
    {
        if (millis() - m_tempsDebutConnexion >= m_timeoutMs) 
        {
            m_timeOut = true;
            return false;
        }
        delay(100);
    }

    m_connecte = true;
    return true;
}