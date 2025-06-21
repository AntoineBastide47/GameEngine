//
// SerializerInjector.cpp
// Author: Antoine Bastide
// Date: 18.06.2025
//

#include <fstream>
#include <set>

#include "Injector.hpp"
#include "Parser.hpp"

#define _e_SERIALIZER_FUNCTION_INJECT_START
#define _e_SERIALIZER_FUNCTION_INJECT_END
#define _e_SERIALIZER_HEADER_INJECT_START
#define _e_SERIALIZER_HEADER_INJECT_END

#define _e_DESERIALIZER_FUNCTION_INJECT_START
#define _e_DESERIALIZER_FUNCTION_INJECT_END
#define _e_DESERIALIZER_HEADER_INJECT_START
#define _e_DESERIALIZER_HEADER_INJECT_END

#define _e_SERIALIZER_POINTER_INJECT_START
#define _e_SERIALIZER_POINTER_INJECT_END

#define STR(x) #x

namespace Engine::Serialization {
  void Injector::InjectRecordsForSerialization(const std::vector<Record> &records, std::set<std::string> &usages) {
    std::ifstream input("./include/Serializer.hpp");
    std::ofstream output("./include/temp.hpp");

    bool inHeaderInject = false;
    bool inPointerInject = false;
    bool inFunctionInject = false;
    std::string line;

    std::filesystem::path inputPath = std::filesystem::canonical("./include/Serializer.hpp").parent_path();
    std::set<std::string> headers;
    for (const auto &record: records) {
      std::filesystem::path absoluteRecordPath = std::filesystem::canonical(record.filePath);
      std::filesystem::path relativePath = std::filesystem::relative(absoluteRecordPath, inputPath);
      headers.insert(relativePath.generic_string());
    }

    while (std::getline(input, line)) {
      if (line.find(STR(_e_SERIALIZER_HEADER_INJECT_START)) != std::string::npos) {
        inHeaderInject = true;
        output << line << '\n';
        for (const auto &header: headers)
          output << R"(#include ")" << header << "\"\n";
        continue;
      }

      if (line.find(STR(_e_SERIALIZER_HEADER_INJECT_END)) != std::string::npos) {
        inHeaderInject = false;
        output << line << '\n';
        continue;
      }

      if (line.find(STR(_e_SERIALIZER_POINTER_INJECT_START)) != std::string::npos) {
        inPointerInject = true;
        output << line << '\n';
        for (const auto &record: records) {
          if (record.templateParameters.empty()) {
            output << "          case " << record.name
                << "::StaticTypeId():\n            save(*std::dynamic_pointer_cast<" << record.name
                << ">(data), format, os, prettyPrint, indent);\n            break;\n";
          } else {
            for (auto it = usages.begin(); it != usages.end();) {
              if (const auto &usage = *it; usage.find(record.name) != std::string::npos) {
                output << "          case " << usage
                    << "::StaticTypeId():\n            save(*std::dynamic_pointer_cast<" << usage
                    << ">(data), format, os, prettyPrint, indent);\n            break;\n";

                it = usages.erase(it);
              } else
                ++it;
            }
          }
        }
        continue;
      }

      if (line.find(STR(_e_SERIALIZER_POINTER_INJECT_END)) != std::string::npos) {
        inPointerInject = false;
        output << line << '\n';
        continue;
      }

      if (line.find(STR(_e_SERIALIZER_FUNCTION_INJECT_START)) != std::string::npos) {
        inFunctionInject = true;
        output << line << '\n';
        for (const auto &record: records)
          SerializeRecord(record, output);
        continue;
      }

      if (line.find(STR(_e_SERIALIZER_FUNCTION_INJECT_END)) != std::string::npos) {
        inFunctionInject = false;
        output << line << '\n';
        continue;
      }

      if (!inHeaderInject && !inPointerInject && !inFunctionInject)
        output << line << '\n';
    }

    input.close();
    output.close();

    std::remove("./include/Serializer.hpp");
    std::rename("./include/temp.hpp", "./include/Serializer.hpp");
  }

  void Injector::SerializeRecord(const Record &record, std::ofstream &output) {
    constexpr auto indent = "      ";
    constexpr auto innerIndent = "          ";

    output << indent;
    if (!record.templateParameters.empty()) {
      output << "template<";
      bool first = true;
      for (const auto &templateParameter: record.templateParameters) {
        if (!first) {
          output.put(',');
          output.put(' ');
        }
        output << "typename " << templateParameter;
        first = false;
      }
      output.put('>');
      output.put(' ');
    }

    output << "static void save(const " << record.name;

    if (!record.templateParameters.empty()) {
      output.put('<');
      bool first = true;
      for (const auto &templateParameter: record.templateParameters) {
        if (!first) {
          output.put(',');
          output.put(' ');
        }
        output << templateParameter;
        first = false;
      }
      output.put('>');
    }

    output << " &data, const Format format, std::ostringstream &os, const bool prettyPrint, const int indent) {\n"
        << indent << "  if (format == JSON) {\n"
        << innerIndent << "os.put('{');\n";

    bool first = true;
    for (const auto &[_, name]: record.fields) {
      if (!first)
        output << innerIndent << "os.put(',');\n";
      output << innerIndent << "if (prettyPrint)\n" << innerIndent << "  os.put('\\n');\n"
          << innerIndent << "applyIndent(os, prettyPrint, indent + 1);\n"
          << innerIndent << R"(os << "\")" << name << R"(\":";)" << "\n"
          << innerIndent << "if (prettyPrint)\n" << innerIndent << "  os.put(' ');\n"
          << innerIndent << "save(data." << name << ", format, os, prettyPrint, 0);\n";
      first = false;
    }

    output << innerIndent << "if (prettyPrint)\n" << innerIndent << "  os.put('\\n');\n"
        << innerIndent << "applyIndent(os, prettyPrint, indent);\n"
        << innerIndent << "os.put('}');\n"
        << indent << "  }\n"
        << indent << "}\n";
  }
}
