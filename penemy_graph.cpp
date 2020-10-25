#include "config.h"
#include "penemy_graph.h"

PEnemy_graph::PEnemy_graph(int Max_col,int Max_row,std::shared_ptr<PEnemy> enemy,std::shared_ptr<QGraphicsScene> g_scene)
{
    maxcol=Max_col;
    maxrow=Max_row;
    image.load(PENEMY);
    image.save(PENEMY,"PNG");

    setPixmap(image);
    pEnemy=enemy;
    G_scene=g_scene;
    G_scene->addItem(this);
    setValue(0);
    connect(this->pEnemy.get(), SIGNAL(dead()), this, SLOT(dead()));
    connect(this->pEnemy.get(), SIGNAL(poisonLevelUpdated(int)), this, SLOT(poison(int)));

}

void PEnemy_graph::display()
{
    this->setScale(1);
    this->setPos(pEnemy->getXPos()*TILE_SIZE,pEnemy->getYPos()*TILE_SIZE);
    this->setZValue(2);//enemy->layer 2
    this->show();
}

int PEnemy_graph::getValue() const
{
    return value;
}

void PEnemy_graph::setValue(int Value)
{
    value = Value;
}

void PEnemy_graph::dead()
{
    image.load(PENEMY_DEAD);
    image.save(PENEMY_DEAD,"PNG");

    this->setPixmap(image);
    poison(static_cast<int>(pEnemy->getPoisonLevel()));

}

void PEnemy_graph::poison(int level)
{
    if(!effectregion.empty())
        effectregion.clear();
    int region=(level<10 && level>0)?1:(level+10)/10;
    if(level==0) return;

    for(int y=pEnemy->getYPos()-region;y<=pEnemy->getYPos()+region;y++)
        for(int x=pEnemy->getXPos()-region;x<=pEnemy->getXPos()+region;x++)
        {
            if(y<0||y>=maxrow||x<0||x>=maxcol)
                continue;
            else
            {
                effectregion.push_back(std::shared_ptr<QGraphicsRectItem>(new QGraphicsRectItem(x*TILE_SIZE,y*TILE_SIZE,TILE_SIZE,TILE_SIZE)));
                G_scene->addItem(effectregion.back().get());
                effectregion.back()->setZValue(1);
                effectregion.back()->setBrush(QColor(0, 180, 0));
            }
        }
    setValue(region);
    emit poison_change(region);
}
