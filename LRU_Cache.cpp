#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <format>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>
#include<set>
#include<list>

using namespace std;

/*

Design a LRU Cache


Requirements

The LRU cache should support the following operations:
put(key, value): Insert a key-value pair into the cache. If the cache is at capacity, remove the least recently used item before inserting the new item.
get(key): Get the value associated with the given key. If the key exists in the cache, move it to the front of the cache (most recently used) and return its value. If the key does not exist, return -1.
The cache should have a fixed capacity, specified during initialization.
The cache should be thread-safe, allowing concurrent access from multiple threads.
The cache should be efficient in terms of time complexity for both put and get operations, ideally O(1).


*/

/*

Fucntional Requirements : 
- Support Put(k , v) operation
- Support get(k) operation
- Capacity should be fixed 

Non Fucntional Requirements : 
- thread-safe
- Efficient
- Extendible
*/

class LRU {
private:
    int capacity;
    list<pair<int, int>> dq;
    unordered_map<int, list<pair<int, int>>::iterator> mp;
    mutex mtx;

public:
    LRU(int cap) {
        capacity = cap;
    }

    void put(int key, int val) {
        lock_guard<mutex> lock(mtx);

        if (capacity == 0)
            return;

        if (mp.count(key)) {
            // Key already exists
            auto it = mp[key];
            dq.erase(it);
            dq.push_front({key, val});
            mp[key] = dq.begin();
        } else {
            if (dq.size() < capacity) {
                // Space left
                dq.push_front({key, val});
                mp[key] = dq.begin();
            } else {
                // Full -> Remove least recently used
                auto last = dq.back();
                dq.pop_back();
                mp.erase(last.first);

                dq.push_front({key, val});
                mp[key] = dq.begin();
            }
        }

        cout << "Successfully Added" << endl;
    }

    int get(int key) {
        lock_guard<mutex> lock(mtx);

        if (!mp.count(key)) {
            cout << "Doesn't exist" << endl;
            return -1;
        }

        auto it = mp[key];
        int val = it->second;   // Save value before erasing

        dq.erase(it);
        dq.push_front({key, val});
        mp[key] = dq.begin();

        return val;
    }
};

int main() {
    LRU cache(3);

    cache.put(1, 10);
    cache.put(2, 20);
    cache.put(3, 30);

    cout << "Get 2: " << cache.get(2) << endl; // 20

    cache.put(4, 40); // Evicts key 1

    cout << "Get 1: " << cache.get(1) << endl; // -1
    cout << "Get 3: " << cache.get(3) << endl; // 30

    cache.put(5, 50); // Evicts key 2

    cout << "Get 2: " << cache.get(2) << endl; // -1
    cout << "Get 4: " << cache.get(4) << endl; // 40
    cout << "Get 5: " << cache.get(5) << endl; // 50

    cache.put(3, 300); // Update existing key

    cout << "Get 3: " << cache.get(3) << endl; // 300

    return 0;
}
