#ifndef TANQUE_H
#define TANQUE_H

#include <vector>
#include "Atuadores/Bomba.h" // O tanque precisa conhecer a classe Bomba para ler os métodos dela

class Tanque {
private:
    bool contaminado;
    double nivelAtual;
    double pressaoAtual;
    double vazaoAtual;
    bool manutencao;
public:
    Tanque();

    // O tanque agora recebe a lista completa de bombas
    // O tanque passa uma lista de ponteiros para evitar copias desnecessariasd 
    void simularFisica(const std::vector<Bomba*>& listaBombas);
    bool iscontaminado() const {return contaminado; }
    bool isinmanutencao() const {return manutencao; }
    double getNivel() const { return nivelAtual; }
    double getPressao() const { return pressaoAtual; }
};

#endif