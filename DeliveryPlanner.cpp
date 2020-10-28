#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_streetMap;
    PointToPointRouter m_generateRoute;
    
    string getDirection(double angle) const;
    string getTurnDirection(double angle) const;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
: m_generateRoute(sm)
{
    m_streetMap = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    double oldCrowDistance, newCrowDistance;
    DeliveryOptimizer optimizer(m_streetMap);
    vector<DeliveryRequest> copyDeliveries = deliveries;
    optimizer.optimizeDeliveryOrder(depot, copyDeliveries, oldCrowDistance, newCrowDistance);                                           // optimize delivery order
    totalDistanceTravelled = 0;
    double requestDistance;
    double segmentDistance;
    double lineAngle;
    string direction;
    DeliveryCommand tempCommand;
    list<StreetSegment> route;
    DeliveryResult testPossible = m_generateRoute.generatePointToPointRoute(depot, copyDeliveries[0].location, route, requestDistance); // check if route is possible
    if (testPossible == NO_ROUTE)
    {
        return NO_ROUTE;
    }
    else if (testPossible == BAD_COORD)
    {
        return BAD_COORD;
    }
    list<StreetSegment>::iterator it = route.begin();
    list<StreetSegment>::iterator segmentIt = route.begin()++;
    totalDistanceTravelled += requestDistance;
    
    int deliveryNum = 0;
    string tempItem = copyDeliveries[0].item;
    while (it != route.end())                                                       // goes till end of a route
    {
        if (segmentIt == route.end())
        {
            if (deliveryNum == copyDeliveries.size()-1)
            {
                tempCommand.initAsDeliverCommand(tempItem);
                testPossible = m_generateRoute.generatePointToPointRoute(copyDeliveries[copyDeliveries.size()-1].location, depot, route, requestDistance);  // add a new route to depot after delivered an item to location
                if (testPossible == NO_ROUTE)
                {
                    return NO_ROUTE;
                }
                else if (testPossible == BAD_COORD)
                {
                    return BAD_COORD;
                }
                totalDistanceTravelled += requestDistance;
            }
            else if (deliveryNum == copyDeliveries.size())
            {
                return DELIVERY_SUCCESS;
            }
            else
            {
                tempCommand.initAsDeliverCommand(tempItem);
                testPossible = m_generateRoute.generatePointToPointRoute(copyDeliveries[deliveryNum].location, copyDeliveries[deliveryNum+1].location, route, requestDistance); // add a new route after delivered an item to location
                if (testPossible == NO_ROUTE)
                {
                    return NO_ROUTE;
                }
                else if (testPossible == BAD_COORD)
                {
                    return BAD_COORD;
                }
                tempItem = copyDeliveries[deliveryNum+1].item;
                totalDistanceTravelled += requestDistance;
            }
            deliveryNum++;
            commands.push_back(tempCommand);
            it = route.begin();
            segmentIt = route.begin()++;
        }
        if (segmentIt != route.end())
        {
            segmentDistance = distanceEarthMiles(it->start, it->end);
            lineAngle = angleOfLine(*it);
            while (segmentIt->name == it->name && segmentIt != route.end())                         // add up all segments that are in a straight line
            {
                segmentDistance += distanceEarthMiles(segmentIt->start, segmentIt->end);
                segmentIt++;
            }
            tempCommand.initAsProceedCommand(getDirection(lineAngle), it->name, segmentDistance);   // proceed forwards
            commands.push_back(tempCommand);
        }
        
        if (segmentIt != route.end() && it != route.end())                                          // turn 
        {
            tempCommand.initAsTurnCommand(getTurnDirection(angleBetween2Lines(*it, *segmentIt)), segmentIt->name);
            commands.push_back(tempCommand);
            segmentDistance = 0;
        }
        if (segmentIt != route.end())
        {
            it = segmentIt;
            segmentIt++;
        }
    }
    return DELIVERY_SUCCESS;
}

string DeliveryPlannerImpl::getDirection(double angle) const
{
    if (angle >= 0 && angle < 22.5)
        return "east";
    else if (angle >= 22.5 && angle < 67.5)
        return "northeast";
    else if (angle >= 67.5 && angle < 112.5)
        return "north";
    else if (angle >= 112.5 && angle < 157.5)
        return "northwest";
    else if (angle >= 157.5 && angle < 202.5)
        return "west";
    else if (angle >= 202.5 && angle < 247.5)
        return "southwest";
    else if (angle >= 247.5 && angle < 292.5)
        return "south";
    else if (angle >= 292.5 && angle < 337.5)
        return "southeast";
    else
        return "east";
}

string DeliveryPlannerImpl::getTurnDirection(double angle) const
{
    if (angle < 1 || angle > 359)
        return "proceed";
    else if (angle >= 1 && angle < 180)
        return "left";
    else
        return "right";
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
