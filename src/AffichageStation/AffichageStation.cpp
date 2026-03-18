#include "AffichageStation/AffichageStation.h"
#include "GestionnaireStation/EtatStation.h"
#include "Composants/LCD.h"
#include "Composants/DEL.h"

AffichageStation::AffichageStation(LCD* p_lcd, DEL* p_del)
{
    this->m_lcd = p_lcd;
    this->m_del = p_del;
}

void AffichageStation::afficherDonnees(float p_temperature, float p_humidite, float p_pression) {
    if (this->m_lcd == nullptr) {
        return;
    }
    this->m_lcd->effacer();
    this->m_lcd->afficherLigne(0, "T:" + String(p_temperature, 1) + " H:" + String(p_humidite, 0) + "%");
    this->m_lcd->afficherLigne(1, "P:" + String(p_pression, 0) + " hPa");
}

void AffichageStation::afficherEtat(int p_mode) {
    if (this->m_del == nullptr) {
        return;
    }
        this->m_del->eteindre();
        if (p_mode == static_cast<int>(EtatStation::CONNEXION)) {
            this->m_del->clignoter(250, 250);
        } else if (p_mode == static_cast<int>(EtatStation::ERREUR)) {
            this->m_del->allumer();
        } else if (p_mode == static_cast<int>(EtatStation::HORS_CONNEXION)) {
            this->m_del->clignoter(500, 500);
        }
}

void AffichageStation::afficherMessage(const String& p_message) {
    if (this->m_lcd == nullptr) {
        return;
    }
    this->m_lcd->effacer();
    this->m_lcd->afficherLigne(0, p_message.substring(0, 16));
    if (p_message.length() > 16) {
        this->m_lcd->afficherLigne(1, p_message.substring(16, 32));
    }
}
