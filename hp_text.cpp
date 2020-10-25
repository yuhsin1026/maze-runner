#include "hp_text.h"
#include "config.h"

Hp_text::Hp_text(std::shared_ptr<Hp_model> hp, std::shared_ptr<QGraphicsScene> t_scene)
{
    Hp=hp;
    T_scene=t_scene;
    T_scene->addItem(this);
    connect(this->Hp.get(),SIGNAL(used()), this, SLOT(Used()));
}

void Hp_text::display(int minx, int miny)
{
    this->setPlainText("HP");
    this->setPos((Hp->getXPos()-minx)*TILE_SIZE+TILE_SIZE/3,(Hp->getYPos()-miny)*TILE_SIZE+TILE_SIZE/2);
    this->setZValue(2);
    this->show();
}

void Hp_text::Used()
{
    T_scene->removeItem(this);
}
