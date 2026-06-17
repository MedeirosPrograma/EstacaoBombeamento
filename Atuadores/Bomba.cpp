#include "Bomba.h"
#include <iostream>

Bomba::Bomba(const std::string& nTag, int nGrupo,bool nEstado, bool nFalha,  bool nTipo) 
    : tag(nTag), ligada(nEstado), falhaBloqueio(nFalha), grupo(nGrupo), tipo(nTipo) {}

void Bomba::ligar() {
    ligada = true;
    std::cout << "Bomba [" << tag << "] foi LIGADA.\n";
}

void Bomba::desligar() {
    ligada = false;
    std::cout << "Bomba [" << tag << "] foi DESLIGADA.\n";
}
bool Bomba::gettipo() const {
    return tipo;
}
int Bomba::getgrupo() const {
    return grupo;
}
std::string Bomba::gettag() const {
    return tag;
}

bool Bomba::isLigada() const {
    return ligada;
}

bool Bomba::emFalha() const {
    return falhaBloqueio;
}