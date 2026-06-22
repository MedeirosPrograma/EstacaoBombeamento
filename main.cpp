#include "EstacaoBombeamento.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>

void rodarCenario(const std::string& arquivoConfig) {
    std::cout << "\n==================================================\n";
    std::cout << " INICIANDO CENÁRIO: " << arquivoConfig << "\n";
    std::cout << "==================================================\n";

    // Instancia a estação passando o arquivo JSON específico deste cenário
    EstacaoBombeamento estacao(arquivoConfig);

    // Simula a quantidade de ciclos determinada para esta rotina
    for (int ciclo = 1; ciclo != 0; ++ciclo) {
        std::cout << "\n--- [CICLO " << ciclo << " / " <<"] ---\n";
        
        // Processa a física, alarmes, dupla bomba, troca de bomba e gera o JSONL na memória
        estacao.processarCiclo(); 
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
; 
    
    std::cout << "==================================================\n";
    std::cout << " FINISHED CÉNARIO: " << arquivoConfig << "\n";
    std::cout << "==================================================\n\n";
}

int main() {
    std::cout << "--- SIMULADOR DE ESTAÇÃO DE BOMBEMENTO INDUSTRIAL ---\n";

    // Cenário 1: Tudo operando perfeitamente por 3 ciclos
    while (true){
    rodarCenario("config_inicial.json");
    }
    // Cenário 2: Uma rotina onde uma bomba já inicia quebrada ou sensores emitem alertas
    //rodarCenario("cenario_falha_bomba.json", "relatorio_falha.jsonl", 3);

    // Cenário 3: Uma rotina onde um sensor crítico está desligado de propósito
    //rodarCenario("cenario_sensor_desligado.json", "relatorio_bloqueio.jsonl", 2);

    std::cout << "Todos os cenários de teste foram executados com sucesso!\n";
    return 0;
}