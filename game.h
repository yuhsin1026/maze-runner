#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "pathfinder.h"
#include "world.h"
#include "view.h"
#include <memory>
#include <vector>
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QDirIterator>

class View;
class Game : public QObject,public QGraphicsPixmapItem
{
Q_OBJECT

public:
    Game(QString const& map, unsigned int nrOfEnemies, unsigned int nrOfHealthpacks);
    ~Game();

    std::shared_ptr<Pathfinder> pathfinder;
    std::shared_ptr<World> world;
    std::vector<std::shared_ptr<Enemy>> dead_enemies;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::vector<std::shared_ptr<Hp_model>> healthpacks;
    std::vector<std::unique_ptr<Tile>> tiles;
    std::vector<std::pair<int,int>> route;
    std::vector<std::shared_ptr<QGraphicsRectItem>> route_tile;
    std::shared_ptr<Warrior_model> my_protagonist;
    std::shared_ptr<View> view;

    std::shared_ptr<Enemy> go_nearest_enemy();
    std::shared_ptr<Hp_model> go_nearest_hp();

    void    heightMountain();
    float   calculate_cost(std::vector<std::pair<int,int>> &route);
    bool    draw_route();
    void    setStartPosition(QString const &world);
    void    auto_move();
    Direction     check_mov(int dx,int dy);
    void    in_poison(int,int);
    void    moveProtagonist(Direction direction);
    void    setmode(bool newmode){mode = newmode;}
    void    setSpeed(int value);
    bool    aliveCheck();
    std::pair<int,int> not_used();

    bool    getPause() const;
    void    setPause(bool value);
    bool    getAction() const;
    bool    getDisconnected() const;
    void    setDisconnected(bool value);
    void    goToXY(int x, int y);

private:
    std::vector<int> valuesProt;
    unsigned int     nrOfXEnemies;
    std::vector<std::shared_ptr<QGraphicsTextItem>> route_path;
    bool    mode,pause,disconnected;
    int     speed;
    QString WorldMap;

signals:
   void     changePos(Direction direction);
   void     changeHealth(int);
   void     changeEnergy(int);
   void     changeNum();
   void     statusUpdate(QString);

public slots:
   void     game_logic(Direction direction);
   void     keyPress(Direction direction);
   void     in_poison(int value);
   void     move_to_next();
};

#endif // GAME_H
