#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

#include "Sensor.h"
#include "../Tipos.h"
#include <string>

class SensorFactory {
public:
    // Retorna um ponteiro bruto da classe base (Sensor*)
    static Sensor* criarSensor(TipoSensor tipo, const std::string& tag, const std::string& unidade, const bool& ligado);
};

#endif