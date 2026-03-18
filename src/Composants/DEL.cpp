#include "Composants/DEL.h"
#include <Arduino.h>

DEL::DEL(uint8_t p_pinDEL)
{
    this->m_pinDEL = p_pinDEL;
    this->m_enClignotement = false;
    this->m_etatActuel = false;
    this->m_tempsAllume = 0;
    this->m_tempsEteint = 0;
    this->m_dernierChangement = 0;
    
    pinMode(this->m_pinDEL, OUTPUT);
    digitalWrite(this->m_pinDEL, LOW);
}

void DEL::allumer()
{
    this->m_enClignotement = false;
    this->m_etatActuel = true;
    digitalWrite(this->m_pinDEL, HIGH);
}

void DEL::eteindre()
{
    this->m_enClignotement = false;
    this->m_etatActuel = false;
    digitalWrite(this->m_pinDEL, LOW);
}

bool DEL::estAllumee() const
{
    return this->m_etatActuel;
}

void DEL::tick()
{
    if (!this->m_enClignotement)
    {
        return;
    }

    unsigned long tempsActuel = millis();
    unsigned long tempsDepuisChangement = tempsActuel - this->m_dernierChangement;

    if (this->m_etatActuel)
    {
        if (tempsDepuisChangement >= this->m_tempsAllume)
        {
            this->m_etatActuel = false;
            this->m_dernierChangement = tempsActuel;
            digitalWrite(this->m_pinDEL, LOW);
        }
    }
    else
    {
        if (tempsDepuisChangement >= this->m_tempsEteint)
        {
            this->m_etatActuel = true;
            this->m_dernierChangement = tempsActuel;
            digitalWrite(this->m_pinDEL, HIGH);
        }
    }
}

void DEL::clignoter(unsigned long p_tempsAllume, unsigned long p_tempsEteint)
{
    this->m_tempsAllume = p_tempsAllume;
    this->m_tempsEteint = p_tempsEteint;

    this->m_enClignotement = true;
    this->m_etatActuel = true;
    this->m_dernierChangement = millis();
    digitalWrite(this->m_pinDEL, HIGH);
}