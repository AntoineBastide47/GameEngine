//
// SerializerInjector.cpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#include <fstream>
#include <ranges>
#include <regex>
#include <set>
#include <sstream>
#include <filesystem>

#include "Generator.hpp"
#include "Parser.hpp"

namespace Engine::Reflection {
  std::string Generator::GenerateRecordContent(const std::vector<Record> &records) {
    std::ostringstream content;
    content << "// Auto-generated – DO NOT EDIT\n\n#pragma once\n\nnamespace Engine::Reflection {\n";

    for (auto it = records.begin(); it != records.end(); ++it) {
      GenerateRecordMacro(*it, content, it == records.end() - 1);
    }
    content << "} // namespace Engine::Reflection\n";
    return content.str();
  }

  bool Generator::WriteFileIfChanged(const std::string &filePath, const std::string &content) {
    // Check if file exists and compare content
    if (std::filesystem::exists(filePath)) {
      std::ifstream existingFile(filePath);
      std::ostringstream existingContent;
      existingContent << existingFile.rdbuf();
      
      if (existingContent.str() == content) {
        // Content is identical, don't write
        return false;
      }
    }
    
    // Content is different or file doesn't exist, write it
    std::ofstream outputFile(filePath);
    outputFile << content;
    return true;
  }

  void Generator::GenerateRecordFiles(const std::vector<Record> &records) {
    std::unordered_map<std::string, std::vector<Record>> grouped;
    for (const auto &record: records)
      grouped[record.filePath].emplace_back(record);

    for (const auto &[filePath, records]: grouped) {
      const std::filesystem::path path(filePath);
      const std::string fileName = path.parent_path().string() + "/" + path.stem().string() + ".gen" +
                                   path.extension().string();

      // Generate content in memory first
      const std::string content = GenerateRecordContent(records);
      
      // Only write if content has changed
      WriteFileIfChanged(fileName, content);

      const std::string header = path.parent_path().string() + "/" + path.stem().string() + path.extension().string();
      std::ifstream headerFile(header);

      AddGeneratedInclude(header, path.stem().string() + ".gen" + path.extension().string());
      InjectSerializeMacro(header, records);
    }
  }

  void Generator::GenerateRecordMacro(const Record &record, std::ostream &output, const bool lastRecord) {
    std::string name;
    if (const auto pos = record.name.rfind("::"); pos == std::string::npos)
      name = record.name;
    else
      name = record.name.substr(pos + 2);
    std::ranges::transform(name, name.begin(), ::toupper);
    output.write("  #define SERIALIZE_", 20);
    output.write(name.c_str(), static_cast<long>(name.size()));
    output.write(" _e_SERIALIZE_RECORD \\\n", 23);

    GenerateReflectionFactoryCode(record, output);
    GenerateSaveFunction(record, output);
    GenerateLoadFunction(record, output);

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

  void Generator::GenerateReflectionFactoryCode(const Record &record, std::ostream &output) {
    output.write(
      "  static inline const bool _reg = [] {\\\n"
      "    Engine::Reflection::ReflectionFactory::RegisterType<", 96
    );
    output.write(record.name.c_str(), static_cast<long>(record.name.size()));
    output.write(">(\"", 3);
    output.write(record.name.c_str(), static_cast<long>(record.name.size()));
    output.write("\");\\\n", 5);
    output.write(
      "    return true;\\\n"
      "  }();\\\n", 26
    );
  }

  void Generator::GenerateSaveFunction(const Record &record, std::ostream &output) {
    output.write(
      "  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \\\n"
      "    if (format == Engine::Reflection::Format::JSON) { \\\n"
      "      json = Engine::JSON::Object();\\\n", 189
    );

    for (const auto &[_, name]: record.fields) {
      output.write("      Engine::Reflection::_e_saveImpl(", 38);
      output.write(name.c_str(), static_cast<long>(name.size()));
      output.write(", format, json[\"", 16);
      output.write(name.c_str(), static_cast<long>(name.size()));
      output.write("\"]);\\\n", 6);
    }
    output.write("    }\\\n  }\\\n", 12);
  }

  void Generator::GenerateLoadFunction(const Record &record, std::ostream &output) {
    output.write(
      "  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \\\n"
      "    if (format == Engine::Reflection::Format::JSON) {", 148
    );

    if (record.fields.empty()) {
      output.write("}\\\n  }", 6);
      return;
    }

    output.write(" \\\n", 3);
    bool first = true;
    for (const auto &[_, name]: record.fields) {
      if (!first)
        output.write("\\\n", 2);
      output.write("      Engine::Reflection::_e_loadImpl(", 38);
      output.write(name.c_str(), static_cast<long>(name.size()));
      output.write(", format, json[\"", 16);
      output.write(name.c_str(), static_cast<long>(name.size()));
      output.write("\"]);", 4);
      first = false;
    }
    output.write("\\\n    }\\\n  }", 12);
  }

  void Generator::AddGeneratedInclude(const std::string &headerPath, const std::string &generatedFile) {
    std::ifstream headerFile(headerPath);
    if (!headerFile)
      throw std::runtime_error("Cannot open header file: " + headerPath);

    std::vector<std::string> lines;
    std::string line;
    size_t lastIncludeIdx = std::string::npos;
    bool foundGeneratedInclude = false;

    while (std::getline(headerFile, line)) {
      lines.push_back(line);
      if (line.find("#include") != std::string::npos) {
        lastIncludeIdx = lines.size() - 1;
        if (line.find(generatedFile) != std::string::npos) {
          foundGeneratedInclude = true;
        }
      }
    }
    headerFile.close();

    if (foundGeneratedInclude)
      return;

    std::string includeLine = "#include \"" + generatedFile + "\"";
    if (lastIncludeIdx != std::string::npos) {
      lines.insert(lines.begin() + static_cast<long>(lastIncludeIdx) + 1, includeLine);
    } else {
      lines.insert(lines.begin(), includeLine);
    }

    std::ofstream outFile(headerPath, std::ios::trunc);
    for (const auto &l: lines)
      outFile << l << "\n";
  }

  void Generator::InjectSerializeMacro(const std::string &headerPath, const std::vector<Record> &records) {
    std::ifstream inFile(headerPath);
    if (!inFile)
      throw std::runtime_error("Cannot open header file: " + headerPath);

    std::vector<std::string> originalLines;
    std::string line;
    while (std::getline(inFile, line))
      originalLines.push_back(line);
    inFile.close();

    std::vector<std::string> lines = originalLines;
    bool modified = false;

    for (const auto &record: records) {
      std::string name = record.name;
      if (const auto pos = record.name.rfind("::"); pos != std::string::npos)
        name = record.name.substr(pos + 2);
      std::string upperName = name;
      std::ranges::transform(upperName, upperName.begin(), ::toupper);

      const std::regex declaration("^\\s*(class|struct)\\s+" + name + "\\b");
      for (size_t i = 0; i < lines.size(); ++i) {
        if (std::regex_search(lines[i], declaration)) {
          // Find opening brace {
          size_t braceLine = i;
          while (braceLine < lines.size() && lines[braceLine].find('{') == std::string::npos)
            ++braceLine;

          if (braceLine < lines.size()) {
            // Check if macro already exists
            if (size_t insertLine = braceLine + 1;
              insertLine < lines.size() && lines[insertLine].find("SERIALIZE_" + upperName) == std::string::npos) {
              size_t leadingWhitespace = 0;
              while (leadingWhitespace < lines[i].size() && (
                       lines[i][leadingWhitespace] == ' ' || lines[i][leadingWhitespace] == '\t'))
                ++leadingWhitespace;

              lines.insert(
                lines.begin() + static_cast<long>(insertLine),
                lines[i].substr(0, leadingWhitespace) + "  SERIALIZE_" + upperName
              );
              modified = true;
            }
          }
        }
      }
    }

    // Only write the file if content has changed
    if (modified) {
      std::ofstream outFile(headerPath, std::ios::trunc);
      for (const auto &l: lines)
        outFile << l << "\n";
    }
  }
}
