#ifndef SENSOR_CONTAMINACAO_H
#define SENSOR_CONTAMINACAO_H
#include "Tanque.h"
#include "Sensor.h"

class SensorContaminacao : public Sensor {
private:
    bool contaminado;
protected:
    void alerta() override;
    //uso de polimorfismo ja que a classe filha requer um alerta proprio
public:
    SensorContaminacao(const std::string& nTag, const std::string& nUnidade, const bool& nligado);
    void atualizarLeitura(const Tanque& tanque) override;
    //uso de polimorfismo ja que a classe filha requer uma leitura propria
};

#endif // dupla 016