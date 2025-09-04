//
// SerializerInjector.hpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#ifndef SERIALIZER_INJECTOR_HPP
#define SERIALIZER_INJECTOR_HPP

#include <string>
#include <vector>

namespace Engine::Reflection {
  struct Enum;
  struct Record;
}

namespace Engine::Reflection {
  /// Class used to write the functions to serialize and deserialize user defined records
  class Generator final {
    public:
      /// Generates files in which the record serialization macros will be written
      static void GenerateRecordFiles(const std::vector<Record> &records, bool overrideWrite);
      /// Generates the content for a record file in memory
      static std::string GenerateRecordContent(const std::vector<Record> &records);
      /// Writes the serialization macros for a user defined record
      static void GenerateRecordMacro(const Record &record, std::ostream &output);
      /// Writes the reflection factory code for a user defined record
      static void GenerateReflectionFactoryCode(const Record &record, std::ostream &output);
      /// Writes the save function for a user defined record
      static void GenerateSaveFunction(const Record &record, std::ostream &output);
      /// Writes the load function for a user defined record
      static void GenerateLoadFunction(const Record &record, std::ostream &output);
      /// Writes the editor render function for a user defined record
      static void GenerateEditorRecordFunction(const Record &record, std::ostream &output);
      /// Writes the macro that enables the enum to be reflected
      static void GenerateEnumReflectionMacro(const Enum &enumerator, std::ostream &output, bool lastEnum);

      /// Writes a file only if the content has changed
      static bool WriteFileIfChanged(const std::string &filePath, const std::string &content, bool overrideWrite);
      /// Writes an include directive for the generated file into it's header file
      static void AddGeneratedInclude(const std::string &headerPath, const std::string &generatedFile);
      /// Writes the generated class macros in the header file
      static void InjectSerializeMacro(const std::string &headerPath, const std::vector<Record> &records);
      /// Writes the generated enum macros in the header file
      static void InjectReflectMacro(const std::string &headerPath, const std::vector<Enum> &enums);
  };
}

#endif //SERIALIZER_INJECTOR_HPP
