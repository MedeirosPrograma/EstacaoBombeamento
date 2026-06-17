#ifndef SENSOR_H
#define SENSOR_H
#include "Tipos.h"
//o sensor futuramente deve conhecer o tanque
#include "Tanque.h"
#include <string>


class Sensor {
protected:
    std::string tag;
    double valorAtual;
    std::string unidade;
    bool ligado;
    virtual void alerta() = 0; 
    //uso explicito de polimorfisomo, classes filhas terão alertas diferentes
    alarme alarmeAtual;
public:
    Sensor(const std::string& nTag, const std::string& nUnidade, const bool& nligado);//passagem de referencia por constante sugerida no cap 2 do AI_log
    virtual ~Sensor() = default; // Destrutor virtual sugerido no cap 1 do AI_log

    double getvalor() const {return valorAtual; }
    bool funcionando() const {return ligado; }
    std::string getTag() const {return tag; }
    std::string getUnidade() const {return unidade; }
    alarme getAlarme() const {return alarmeAtual; }
};

#endif // dupla 016