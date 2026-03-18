#pragma once

#include "GestionnaireFichier/GestionnaireFichier.h"

class ConfigurateurPortail {
public:
    ConfigurateurPortail(GestionnaireFichier& p_gestionnaireFichier);
    bool ouvrir();

private:
    GestionnaireFichier& m_gestionnaireFichier;
};
