#include "SensorContaminacao.h"
#include <iostream>

// Construtor: repassa a tag e unidade para a classe mãe (Sensor)
// e inicializa a flag local 'contaminado' como falsa
SensorContaminacao::SensorContaminacao(const std::string& nTag, const std::string& nUnidade, const bool& nligado)
    : Sensor(nTag, nUnidade, nligado), contaminado(false) {
        alarmeAtual.tipo = ""; // Inicializa sem nenhum alarme ativo
    }

void SensorContaminacao::atualizarLeitura(const Tanque& tanque) {
    contaminado = tanque.iscontaminado();
    valorAtual = contaminado ? 1.0 : 0.0;
    //executa a verificação de segurança
    alerta();
}

void SensorContaminacao::alerta() {
    // Reseta o alarme do ciclo anterior para evitar lixo de memória
    alarmeAtual.tipo = "";

    // Se o sensor detectou contaminação, preenche alarme
    if (contaminado) {
        alarmeAtual.tag = tag;
        alarmeAtual.tipo = "FLUIDO_CONTAMINADO";
        alarmeAtual.valor = 1.0; 

        // Mensagem de aviso no console do simulador
        std::cout << "[ALERTA INTERNO CRITICO] " << tag 
                  << " detectou impurezas ou contaminacao quimica no tanque!\n";
    }
}