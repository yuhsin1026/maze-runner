#ifndef ENEMY_GRAPH_H
#define ENEMY_GRAPH_H
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "world.h"

class Enemy_graph: public QObject,public QGraphicsPixmapItem
{
Q_OBJECT
public:
    Enemy_graph(std::shared_ptr<Enemy>,std::shared_ptr<QGraphicsScene>);

    std::shared_ptr<Enemy> getenemy(){return eEnemy;}
    void display();

private:
    std::shared_ptr<Enemy> eEnemy;
    std::shared_ptr<QGraphicsScene> G_scene;
    QPixmap image;

public slots:
    virtual void dead();

};
#endif // ENEMY_GRAPH_H
