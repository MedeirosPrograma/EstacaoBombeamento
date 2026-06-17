#include "SensorVazao.h"
#include <iostream>
#include <cmath> // Para usar std::abs pra vazão em módulo

SensorVazao::SensorVazao(const std::string& nTag, const std::string& nUnidade, const bool& nligado)
    : Sensor(nTag, nUnidade, nligado) {
        alarmeAtual.tipo = ""; // Inicializa sem nenhum alarme ativo
    }

void SensorVazao::atualizarLeitura(const Tanque& tanque) {
    static double nivelAnterior = 20.0; 
    double nivelAtual = tanque.getNivel();
    valorAtual = nivelAtual - nivelAnterior;
    nivelAnterior = nivelAtual;
    alerta();
}

void SensorVazao::alerta() {
    // Reseta o alarme do ciclo anterior para evitar dados antigos na memória
    alarmeAtual.tipo = "";
    // Se a vazão (positiva ou negativa) for excessiva, pode indicar um cano estourado ou transbordo
    if (std::abs(valorAtual) > limiteVazaoCritica) {
        alarmeAtual.tag = tag;
        alarmeAtual.tipo = "VAZAO_ANORMAL";
        alarmeAtual.valor = valorAtual;

        std::cout << "[ALERTA INTERNO] Vazão instável ou excessiva detectada em " 
                  << tag << ": " << valorAtual << " " << unidade << "/ciclo\n";
    }
}