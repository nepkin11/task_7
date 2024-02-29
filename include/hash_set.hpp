#pragma once

#include <iostream>
#include <functional>
#include <cstddef>

//header
template<class K>
class node {
public:
    node *next;
    K data;

    explicit node(K);

    ~node();

    friend std::ostream &operator<<(std::ostream &os, const node &nd) {
        os << nd.data;
        return os;
    };
};

template<class K>
class entry {
    node<K> *root;
    node<K> **current;

public:
    entry();

    ~entry();

    void add_node(K);

    void remove_node(K);

    bool has_key(K);

    node<K> *current_node();

    void next_node();

    void restart();

    friend std::ostream &operator<<(std::ostream &os, const entry &en) {
        if (!en.root) {
            return os;
        }
        //________else
        node<K> *x = en.root;
        while (x->next) {
            os << *x << ", ";
            x = x->next;
        }
        os << *x;
        return os;
    }
};

template<class K>
class hash_set {
    entry<K> *entries;
    int num_entries;
    int card;

    void _add(K);

    [[nodiscard]] int redundancy() const;

    void resize(int);

    std::size_t hash(K);

public:
    hash_set();

    ~hash_set();

    void add(K);

    void remove(K);

    bool has_key(K);

    [[nodiscard]] int size() const;

    friend std::ostream &operator<<(std::ostream &os, const hash_set<K> &ht) {
        int first = 0;
        while (!ht.entries[first].current_node()) {
            first++;
        }
        os << "{" << ht.entries[first];
        for (int i = first + 1; i < ht.num_entries; i++) {
            if (ht.entries[i].current_node()) {
                os << ", " << ht.entries[i];
            }
        }
        os << "}";
        return os;
    };

    friend hash_set<K> operator+(const hash_set<K> &first, const hash_set<K> &second) {
        hash_set<K> result;

        for (int i = 0; i < first.num_entries; i++) {
            if (first.entries[i].current_node()) {
                result.add(first.entries[i].current_node()->data);
            }
        }
        for (int i = 0; i < second.num_entries; i++) {
            if (second.entries[i].current_node()) {
                result.add(second.entries[i].current_node()->data);
            }
        }
        return result;

    }
};

template<class K>
node<K>::node(K key) {
    data = key;
    next = nullptr;
}

template<class K>
node<K>::~node() {
    delete next;
}

// end node methods


// entry methods
template<class K>
entry<K>::entry() {
    root = nullptr;
    current = &root;
}

template<class K>
entry<K>::~entry() {
    delete root;
}

template<class K>
void entry<K>::add_node(K key) {
    if (!root) {
        root = new node<K>(key);
        return;
    }
    //__________

    node<K> *x = root;
    while (x->data != key) {
        if (x->next) {
            x = x->next;
        } else {
            x->next = new node<K>(key);
            return;
        }
    }
}

template<class K>
void entry<K>::remove_node(K key) {
    if (!root) { return; } //if there's no root, then there is no data here and you can't delete the key
    //______________________

    if (root->data == key) //if the root has the key, delete the root and rearrange
    {
        node<K> *old = root;
        root = root->next;
        old->next = nullptr;
        delete old;
        return;
    }
    //______________________

    // if the root doesn't have the data, we need to search through the children
    node<K> *x = root;
    while (x->next) {
        if (x->next->data == key) {
            node<K> *old = x->next;
            x->next = x->next->next;
            old->next = nullptr;
            delete old;
            return;
        }
        x = x->next;
    }
}

template<class K>
bool entry<K>::has_key(K key) {
    node<K> *x = root;
    while (x) {
        if (x->data == key) { return true; }
        x = x->next;
    }
    return false;
}

template<class K>
node<K> *entry<K>::current_node() {
    return *current;
}

template<class K>
void entry<K>::next_node() {
    current = &(*current)->next;
}

template<class K>
void entry<K>::restart() {
    current = &root;
}


//end entry


//begin hashSet

template<class K>
hash_set<K>::hash_set() {
    num_entries = 10;
    card = 0;
    entries = new entry<K>[num_entries];
}

template<class K>
hash_set<K>::~hash_set() {
    delete[] entries;
}

template<class K>
std::size_t hash_set<K>::hash(K key) {
    return std::hash<K>{}(key) % num_entries;
}

template<class K>
void hash_set<K>::_add(K key) {
    std::size_t address = hash(key);
    entries[address].add_node(key);
    card++;
}

template<class K>
void hash_set<K>::add(K key) {
    if (redundancy() > 3) {
        resize(card + 1);
    }
    _add(key);
}

template<class K>
void hash_set<K>::remove(K key) {
    std::size_t address = hash(key);
    entries[address].remove_node(key);
    card--;
}

template<class K>
bool hash_set<K>::has_key(K key) {
    std::size_t address = hash(key);
    return entries[address].has_key(key);
}

template<class K>
int hash_set<K>::redundancy() const {
    return card / num_entries;
}

template<class K>
void hash_set<K>::resize(int newSize) {
    entry<K> *old_entries = entries;
    const int old_num_entries = num_entries;

    num_entries = newSize;
    entries = new entry<K>[num_entries];

    for (int i = 0; i < old_num_entries; i++) {
        while (old_entries[i].current_node()) {
            add(old_entries[i].current_node()->data);
            old_entries[i].next_node();
        }
    }
    delete[] old_entries;
}

template<class K>
int hash_set<K>::size() const {
    return card;
}