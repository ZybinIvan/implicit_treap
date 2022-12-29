#include "treap.h"

Treap::Treap(): root(nullptr) {}

Treap* Treap::merge(Treap *a, Treap *b) {
    if (!a->root || !b->root) {
        return a->root ? a : b;
    }
}