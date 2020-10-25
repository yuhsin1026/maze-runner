#ifndef HP_GRAPH_H
#define HP_GRAPH_H
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "hp_model.h"
#include "world.h"

class Hp_model;
class Hp_graph: public QObject,public QGraphicsPixmapItem
{
Q_OBJECT
public:
    Hp_graph(std::shared_ptr<Hp_model>,std::shared_ptr<QGraphicsScene>);
    std::shared_ptr<Hp_model> gethp(){return Hp;}
public slots:
    void display();
private:
    std::shared_ptr<Hp_model> Hp;
    std::shared_ptr<QGraphicsScene> G_scene;
    QPixmap image;

};
#endif // HP_GRAPH_H
