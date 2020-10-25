#include "enemy_text.h"
#include "config.h"
Enemy_text::Enemy_text(std::shared_ptr<Enemy> enemy, std::shared_ptr<QGraphicsScene>t_scene)
{
    eEnemy=enemy;
    T_scene=t_scene;
    T_scene->addItem(this);
    connect(this->eEnemy.get(), SIGNAL(dead()), this, SLOT(dead()));

}

void Enemy_text::display(int minx, int miny)
{
    if(!this->getenemy()->getDefeated())
        this->setPlainText("/E\\");
    this->setPos((eEnemy->getXPos()-minx)*TILE_SIZE+TILE_SIZE/4,(eEnemy->getYPos()-miny)*TILE_SIZE+TILE_SIZE/2);
    this->setZValue(2);
    this->show();
}

void Enemy_text::dead()
{
    this->setPlainText("/x\\");
}
