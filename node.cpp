#include "node.h"

Node::Node(): parent{nullptr}, finalCost{0},  givenCost{0}, heuristicCost{}, inOpenList{false}, inClosedList{false}
{
}

Node::Node(int x, int y,float H): pos_x{x}, pos_y{y}, parent{nullptr},finalCost{0},  givenCost{0}, heuristicCost{H}, inOpenList{false}, inClosedList{false}
{
}

std::shared_ptr<Node> Node::getParent() const
{
    return parent;
}

void Node::setParent(const std::shared_ptr<Node> &value)
{
    parent = value;
}

float Node::getFinalCost() const
{
    return finalCost;
}

void Node::setFinalCost(float value)
{
    finalCost = value;
}

bool Node::getInOpenList() const
{
    return inOpenList;
}

void Node::setInOpenList(bool value)
{
    inOpenList = value;
}

bool Node::getInClosedList() const
{
    return inClosedList;
}

void Node::setInClosedList(bool value)
{
    inClosedList = value;
}

int Node::getPos_y() const
{
    return pos_y;
}


int Node::getPos_x() const
{
    return pos_x;
}

void Node::setPos(int x,int y)
{
    pos_x = x;
    pos_y = y;
}

float Node::getHeuristicCost() const
{
    return heuristicCost;
}

void Node::setHeuristicCost(float value)
{
    heuristicCost = value;
}

float Node::getGivenCost() const
{
    return givenCost;
}

void Node::setGivenCost(float value)
{
    givenCost = value;
}
