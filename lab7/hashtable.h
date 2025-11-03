#ifndef HASHTABLE_H
#define HASHTABLE_H

// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_deleted counter is *optional*. You might find it helpful, but it
// is not required to pass the lab assignment.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.

#include <cstdint>
#include <functional> // where std::hash lives
#include <vector>
#include <cassert> // useful for debugging!

// A bucket's status tells you whether it's empty, occupied, 
// or contains a deleted element.
enum class Status : uint8_t {
    Empty,
    Occupied,
    Deleted
};

const size_t npos = size_t(-1);


template<typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
    // used by the autograder; do not change/remove.
    friend class Verifier;
public:
    // A bucket has a status, a key, and a value.
    struct Bucket {
        // Do not modify Bucket.
        Status status = Status::Empty;
        K key;
        V val;
    };

    HashTable() {
        // TODO: a default constructor (possibly empty).
        buckets.resize(20);
        // You can use the following to avoid implementing rehash_and_grow().
        // However, you will only pass the AG test cases ending in _C.
        // To pass the rest, start with at most 20 buckets and implement rehash_and_grow().
        //    buckets.resize(10000);
    }

    size_t size() const {
        return num_elements;
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    V& operator[](const K& key) {
        // TODO
        size_t idx = find_index(key);
        if (idx != npos) {
            return buckets[idx].val;
        }

        // insert default value
        insert(key, V{});
        // now it exists
        idx = find_index(key);
        return buckets[idx].val;

    }

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        // TODO
        if ((num_elements + num_deleted) * 2 >= buckets.size()) {
            rehash_and_grow();
        }
        return do_insert(key, val);
    }
    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        // TODO
        size_t idx = find_index(key);
        if (idx == npos) return 0;
        buckets[idx].status = Status::Deleted;
        num_elements--;
        num_deleted++;
        return 1;
    }

private:
    size_t num_elements = 0;
    size_t num_deleted = 0; // OPTIONAL: you don't need to use num_deleted to pass
    std::vector<Bucket> buckets;




    
    // You can add methods here if you like.
    // TODO
    size_t find_index(const K& key) const {
        Hasher hasher;
        size_t cap = buckets.size();
        size_t idx = hasher(key) % cap;

        for (size_t i = 0; i < cap; i++) {
            size_t p = (idx + i) % cap;
            if (buckets[p].status == Status::Empty) {
                return npos;
            }
            if (buckets[p].status == Status::Occupied && buckets[p].key == key) {
                return p;
            }
        }
        return npos;
    }

    bool do_insert(const K& key, const V& val) {
        Hasher hasher;
        size_t cap = buckets.size();
        size_t idx = hasher(key) % cap;
        size_t first_deleted = npos;

        for (size_t i = 0; i < cap; i++) {
            size_t p = (idx + i) % cap;
            if (buckets[p].status == Status::Occupied && buckets[p].key == key) {
                return false; // already exists
            }
            if (buckets[p].status == Status::Deleted && first_deleted == npos) {
                first_deleted = p;
            }
            if (buckets[p].status == Status::Empty) {
                size_t target = (first_deleted != npos) ? first_deleted : p;
                buckets[target].status = Status::Occupied;
                buckets[target].key = key;
                buckets[target].val = val;
                num_elements++;
                if (first_deleted != npos) num_deleted--;
                return true;
            }
        }
        return false; // full (should not happen because rehashing)
    }

    void rehash_and_grow() {
        std::vector<Bucket> old = buckets;
        buckets.clear();
        buckets.resize(old.size() * 2);

        num_elements = 0;
        num_deleted = 0;

        for (auto &b : old) {
            if (b.status == Status::Occupied) {
                do_insert(b.key, b.val);
            }
        }
    }

};

#endif // HASHTABLE_H