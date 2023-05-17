#include <iostream>

class Node {
    public:
        int val1;
        int val2;
        int val3;
        int numChildren;
        Node * left;
        Node * middle;
        Node * right;
        Node * parent;
        
        Node(int v1, int v2 = -1, int v3 = -1, Node* l = nullptr, Node* m = nullptr, Node* r = nullptr, Node* p = nullptr);
        ~Node();
        int hasChildren();
        int getNumChildren();
        Node * absorb(Node * newChild);
        Node * discard(Node * removeChild);
        void updateMax(Node * candidate, int valToUpdate, int updatedVal);
};