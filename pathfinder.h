#ifndef PATHFINDER_H
#define PATHFINDER_H
#include <vector>
#include <utility>
#include <queue>
#include <memory>
#include <assert.h>
#include <iterator>
#include <world.h>
#include <world_global.h>
#include <node.h>
#include <math.h>

class compareFunction
{
public:
    int operator()(const std::shared_ptr<Node> &a , const std::shared_ptr<Node> &b)
    {
        return a->getFinalCost() > b->getFinalCost();
    }
};

class Pathfinder
{
public:
    Pathfinder(int rows, int cols);
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, compareFunction > openList;
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Node>> closedList;

    int getNrOfCol() const;
    void setNrOfCol(int value);
    int getNrOfRow() const;
    void setNrOfRow(int value);
    void setWeight(int value);
    bool startPathfinding(int xstart, int ystart, int xend, int yend, std::vector<std::unique_ptr<Tile>> &,std::vector<std::pair<int,int>> &);
    float calcHeuristicCost(int x,int y);

private:
    int weight;
    int xStart;
    int yStart;
    int xEnd;
    int yEnd;
    int nrOfCol;
    int nrOfRow;

};

#endif // PATHFINDER_H
