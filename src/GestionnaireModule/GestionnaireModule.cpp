#include "GestionnaireModule/GestionnaireModule.h"
#include "Composants/BME.h"

GestionnaireModule::GestionnaireModule(BME& p_bme) {
    this->m_bme = &p_bme;
}

void GestionnaireModule::initialiser() {
    if (this->m_bme != nullptr) {
        this->m_bme->initialiser();
    }
}

float GestionnaireModule::lireTemperature() {
    if (this->m_bme == nullptr) {
        return 0.0f;
    }
    return this->m_bme->lireTemperature();
}

float GestionnaireModule::lireHumidite() {
    if (this->m_bme == nullptr) {
        return 0.0f;
    }
    return this->m_bme->lireHumidite();
}

float GestionnaireModule::lirePression() {
    if (this->m_bme == nullptr) {
        return 0.0f;
    }
    return this->m_bme->lirePression();
}