#include "Sensor.h"

Sensor::Sensor(const std::string& nTag, const std::string& nUnidade, const bool& nligado)
    : tag(nTag), valorAtual(20.0), unidade(nUnidade), ligado(nligado) {
        alarmeAtual.tipo = "";
    }
