#ifndef HP_MODEL_H
#define HP_MODEL_H
#include "world.h"
class Hp_model:public QObject, public Tile
{
 Q_OBJECT
public:
    Hp_model(int,int,float);
    bool    getUsed() const {return !exist;}
    void    setUsed(bool v){exist=!v; if(v) emit used();}
signals:
    void used();
private:
    bool exist;

};
#endif // HP_MODEL_H
