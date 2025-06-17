// TestSerialization.hpp
#ifndef TEST_SERIALIZATION_HPP
#define TEST_SERIALIZATION_HPP

#include <functional>
#include <memory>

// Add the macro definitions
#include <bitset>
#include <map>

#include "../../../include/Engine/Macros/Serialization.hpp"

namespace Engine::Serialization {
  // Test 1: basic serialization with simple types
  class BasicTest final {
    ENGINE_SERIALIZE_RECORD
    public:
      int aPub;
      float bPub;
      ENGINE_NON_SERIALIZABLE_FIELD double dPubNonSer;
    private:
      ENGINE_SERIALIZE_FIELD double cPriSer;
      ENGINE_NON_SERIALIZABLE_FIELD ENGINE_SERIALIZE_FIELD double dPriSer;
  };

  // Test 2: function pointer and reference serialization
  class FuncRefTest final {
    ENGINE_SERIALIZE_RECORD
    public:
      FuncRefTest(void (*fptrPub)(), int &refValuePub, int hiddenPri, char annotatedFieldPriSer)
        : fptrPub(fptrPub), refValuePub(refValuePub), hiddenPri(hiddenPri),
          annotatedFieldPriSer(annotatedFieldPriSer) {}

      ENGINE_SERIALIZE_POINTER void (*fptrPub)();
      ENGINE_SERIALIZE_POINTER int &refValuePub;
    private:
      int hiddenPri; // should be skipped
      ENGINE_SERIALIZE_FIELD char annotatedFieldPriSer; // serialized despite private
  };

  // Test 3: C and smart pointer serialization
  class PointerTest final {
    ENGINE_SERIALIZE_RECORD
    public:
      ENGINE_SERIALIZE_POINTER int *rawPtrPubSer;
      ENGINE_SERIALIZE_POINTER std::shared_ptr<int> sptrPubSer;
      ENGINE_SERIALIZE_POINTER std::unique_ptr<float> uptrPubSer;
      ENGINE_SERIALIZE_POINTER std::weak_ptr<double> wptrPubSer; // skipped (no SERIALIZE_POINTER)
    private:
      ENGINE_SERIALIZE_POINTER std::unique_ptr<FuncRefTest> uptrPriSer;
  };

  // Test 4: nested types serialization
  class NestedTest final {
    ENGINE_SERIALIZE_RECORD
    public:
      ENGINE_SERIALIZE_FIELD std::vector<std::map<int, std::string>> nestedMapPub;
      ENGINE_SERIALIZE_POINTER std::vector<std::shared_ptr<char>> nestedPtrVecPubSer;
    private:
      std::map<int, void *> skipFuncMapPri; // skipped as void* is pointer but no SERIALIZE_POINTER
  };

  // Test 5: deeply nested types serialization
  class DeepNestedTest final {
    ENGINE_SERIALIZE_RECORD
    public:
      ENGINE_SERIALIZE_POINTER std::vector<std::map<int, std::vector<std::unique_ptr<std::shared_ptr<long>>>>>
      deepPtrNestPub;
      ENGINE_SERIALIZE_FIELD std::map<std::string, std::vector<std::map<int, void(*)()>>> deepFuncNestPub;
    private:
      double privateDataPri;
  };

  // Test 6: nested classes and classes not marked as serializable
  class NotSerializable final {
    public:
      int a;
      ENGINE_SERIALIZE_FIELD double b;
  };

  class Outer final {
    public:
      struct Inner final {
        ENGINE_SERIALIZE_RECORD
        ENGINE_SERIALIZE_FIELD int innerFieldPub;
        private:
          int skipInnerPri;
      };

      ENGINE_SERIALIZE_FIELD int outerFieldPub; // skipped: Outer has no friend
  };
}

#endif // TEST_SERIALIZATION_HPP
