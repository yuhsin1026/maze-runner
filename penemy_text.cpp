#include "penemy_text.h"
#include "config.h"
PEnemy_text::PEnemy_text(int Max_col, int Max_row, std::shared_ptr<PEnemy> enemy, std::shared_ptr<QGraphicsScene>t_scene)
{
    maxcol = Max_col;
    maxrow = Max_row;

    pEnemy=enemy;
    T_scene=t_scene;
    T_scene->addItem(this);
    connect(this->pEnemy.get(), SIGNAL(dead()), this, SLOT(dead()));
    connect(this->pEnemy.get(), SIGNAL(poisonLevelUpdated(int)), this, SLOT(poison(int)));

}

void PEnemy_text::display(int minx, int miny)
{
    min_x=minx;
    min_y=miny;
    if(!this->getenemy()->getDefeated())
        this->setPlainText("/PE\\");
    this->setPos((pEnemy->getXPos()-minx)*TILE_SIZE+TILE_SIZE/4,(pEnemy->getYPos()-miny)*TILE_SIZE+TILE_SIZE/2);
    this->setZValue(2);
    this->show();
}

void PEnemy_text::setboundary(int x, int y)
{
    min_x=x;
    min_y=y;
}

int PEnemy_text::getValue() const
{
    return value;
}

void PEnemy_text::dead()
{
    this->setPlainText("/!\\");
    poison(static_cast<int>(pEnemy->getPoisonLevel()));
}

void PEnemy_text::poison(int level)
{
    value=level;
    if(pEnemy->getValue()<=pEnemy->getPoisonLevel())
        return;
    if(!region.empty())
        region.clear();
    int size = (level<10 && level>0)?1:(level+10)/10;
    if(level==0) return;
    for(int y=pEnemy->getYPos()-size; y<=pEnemy->getYPos()+size; y++)
    {
        for(int x=pEnemy->getXPos()-size; x<=pEnemy->getXPos()+size; x++)
        {
            if(x==pEnemy->getXPos() && y==pEnemy->getYPos())
                continue;
            if(x>=min_x && x<min_x+NUM_HOR_TILES &&y>=min_y&&y<min_y+NUM_VER_TILES)
            {
                QString oxo = "X";
                region.push_back(std::shared_ptr<QGraphicsTextItem>(new QGraphicsTextItem(oxo)));
                T_scene->addItem(region.back().get());
                region.back()->setPos((x-min_x)*TILE_SIZE+TILE_SIZE/4, (y-min_y)*TILE_SIZE+TILE_SIZE/2);
                region.back()->setZValue(1);
            }
        }
    }
    //emit poison_change(size);
}
