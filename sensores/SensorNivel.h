#ifndef SENSOR_NIVEL_H
#define SENSOR_NIVEL_H
#include "../Tanque.h"
#include "Sensor.h"

class SensorNivel : public Sensor {
    //uso de herança
private:
    double limiteMuitoBaixo = 16.0;
    double limiteBaixo = 25.0;
    double limiteAlto = 75.0;
    double limiteMuitoAlto = 95.0;
protected:
    void alerta() override;
    //uso de polimorfismo ja que a classe filha requer um alerta proprio
public:
    SensorNivel(const std::string& nTag, const std::string& nUnidade, const bool& nligado);
    void atualizarLeitura(const Tanque& tanque) override;
    //uso de polimorfismo ja que a classe filha requer uma leitura propria
};

#endif // dupla 016