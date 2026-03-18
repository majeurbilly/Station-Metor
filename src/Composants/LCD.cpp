#include "Composants/LCD.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LCD::LCD(uint8_t p_adresseI2C, uint8_t p_colonnes, uint8_t p_lignes)
{
    this->m_adresseI2C = p_adresseI2C;
    this->m_colonnes = p_colonnes;
    this->m_lignes = p_lignes;
    this->m_lcd = nullptr;
}

void LCD::initialiser() {
    this->m_lcd = new LiquidCrystal_I2C(this->m_adresseI2C, this->m_colonnes, this->m_lignes);
    this->m_lcd->begin(this->m_colonnes, this->m_lignes);
    m_lcd->setBacklight(4);
}

void LCD::effacer() {
    if (this->m_lcd != nullptr) {
        this->m_lcd->clear();
    }
}

void LCD::afficherLigne(uint8_t p_ligne, const String& p_texte) {
    if (this->m_lcd == nullptr || p_ligne >= this->m_lignes){
        return;
    }
    this->m_lcd->setCursor(0, p_ligne);
    this->m_lcd->print(p_texte.substring(0, this->m_colonnes));
}
