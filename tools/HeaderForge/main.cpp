#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>

#include "include/Injector.hpp"
#include "include/Parser.hpp"
#include "include/Serializer.hpp"

namespace fs = std::filesystem;

static inline std::set<std::string> traverse(const fs::path &p) {
  std::set<std::string> usages;
  if (fs::is_regular_file(p) && (p.extension() == ".hpp" || p.extension() == ".cpp" || p.extension() == ".i"))
    usages.insert(p.string());
  else if (fs::is_directory(p)) {
    for (const auto &entry: fs::recursive_directory_iterator(p)) {
      if (fs::is_regular_file(entry) && (entry.path().extension() == ".hpp" || entry.path().extension() == ".cpp" ||
                                         entry.path().extension() == ".i"))
        usages.insert(entry.path().string());
    }
  }
  return usages;
}

int main(const int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Error: at least one file or directory must be specified.\n";
    std::cerr << "Usage: " << argv[0] << " <file|directory>...\n";
    return 1;
  }

  std::set<std::string> paths;
  for (size_t i = 1; i < argc; i++) {
    if (fs::path p(argv[i]); fs::exists(p)) {
      const auto _paths = traverse(p);
      paths.insert(_paths.begin(), _paths.end());
    } else {
      std::cerr << "Path does not exist: " << p << "\n";
    }
  }

  // Combine all files that need parsing in a single file
  std::ofstream allInOne("all_in_one.cpp");
  for (const auto &path: paths)
    allInOne << "#include \"" << path << "\"\n";
  allInOne.close();

  // Preprocess the file (faster than the clang tool doing it)
  std::system("clang++ -E all_in_one.cpp -o all_in_one.i");

  // Parse the preprocessed file
  std::vector<Engine::Serialization::Record> records;
  std::set<std::string> usages;
  Engine::Serialization::Parser::ParseHeader("all_in_one.i", records, usages);

  // Write all the data and remove the generated files
  Engine::Serialization::Injector::InjectRecordsForSerialization(records, usages);
  std::remove("all_in_one.cpp");
  std::remove("all_in_one.i");

  /// Testing

  Derived3<int, int> derived3;
  const std::shared_ptr<Base> test = std::make_shared<Derived3<float, double>>();
  test->base = 4;

  std::cout << Engine::Serialization::Serializer::ToJson(test) << "\n";
  std::cout << Engine::Serialization::Serializer::ToJson(std::make_shared<std::vector<int>>(std::initializer_list{1, 2, 3})) << "\n";
}
