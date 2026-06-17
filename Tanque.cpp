#include "Tanque.h"
#include <random>
#include <algorithm> // Para usar o std::clamp

// Construtor corrigido: inicializa também a variável de contaminação
Tanque::Tanque() : nivelAtual(0.0), pressaoAtual(2.0), vazaoAtual(0.0), contaminado(false) {}

void Tanque::simularFisica(const std::vector<Bomba*>& listaBombas) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    double variacaoPressao = 0.0;

    for (const Bomba* b : listaBombas) {
        if (b->isLigada()) {
            
            // CENÁRIO A: Bomba operando normalmente (Sem falha)
            if (!b->emFalha()) {
                if (b->gettipo()) { // Se b.gettipo() for true (Bomba de Encher)
                    std::uniform_real_distribution<double> distEnche(10.0, 20.0);
                    nivelAtual += distEnche(gen);
                    
                    // CORRIGIDO: Sorteio de 1 em 1000 usando distribuição de INTEIROS
                    std::uniform_int_distribution<int> distContaminado(1, 1000);
                    if (distContaminado(gen) == 1) {
                        contaminado = true;
                    }
                } 
                else { // Se b.gettipo() for false (Bomba de Esvaziar)
                    std::uniform_real_distribution<double> distEscoa(10.0, 20.0);
                    nivelAtual -= distEscoa(gen);  
                }        
            }
            // CENÁRIO B: Bomba ligada, mas EM FALHA
            else {
                // O nível se mantém (não mexe no valorAtual), mas a pressão do sistema sobe!
                std::uniform_real_distribution<double> distPressao(0.8, 2.0);
                variacaoPressao += distPressao(gen);
            }
        }
        // Se a bomba estiver desligada, entra aqui (não faz nada, o nível se mantém)
    }

    // Aplica a variação acumulada na pressão real
    if (variacaoPressao == 0.0) {
        // Se nenhuma bomba falha, aplica apenas uma oscilação de leve
        std::uniform_real_distribution<double> ruido(-0.05, 0.05);
        pressaoAtual += ruido(gen);
    } else {
        pressaoAtual += variacaoPressao;
    }

    // CORRIGIDO: Travando os limites físicos para evitar falhas matemáticas na simulação
    nivelAtual = std::clamp(nivelAtual, 0.0, 100.0);    // Nível entre 0% e 100%
    pressaoAtual = std::clamp(pressaoAtual, 0.0, 15.0);   // Pressão máxima do cano
}