#include "provided.h"
#include <vector>
#include <algorithm>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap* m_streetmap;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_streetmap = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = 0;
    for (int i = 0; i < deliveries.size(); i++)
    {
        if (i == 0)
            oldCrowDistance += distanceEarthMiles(depot, deliveries[0].location);
        else
            oldCrowDistance += distanceEarthMiles(deliveries[i-1].location, deliveries[i].location);
    }
    oldCrowDistance += distanceEarthMiles(deliveries[deliveries.size()-1].location, depot);
    
    vector<DeliveryRequest> sortedDeliveries;
    GeoCoord currentStartPoint = depot;
    auto compDistanceFromStartPoint = [&currentStartPoint](DeliveryRequest loc1, DeliveryRequest loc2)
    {
        if (distanceEarthMiles(currentStartPoint, loc1.location) < distanceEarthMiles(currentStartPoint, loc2.location))
            return true;
        else
            return false;
    };
    
    for (int i = 0; i < deliveries.size(); i++)
    {
        sort(deliveries.begin()+i, deliveries.end(), compDistanceFromStartPoint);
        currentStartPoint = deliveries[i].location;
    }
    newCrowDistance = 0;
    for (int i = 0; i < deliveries.size(); i++)
    {
        if (i == 0)
            newCrowDistance += distanceEarthMiles(depot, deliveries[0].location);
        else
            newCrowDistance += distanceEarthMiles(deliveries[i-1].location, deliveries[i].location);
    }
    oldCrowDistance += distanceEarthMiles(deliveries[deliveries.size()-1].location, depot);
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
