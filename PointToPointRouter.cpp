#include "provided.h"
#include "ExpandableHashMap.h"
#include <algorithm>
#include <list>
#include <queue>
#include <set>
#include <stack>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(   // deliveryresult
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_streetMap;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_streetMap = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    route.clear();
    totalDistanceTravelled = 0;
    vector<StreetSegment> throwAway;
    if (!m_streetMap->getSegmentsThatStartWith(start, throwAway) || !m_streetMap->getSegmentsThatStartWith(end, throwAway))
        return BAD_COORD;
    queue<GeoCoord> routeQueue;
    routeQueue.push(start);
    ExpandableHashMap<GeoCoord, bool> encountered;                          // keeps track of points that have been visited
    encountered.associate(start, true);
    ExpandableHashMap<GeoCoord, GeoCoord> locationOfPreviousWayPoint;
    
    auto compDistanceFromEnd = [&end](StreetSegment seg1, StreetSegment seg2)
    {
        return (distanceEarthMiles(seg1.end, end) > distanceEarthMiles(seg2.end, end));
    };
    
    while (!routeQueue.empty())
    {
        GeoCoord temp = routeQueue.front();
        routeQueue.pop();
        if (temp == end)                                                    // if route has finished
        {
            GeoCoord endSegment = end;
            GeoCoord startSegment = *locationOfPreviousWayPoint.find(end);
            StreetSegment tempSegment;
            while (startSegment != start)
            {
                m_streetMap->getSegmentsThatStartWith(startSegment, throwAway);
                for (int i = 0; i < throwAway.size(); i++)
                {
                    if (throwAway[i].end == endSegment)                     // find segment that we traveled through
                    {
                        tempSegment = throwAway[i];
                        route.insert(route.begin(), tempSegment);
                        break;
                    }
                }
                totalDistanceTravelled += distanceEarthMiles(tempSegment.start, tempSegment.end);
                endSegment = tempSegment.start;
                startSegment = *locationOfPreviousWayPoint.find(endSegment);
            }
            route.insert(route.begin(), tempSegment);
            totalDistanceTravelled += distanceEarthMiles(tempSegment.start, tempSegment.end);
            return DELIVERY_SUCCESS;
        }
        vector<StreetSegment> streetSegs;
        m_streetMap->getSegmentsThatStartWith(temp, streetSegs);
        sort(streetSegs.begin(), streetSegs.end(), compDistanceFromEnd);
        for (int i = 0; i < streetSegs.size(); i++)
        {
            if (encountered.find(streetSegs[i].end) == nullptr)             // checks to see if endpoint of segment has already been traveled to
            {
                routeQueue.push(streetSegs[i].end);
                encountered.associate(streetSegs[i].end, true);
                locationOfPreviousWayPoint.associate(streetSegs[i].end, streetSegs[i].start);
            }
        }
    }
    return NO_ROUTE;
}


//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(  // deliveryresult
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}

