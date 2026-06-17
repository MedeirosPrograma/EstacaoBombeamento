#ifndef BOMBA_H
#define BOMBA_H
#include <string>

class Bomba {
private:
    std::string tag;
    bool ligada;
    bool falhaBloqueio; 
    int grupo;
    bool tipo;
public:
    Bomba(const std::string& nTag, int nGrupo,bool nEstado, bool nFalha, bool nTipo);
    virtual ~Bomba() = default;
    virtual void ligar();
    void desligar();
    std::string gettag() const;
    int getgrupo() const;
    bool gettipo() const;
    bool isLigada() const;
    bool emFalha() const;
};

#endif // dupla 016