#include <filesystem>
#include <iostream>

#include "include/Parser.hpp"

namespace fs = std::filesystem;

std::vector<Engine::Serialization::Record> process(const fs::path &p) {
  if (p.extension() != ".hpp")
    return {};
  std::cout << "Processing: " << p << std::endl;
  return Engine::Serialization::Parser::ParseHeader(p.string());
}

std::vector<Engine::Serialization::Record> traverse(const fs::path &p) {
  std::vector<Engine::Serialization::Record> records;
  if (fs::is_regular_file(p)) {
    const auto res = process(p);
    records.insert(records.end(), res.begin(), res.end());
  } else if (fs::is_directory(p)) {
    for (const auto &entry: fs::recursive_directory_iterator(p)) {
      if (fs::is_regular_file(entry)) {
        const auto res = process(entry.path());
        records.insert(records.end(), res.begin(), res.end());
      }
    }
  }
  return records;
}

int main(const int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " {file|directory}..." << std::endl;
    return EXIT_FAILURE;
  }

  for (int i = 1; i < argc; ++i) {
    if (fs::path p(argv[i]); fs::exists(p)) {
      const auto records = traverse(p);
      Engine::Serialization::Parser::PrintRecords(records);
    } else {
      std::cerr << "Path does not exist: " << p << std::endl;
    }
  }

  return 0;
}
