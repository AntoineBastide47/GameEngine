//
// OrderedMap.hpp
// Author: Antoine Bastide
// Date: 28/03/2025
//

#ifndef ORDERED_MAP_HPP
#define ORDERED_MAP_HPP

#include <iostream>
#include <list>
#include <unordered_map>

template<typename Key, typename Value> class OrderedMap {
  public:
    void insert(const Key &key, Value value) {
      if (!map.contains(key)) {
        data.emplace_back(key, std::move(value));
        map[key] = std::prev(data.end());
      } else {
        map[key]->second = std::move(value);
      }
    }

    bool contains(const Key &key) const {
      return map.contains(key);
    }

    Value &operator[](const Key &key) const {
      return map.at(key)->second;
    }

    auto begin() {
      return data.begin();
    }

    auto end() {
      return data.end();
    }

    auto begin() const {
      return data.begin();
    }

    auto end() const {
      return data.end();
    }
  private:
    std::list<std::pair<Key, Value>> data;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> map;
};

#endif //ORDERED_MAP_HPP
