// TestSerialization.hpp
#ifndef TEST_SERIALIZATION_HPP
#define TEST_SERIALIZATION_HPP

#include "TestSerialization.gen.hpp"
#include "../../../include/Engine/Reflection/Reflectable.hpp"

class Base : public Engine::Reflection::Reflectable {
    SERIALIZE_BASE
  public:
    int base = 3;
};

namespace Test {
  class Derived final : public Base {
    SERIALIZE_DERIVED
  public:
      int derived = 10;

      explicit Derived(const int t)
        : derived(t) {}
  };
}

using namespace Test;

class Derived2 final : public Base {
    SERIALIZE_DERIVED2
};

template<typename T, typename V> class Derived3 final : public Base {
    SERIALIZE_DERIVED3
    ENGINE_SERIALIZE std::vector<T> t;
    ENGINE_SERIALIZE V v;
};

#endif // TEST_SERIALIZATION_HPP
