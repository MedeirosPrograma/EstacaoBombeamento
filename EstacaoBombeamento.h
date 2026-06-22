#ifndef ESTACAOBOMBEAMENTO_H
#define ESTACAOBOMBEAMENTO_H
#include "Tipos.h"
#include <string>
#include <vector>
class Bomba;
class Sensor;
class EstacaoBombeamento {
private:
    std::string idEstacao;
    std::vector<Sensor*> sensores;
    std::vector<Bomba*> bombas;
    std::vector<std::string> historicoAlarmes;
    int grupoAtual;         // Controla qual grupo de bombas está ativo (ex: 1 ou 2)
    bool emManutencao;      // Flag que indica se o sistema está travado para manutenção
    bool cicloBloqueado;    // Flag que indica se o sistema está com sensores disfuncionais
    void trocabomba(bool tipo);
    void manutencao();
    void duplabomba();  //regra de controle especifica da dupla
    void sensoresligados();
    //regras de controle

public:
    EstacaoBombeamento(const std::string& nomeArquivoConfig);
    virtual ~EstacaoBombeamento();
    void processarCiclo();
    void atualizardados();
    void gerarCSV();
    void ResolveAlertas();
};

#endif // dupla 016