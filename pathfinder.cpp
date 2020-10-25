#include "pathfinder.h"
#include <QDebug>
#include <QElapsedTimer>
Pathfinder::Pathfinder(int rows, int cols) :weight{70},nrOfCol{cols},nrOfRow{rows}
{
    //vector open
    //list closed
}

int Pathfinder::getNrOfCol() const
{
    return nrOfCol;
}

void Pathfinder::setNrOfCol(int value)
{
    nrOfCol = value;
}

int Pathfinder::getNrOfRow() const
{
    return nrOfRow;
}

void Pathfinder::setNrOfRow(int value)
{
    nrOfRow = value;
}

bool Pathfinder::startPathfinding(int xstart,int ystart,int xend , int yend, std::vector<std::unique_ptr<Tile>> &tiles,std::vector<std::pair<int,int>> & Walls)
{
    // clear node list & make it the right size
    nodes.clear();
    nodes.resize(static_cast<unsigned long>(nrOfCol*nrOfRow));
    // Set end
    xEnd=xend;
    yEnd=yend;

    // clear all lists
    closedList.clear();
    while (!openList.empty())
          openList.pop();
    /***1 & 2) Create rootNode & push rootNode in open**/
    bool found = false;
    nodes[static_cast<unsigned long>(nrOfCol*ystart+xstart)] = std::shared_ptr<Node>(std::make_shared<Node>(xstart,ystart,calcHeuristicCost(xstart,ystart)));
    std::shared_ptr<Node> startNode = nodes[static_cast<unsigned long>(nrOfCol*ystart+xstart)];//(new Node());
    //qDebug() << "X-Co: " << xStart << " Y-Co: " << yStart << "nrOfCol: " << nrOfCol;
    startNode->setInOpenList(true);
    startNode->setParent(nullptr);
    openList.push(startNode);
    std::shared_ptr<Node> currentNode;
    std::shared_ptr<Node> nearbyNode;

    static std::array<int,8> dx = {1, 1, 0,-1,-1,-1, 0, 1};
    static std::array<int,8> dy = {0, 1, 1, 1, 0,-1,-1,-1};
    /***3) While open list is not empty && solution not found***/
    while(!openList.empty() && !found)
    {
        /***1) pop best finalCost from openList***/
        currentNode = openList.top();
        currentNode->setInOpenList(false);
        openList.pop();
        if(currentNode->getPos_x() == xEnd && currentNode->getPos_y() == yEnd)
        {
            found = true;
            // push nodes that are part of path in to solution
            closedList.emplace_back(currentNode);
            break;
        }
        /***3) for every nearbyPoint around currentNode do:***/
        for(unsigned long i = 0; i < dx.size() && !found; i++)
        {
            int dum_x = currentNode->getPos_x()+dx[i], dum_y=currentNode->getPos_y()+dy[i];
            // boundary check
            if(dum_x < 0 || dum_y < 0 || dum_x > nrOfCol-1 || dum_y > nrOfRow-1)
                continue;
            // cost (value) of nearbyPoint
            auto dum_value = tiles[static_cast<unsigned long>(nrOfCol*dum_y+dum_x)]->getValue();
            // 1 if illegal spot (like wall), then skip to next one
            if(dum_value <= 0 ||dum_value > 1)
                continue;
            // set walls based on different situation
            std::pair<int,int> dummy(dum_x,dum_y);
            auto it = std::find(Walls.begin(), Walls.end(), dummy);
            if(it!=Walls.end())
                continue;

            // move cost based on difference between tile values
            dum_value = 30000*(0.001+fabs((tiles[static_cast<unsigned long>(nrOfCol*(dum_y-dy[i])+dum_x-dx[i])]->getValue()-dum_value)))/(0.009*tiles.size());
            // Diagonal penalty cost
            if(i % 2 == 1)
            {
                dum_value *= 1.4142;
            }

            nearbyNode = nodes[static_cast<unsigned long>(nrOfCol*dum_y+dum_x)];
            if(nearbyNode == nullptr)
            {
                nearbyNode = std::shared_ptr<Node>(std::make_shared<Node>(dum_x,dum_y,calcHeuristicCost(dum_x,dum_y)));
                nodes[static_cast<unsigned long>(nrOfCol*dum_y+dum_x)] = nearbyNode;
            }
            else if(nearbyNode==startNode)
                continue;

            // 2 if node for nearbyPoint created before
            if(nearbyNode->getInOpenList() || nearbyNode->getInClosedList())
            {
                // if exists compare givenCost & choose cheapest
                // finalCost = givenCost + heuristicCost
                // givenCost = parent's cost + mapData[y][x]
                // givenCost is not the weight of the terain for the spot on
                // the map the node is on. It is rather the accumulated cost
                // paid to get from the start to a node
                // heuristicCost = distance of nearbyPoint from the goalPoint
                if(nearbyNode->getInClosedList())
                    continue;
                if(nearbyNode->getGivenCost() > (currentNode->getGivenCost()+dum_value))
                {
                    nearbyNode->setParent(currentNode);
                    nearbyNode->setGivenCost(currentNode->getGivenCost()+dum_value);
                    nearbyNode->setFinalCost(nearbyNode->getGivenCost()+nearbyNode->getHeuristicCost());
                    continue;
                }
                else
                    continue;
            }
            // 3 Creating sucessorNode
            nearbyNode->setParent(currentNode);
            nearbyNode->setGivenCost(currentNode->getGivenCost()+dum_value);
            nearbyNode->setFinalCost(nearbyNode->getGivenCost()+nearbyNode->getHeuristicCost());
            //qDebug()<<nearbyNode->getGivenCost()<<' '<<nearbyNode->getHeuristicCost();

            // 4 push sucessorNode to open
            nearbyNode->setInOpenList(true);
            openList.push(nearbyNode);
        }
        //4)push the currentNode to closed
        closedList.emplace_back(currentNode);
        currentNode->setInClosedList(true);

    }
    return found;
}

float Pathfinder::calcHeuristicCost(int x,int y)
{
    // Euclidian distance
    auto heuristicCost = sqrt(pow((x-xEnd),2) + pow((y-yEnd),2));
    return static_cast<float>(weight)/1000*static_cast<float>(heuristicCost);
}

void Pathfinder::setWeight(int value)
{
    weight = value;
}



