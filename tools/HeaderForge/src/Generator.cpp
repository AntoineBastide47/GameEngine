//
// SerializerInjector.cpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#include <filesystem>
#include <fstream>
#include <ranges>
#include <regex>
#include <set>
#include <sstream>

#include "Generator.hpp"
#include "Parser.hpp"

namespace Engine::Reflection {
  void Generator::GenerateRecordFiles(const std::vector<Record> &records, const bool overrideWrite) {
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
      WriteFileIfChanged(fileName, content, overrideWrite);

      const std::string header = path.parent_path().string() + "/" + path.stem().string() + path.extension().string();
      std::ifstream headerFile(header);

      AddGeneratedInclude(header, path.stem().string() + ".gen" + path.extension().string());
      InjectSerializeMacro(header, records);

      for (const auto &record: records)
        InjectReflectMacro(header, record.enums);
    }
  }

  std::string Generator::GenerateRecordContent(const std::vector<Record> &records) {
    std::ostringstream content;
    content.write("// Auto-generated – DO NOT EDIT\n\n#pragma once\n\n", 49);

    for (auto it = records.begin(); it != records.end(); ++it) {
      GenerateRecordMacro(*it, content);
      for (auto iit = it->enums.begin(); iit != it->enums.end(); ++iit)
        GenerateEnumReflectionMacro(*iit, content, iit == it->enums.end() - 1);

      const bool lastRecord = it == records.end() - 1;
      if (!lastRecord) {
        if (!it->isClass)
          content.put('\n');
        content.put('\n');
      }

      if (lastRecord && !it->isClass)
        content.put('\n');
    }
    return content.str();
  }

  void Generator::GenerateRecordMacro(const Record &record, std::ostream &output) {
    std::string name;
    if (const auto pos = record.name.rfind("::"); pos == std::string::npos)
      name = record.name;
    else
      name = record.name.substr(pos + 2);
    std::ranges::transform(name, name.begin(), toupper);
    output.write("#define SERIALIZE_", 18);
    output.write(name.c_str(), static_cast<long>(name.size()));
    output.write(" _e_SERIALIZE_RECORD \\\n", 23);

    GenerateReflectionFactoryCode(record, output);
    GenerateSaveFunction(record, output);
    GenerateLoadFunction(record, output);

    output.write("\\\n  RENDER_", 11);
    output.write(name.c_str(), static_cast<long>(name.size()));
    if (record.isClass)
      output.write("\\\n  private: \n", 14);

    GenerateEditorRecordFunction(record, output, name);
  }

  void Generator::GenerateReflectionFactoryCode(const Record &record, std::ostream &output) {
    output.write(
      "  static inline const bool _reg = [] {\\\n"
      "    Engine::Reflection::ReflectionFactory::RegisterType<", 96
    );
    output.write(record.name.c_str(), static_cast<long>(record.name.size()));
    output.write(">(\"", 3);
    output.write(record.name.c_str(), static_cast<long>(record.name.size()));
    output.write(
      "\");\\\n"
      "    return true;\\\n"
      "  }();\\\n", 31
    );
  }

  void Generator::GenerateSaveFunction(const Record &record, std::ostream &output) {
    output.write(
      "  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \\\n"
      "    if (format == Engine::Reflection::Format::JSON) { \\\n"
      "      json = Engine::JSON::Object();\\\n", 189
    );

    for (const auto &field: record.fields) {
      output.write("      Engine::Reflection::_e_saveImpl(", 38);
      output.write(field.c_str(), static_cast<long>(field.size()));
      output.write(", format, json[\"", 16);
      output.write(field.c_str(), static_cast<long>(field.size()));
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
    for (const auto &field: record.fields) {
      output.write("      Engine::Reflection::_e_loadImpl(", 38);
      output.write(field.c_str(), static_cast<long>(field.size()));
      output.write(", format, json.At(\"", 19);
      output.write(field.c_str(), static_cast<long>(field.size()));
      output.write("\"));\\\n", 6);
    }
    output.write("    }\\\n  }", 10);
  }

  void Generator::GenerateEditorRecordFunction(const Record &record, std::ostream &output, const std::string &name) {
    output.write(
      "\n\n#if !ENGINE_EDITOR\n"
      "#define RENDER_", 36
    );
    output.write(name.c_str(), static_cast<long>(name.size()));
    output.write(
      "\n#else\n"
      "#define RENDER_", 22
    );
    output.write(name.c_str(), static_cast<long>(name.size()));
    output.write("\\\n  bool _e_renderInEditor(const bool readOnly) override {", 58);

    if (record.editorFields.empty()) {
      output.write("\\\n    return false;\\\n}\n#endif", 29);
      return;
    }

    output.write("\\\n    bool changed = false;\\\n", 29);
    for (const auto &field: record.editorFields) {
      if (field.empty())
        continue;

      output.write("    changed |= Engine::Reflection::_e_renderInEditorImpl(", 57);
      output.write(field.c_str(), static_cast<long>(field.size()));
      output.write(", \"", 3);
      for (size_t i = 0; i < field.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(field[i]);
        if (i == 0)
          output.put(static_cast<char>(std::toupper(c)));
        else {
          if (std::isupper(c))
            output.put(' ');
          output.put(static_cast<char>(c));
        }
      }

      output.write("\", readOnly);\\\n", 15);
    }

    output.write("    return changed;\\\n  }\n#endif", 31);
  }

  void Generator::GenerateEnumReflectionMacro(const Enum &enumerator, std::ostream &output, const bool lastEnum) {
    std::string name;
    if (const auto pos = enumerator.name.rfind("::"); pos == std::string::npos)
      name = enumerator.name;
    else
      name = enumerator.name.substr(pos + 2);
    std::ranges::transform(name, name.begin(), toupper);

    output.write("\n\n  #define REFLECT_", 20);
    output.write(name.c_str(), static_cast<long>(name.size()));
    output.write("\\\n  static inline const bool _reg_", 34);
    output.write(name.c_str(), static_cast<long>(name.size()));
    output.write(
      " = [] {\\\n"
      "    Engine::Reflection::ReflectionFactory::RegisterEnum<", 65
    );
    output.write(enumerator.name.c_str(), static_cast<long>(enumerator.name.size()));
    output.write(">(\\\n    \"", 9);
    output.write(enumerator.name.c_str(), static_cast<long>(enumerator.name.size()));
    output.write("\", {\\\n", 6);

    for (const auto &value: enumerator.values) {
      output.write("      std::pair<std::string, ", 29);
      output.write(enumerator.name.c_str(), static_cast<long>(enumerator.name.size()));
      output.write(">{\"", 3);
      output.write(value.c_str(), static_cast<long>(value.size()));
      output.write("\", ", 3);
      output.write(enumerator.name.c_str(), static_cast<long>(enumerator.name.size()));
      output.write("::", 2);
      output.write(value.c_str(), static_cast<long>(value.size()));
      output.write("},\\\n", 4);
    }

    output.write(
      "      }\\\n"
      "    );\\\n"
      "    return true;\\\n"
      "  }();", 41
    );

    if (!lastEnum)
      output.put('\\');
    output.put('\n');
  }

  bool Generator::WriteFileIfChanged(
    const std::string &filePath, const std::string &content, const bool overrideWrite
  ) {
    // Check if file exists and compare content
    if (!overrideWrite && std::filesystem::exists(filePath)) {
      std::ifstream existingFile(filePath);
      std::ostringstream existingContent;
      existingContent << existingFile.rdbuf();

      if (existingContent.str() == content)
        return false;
    }

    // Content is different or file doesn't exist, write it
    std::ofstream outputFile(filePath);
    outputFile << content;
    return true;
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
      std::ranges::transform(upperName, upperName.begin(), toupper);

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

  void Generator::InjectReflectMacro(const std::string &headerPath, const std::vector<Enum> &enums) {
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

    for (const auto &enumerator: enums) {
      std::string name = enumerator.name;
      if (const auto pos = enumerator.name.rfind("::"); pos != std::string::npos)
        name = enumerator.name.substr(pos + 2);
      std::string upperName = name;
      std::ranges::transform(upperName, upperName.begin(), toupper);

      const std::regex declaration("^\\s*(enum)\\s+" + name + "\\b");
      for (size_t i = 0; i < lines.size(); ++i) {
        if (std::regex_search(lines[i], declaration)) {
          // Find opening brace {
          size_t braceLine = i;
          while (braceLine < lines.size() && lines[braceLine].find('}') == std::string::npos)
            ++braceLine;

          if (braceLine < lines.size()) {
            // Check if macro already exists
            if (size_t insertLine = braceLine + 1;
              insertLine < lines.size() && lines[insertLine].find("REFLECT_" + upperName) == std::string::npos) {
              size_t leadingWhitespace = 0;
              while (leadingWhitespace < lines[i].size() && (
                       lines[i][leadingWhitespace] == ' ' || lines[i][leadingWhitespace] == '\t'))
                ++leadingWhitespace;

              lines.insert(
                lines.begin() + static_cast<long>(insertLine),
                lines[i].substr(0, leadingWhitespace) + "REFLECT_" + upperName
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
