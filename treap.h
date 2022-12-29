#ifndef COURSECPP_TREAP_H
#define COURSECPP_TREAP_H

#include <iostream>
#include <algorithm>
#include <random>


typedef struct Node {
    size_t key, priority;
    Node *l = nullptr;
    Node *r = nullptr;
    Node(size_t key, size_t priority) : key(key), priority(priority){}
} Node;

class Treap {
    Node *root;
//    static Node *split
public:
    static Treap *merge (Treap *a, Treap *b);
    Treap();
};




#endif

