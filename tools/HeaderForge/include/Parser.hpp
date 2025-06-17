//
// Parser.hpp
// Author: Antoine Bastide
// Date: 13/06/2025
//

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

namespace Engine::Serialization {
  /// Represents a member variable with its fully-qualified type name and identifier.
  struct Variable {
    std::string type; ///< Fully qualified type name (including namespace, templates, pointers).
    std::string name; ///< Variable name as declared in the class.
  };

  /// Represents a record (class/struct) with its name, location, and serializable fields.
  struct Record {
    std::string name; ///< Fully qualified name of the class/struct.
    std::string filePath; ///< File path where this class is defined.
    std::vector<Variable> fields; ///< List of serializable member variables.
  };

  /**
   * Parser class that uses Clang's AST to extract `Record` information from a C++ header.
   * Only classes/structs that are friends of Engine::Serialization::Serializer are considered.
   */
  class Parser final {
    public:
      /**
       * Parse the given header file and return a list of Records representing
       * serializable classes and their fields.
       * @param headerPath Path to the header file to parse.
       * @return Vector of Record structures with class and field info.
       */
      static std::vector<Record> ParseHeader(const std::string &headerPath);

      /**
       * Utility function to print the collected records and their fields to standard output.
       * @param records Vector of Record objects to print.
       */
      static void PrintRecords(const std::vector<Record> &records);
    private:
      static std::string getClangResourceDir();
      static std::string getMacOSSDKPath();
  };
} // namespace Engine::Serialization

#endif // PARSER_HPP
