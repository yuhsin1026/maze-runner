#include "hp_model.h"

Hp_model::Hp_model(int x, int y, float strength):Tile(x, y, strength)
{
    value=qrand()%50+50;
    exist=true;
}
