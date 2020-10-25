#ifndef HP_TEXT_H
#define HP_TEXT_H
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "hp_model.h"
#include "world.h"
class Hp_model;
class Hp_text:public QGraphicsTextItem
{
Q_OBJECT
public:
    Hp_text(std::shared_ptr<Hp_model>,std::shared_ptr<QGraphicsScene>);
    void display(int minx,int miny);
    std::shared_ptr<Hp_model> gethp(){return Hp;}
public slots:
    void Used();
private:
    std::shared_ptr<Hp_model> Hp;
    std::shared_ptr<QGraphicsScene> T_scene;
};

#endif // HP_TEXT_H
