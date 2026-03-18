#pragma once

#include <Arduino.h>

class LCD;
class DEL;

class AffichageStation {
public:
    AffichageStation(LCD* lcd, DEL* del);

    void afficherDonnees(float temperature, float humidite, float pression);
    void afficherEtat(int mode);
    void afficherMessage(const String& message);

private:
    LCD* m_lcd;
    DEL* m_del;
};

