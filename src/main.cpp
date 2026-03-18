#include <Arduino.h>
#include "Program.h"

Program* m_program = nullptr;

void setup() {
    Serial.begin(115200);
    m_program = new Program();
    m_program->setup();
}

void loop() {
    if (m_program) {
        m_program->loop();
    }
}
