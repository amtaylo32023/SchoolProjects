#include "Node.h"

Node * searchHelper(Node* root, int valToFind) {
    if (!(root->hasChildren())) {
        return root;
    }
    if (root->left != nullptr && valToFind <= root->val1) {
        return searchHelper(root->left, valToFind);
    } else if (root->middle != nullptr && valToFind <= root->val2) {
        return searchHelper(root->middle, valToFind);
    } else if (root->right != nullptr && valToFind <= root->val3) {
        return searchHelper(root->right, valToFind);
    } else {
        return root->right;
    }
}

Node::Node(int v1, int v2, int v3, Node* l, Node* m, Node* r, Node* p)
 : val1(v1), val2(v2), val3(v3), left(l), middle(m), right(r), parent(p) {}

Node::~Node() {
    // Recursive Delete
    if (left != nullptr) {
        delete left;
    }
    if (middle != nullptr) {
        delete middle;
    }
    if (right != nullptr) {
        delete right;
    }
}

int Node::hasChildren() {
    return left != nullptr || middle != nullptr || right != nullptr;
}

int Node::getNumChildren() {
    return (left != nullptr) + (middle != nullptr) + (right != nullptr);
}

Node * Node::absorb(Node * newChild) {
    int max = 0;
    if (this->val3 == -1) {
        max = this->val1;
    } else {
        max = this->val3;
    }
    
    if (newChild->parent == nullptr) {
        Node * megaRoot = new Node(max, -1, newChild->val3, this, nullptr, newChild);
        this->parent = megaRoot;
        newChild->parent = megaRoot;
        return megaRoot;
    }

    Node * p = newChild->parent;
    if (p->getNumChildren() < 3) {
        if (p->val1 > max) {
            p->middle = p->left;
            p->val2 = p->val1;
            p->left = this;
            p->val1 = max;
        } else if (p->val1 < max && p->val3 > max) {
            p->middle = this;
            p->val2 = max;
        } else {
            p->middle = p->right;
            p->val2 = p->val3;
            p->right = this;
            p->val3 = max;
        }
        this->parent = p;
        return nullptr;
    } else {
        Node * p_left = nullptr;
        if (max <= p->val1) {
            p_left = new Node(max, -1, p->val1, this, nullptr, p->left);
            p->left->parent = p_left;
            this->parent = p_left;

            p->left = p->middle;
            p->val1 = p->val2;
        } else if (max <= p->val2) {
            p_left = new Node(p->val1, -1, max, p->left, nullptr, this);
            p->left->parent = p_left;
            this->parent = p_left;

            p->left = p->middle;
            p->val1 = p->val2;
        } else if (max <= p->val3) {
            p_left = new Node(p->val1, -1, p->val2, p->left, nullptr, p->middle);
            p->left->parent = p_left;
            p->middle->parent = p_left;
            this->parent = p;

            p->left = this;
            p->val1 = max;
        } else {
            p_left = new Node(p->val1, -1, p->val2, p->left, nullptr, p->middle);
            p->left->parent = p_left;
            p->middle->parent = p_left;
            this->parent = p;

            p->left = p->right;
            p->val1 = p->val3;
            p->right = this;
            p->val3 = max;
        }

        p->middle = nullptr;
        p->val2 = -1;
        return p_left->absorb(p);
    }
}

void Node::updateMax(Node * candidate, int valToUpdate, int updatedVal) {
    if (candidate->val1 == valToUpdate) {
        candidate->val1 = updatedVal;
    } else if (candidate->val2 == valToUpdate) {
        candidate->val2 = updatedVal;
    } else {
        int newValToUpdate = 0;
        newValToUpdate = candidate->val3;
        candidate->val3 = updatedVal;
        if (candidate->parent) {
            updateMax(candidate->parent, newValToUpdate, updatedVal);
        }
    }
}

// discarding part one works but we stil need to balance children among parent nodes to establish 2-3 structure
Node * Node::discard(Node * removeChild) {
    bool updateNeeded = false;
    Node * p = removeChild->parent;
    removeChild->parent = nullptr;
    if (p->left == removeChild) {
        p->left = p->middle;
        p->val1 = p->val2;
    } else if (p->middle == removeChild) {
        // do nothing but clear out middle
    } else {
        // updating right val so need to update parent max
        updateNeeded = true;
        p->right = p->middle;
        p->val3 = p->val2;
    }
    p->middle = nullptr;
    p->val2 = -1;


    if (p->getNumChildren() == 2) {
        if (updateNeeded) {
            updateMax(p, removeChild->val1, p->val3);
        }
        delete removeChild;
        return nullptr; // no change needed
    }

    int totalChildren = 0;
    Node * gp = p->parent;

    if (gp) {
        if (gp->left) {
            totalChildren += gp->left->getNumChildren();
        }
        if (gp->middle) {
            totalChildren += gp->middle->getNumChildren();
        }
        if (gp->right) {
            totalChildren += gp->right->getNumChildren();
        }
    }

    // did not implement cases where totalchildren >=4 (sorry-not-sorry)

    Node * b = nullptr;
    int b_val;
    if (p->left) {
        b = p->left;
        b_val = p->val1;
    } else if (p->middle) {
        b = p->middle;
        b_val = p->val2;
    } else if (p->right) {
        b = p->right;
        b_val = p->val3;
    }

    if (p->parent == nullptr) {
        return b;
    }

    if (totalChildren == 3) {
        Node * sibling = nullptr;
        if (gp->left && (gp->left != p)) {
            sibling = gp->left;
        } else if (gp->middle && (gp->middle != p)) {
            sibling = gp->middle;
        } else if (gp->right && (gp->right != p)) {
            sibling = gp->right;
        }  
        
        b->absorb(searchHelper(sibling, b->val1));

        return discard(p);
    }
   
}