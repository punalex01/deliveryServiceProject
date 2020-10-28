

// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

#ifndef expandableHashMap_h
#define expandableHashMap_h

#include <iostream>
#include <vector>

unsigned int hasher(const GeoCoord& g);
unsigned int hasher(const std::string &testString);

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;
private:
    struct Node
    {
        KeyType m_key;
        ValueType m_value;
        Node* next;
        bool isDummy;
    };
    
    Node* m_map;
    int m_size;
    int m_numBuckets;
    double m_maxLoadFactor;
    
    void resize();
    double getCurrLoadFactor();
    unsigned int getBucketNumber(const KeyType& key, int numBuckets) const;
    void insertDummy(int bucketNum);
    void removeNode(const KeyType& key);
    
    
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    m_map = new Node[8];
    m_size = 0;
    m_numBuckets = 8;
    m_maxLoadFactor = maximumLoadFactor;
    for (int i = 0; i < m_numBuckets; i++)
    {
        insertDummy(i);
    }
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    int i = 0;
    while (i < m_numBuckets)
    {
        if (m_map[i].isDummy == false)
        {
            Node* track = m_map[i].next;
            Node* trackNext = nullptr;
            if (track != nullptr)
            {
                trackNext = track->next;
            }
            while (track != nullptr)
            {
                delete track;
                track = trackNext;
                if (trackNext != nullptr)
                    trackNext = trackNext->next;
            }
        }
        i++;
    }
    delete [] m_map;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_size;  // Delete this line and implement this function correctly
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    resize();
    int bucketNum = getBucketNumber(key, m_numBuckets);
    if (m_map[bucketNum].isDummy == true)   // if bucket is empty
    {
        m_map[bucketNum].m_key = key;
        m_map[bucketNum].m_value = value;
        m_map[bucketNum].next = nullptr;
        m_map[bucketNum].isDummy = false;
        m_size++;
    }
    else
    {
        Node* track = &m_map[bucketNum];
        while (track->next != nullptr)
        {
            if (track->m_key == key)        // found existing association
            {
                track->m_value = value;     // replace with new value
                return;
            }
            track = track->next;
        }
        if (track->m_key == key)        // found existing association
        {
            track->m_value = value;     // replace with new value
            return;
        }
        track->next = new Node;                // no existing association
        track = track->next;
        track->m_key = key;
        track->m_value = value;
        track->next = nullptr;
    }
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    int bucketNum = getBucketNumber(key, m_numBuckets);
    Node* track = &m_map[bucketNum];
    if (track->isDummy == true)
        return nullptr;
    while (track != nullptr)
    {
        if (track->m_key == key)
        {
            return &track->m_value;
        }
        track = track->next;
    }
    return nullptr;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::resize()
{
    if (getCurrLoadFactor() >= m_maxLoadFactor)
    {
        m_size = 0;
        int numOfBuckets = m_numBuckets * 2;
        Node* newMap = new Node[m_numBuckets*2];
        for (int n = 0; n < numOfBuckets; n++)
        {
            newMap[n].isDummy = true;
        }
        for (int i = 0; i < m_numBuckets; i++)
        {
            if (m_map[i].isDummy == false)                                      // only need to copy buckets with actual nodes
            {
                int newBucketNum = getBucketNumber(m_map[i].m_key, numOfBuckets); // for node in actual bucket
                if (newMap[newBucketNum].isDummy == true)
                {
                    newMap[newBucketNum].m_key = m_map[i].m_key;
                    newMap[newBucketNum].m_value = m_map[i].m_value;
                    newMap[newBucketNum].isDummy = false;
                    newMap[newBucketNum].next = nullptr;
                    m_size++;
                }
                else                                                            // for nodes that collide
                {
                    Node* track = &newMap[newBucketNum];
                    while (track != nullptr)
                    {
                        track = track->next;
                    }
                    track = new Node;
                    track->m_key = m_map[i].m_key;
                    track->m_value = m_map[i].m_value;
                    track->next = nullptr;
                }
                
                Node* track2 = m_map[i].next;
                Node* trackNext = nullptr;
                if (track2 != nullptr)
                {
                    trackNext = track2->next;
                }
                while (track2 != nullptr)
                {
                    newBucketNum = getBucketNumber(track2->m_key, numOfBuckets); // insert node into new array
                    if (newMap[newBucketNum].isDummy == true)
                    {
                        newMap[newBucketNum].m_key = track2->m_key;
                        newMap[newBucketNum].m_value = track2->m_value;
                        newMap[newBucketNum].isDummy = false;
                        newMap[newBucketNum].next = nullptr;
                        m_size++;
                    }
                    else
                    {
                        Node* track3 = &newMap[newBucketNum];
                        while (track3->next != nullptr)
                        {
                            track3 = track3->next;
                        }
                        track3->next = new Node;
                        track3 = track3->next;
                        track3->m_key = track2->m_key;
                        track3->m_value = track2->m_value;
                        track3->next = nullptr;
                    }
                    delete track2;
                    track2 = trackNext;
                    if (trackNext != nullptr)
                    {
                        trackNext = trackNext->next;
                    }
                }
            }
        }
        delete [] m_map;
        m_map = newMap;
        m_numBuckets = numOfBuckets;
    }
    else
        return;
}
        
template<typename KeyType, typename ValueType>
double ExpandableHashMap<KeyType, ValueType>::getCurrLoadFactor()
{
    return static_cast<double>(m_size)/m_numBuckets;
}

template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType& key, int numBuckets) const
{
    unsigned int temp = hasher(key);
    return temp % numBuckets;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::insertDummy(int bucketNum)
{
    m_map[bucketNum].isDummy = true;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::removeNode(const KeyType &key)
{
    int bucketNum = getBucketNumber(key, m_numBuckets);
    if (m_map[bucketNum].m_key == key)              // if node is in actual bucket
    {
        if (m_map[bucketNum].next != nullptr)       // if no node next to it
        {
            Node* temp = m_map[bucketNum].next;
            m_map[bucketNum].m_key = temp->m_key;
            m_map[bucketNum].m_value = temp->m_value;
            m_map[bucketNum].next = temp->next;
            delete temp;
        }
        else
            m_map[bucketNum].isDummy = true;
        m_size--;
        return;
    }
    Node* track = &m_map[bucketNum];
    while (track != nullptr)
    {
        if (track->next != nullptr && track->next->m_key == key)
        {
            Node* temp = track->next;
            track->next = temp->next;
            delete temp;
        }
        if (track != nullptr)
            track = track->next;
    }
}


#endif
