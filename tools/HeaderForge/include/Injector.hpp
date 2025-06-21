//
// SerializerInjector.hpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#ifndef SERIALIZER_INJECTOR_HPP
#define SERIALIZER_INJECTOR_HPP

#include <set>
#include <vector>

namespace Engine::Serialization {
  struct Record;
}

namespace Engine::Serialization {
  /// Class used to write the functions to serialize and deserialize user defined records
  class Injector final {
    public:
      /// Writes the serialization functions for user defined records
      static void InjectRecordsForSerialization(const std::vector<Record> &records, std::set<std::string> &usages);
      /// Writes the serialization functions for a user defined record
      static void SerializeRecord(const Record &record, std::ofstream &output);
  };
}

#endif //SERIALIZER_INJECTOR_HPP
