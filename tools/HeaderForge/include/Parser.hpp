//
// Parser.hpp
// Author: Antoine Bastide
// Date: 13/06/2025
//

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

namespace Engine::Reflection {
  /// Represents a record (class/struct) with its name, location, and serializable fields.
  struct Record {
    /// True if this record is a class, False if not
    bool isClass;
    /// Fully qualified name of the class/struct.
    std::string name;
    /// File path where this class is defined.
    std::string filePath;
    /// List of serializable member variables.
    std::vector<std::string> fields;
    /// List of editor inspectable member variables.
    std::vector<std::string> editorFields;
  };

  /**
   * Parser class that uses Clang's AST to extract `Record` information from a C++ header.
   * Only classes/structs that are friends of Engine::Reflection::Serializer are considered.
   */
  class Parser final {
    public:
      /**
       * Parse the given header file and return a list of Records representing
       * serializable classes and their fields.
       * @param headerPath Path to the header file to parse.
       * @param records All the extracted records
       * @return Vector of Record structures with class and field info.
       */
      static void ParseHeader(
        const std::string &headerPath, std::vector<Record> &records
      );

      /**
       * Utility function to print the collected records and their fields to standard output.
       * @param records Vector of Record objects to print.
       */
      static void PrintRecords(const std::vector<Record> &records);
    private:
      /// @returns The path to the clang resource directory
      static std::string getClangResourceDir();
      /// @returns The path to the macOS SDK
      static std::string getMacOSSDKPath();
  };
} // namespace Engine::Reflection

#endif // PARSER_HPP
