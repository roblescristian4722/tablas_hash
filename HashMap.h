#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <iostream>
#include <sstream>
#include "LSL.h"
using namespace std;

size_t string_to_size_t(string str);

template<typename T>
long binary_search(LSL<T> &list, T data);

template<typename K, typename V>
class Node
{
private:
    K key;
    V value;
public:
    Node(){}
    Node(K _key, V _val): key(_key), value(_val){}
    ~Node() {}

    void setKey(const K &val)
    { key = val; }
    void setValue(const V &val)
    { value = val; }

    K getKey() const
    { return key; }
    V* getValue()
    { return &value; }
    bool operator > (Node<K, V>&other)
    { return this->getKey() > other.getKey(); }

    bool operator == (Node<K, V>&other)
    { return this->getKey() == other.getKey(); }

    bool operator < (Node<K, V>&other)
    { return this->getKey() < other.getKey(); }
};

template <typename K, typename V>
class Bucket
{
private:
    size_t hashValue; 
    LSL<Node<K, V>> values;

public:
    Bucket() {}
    ~Bucket()
    { values.clear(); }

    size_t size_bucket()
    { return values.size(); }

    //funciones para la lista ligada
    LSL<Node<K, V>> &getValues()
    { return values; }

    void append(const Node<K, V> &node)
    { values.push_back(node); }

    //funciones para el valor de hash
    void setHashValue(size_t val)
    { hashValue = val; }

    size_t getHashValue() const
    { return hashValue; }

    bool operator > (Bucket<K, V>&other)
    { return this->getHashValue() > other.getHashValue(); }

    bool operator < (Bucket<K, V>&other)
    { return this->getHashValue() < other.getHashValue(); }

    bool operator == (Bucket<K, V>&other)
    { return this->getHashValue() == other.getHashValue(); }
};

template<typename K, typename V>
class HashMap
{
private:
    LSL<Bucket<K, V>> buckets;
    size_t m_mod;
public:
    HashMap(size_t mod = 13) : m_mod(mod) {}
    ~HashMap()
    { clear(); }

    bool isEmpty();
    long isColide(size_t hash);
    size_t size();

    void insert(const K &keyVal, const V &val);
    void deleteValue(const K &keyVal);
    void clear();

    LSL<Bucket<K, V>>& getBucket()
    { return buckets; }

    V* operator[](K index);
    size_t HashFunction(K val);
};


template<typename K, typename V>
bool HashMap<K,V>::isEmpty()
{ return buckets.empty(); }


template<typename K, typename V>
long HashMap<K,V>::isColide(size_t hash)
{
    if(isEmpty())
        return -1;
    
    Bucket<K, V> bucketTmp;
    bucketTmp.setHashValue(hash);
    return binary_search(buckets, bucketTmp);
}

template <typename K, typename V>
size_t HashMap<K,V>::size()
{
    size_t sizeTotal = 0;
    for (size_t i = 0; i < buckets.size(); ++i)
        sizeTotal += buckets[i].size_bucket();
    return sizeTotal;
}

template<typename K, typename V>
void HashMap<K,V>::insert(const K &keyVal, const V &val)
{
    Node<K, V> n(keyVal, val);
    Bucket<K, V> b;
    long pos;
    size_t hash;

    hash = HashFunction(keyVal);

    pos = isColide(hash);
    if(pos != -1){
        buckets[pos].append(n);
        sort(buckets[pos].getValues());
    }

    else{
        b.setHashValue(hash);
        b.append(n);
        buckets.push_back(b);
        sort(buckets);
    }
}

template<typename K, typename V>
void HashMap<K,V>::deleteValue(const K &keyVal)
{
    size_t aux = HashFunction(keyVal);
    size_t i;
    LSL<Node<K, V>> *auxList;
    long pos = isColide(HashFunction(keyVal));

    if (pos != -1){
        auxList = &buckets[pos].getValues();

        for (i = 0; i < auxList->size(); ++i)
            if ((*auxList)[i].getKey() == keyVal){
                auxList->erase(i);
                break;
            }
        if (auxList->empty())
            buckets.erase(pos);
        else if (auxList->size() == i)
            cout << "There is not such value" << endl;
    }
    else
        cout << "There is not such value" << endl;
    cout << buckets.size() << endl;
}

template<typename K, typename V>
void HashMap<K, V>::clear()
{
    buckets.clear();
}

template<typename K, typename V>
size_t HashMap<K,V>::HashFunction(K val)
{
    stringstream ss;
    ss << val;
    string aux = ss.str();
    return string_to_size_t(aux) % m_mod;
}

template<typename K, typename V>
V* HashMap<K,V>::operator[](K index)
{
    size_t aux = HashFunction(index);
    LSL<Node<K, V>> *auxList;
    Node<K, V> nodeTmp;
    long posBucket = isColide(HashFunction(index));
    long posNode;
    V* v = nullptr;
    nodeTmp.setKey(index);
    
    if(posBucket != -1){
        auxList = &buckets[posBucket].getValues();
        
        posNode = binary_search(*auxList, nodeTmp);
        if(posNode != -1)
            v = (*auxList)[posNode].getValue();
    }
    return v;
}

size_t string_to_size_t(string str)
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
    while (l <= r)
    {
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