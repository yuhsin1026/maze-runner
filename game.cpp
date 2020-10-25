#include <QDebug>
#include <QKeyEvent>
#include <QString>
#include <QTimer>
#include <cmath>
#include <ctime>
#include <typeinfo>

#include "game.h"
#include "config.h"
#include "xenemy.h"

using namespace std;


Game::Game(QString const& worldMap, unsigned int nrOfEnemies, unsigned int nrOfHealthpacks)
{
    /*----------CREATE WORLD---------------*/
    WorldMap=worldMap;
    world = make_shared<World>();
    pause=RESUME;

    //random position for new enemy
    qsrand(time(nullptr));
    nrOfXEnemies= qrand() %3;
    if(nrOfEnemies==1)
        nrOfXEnemies=0;
    else if(nrOfEnemies==nrOfXEnemies && nrOfEnemies==2)
        nrOfXEnemies=1;

    nrOfEnemies=nrOfEnemies-nrOfXEnemies;
    world->createWorld(worldMap, nrOfEnemies,nrOfHealthpacks);

    /*-----------CREATE MODELS---------------------*/
    //(Use our own model instead of library class)

    //PROTAGONIST MODEL
    my_protagonist = make_shared<Warrior_model>();
    setStartPosition(worldMap);

    //HEALTHPACK MODEL
    auto temp_healthPacks = world->getHealthPacks();
    while(!temp_healthPacks.empty())
    {
        auto temp=temp_healthPacks[0].release();
        temp_healthPacks.erase(temp_healthPacks.begin());
        healthpacks.push_back(shared_ptr<Hp_model>(make_shared <Hp_model>(temp->getXPos(),temp->getYPos(),temp->getValue())));
        delete temp;
    }

    //ENEMY MODEL
    auto temp_enemies = world->getEnemies();
    while(!temp_enemies.empty())
    {
        std::shared_ptr<Enemy> temp = move(temp_enemies[0]);
        auto dummy=temp.get();
        if(typeid(PEnemy).name()==typeid(*dummy).name())
        {
            temp->setValue(31);
            std::dynamic_pointer_cast<PEnemy>(temp)->setPoisonLevel(31);
        }
        enemies.push_back(temp);
        temp_enemies.erase(temp_enemies.begin());
    }

    //XENEMY
    while(enemies.size()<(nrOfXEnemies+nrOfEnemies))
    {
        //Calc a new position pair
        auto position=not_used();
        //Put XEnemy (with new pos) in enemies vector
        std::shared_ptr<Enemy> eptr;
        eptr = std::make_shared<XEnemy>(position.first, position.second, 20);
        enemies.push_back(std::move(eptr));
    }

    /*-----------FURTHER INITIALIZATIONs-------------*/
    view = make_shared<View>(world->getCols(),world->getRows(),worldMap,enemies,healthpacks,my_protagonist);
    tiles = world->getTiles();
    pathfinder = make_shared<Pathfinder>(world->getRows(),world->getCols());
    mode = NORMAL;
    disconnected=false;
    speed = 200;
    //Connect poisonUpdate for each g_penemy
    for(auto &temp:view->g_penemy)
        connect(temp.get(), SIGNAL(poison_change(int)), this, SLOT(in_poison(int)));
}

Game::~Game()
{
    //view->g_scene->removeItem(this);
}


//REGISTER KEYPRESS
void Game::keyPress(Direction direction)
{
    if(!aliveCheck()||disconnected)
        return;
    switch (direction)
    {
        case Direction::LEFT:
            if(my_protagonist->getXPos()!=0)
            {
                emit changePos(Direction::LEFT);   //Set pos of warrior
                game_logic(Direction::LEFT);       //Update world
            }
            break;
        case Direction::RIGHT:
            if(my_protagonist->getXPos()!=world->getCols()-1)
            {
                emit changePos(Direction::RIGHT);
                game_logic(Direction::RIGHT);
            }
            break;
        case Direction::UP:
            if(my_protagonist->getYPos()!=0)
            {
                emit changePos(Direction::UP);
                game_logic(Direction::UP);
            }
            break;
        case Direction::DOWN:
            if(my_protagonist->getYPos()!=world->getRows()-1)
            {
                emit changePos(Direction::DOWN);
                game_logic(Direction::DOWN);
            }
            break;
        case Direction::LEFT_UP:
            if(my_protagonist->getXPos()!=0 || my_protagonist->getYPos()!=0)
            {
                emit changePos(Direction::LEFT_UP);
                game_logic(Direction::LEFT_UP);
            }
            break;
        case Direction::LEFT_DOWN:
            if(my_protagonist->getXPos()!=0 || my_protagonist->getYPos()!=world->getRows()-1)
            {
                emit changePos(Direction::LEFT_DOWN);
                game_logic(Direction::LEFT_DOWN);
            }
            break;
        case Direction::RIGHT_UP:
            if(my_protagonist->getXPos()!=world->getCols()-1||my_protagonist->getYPos()!=0)
            {
                emit changePos(Direction::RIGHT_UP);
                game_logic(Direction::RIGHT_UP);
            }
                break;
        case Direction::RIGHT_DOWN:
            if(my_protagonist->getXPos()!=world->getCols()-1||my_protagonist->getYPos()!=world->getRows()-1)
            {
                emit changePos(Direction::RIGHT_DOWN);
                game_logic(Direction::RIGHT_DOWN);
            }
            break;

        default:
            break;
    }

    view->g_warrior->display();

    heightMountain();
}


// CALCULATE VALUES OF MOVEMENT HEIGHT AROUND PROTAGONIST
void Game::heightMountain()
{
    int x,y;
    unsigned long nrProt = 0;
    valuesProt.clear();
    valuesProt.resize(24);

    //Put height of protagonist tile in 'current'
    x = my_protagonist->getXPos();
    y = my_protagonist->getYPos();
    auto current = tiles[static_cast<unsigned long>(x + y*(world->getCols()))]->getValue();

    //Calculate the heights
    for (int i = x-2; i <= x+2; i++)
    {
        for (int j = y-2; j <= y+2; j++)
        {
            if ( !(i == x && j == y) && view->inBounds(i,j))
            {
                int conv = static_cast<int>(100*fabs(current - tiles[static_cast<unsigned long>(i+j*(world->getCols()))]->getValue()));
                valuesProt[nrProt] = conv;
                nrProt++;
            }
            else if ( !(i == x && j == y))
            {
                valuesProt[nrProt] = -1;
                nrProt++;
            }
        }
    }
    view->display_text(valuesProt);
}



// CONTROLLER: UPDATE WORLD WHEN MOVED
void Game::game_logic(Direction direction)
{
    if(disconnected)return;

    auto x = my_protagonist->getXPos();
    auto y = my_protagonist->getYPos();

    // DEAD ENEMIES
    for(auto const &cur:dead_enemies)
    {
        auto dummy=cur.get();
        //Update /P\ health
        if(typeid(PEnemy).name()==typeid(*dummy).name())
        {
            auto poison_level=std::dynamic_pointer_cast<PEnemy>(cur)->getPoisonLevel();
            if(poison_level>0)
            {
                int value = (poison_level<10)?1:static_cast<int>((poison_level+10)/10);   //Size of poison field
                in_poison(value);
            }
        }
        //Prevent impassible tile
        if(cur->getXPos()==x && cur->getYPos()==y)
            moveProtagonist(direction);
    }

    if(enemies.size() <= nrOfXEnemies)
        for(auto &cur:enemies)
            if(cur->getDefeated() && !std::dynamic_pointer_cast<XEnemy>(cur)->getkilled())
                std::dynamic_pointer_cast<XEnemy>(cur)->setkilled(true);

    //ALIVE ENEMIES
    for(auto const &cur:enemies)
    {
        auto dummy = cur.get();

        //PEnemies
        if(typeid(PEnemy).name()==typeid(*dummy).name())
        {
            //Update /P\ health
            auto poison_level=std::dynamic_pointer_cast<PEnemy>(cur)->getPoisonLevel();
            if(cur->getValue() > poison_level)
            {
                int value=(poison_level<10 && poison_level>0)?1:static_cast<int>((poison_level+10)/10);   //Size of poison field
                if(poison_level==0.F) value=0;
                in_poison(value);
            }
            if(pow(cur->getXPos()-x,2)+pow(cur->getYPos()-y,2) <= pow(3,2) && cur->getValue() <= std::dynamic_pointer_cast<PEnemy>(cur)->getPoisonLevel())
            {
                std::dynamic_pointer_cast<PEnemy>(cur)->poison();
            }
            //Enemy defeated
            if(std::dynamic_pointer_cast<PEnemy>(cur)->getPoisonLevel()<=0)
            {
                auto it = std::find(enemies.begin(), enemies.end(), cur);
                dead_enemies.push_back(*(it));
                cur->setDefeated(true);
                enemies.erase(it);
                //emit statusUpdate(QString("PEnemy got defeated!"));
                emit changeNum();
                break;
            }
        }


        if(cur->getXPos() == x && cur->getYPos() == y)
        {
            //XEnemies
            if(typeid(XEnemy).name()==typeid(*dummy).name())
            {
                //XE Not defeated yet.
                if(cur->getValue() < my_protagonist->getHealth() && !cur->getDefeated())
                {
                    //XEnemy killed but not defeated
                    qDebug()<<"There is a XEnemy";
                    cur->setDefeated(true);
                    auto position = not_used();
                    cur->setXPos(position.first);
                    cur->setYPos(position.second);
                    my_protagonist->setHealth(my_protagonist->getHealth() - cur->getValue());
                    my_protagonist->setEnergy(100);
                    emit changeEnergy(static_cast<int>(my_protagonist->getEnergy()));
                    emit changeHealth(static_cast<int>(my_protagonist->getHealth()));

                    //Split into new normal enemy
                    std::dynamic_pointer_cast<XEnemy>(cur)->split();
                    auto position2 = not_used();
                    auto child=std::dynamic_pointer_cast<XEnemy>(cur)->getChild();
                    if(child!=nullptr)
                    {
                        child->setXPos(position2.first);
                        child->setXPos(position2.second);
                        enemies.push_back(child);
                        view->add_new_enemy(child);
                        emit changeNum();
                    }
                    //emit statusUpdate(QString("XEnemy defeated! Split into zombie and normal enemy."));
                }
                //XE defeated but not killed.
                else if(cur->getDefeated() && !std::dynamic_pointer_cast<XEnemy>(cur)->getkilled())
                {
                    my_protagonist->setHealth(my_protagonist->getHealth()-5);
                    emit changeHealth(static_cast<int>(my_protagonist->getHealth()));
                    //emit statusUpdate(QString("There is a zombie."));
                    //qDebug()<<"There is a zombie";
                }
                //XE defeated AND killed.
                else if(cur->getDefeated() && std::dynamic_pointer_cast<XEnemy>(cur)->getkilled())
                {
                    qDebug()<<"Kill a zombie";
                    nrOfXEnemies--;
                    auto it = std::find(enemies.begin(), enemies.end(), cur);
                    (*it)->setDefeated(true);
                    dead_enemies.push_back(*(it));
                    enemies.erase(it);
                    emit changeNum();
                    //emit statusUpdate(QString("XEnemy killed."));
                }
                view->display_graph();
                view->display_text(valuesProt);
                moveProtagonist(direction);
                return;
            }
            //Defeat enemy
            if(cur->getValue()/2 < my_protagonist->getHealth() && !cur->getDefeated())
             {
                cur->setDefeated(true);
                my_protagonist->setHealth(my_protagonist->getHealth()-cur->getValue()/2);
                my_protagonist->setEnergy(100);
                emit changeEnergy(static_cast<int>(my_protagonist->getEnergy()));
                emit changeHealth(static_cast<int>(my_protagonist->getHealth()));
                auto it = std::find(enemies.begin(), enemies.end(), cur);
                dead_enemies.push_back(*(it));
                enemies.erase(it);
                emit changeNum();                
             }

            if(enemies.size()==0)
            {
                //emit statusUpdate(QString("You won! Click reset or restart to play another game."));
            }
            else
                emit statusUpdate(QString("Enemy defeated."));

             //Prevent impassible tile
             moveProtagonist(direction);
             return;

        }
    }


    //CALC ENERGY_consume (energy to move to tile)
    float energy_consume = 0;
    auto prot_value = tiles[static_cast<unsigned long>(world->getCols()*y+x)]->getValue();
    switch(direction)
                 {
                    case    Direction::UP:
                        energy_consume = fabs(tiles[static_cast<unsigned long>(world->getCols()*(y+1)+x)]->getValue()-prot_value);
                        break;
                    case    Direction::DOWN:
                        energy_consume = fabs(tiles[static_cast<unsigned long>(world->getCols()*(y-1)+x)]->getValue()-prot_value);
                        break;
                    case    Direction::RIGHT:
                        energy_consume = fabs(tiles[static_cast<unsigned long>(world->getCols()*y+x-1)]->getValue()-prot_value);
                        break;
                    case    Direction::LEFT:
                        energy_consume = fabs(tiles[static_cast<unsigned long>(world->getCols()*y+x+1)]->getValue()-prot_value);
                        break;
                    case    Direction::LEFT_UP:
                        energy_consume = fabs(tiles[static_cast<unsigned long>(world->getCols()*(y+1)+x+1)]->getValue()-prot_value);
                        break;
                    case    Direction::LEFT_DOWN:
                        energy_consume = fabs(tiles[static_cast<unsigned long>(world->getCols()*(y-1)+x+1)]->getValue()-prot_value);
                        break;
                    case    Direction::RIGHT_UP:
                        energy_consume = fabs(tiles[static_cast<unsigned long>(world->getCols()*(y+1)+x-1)]->getValue()-prot_value);
                        break;
                    case    Direction::RIGHT_DOWN:
                        energy_consume = fabs(tiles[static_cast<unsigned long>(world->getCols()*(y-1)+x-1)]->getValue()-prot_value);
                        break;
                    default:
                        break;
                 }

    //MOVE (based on energy_consume)
    energy_consume = 10*(0.001+energy_consume);
    if(energy_consume > my_protagonist->getEnergy()){
        emit(statusUpdate("Not enough energy to move"));
        qDebug()<<"Not enough energy to move";
        moveProtagonist(direction);
    }
    else
    {
        my_protagonist->setEnergy(my_protagonist->getEnergy()-energy_consume);
        emit changeEnergy(static_cast<int>(my_protagonist->getEnergy()));
    }

    //HEALTHPACKS
    for(auto const &cur:healthpacks)
    {   //ON HP TILE?
        if(cur->getXPos() == my_protagonist->getXPos() && cur->getYPos() == my_protagonist->getYPos())
        {    //HP ALREADY USED?
             if(!cur->getUsed())
             {  //USE HP
                cur->setUsed(true);
                float increased_health = (my_protagonist->getHealth()+cur->getValue());
                my_protagonist->setHealth(increased_health>100?100:increased_health);
                emit changeHealth(static_cast<int>(my_protagonist->getHealth()));                
                auto it = std::find(healthpacks.begin(), healthpacks.end(), cur);
                healthpacks.erase(it);
                emit changeNum();
                return;
             }
        }
    }
}


/* PATHFINDER: FIND NEAREST ENEMY */
std::shared_ptr<Enemy> Game::go_nearest_enemy()
{
    if(!aliveCheck()||disconnected)
        return nullptr;
    if(enemies.empty())
    {
        qDebug() << "No enemies left";
        //emit statusUpdate(QString("You won! Click reset or restart to play another game."));
        return nullptr;
    }

    if(enemies.size() <= nrOfXEnemies)
        for(auto &cur:enemies)
            if(cur->getDefeated() && !std::dynamic_pointer_cast<XEnemy>(cur)->getkilled())
                std::dynamic_pointer_cast<XEnemy>(cur)->setkilled(true);

    //Collect positions all enemies (in a vector containing pairs (x,y))
    std::vector<std::pair<int,int>> wall;
    for(const auto & cur:healthpacks)
        wall.emplace_back(make_pair(cur->getXPos(),cur->getYPos()));
    for(const auto & cur:dead_enemies)
        wall.emplace_back(make_pair(cur->getXPos(),cur->getYPos()));

    double distance=pow(10,8);
    auto x=my_protagonist->getXPos();
    auto y=my_protagonist->getYPos();
    std::shared_ptr<Enemy> nearest;

    for(auto &cur:enemies)
    {//Add enemy position to wall
        wall.emplace_back(make_pair(cur->getXPos(),cur->getYPos()));
     //Find nearest enemy
        auto dummy = cur.get();
        if(typeid(XEnemy).name()==typeid(*dummy).name())
            if(!std::dynamic_pointer_cast < XEnemy>(cur)->getkilled( )&& cur->getDefeated())
                continue;
        if(distance > (pow(cur->getXPos()-x,2)+pow(cur->getYPos()-y, 2)))
        {
            distance = (pow(cur->getXPos()-x,2)+pow(cur->getYPos()-y, 2));
            nearest = cur;
        }
    } //Remove nearest enemy position from wall
    auto it = std::find(wall.begin(), wall.end(), make_pair(nearest->getXPos(),nearest->getYPos()));
        if(it != wall.end())
            wall.erase(it);

    //Determine the path
    pathfinder->startPathfinding(x,y,nearest->getXPos(),nearest->getYPos(),tiles,wall);
    emit statusUpdate(QString("Determined path to next enemy"));
    return nearest;
}

/* PATHFINDER: FIND NEAREST HP */
std::shared_ptr<Hp_model> Game::go_nearest_hp()
{
    if(!aliveCheck()||disconnected)
        return nullptr;
    if(healthpacks.empty())
    {
        qDebug() << "No healthpacks left";
        return nullptr;
    }
    //Collect positions all enemies (in a vector containing pairs (x,y))
    std::vector<std::pair<int,int>> wall;
    for(auto &cur:enemies)
        wall.emplace_back(make_pair(cur->getXPos(),cur->getYPos()));
    for(auto &cur:dead_enemies)
        wall.emplace_back(make_pair(cur->getXPos(),cur->getYPos()));

    double distance = pow(10,8);
    auto x = my_protagonist->getXPos();
    auto y = my_protagonist->getYPos();
    std::shared_ptr<Hp_model> nearest;
    //Search nearest HP
    for(auto const &cur:healthpacks)
        if(distance > (pow(cur->getXPos()-x,2)+pow(cur->getYPos()-y,2)))
        {
            distance = (pow(cur->getXPos()-x,2)+pow(cur->getYPos()-y,2));
            nearest = cur;
        }
    //Determine the path
    pathfinder->startPathfinding(x,y,nearest->getXPos(),nearest->getYPos(),tiles,wall);
    emit statusUpdate(QString("Path to next healthpack determined."));
    return nearest;
}

// DRAW ROUTE WHEN NEAREST HP/ENEMY WANTED
bool Game::draw_route()
{
    if(!aliveCheck()||disconnected)
        return false;
    if(!route.empty())                          //Reset route
    {
        route_path.clear();
        route_tile.clear();
        route.clear();
    }
    auto temp = pathfinder->closedList.back();  //Find route
    if(temp==nullptr)
    {                                           //No route available
        qDebug()<<"no route";
        emit statusUpdate(QString("No route available."));
        return false;
    }
    while(temp->getParent() != nullptr)         //Display route
    {
        int x = temp->getPos_x();
        int y = temp->getPos_y();
        //GRAPH
        route.emplace_back(make_pair(x,y));
        route_tile.push_back(std::shared_ptr<QGraphicsRectItem>(new QGraphicsRectItem(x*TILE_SIZE,y*TILE_SIZE,TILE_SIZE,TILE_SIZE)));
        view->g_scene->addItem(route_tile.back().get());
        route_tile.back()->setZValue(1);
        route_tile.back()->setBrush(QColor(180, 0, 0));
        //TEXT
        QString o = "o";
        route_path.push_back(std::shared_ptr<QGraphicsTextItem>(new QGraphicsTextItem(o)));
        view->t_scene->addItem(route_path.back().get());
        temp = temp->getParent();
    }
    view->text_route(route_path,route);
    qDebug()<<"need : "<< calculate_cost(route);
    if(calculate_cost(route) > my_protagonist->getEnergy())     //Energy needed
    {
        qDebug()<<"Not enough energy to go";                    //Clear route if not enough energy
        route_tile.clear();
        route.clear();
        route_path.clear();
        emit statusUpdate(QString("Not enoug energy to reach this..."));
        return false;
    }
    return true;

}


//AUTO MOVE 1 POSITION
void Game::move_to_next()
{
    if(disconnected)return;
    if(route_tile.empty())
    {
        qDebug()<<"NO ROUTE";
        if(enemies.size() == 0)
            emit statusUpdate(QString("You won! Click reset or restart to play another game."));
        else
            emit statusUpdate(QString("No route available. Finished route."));
        auto_move();
        return;
    }

    int dx,dy;
    if(!route.empty() && pause==RESUME)
    {
        dx = route.back().first - my_protagonist->getXPos();
        dy = route.back().second - my_protagonist->getYPos();
        keyPress(check_mov(dx,dy));
        route.pop_back();
        route_tile.pop_back();
        route_path.pop_back();
        view->text_route(route_path,route);
        QTimer::singleShot(speed, this, SLOT(move_to_next()));
        emit statusUpdate(QString("Following the route."));
    }
}


//CHECK COST TO MOVE TO NEAREST HP/ENEMY
float  Game::calculate_cost(std::vector<std::pair<int,int>> &route)
{
    float total = 0;

    if(route.empty())
    {
        qDebug()<<"no route";
        return 0;
    }

    auto it=route.begin();
    while(it!=route.end())
    {   //Value of current tile
        float cur = tiles[static_cast<unsigned long>((*it).first + world->getCols()*(*it++).second)]->getValue();
        //Value of next tile
        float next;
        if(it == route.end())
            next = tiles[static_cast<unsigned long>(my_protagonist->getXPos() + world->getCols()*my_protagonist->getYPos())]->getValue();
        else
            next = tiles[static_cast<unsigned long>((*it).first + world->getCols()*(*it).second)]->getValue();
        //Add difference
        total += fabs(cur - next);
    }
    return total;
}


//STRATEGY
void Game::auto_move()
{
    //Find nearest enemy
    if(mode || !aliveCheck()||disconnected)
        return;
    if(!route.empty())
    {
        move_to_next();
        return;
    }
    auto temp_enemy=go_nearest_enemy();
    // No health left
    if(my_protagonist->getHealth()==0.F)
    {
        emit statusUpdate(QString("You lose: not enough health"));
        qDebug()<<"no health : lose";
        return;
    }

    //All enemies defeated
    if(temp_enemy == nullptr)
    {
        //emit statusUpdate(QString("You won! Click reset or restart to play another game."));
        qDebug()<<"win";
        return;
    }
    //Not enough health
    else if (temp_enemy->getValue() > my_protagonist->getHealth())
    {   //Find nearest HP
        auto temp_hp = go_nearest_hp();

        if(temp_hp==nullptr)
        {//No HPs available
            emit statusUpdate(QString("You lose: not enough healthpacks"));
            qDebug()<<"no hp left : lose";
            return;
        }
        else
        {
            if(draw_route())
            {//Move to HP
                //emit statusUpdate(QString("Moving to next healthpack"));
                move_to_next();
            }           
            else
            {//Not enough energy to reach HP
                emit statusUpdate(QString("You lose: not enough energy"));
                qDebug()<<"can not walk to nearst hp : lose";
                return;
            }
        }
    }
    //Enough health
    else
    {
        if(draw_route())
        {//Move to enemy
            //emit statusUpdate(QString("Moving to next target"));
            move_to_next();
        }
        else
        {//Out of energy
            emit statusUpdate(QString("You lose: not enough energy"));
            qDebug()<<"can not walk to nearst enemy : lose";
            return;
        }
    }
}

// TRANSLATE MOVE
Direction Game::check_mov(int dx, int dy)
{
    if(dx==1 && dy==-1)
        return Direction::RIGHT_UP;
    else if(dx==1 && dy==0)
        return Direction::RIGHT;
    else if(dx==1 && dy==1)
        return Direction::RIGHT_DOWN;
    else if(dx==0 && dy==1)
        return Direction::DOWN;
    else if(dx==0 && dy==-1)
        return Direction::UP;
    else if(dx==-1 && dy==1)
        return Direction::LEFT_DOWN;
    else if(dx==-1 && dy==0)
        return Direction::LEFT;
    else if(dx==-1 && dy==-1)
        return Direction::LEFT_UP;
}

//IMPASSIBLE TILE. REVERT STEP.
void Game::moveProtagonist(Direction direction)
{
    auto x = my_protagonist->getXPos();
    auto y = my_protagonist->getYPos();

    switch(direction)
    {
        case    Direction::UP:
            my_protagonist->setYPos(y+1);
            return;
        case    Direction::DOWN:
            my_protagonist->setYPos(y-1);
            return;
        case    Direction::RIGHT:
            my_protagonist->setXPos(x-1);
            return;
        case    Direction::LEFT:
            my_protagonist->setXPos(x+1);
            return;
        case    Direction::LEFT_UP:
            my_protagonist->setPos(x+1,y+1);
            return;
        case    Direction::LEFT_DOWN:
            my_protagonist->setPos(x+1,y-1);
            return;
        case    Direction::RIGHT_UP:
            my_protagonist->setPos(x-1,y+1);
            return;
        case    Direction::RIGHT_DOWN:
            my_protagonist->setPos(x-1,y-1);
            return;
        default:
            break;
    }
}


//UPDATE HEALTH WHEN /P\ IN POSION REGION
// ( Called by g_penemy::poisonChange + game::game_logic )
void Game::in_poison(int value)
{
    auto x = my_protagonist->getXPos();
    auto y = my_protagonist->getYPos();

    for(auto const &cur:enemies)
    {
        auto dummy = cur.get();
        if(typeid(PEnemy).name()==typeid(*dummy).name())
        {   //Inside poison field
            if(x >= cur->getXPos()-value && x <= cur->getXPos()+value
             && y >= cur->getYPos()-value && y <= cur->getYPos()+value)
            {   //Update health
                my_protagonist->setHealth(my_protagonist->getHealth()-0.5F);
                emit changeHealth(static_cast<int>(my_protagonist->getHealth()));
            }

        }
    }
    for(auto const &cur:dead_enemies)
    {
        auto dummy=cur.get();
        if(typeid(PEnemy).name()==typeid(*dummy).name())
        {   //Inside poison field
            if(x >= cur->getXPos()-value && x <= cur->getXPos()+value
             && y >= cur->getYPos()-value && y <= cur->getYPos()+value)
            {   //Update health
                my_protagonist->setHealth(my_protagonist->getHealth()-0.5F);
                emit changeHealth(static_cast<int>(my_protagonist->getHealth()));
            }
        }
    }
}

void Game::setStartPosition(QString const &world)
{
    if(world.contains("worldmap4"))
        my_protagonist->setPos(0,45);
    else if(world.contains("worldmap") || world.contains("xd"))
        my_protagonist->setPos(0,0);
    else if(world.contains("maze1"))
        my_protagonist->setPos(4,4);
    else if(world.contains("maze2"))
        my_protagonist->setPos(0,212);
    else if(world.contains("maze"))
        my_protagonist->setPos(35,23);
    else
        my_protagonist->setPos(0,0);
}
void Game::setSpeed(int value)
{
    speed = value;
}

//CHECK AMOUNT OF HEALTH
bool Game::aliveCheck()
{   //No health left
    if(my_protagonist->getHealth() <= 0)
    {
        emit statusUpdate(QString("You have lost, no HP left"));
        return false;
    }
    else
        return true;

}

bool Game::getPause() const
{
    return pause;
}
//DETERMINE NEW NON-USED POSITION ON MAP
std::pair<int,int> Game::not_used()
{
    QImage pic;
    pic.load(WorldMap);
    int xPos;
    int yPos;
    bool done = false;
    const unsigned int black {0xff000000};

    while(!done)
    {
        //Generate random pos
        xPos = qrand() % world->getCols();
        yPos = qrand() % world->getRows();

        //CHECKS FOR GENERATED POSITION
        //Tile height
        if (pic.pixel(xPos, yPos) == black)
            continue;
        // /P\ position
        if (xPos != my_protagonist->getXPos() || yPos != my_protagonist->getYPos())
        {
            //HP positions
            if (std::find_if(healthpacks.begin(), healthpacks.end(), [&xPos,&yPos](std::shared_ptr<Hp_model>& p)-> bool
                            {return (p->getXPos() == xPos && p->getYPos() == yPos) ;})== healthpacks.end())
              {
                //Enemy positions
                if (std::find_if(enemies.begin(), enemies.end(),[&xPos,&yPos](std::shared_ptr<Enemy>& p)-> bool
                                    {return (p->getXPos() == xPos && p->getYPos() == yPos) ;})== enemies.end())
                {   //Dead_enemy positions
                    if (std::find_if(dead_enemies.begin(), dead_enemies.end(), [&xPos,&yPos](std::shared_ptr<Enemy>& p)-> bool
                            {return (p->getXPos() == xPos && p->getYPos() == yPos) ;})== dead_enemies.end())
                    {
                        done = true;
                    }
                }
              }
       }
    }
    return make_pair(xPos,yPos);
}

void Game::goToXY(int x, int y)
{
    if(!aliveCheck()||disconnected)
        return;

    //Collect positions all enemies (in a vector containing pairs (x,y))
    std::vector<std::pair<int,int>> wall;
    for(auto &cur:enemies)
        wall.emplace_back(make_pair(cur->getXPos(),cur->getYPos()));
    for(const auto & cur:dead_enemies)
        wall.emplace_back(make_pair(cur->getXPos(),cur->getYPos()));
    auto protX = my_protagonist->getXPos();
    auto protY = my_protagonist->getYPos();
    //Determine the path
    pathfinder->startPathfinding(protX,protY,x,y,tiles,wall);
    string string = "Pathfinding to position x: " + std::to_string(x) + " y: " + std::to_string(y);
    emit statusUpdate(QString::fromStdString(string));
    return;
}

void Game::setPause(bool value)
{
    pause = value;
}

bool Game::getDisconnected() const
{
    return disconnected;
}

void Game::setDisconnected(bool value)
{
    disconnected = value;
}


