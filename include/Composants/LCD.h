#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <config.h>

class LCD {
public:
    LCD(uint8_t adresseI2C, uint8_t colonnes, uint8_t lignes);

    void initialiser();
    void effacer();
    void afficherLigne(uint8_t ligne, const String& texte);

private:
    uint8_t m_adresseI2C;
    uint8_t m_colonnes;
    uint8_t m_lignes;
    LiquidCrystal_I2C* m_lcd;
};
