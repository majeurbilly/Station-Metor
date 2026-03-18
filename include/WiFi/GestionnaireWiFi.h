#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <config.h>

#include "GestionnaireFichier/GestionnaireFichier.h"

class GestionnaireWiFi {
public:
    GestionnaireWiFi();

    void configurerDepuisFichier(GestionnaireFichier* p_gestionnaireFichier);
    bool connecter();
    void deconnecter();

    bool estConnecte() const;
    bool timeOutAtteint() const;
    IPAddress obtenirAdresseIP() const;
    String obtenirInformationsReseau() const;

private:
    String m_ssid;
    String m_motDePasse;
    bool m_connecte;
    bool m_timeOut;
    unsigned long m_tempsDebutConnexion;
    const unsigned long m_timeoutMs;
    GestionnaireFichier* m_gestionnaireFichier;

    bool connecterDepuisListe();
    bool connecterReseau(const String& p_ssid, const String& p_motDePasse);
};