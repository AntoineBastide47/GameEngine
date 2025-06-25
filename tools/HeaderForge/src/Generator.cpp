//
// SerializerInjector.cpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#include <fstream>
#include <ranges>
#include <set>
#include <sstream>

#include "Generator.hpp"

#include <iostream>

#include "Parser.hpp"

namespace Engine::Reflection {
  void Generator::GenerateRecordFiles(const std::vector<Record> &records) {
    std::unordered_map<std::string, std::vector<Record>> grouped;
    for (const auto &record: records)
      grouped[record.filePath].emplace_back(record);

    for (const auto &[filePath, records]: grouped) {
      const std::filesystem::path path(filePath);
      const std::string fileName = path.parent_path().string() + "/" + path.stem().string() + ".gen" +
                                   path.extension().string();

      std::ofstream generatedFile(fileName);
      generatedFile.write("// Auto-generated – DO NOT EDIT\n\n#pragma once\n\nnamespace Engine::Reflection {\n", 80);

      for (auto it = records.begin(); it != records.end(); ++it)
        GenerateRecordMacro(*it, generatedFile, it == records.end() - 1);
      generatedFile.write("} // namespace Engine::Reflection\n", 34);
    }
  }

  void Generator::GenerateRecordMacro(const Record &record, std::ofstream &output, const bool lastRecord) {
    std::string name;
    if (const auto pos = record.name.rfind("::"); pos == std::string::npos)
      name = record.name;
    else
      name = record.name.substr(pos + 2);
    std::ranges::transform(name, name.begin(), ::toupper);

    output << "  #define SERIALIZE_" << name;
    output.write(
      " _e_SERIALIZE_RECORD \\\n"
      "  inline void _e_save(const Engine::Reflection::Format format, std::ostringstream& os, const bool prettyPrint, const int indent) const { \\\n"
      "    if (format == Engine::Reflection::Format::JSON) { \\\n      os.put('{'); \\\n", 239
    );

    bool first = true;
    for (const auto &[_, name]: record.fields) {
      if (!first)
        output.write("      os.put(','); \\\n", 21);
      output.write(
        "      if (prettyPrint) os.put('\\n'); \\\n"
        "      Engine::Reflection::applyIndent(os, prettyPrint, indent + 1); \\\n"
        R"(      os.write("\")", 127
      );
      output << name << R"(\":", )" << name.size() + 3;
      output.write(
        "); \\"
        "\n      if (prettyPrint) os.put(' '); \\\n"
        "      Engine::Reflection::_e_saveImpl(this->", 87
      );
      output << name << ", format, prettyPrint, 0, &os); \\\n";
      first = false;
    }
    output.write(
      "      if (prettyPrint) os.put('\\n'); \\\n"
      "      Engine::Reflection::applyIndent(os, prettyPrint, indent); \\\n"
      "      os.put('}'); \\\n    } \\\n  }", 137
    );
    if (record.isClass)
      output.write("\\\n  private: \n", 14);

    if (!lastRecord) {
      if (!record.isClass)
        output.put('\n');
      output.put('\n');
    }

    if (lastRecord && !record.isClass)
      output.put('\n');
  }
}
