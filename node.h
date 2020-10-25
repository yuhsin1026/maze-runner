#ifndef NODE_H
#define NODE_H
#include "world.h"
#include "world_global.h"
#include <memory>


class Node
{
public:
    Node();
    Node(int x,int y,float H);
    // Disable copying, remove the two lines below if it would give too much trouble to deal with
    Node (Node const&) = delete;
    Node& operator= (Node const&) = delete;

    std::shared_ptr<Node> getParent() const;
    void setParent(const std::shared_ptr<Node> &value);

    float getFinalCost() const;
    void setFinalCost(float value);

    float getGivenCost() const;
    void setGivenCost(float value);

    float getHeuristicCost() const;
    void setHeuristicCost(float value);

    bool getInOpenList() const;
    void setInOpenList(bool value);

    bool getInClosedList() const;
    void setInClosedList(bool value);

    int getPos_y() const;
    int getPos_x() const;
    void setPos(int x,int y);

private:

    int pos_x,pos_y;
    std::shared_ptr<Node> parent;
    float finalCost;
    float givenCost;
    float heuristicCost;
    bool inOpenList;
    bool inClosedList;
};

#endif // NODE_H
