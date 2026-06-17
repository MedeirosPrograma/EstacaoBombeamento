#ifndef SENSOR_VAZAO_H
#define SENSOR_VAZAO_H
#include "Tanque.h"
#include "Sensor.h"

class SensorVazao : public Sensor {
protected:
    void alerta() override;
    double limiteVazaoCritica = 40.0;
    //uso de polimorfismo ja que a classe filha requer um alerta proprio
public:
    SensorVazao(const std::string& nTag, const std::string& nUnidade, const bool& nligado);
    void atualizarLeitura(const Tanque& tanque) override;
    //uso de polimorfismo ja que a classe filha requer uma leitura propria
};

#endif // dupla 016