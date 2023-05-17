#include "Node.h"
#include <queue>

#define MAX_NODE_VAL 2000

class Tree {
    public:
        Node * root;
        void print();
        void printHelper(Node * start);
        Tree(); // constructor
        ~Tree();
        Node * search(int valToFind);
        Node * searchHelper(Node*, int);
        bool insert(int valToAdd);
        bool remove(int valToKill);
};