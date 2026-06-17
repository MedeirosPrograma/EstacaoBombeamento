#ifndef SENSOR_PRESSAO_H
#define SENSOR_PRESSAO_H
#include "Tanque.h"
#include "Sensor.h"

class SensorPressao : public Sensor {
private:
    double limitePressaoAlta = 5.7;

protected:
    void alerta() override;
    //uso de polimorfismo ja que a classe filha requer um alerta proprio
public:
    SensorPressao(const std::string& nTag, const std::string& nUnidade, const bool& nligado);
    void atualizarLeitura(const Tanque& tanque) override;
    //uso de polimorfismo ja que a classe filha requer uma leitura propria
};

#endif // dupla 016