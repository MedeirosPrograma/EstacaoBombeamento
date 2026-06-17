#include "SensorPressao.h"
#include <iostream>

SensorPressao::SensorPressao(const std::string& nTag, const std::string& nUnidade, const bool& nligado)
    : Sensor(nTag, nUnidade, nligado) {
        alarmeAtual.tipo = "";
    }

void SensorPressao::atualizarLeitura(const Tanque& tanque) {
    // se o sensor estiver desligado, avisa a estação parando o ciclo
    valorAtual = tanque.getPressao();
    alerta();
}
void SensorPressao::alerta() {
    alarmeAtual.tipo = "";
    // zera o alarme atual evitando erro de lixo 
    if (valorAtual > limitePressaoAlta) {
        alarmeAtual.tag = tag;
        alarmeAtual.tipo = "PRESSAO_ALTA";
        alarmeAtual.valor = valorAtual;

        std::cout << "[ALERTA INTERNO] Nivel muito alto de pressao detectado em " << tag << ": " << valorAtual << unidade << "\n";
    }
}