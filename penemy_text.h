#ifndef PENEMY_TEXT_H
#define PENEMY_TEXT_H
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "world.h"


class PEnemy_text: public QGraphicsTextItem
{
Q_OBJECT
public:
    PEnemy_text(int maxcol, int maxrow, std::shared_ptr<PEnemy>,std::shared_ptr<QGraphicsScene>);
    void display(int minx,int miny);
    void setboundary(int x,int y);
    std::shared_ptr<PEnemy> getenemy(){return pEnemy;}
    int getValue() const;

private:
    std::shared_ptr<PEnemy> pEnemy;
    std::shared_ptr<QGraphicsScene> T_scene;
    std::vector<std::shared_ptr<QGraphicsTextItem>> region;
    int maxcol,maxrow,min_x,min_y,value;

public slots:
    virtual void dead();
    void poison(int level);
};
#endif // PENEMY_TEXT_H
