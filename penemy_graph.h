#ifndef PENEMY_GRAPH_H
#define PENEMY_GRAPH_H
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "world.h"


class PEnemy_graph: public QObject,public QGraphicsPixmapItem
{
Q_OBJECT
public:
    PEnemy_graph(int,int,std::shared_ptr<PEnemy>,std::shared_ptr<QGraphicsScene>);
    void display();


    std::shared_ptr<PEnemy> getenemy(){return pEnemy;}
    int getValue() const;
    void setValue(int value);

private:
    std::vector<std::shared_ptr<QGraphicsRectItem>> effectregion;
    std::shared_ptr<PEnemy> pEnemy;
    std::shared_ptr<QGraphicsScene> G_scene;
    QPixmap image;
    int value,maxcol,maxrow;

public slots:
    virtual void dead();
    void poison(int level);

signals:
    void poison_change(int);

};
#endif // PENEMY_GRAPH_H
