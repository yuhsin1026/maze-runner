#ifndef ENEMY_TEXT_H
#define ENEMY_TEXT_H
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "world.h"


class Enemy_text: public QGraphicsTextItem
{
Q_OBJECT
public:
    Enemy_text(std::shared_ptr<Enemy>,std::shared_ptr<QGraphicsScene>);

    std::shared_ptr<Enemy> getenemy(){return eEnemy;}
        void display(int minx,int miny);

private:
    std::shared_ptr<Enemy> eEnemy;
    std::shared_ptr<QGraphicsScene> T_scene;

public slots:
    virtual void dead();

};
#endif // ENEMY_TEXT_H
