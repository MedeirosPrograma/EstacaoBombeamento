#include "EstacaoBombeamento.h"
#include "sensores/SensorFactory.h"
#include "Tanque.h"
#include "Tipos.h"
#include <iostream>
#include <fstream>  // Biblioteca para abertura e fechamento de arquivos físicos
#include <chrono>   // Para capturar o tempo do sistema (Timestamps)
#include <ctime>    // Para formatar a estrutura de tempo
#include <iomanip>  // Para manipulação e formatação de saídas de texto (std::put_time)
#include <sstream>  // Para manipulação de strings em fluxo de memória
#include <nlohmann/json.hpp> 
using json = nlohmann::json;
Tanque tanqueSimulado;

// Função utilitária para gerar o Timestamp atual no formato YYYY-MM-DD HH:MM:SS
std::string obterTimestampAtual() {
    auto agora = std::chrono::system_clock::now();
    std::time_t tempo_c = std::chrono::system_clock::to_time_t(agora);
    std::tm* tempo_local = std::localtime(&tempo_c);
    
    std::stringstream ss;
    ss << std::put_time(tempo_local, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// CONSTRUTOR: Abre o arquivo de configuração para inicializar a rotina do sistema
EstacaoBombeamento::EstacaoBombeamento(const std::string& nomeArquivoConfig) {
    idEstacao = "EB-016";
    grupoAtual = 1;       // Inicialização dos atributos de controle da dupla
    emManutencao = false;
    cicloBloqueado = false;

    std::ifstream arquivoLeitura(nomeArquivoConfig);

    if (!arquivoLeitura.is_open()) {
        std::cerr << "[" << obterTimestampAtual() << "] [ERRO] O arquivo 'config_inicial.json' nao foi encontrado!\n";
        std::cerr << "Abortando inicializacao automatica.\n";
        return;
    }

    try {
        json dados;
        arquivoLeitura >> dados; 
        arquivoLeitura.close();
        idEstacao = dados["id_estacao"];
        std::cout << "[" << obterTimestampAtual() << "] Inicializando " << idEstacao << " via arquivo JSON...\n";
        
        // Carga dinâmica de sensores via Fábrica (padrao escolhido)
        for (const auto& s : dados["sensores_iniciais"]) {
            std::string tipoStr = s["tipo"];
            TipoSensor tipo;

            if (tipoStr == "NIVEL") tipo = TipoSensor::NIVEL;
            else if (tipoStr == "VAZAO") tipo = TipoSensor::VAZAO;
            else if (tipoStr == "CONTAMINACAO") tipo = TipoSensor::CONTAMINACAO;
            else if (tipoStr == "PRESSAO") tipo = TipoSensor::PRESSAO;
            else continue;
            sensores.push_back(SensorFactory::criarSensor(tipo, s["tag"], s["unidade"], s["ligado"]));
        }

          for (const auto& b : dados["bombas_iniciais"]) {
            std::string tag = b["tag"];
            int grupo = b["grupo"];
            bool ligada = b["ligada"];
            bool falha = b["emFalha"];
            bool tipo = b["tipo"];
            bombas.push_back(new Bomba(tag, grupo, ligada, falha, tipo));
        }

        std::cout << "[" << obterTimestampAtual() << "] Sistema carregado com sucesso!\n";

    } catch (const std::exception& e) {
        std::cerr << "[ERRO] Falha crítica ao fazer o parse do JSON de entrada: " << e.what() << "\n";
        if (arquivoLeitura.is_open()) {
            arquivoLeitura.close(); 
        }
    }
}

// DESTRUTOR: Libera a memória alocada dinamicamente
EstacaoBombeamento::~EstacaoBombeamento() {
    for (Sensor* s : sensores) {
        delete s;
    }
    for (Bomba* b : bombas) {
        delete b;
    }
    sensores.clear();
    bombas.clear();
    std::cout << "[" << obterTimestampAtual() << "] Estacao finalizada e memoria limpa.\n";
}

// Processamento do ciclo físico-lógico da estação de bombeamento
void EstacaoBombeamento::processarCiclo() {
    atualizardados();
    cicloBloqueado = false;

    // roda as travas de segurança primeiro
    manutencao();
    sensoresligados();
    
    // se alguma trava ativou o 'cicloBloqueado', interrompe o processamento imediatamente
    if (cicloBloqueado) {
        std::cout << "[" << obterTimestampAtual() << "] Ciclo abortado devido a condicoes inseguras.\n";
        gerarCSV(); // Registra o estado travado
        return; 
    }

    
    tanqueSimulado.simularFisica(bombas);
    
    //atualiza a leitura de cada sensor baseado no novo estado do tanque
    for (Sensor* s : sensores) {
        s->atualizarLeitura(tanqueSimulado);
        
        // se houver novos alarmes, registra no histórico
        if (!s->getAlarme().tipo.empty()) {
            std::string logAlarme = "[" + obterTimestampAtual() + "] Sensor " + 
                                    s->getAlarme().tag + " reportou: " + s->getAlarme().tipo;
            historicoAlarmes.push_back(logAlarme);
        }
    }
        manutencao();
    sensoresligados();
    // roda as travas de segurança primeiro
    ResolveAlertas();
    duplabomba();
    gerarCSV();
}

// Monta a estrutura JSON comprimida em uma linha e joga na lista (Conceito de Pushback)
void EstacaoBombeamento::gerarCSV() {

    // Abre o arquivo em modo "append" (anexa ao final sem destruir dados passados)

    std::ofstream arquivoCSV("historico_estacao.csv", std::ios::app);



    if (!arquivoCSV.is_open()) {

        std::cerr << "[" << obterTimestampAtual() << "] [ERRO] Falha ao abrir o arquivo 'historico_estacao.csv' para gravacao.\n";

        return;

    }



    // Criação automática do cabeçalho caso o arquivo acabe de ser gerado (esteja vazio)

    arquivoCSV.seekp(0, std::ios::end);

    if (arquivoCSV.tellp() == 0) {

        arquivoCSV << "timestamp;id_estacao;emManutenção";

       
        
        // Colunas dinâmicas para Sensores

        for (Sensor* s : sensores) {

            arquivoCSV << ";" << s->getTag() << "_tag"
                       << ";" << s->getTag() << "_valor"
                       << ";" << s->getTag() << "_unidade"
                       << ";" << s->getTag() << "_ligado";
        }

       

        // Colunas dinâmicas para Bombas

        for (Bomba* b : bombas) {

            arquivoCSV << ";" << b->gettag() << "_tag"
                       << ";" << b->gettag() << "_tipo"
                       << ";" << b->gettag() << "_grupo"
                       << ";" << b->gettag() << "_ligada"
                       << ";" << b->gettag() << "_em_falha";

        }

       

        arquivoCSV << ";ultimo_alarme\n";

    }



    // Injeção de dados básicos do ciclo

    arquivoCSV << obterTimestampAtual() << ";"

               << idEstacao<< ";"

               << emManutencao;



    // Injeção de dados de telemetria dos Sensores (ativos = 1, desligados = 0)

    for (Sensor* s : sensores) {

        arquivoCSV << ";" << s->getTag()
                   << ";" << s->getvalor()
                   << ";" << s->getUnidade()
                   << ";" << (s->funcionando() ? "Ligado" : "Desligado");

    }



    // Injeção de dados de estados das Bombas (ativo = 1, inativo = 0)

    for (Bomba* b : bombas) {

        arquivoCSV << ";" << b->gettag()
                   << ";" << (b->gettipo() ? "Encher" : "Esvaziar")
                   << ";" << b->getgrupo()
                   << ";" << (b->isLigada() ? "Ligada" : "Desligada")
                   << ";" << (b->emFalha() ? "emFalha" : "SemFalha");
    }
    // Higienização e injeção da string do último alarme

    std::string alarmeStr = !historicoAlarmes.empty() ? historicoAlarmes.back() : "NENHUM";
    // Substitui caracteres ';' por espaço para não corromper o alinhamento das colunas CSV

    std::replace(alarmeStr.begin(), alarmeStr.end(), ';', ' ');
    arquivoCSV << ";" << alarmeStr << "\n";

    arquivoCSV.close();
}
void EstacaoBombeamento::atualizardados(){
    std::ifstream arquivoLeitura("comandos_python.json");
    if (!arquivoLeitura.is_open()) return;

    bool comandoExecutado = false;
    try {
        json comandos;
        arquivoLeitura >> comandos;
        arquivoLeitura.close();
        //VERIFICA E PROCESSA OS COMANDOS DAS BOMBAS
        if (comandos.contains("bomba_comandos") && !comandos["bomba_comandos"].empty()) {
            for (const auto& cmd : comandos["bomba_comandos"]) {
                if (!cmd.contains("tag") || !cmd.contains("comando")) continue;
                
                std::string tagCmd = cmd["tag"];
                std::string acao = cmd["comando"];
                
                for (Bomba* b : bombas) {
                    if (b->gettag() == tagCmd) {
                        if (acao == "FORCAR_DESLIGAR") {
                            b->desligar(); 
                            comandoExecutado = true;
                        } 
                        else if (acao == "FORCAR_LIGAR") {
                            b->ligar();
                            comandoExecutado = true;
                        }
                    }
                }
            }
        }

        //SE PROCESSOU COMANDOS, LIMPA A FILA
        if (comandoExecutado) {
            std::ofstream arquivoEscrita("comandos_python.json");
            if (arquivoEscrita.is_open()) {
                json jsonRetorno;
                jsonRetorno["bomba_comandos"] = json::array(); 
                
                arquivoEscrita << jsonRetorno.dump(4);
                arquivoEscrita.close();
            }
        }
    } catch (...) {
        // Protege contra leitura concorrente se o Python estiver escrevendo
    }
}
// REGRA: Garante a integridade dos hardwares de sensoriamento
void EstacaoBombeamento::sensoresligados() {
    for (Sensor* s : sensores) {
        if (!s->funcionando()) {
            std::cout << "[" << obterTimestampAtual() << "] [BLOQUEIO CRÍTICO] O sensor " 
                      << s->getTag() << " esta DESLIGADO. Parando o processamento por seguranca.\n";
            cicloBloqueado = true;
            return;
        }
    }
}

// REGRA: para níveis alarmantes fora do controle padrão liga duas bombas
void EstacaoBombeamento::duplabomba() {
    for (Sensor* s : sensores) {
        if (s->getAlarme().tipo.empty()) continue;
        
        // Emergência de nível baixo: Aciona força máxima de entrada e desliga as de saida
        if (s->getAlarme().tipo == "NIVEL_CRITICO_BAIXO") {
            int bombasAtivadasAqui = 0; 
            for (Bomba* b : bombas) {
                if (b->gettipo() == true && bombasAtivadasAqui < 2) { // true = encher
                    b->ligar();
                    bombasAtivadasAqui++;
                } else if (b->gettipo() == false) {
                    b->desligar(); 
                }
            }
        } 
        // Emergência de nível alto: Aciona força máxima de saída e desliga as de entrada
        else if (s->getAlarme().tipo == "NIVEL_CRITICO_ALTO") {
            int bombasAtivadasAqui = 0; 
            for (Bomba* b : bombas) {
                if (b->gettipo() == false && bombasAtivadasAqui < 2) { // false = esvaziar
                    b->ligar();
                    bombasAtivadasAqui++;
                } else if (b->gettipo() == true) {
                    b->desligar();
                }
            }
        }
    }
}

// REGRA:revezamento de grupos para diminuir desgaste material
void EstacaoBombeamento::trocabomba(bool tipo) {
    std::cout << "[" << obterTimestampAtual() << "] [REVESAMENTO] Alternando do Grupo " 
              << grupoAtual << " para o Grupo " << (grupoAtual == 1 ? 2 : 1) << ".\n";

    grupoAtual = (grupoAtual == 1) ? 2 : 1;
    // Liga as bombas do grupo da vez e desliga as do outro grupo
    for (Bomba* b : bombas) {
        // Lógica grupos impares e pares alternam trocados (para quatro bombas que seria o padrão por tanque na cabeça dos projetistas :) )
        int grupoDaBomba = (b->getgrupo() % 2 != 0) ? 1 : 2;
        if (grupoDaBomba == grupoAtual && tipo == b->gettipo()) {
            b->ligar();
        } else {
            b->desligar();
        }
    }
}

// REGRA: Bloqueio total por intervenção técnica humana ou falha física catastrófica
void EstacaoBombeamento::manutencao() {
    if (emManutencao) {
        std::cout << "[" << obterTimestampAtual() << "] [MANUTENCAO] Planta em estado de manutencao ativo. Operacoes travadas.\n";
        cicloBloqueado = true;
    }
}

// Central de Inteligência que analisa os alarmes e despacha as ações
void EstacaoBombeamento::ResolveAlertas() {
    for (Sensor* s : sensores) {
        if (s->getAlarme().tipo.empty()) continue;
        
        // Níveis normais de flutuação disparam o revesamento com exclusividade
        if (s->getAlarme().tipo == "NIVEL_BAIXO") {
            trocabomba(true);  // Revesa e garante apenas bombas de ENCHER operando
        } 
        else if (s->getAlarme().tipo == "NIVEL_ALTO") {
            trocabomba(false); // Revesa e garante apenas bombas de ESVAZIAR operando
        }
        // Falhas graves forçam parada de manutenção
        else if (s->getAlarme().tipo == "PRESSAO_ALTA" || 
                 s->getAlarme().tipo == "VAZAO_ANORMAL" || 
                 s->getAlarme().tipo == "FLUIDO_CONTAMINADO") {
            emManutencao = true;
        }
    }
}
