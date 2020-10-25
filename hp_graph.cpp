#include "hp_graph.h"
#include "config.h"

Hp_graph::Hp_graph(std::shared_ptr<Hp_model> hp,std::shared_ptr<QGraphicsScene> g_scene)
{
    image.load(HP);
    image.save(HP,"PNG");
    setPixmap(image);
    Hp=hp;
    G_scene=g_scene;
    G_scene->addItem(this);
    connect(this->Hp.get(),SIGNAL(used()), this, SLOT(display()));
}

void Hp_graph::display()
{
    //this->setScale(0.5);
    this->setPos(Hp->getXPos()*TILE_SIZE,Hp->getYPos()*TILE_SIZE);
    this->setZValue(2);//healthpack->layer 1
    //this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    if(Hp->getUsed())
        this->hide();
    else
        this->show();
}
