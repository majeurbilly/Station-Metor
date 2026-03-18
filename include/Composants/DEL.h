#pragma once

#include <Arduino.h>

class DEL
{
public:
    DEL(uint8_t p_pinDEL);

    void allumer();
    void eteindre();
    bool estAllumee() const;

    void clignoter(unsigned long p_tempsAllume, unsigned long p_tempsEteint);
    void tick();

private:
    uint8_t m_pinDEL;

    bool m_enClignotement;
    bool m_etatActuel;
    unsigned long m_tempsAllume;
    unsigned long m_tempsEteint;
    unsigned long m_dernierChangement;
};
