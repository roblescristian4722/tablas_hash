#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <iostream>
#include <sstream>
#include "lsl.h"
#define DEFAULT_MOD 13
using namespace std;

//// CLASS PROTOTYPES ////
template<typename K, typename V>
class Node;
template <typename K, typename V>
class Bucket;
template <typename K, typename V>
class HashMap;

/////////////////////////
//// EXTRA FUNCTIONS ////
/////////////////////////
template <typename T>
long binary_search(LSL<T> &list, T data);

//////////////
//// NODE ////
//////////////
template <typename K, typename V>
class Node
{
private:
    K key;
    V value;

public:
    Node() {}
    Node(K _key, V _val) : key(_key), value(_val) {}
    ~Node() {}

    bool operator>(Node<K, V> &other)
    { return this->key > other.key; }

    bool operator==(Node<K, V> &other)
    { return this->key == other.key; }

    bool operator<(Node<K, V> &other)
    { return this->key < other.key; }

    friend class Bucket<K,V>;
    friend class HashMap<K, V>;
};

////////////////
//// BUCKET ////
////////////////
template <typename K, typename V>
class Bucket
{
private:
    size_t m_hashValue;
    LSL<Node<K, V>> m_nodes;

public:
    Bucket() {}
    ~Bucket()
    { m_nodes.clear(); }

    size_t size_bucket()
    { return m_nodes.size(); }

    void append(const Node<K, V> &node)
    { m_nodes.push_back(node); }

    bool operator>(Bucket<K, V> &other)
    { return this->m_hashValue > other.m_hashValue; }

    bool operator<(Bucket<K, V> &other)
    { return this->m_hashValue < other.m_hashValue; }

    bool operator==(Bucket<K, V> &other)
    { return this->m_hashValue == other.m_hashValue; }

    friend class HashMap<K, V>;
};
//////////////////
//// HASH MAP ////
//////////////////
template <typename K, typename V>
class HashMap
{
private:
    LSL<Bucket<K, V>> m_buckets;
    size_t m_mod;

public:
    HashMap(size_t mod = DEFAULT_MOD) : m_mod(mod) {}
    ~HashMap()
    { clear(); }

    struct Pair
    {
        K *key;
        V *value;
        bool operator < (const HashMap<K,V>::Pair& other)
        { return *key < *other.key; }
        bool operator > (const HashMap<K,V>::Pair& other)
        { return *key > *other.key; }
        bool operator > (const K &other)
        { return *key > other; }
        bool operator == (const HashMap<K,V>::Pair& other)
        { return *key == *other.key; }
        bool operator == (const K &other)
        { return *key == other; }
    };

    bool is_empty();
    long is_colide(size_t hash);
    size_t size();

    void insert(const K &keyVal, const V &val);
    void delete_value(const K &keyVal);
    void clear();
    V *operator[](K index);
    size_t hash_function(K val);
    HashMap::Pair get_position(size_t index);
    size_t string_to_size_t(string str);
};

//////////////////////
//// HASH MAP     ////
//// DEFINITIONS  ////
//////////////////////

// Returns true if empty
// and false otherwise
template <typename K, typename V>
bool HashMap<K, V>::is_empty()
{ return m_buckets.empty(); }

// indicates wheter the hash value returned
// by hash_function already exists and if it does this
// method gives you it's exact position
template <typename K, typename V>
long HashMap<K, V>::is_colide(size_t hash)
{
    if (is_empty())
        return -1;

    Bucket<K, V> bucketTmp;
    bucketTmp.m_hashValue = hash;
    return binary_search(m_buckets, bucketTmp);
}

// Returns the total size of the hash map (i.e. the
// summation of the size of every bucket in buckets list)
template <typename K, typename V>
size_t HashMap<K, V>::size()
{
    size_t sizeTotal = 0;
    for (size_t i = 0; i < m_buckets.size(); ++i)
        sizeTotal += m_buckets[i].size_bucket();
    return sizeTotal;
}

// Inserts a new node in the hash map
template <typename K, typename V>
void HashMap<K, V>::insert(const K &keyVal, const V &val)
{
    Node<K, V> n(keyVal, val);
    Bucket<K, V> b;
    long pos;
    size_t hash;

    hash = hash_function(keyVal);
    pos = is_colide(hash);
    if (pos != -1) {
        m_buckets[pos].append(n);
        sort(m_buckets[pos].m_nodes);
    }

    else {
        b.m_hashValue = hash;
        b.append(n);
        m_buckets.push_back(b);
        sort(m_buckets);
    }
}

// Deletes an existing value in the hash map
template <typename K, typename V>
void HashMap<K, V>::delete_value(const K &keyVal)
{
    size_t aux = hash_function(keyVal);
    size_t i;
    LSL<Node<K, V>> *auxList;
    long pos = is_colide(hash_function(keyVal));

    if (pos != -1){
        auxList = &m_buckets[pos].m_nodes;

        for (i = 0; i < auxList->size(); ++i)
            if ((*auxList)[i].key == keyVal) {
                auxList->erase(i);
                break;
            }
        if (auxList->empty())
            m_buckets.erase(pos);
        else if (auxList->size() == i)
            cout << "There is not such value" << endl;
    }
    else
        cout << "There is not such value" << endl;
}

// Removes every value stored in the hash map
template <typename K, typename V>
void HashMap<K, V>::clear()
{ m_buckets.clear(); }

template <typename K, typename V>
size_t HashMap<K, V>::hash_function(K val)
{
    stringstream ss;
    ss << val;
    string aux = ss.str();
    return string_to_size_t(aux) % m_mod;
}

// Returns a value stored in the hash map
// using a key as the way to find it
template <typename K, typename V>
V *HashMap<K, V>::operator[](K index)
{
    LSL<Node<K, V>> *auxList;
    Node<K, V> nodeTmp;
    long posNode;
    size_t hashValue = hash_function(index);
    long posBucket = is_colide(hashValue);
    V *v = nullptr;

    nodeTmp.key = index;
    if (posBucket != -1) {
        auxList = &m_buckets[posBucket].m_nodes;
        posNode = binary_search(*auxList, nodeTmp);
        if (posNode != -1)
            v = &(*auxList)[posNode].value;
    }
    return v;
}

// Gets a node from the hash map using a numeric index.
// It's useful when iterating over every node in the hash map.
template <typename K, typename V>
typename HashMap<K,V>::Pair HashMap<K, V>::get_position(size_t index)
{
    HashMap::Pair pair;
    pair = {nullptr};
    if (index >= size())
        cout << "error: index is grater or equal than size" << endl;
    else{
        for (size_t i = 0; i < m_buckets.size(); ++i){
            for (size_t j = 0; j < m_buckets[i].m_nodes.size(); ++j){
                if (!index){
                    pair = {
                        &m_buckets[i].m_nodes[j].key,
                        &m_buckets[i].m_nodes[j].value
                    };
                    return pair;
                }
                --index;
            }
        }
    }
    return pair;
}

/////////////////////////
//// EXTRA FUNCTIONS ////
//// DEFINITIONS     ////
/////////////////////////
template <typename K, typename V>
size_t HashMap<K,V>::string_to_size_t(string str)
{
    size_t sum = 0;
    for (size_t i = 0; i < str.length(); ++i)
        sum += (size_t)str[i];
    return sum + (size_t)str.length();
}

template <typename T>
long binary_search(LSL<T> &list, T data)
{
    int l = 0;
    int r = int(list.size() - 1);
    while (l <= r) {
        int m = (l + r) / 2;
        if (data == list[m])
            return m;
        else if (data < list[m])
            r = m - 1;
        else
            l = m + 1;
    }
    return -1;
}

#endif //HASHMAP_H