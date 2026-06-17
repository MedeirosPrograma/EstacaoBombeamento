#include "SensorNivel.h"
#include <iostream>

SensorNivel::SensorNivel(const std::string& nTag, const std::string& nUnidade, const bool& nligado)
    : Sensor(nTag, nUnidade, nligado) {
        alarmeAtual.tipo = "";
    }

void SensorNivel::atualizarLeitura(const Tanque& tanque) {
    // O sensor não inventa o valor, ele apenas "copia" do tanque
    valorAtual = tanque.getNivel(); 
    alerta(); // O alerta continua testando se violou os limites
}
void SensorNivel::alerta() {
    alarmeAtual.tipo = "";
    // zera o alarme atual evitando erro de lixo 
    if (valorAtual > limiteMuitoAlto) { 
        alarmeAtual.tag = tag;
        alarmeAtual.tipo = "NIVEL_CRITICO_ALTO";
        alarmeAtual.valor = valorAtual;

        std::cout << "[ALERTA INTERNO] Nivel muito alto detectado em " << tag << ": " << valorAtual << "%\n";
    }
    else if (valorAtual > limiteAlto) { 
        alarmeAtual.tag = tag;
        alarmeAtual.tipo = "NIVEL_ALTO";
        alarmeAtual.valor = valorAtual;

        std::cout << "[ALERTA INTERNO] Nivel alto detectado em " << tag << ": " << valorAtual << "%\n";
    } 
    else if (valorAtual < limiteMuitoBaixo) { 
        alarmeAtual.tag = tag;     
        alarmeAtual.tipo = "NIVEL_CRITICO_BAIXO";
        alarmeAtual.valor = valorAtual;

        std::cout << "[ALERTA INTERNO] Nivel muito baixo detectado em " << tag << ": " << valorAtual << "%\n";
    } 
     else if (valorAtual < limiteBaixo) { 
        alarmeAtual.tag = tag;
        alarmeAtual.tipo = "NIVEL_BAIXO";
        alarmeAtual.valor = valorAtual;

        std::cout << "[ALERTA INTERNO] Nivel baixo detectado em " << tag << ": " << valorAtual << unidade << "\n";
    }
}