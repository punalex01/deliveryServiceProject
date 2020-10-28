#include "provided.h"
#include "ExpandableHashMap.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

unsigned int hasher(const string &testString)
{
    std::hash<string> str_hash;
    return str_hash(testString);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    StreetSegment reverse(StreetSegment oldSegment);
    ExpandableHashMap<GeoCoord, vector<StreetSegment> > streetMap;
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    
    string s;
    if ( ! infile )                // Did opening the file fail?
    {
        cerr << "Error: Cannot open data.txt!" << endl;
        return false;
    }
    while (getline(infile, s))
    {
        if (isalpha(s[s.size()-1]))
        {
            string streetName = s;
            string numSegments;
            getline(infile, numSegments);
            for (int i = stoi(numSegments); i > 0; i--)
            {
                getline(infile, s);
                string startLat = s.substr(0, 10);                              // get individual coords
                string startLon = s.substr(11, 12);
                string endLat = s.substr(24, 10);
                string endLon = s.substr(35, 12);
                
                GeoCoord tempStart(startLat, startLon);
                GeoCoord tempEnd(endLat, endLon);
                StreetSegment tempSegment(tempStart, tempEnd, streetName);
                StreetSegment reversedSegment = reverse(tempSegment);
                
                vector<StreetSegment> firstVector;
                if (streetMap.find(tempStart) == nullptr)                       // only associate if vector is not found in map
                {
                    streetMap.associate(tempStart, firstVector);
                }
                streetMap.find(tempStart)->push_back(tempSegment);              // push the streetsegment into the map
                
                vector<StreetSegment> nextVector;
                if (streetMap.find(tempEnd) == nullptr)                         // only associate if vector is not found in map
                {
                    streetMap.associate(tempEnd, nextVector);
                }
                streetMap.find(tempEnd)->push_back(reversedSegment);
            }
        }
    }
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    if (streetMap.find(gc) == nullptr)
        return false;
    else
        segs = *streetMap.find(gc);                                              // return the vector of streetsegments if found
    return true;
}

StreetSegment StreetMapImpl::reverse(StreetSegment oldSegment)
{
    GeoCoord newStart = oldSegment.end;
    GeoCoord newEnd = oldSegment.start;
    StreetSegment reversedSegment(newStart, newEnd, oldSegment.name);
    return reversedSegment;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
