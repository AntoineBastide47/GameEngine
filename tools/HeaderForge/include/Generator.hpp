//
// SerializerInjector.hpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#ifndef SERIALIZER_INJECTOR_HPP
#define SERIALIZER_INJECTOR_HPP

#include <vector>

namespace Engine::Reflection {
  struct Record;
}

namespace Engine::Reflection {
  /// Class used to write the functions to serialize and deserialize user defined records
  class Generator final {
    public:
      /// Generates files in which the record serialization macros will be written
      static void GenerateRecordFiles(const std::vector<Record> &records);
      /// Writes the serialization macros for a user defined record
      static void GenerateRecordMacro(const Record &record, std::ofstream &output, bool lastRecord);
  };
}

#endif //SERIALIZER_INJECTOR_HPP
