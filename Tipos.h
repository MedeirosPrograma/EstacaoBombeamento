#ifndef TIPOS_H
#define TIPOS_H

#include <string>

// A struct agora vive isolada e protegida aqui
struct alarme {
    std::string tag;
    std::string tipo;
    double valor;
};
enum class TipoSensor {
    NIVEL,
    VAZAO,
    CONTAMINACAO,
    PRESSAO
};

// Se no futuro precisar de outras structs, colocarei aqui!
//sensor não deve conhecer o que é o struct alarme

#endif