#include "Tree.h"

void Tree::print() {
    if (!root) {
        return;
    }

    std::queue<Node*> q;
    q.push(root);

    while (!q.empty()) {
        int size = q.size();
        for (int i = 0; i < size; i++) {
            Node* curr = q.front();
            q.pop();
            std::cout << "|" << curr->val1 << " " << curr->val2 << " " << curr->val3 << "|" ;

            if (curr->left) {
                q.push(curr->left);
            }
            if (curr->middle) {
                q.push(curr->middle);
            }
            if (curr->right) {
                q.push(curr->right);
            }
        }
        std::cout << std::endl;
    }
}

Tree::Tree() {
    Node * rootToBe = new Node(0, -1, MAX_NODE_VAL);
    Node * initLeaf1 = new Node(0,-1,-1, nullptr, nullptr, nullptr, rootToBe);
    Node * initLeaf2 = new Node(MAX_NODE_VAL, -1, -1, nullptr, nullptr, nullptr, rootToBe);
    rootToBe->left = initLeaf1;
    rootToBe->right = initLeaf2;
    root = rootToBe;
}

Tree::~Tree() {
    if (root != nullptr) {
        delete root;
    }
}

Node * Tree::searchHelper(Node* root, int valToFind) {
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
        // should never run
    }
}

Node * Tree::search(int valToFind) {
    return searchHelper(root, valToFind);
}


bool Tree::insert(int valToAdd) {
    Node * a = new Node(valToAdd);
    Node * b = search(valToAdd);
    if (b->val1 == valToAdd) {
        return true;
    }
    Node * temp = a->absorb(b);
    if (temp != nullptr) {
        root = temp;
    }
    return true;
}

bool Tree::remove(int valToKill) {
    Node * temp = nullptr;
    Node * a = new Node(valToKill);
    Node * b = search(valToKill);
    if (valToKill == b->val1) {
        temp = a->discard(b);
    }
    if (temp != nullptr) {
        root = temp;
    }
    return true;
}