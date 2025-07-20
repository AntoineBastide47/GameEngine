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
      /// Writes the reflection factory code for a user defined record
      static void GenerateReflectionFactoryCode(const Record &record, std::ofstream &output);
      /// Writes the save function for a user defined record
      static void GenerateSaveFunction(const Record &record, std::ofstream &output);
      /// Writes the load function for a user defined record
      static void GenerateLoadFunction(const Record &record, std::ofstream &output);

      /// Writes an include directive for the generated file into it's header file
      static void AddGeneratedInclude(const std::string &headerPath, const std::string &generatedFile);
      /// Writes the generated class macros in the header file
      static void InjectSerializeMacro(const std::string &headerPath, const std::vector<Record> &records);
  };
}

#endif //SERIALIZER_INJECTOR_HPP
