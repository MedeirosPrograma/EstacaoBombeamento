#include "SensorFactory.h"
#include "SensorNivel.h"
#include "SensorPressao.h"
#include "SensorContaminacao.h"
#include "SensorVazao.h"
#include <stdexcept> // para o caso invalido throw std::invalid_argument

Sensor* SensorFactory::criarSensor(TipoSensor tipo, const std::string& tag, const std::string& unidade, const bool& nligado) {
    switch (tipo) {
        case TipoSensor::NIVEL:
            return new SensorNivel(tag, unidade, nligado);
        case TipoSensor::CONTAMINACAO:
            return new SensorContaminacao(tag, unidade, nligado);
        case TipoSensor::VAZAO:
            return new SensorVazao(tag, unidade, nligado);
        case TipoSensor::PRESSAO:
            return new SensorPressao(tag, unidade, nligado);
        default:
            throw std::invalid_argument("Tipo de sensor desconhecido na Fabrica.");
    }
}