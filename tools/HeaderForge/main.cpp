#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>

#include "include/Generator.hpp"
#include "include/Parser.hpp"

#define ALL_IN_ONE_CPP "allInOne.cpp"
#define ALL_IN_ONE_I "allInOne.i"
#define PREPROCESS_CMD "clang++ -E -DHEADER_FORGE_ENABLE_ANNOTATIONS " ALL_IN_ONE_CPP " -o " ALL_IN_ONE_I
#define LAST_PROCESSING_TIMES_TXT "./Engine/lastProcessingTimes.txt"

namespace fs = std::filesystem;

int main(const int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Error: at least one file or directory must be specified.\n";
    std::cerr << "Usage: " << argv[0] << " <file|directory>...\n";
    return -1;
  }

  if (!fs::exists("./Engine"))
    fs::create_directory("./Engine");

  bool overrideProcessingCheck = !fs::exists(LAST_PROCESSING_TIMES_TXT);
  std::fstream lastProcessingTimes(LAST_PROCESSING_TIMES_TXT, std::ios::in);
  std::unordered_map<std::string, std::string> fileProcessingTimes;

  if (!overrideProcessingCheck) {
    std::string line;
    while (std::getline(lastProcessingTimes, line)) {
      const size_t splitPoint = line.find_first_of(' ');
      fileProcessingTimes[line.substr(0, splitPoint)] = line.substr(splitPoint + 1);
    }
  }

  // Gather each file path recursively
  std::unordered_set<std::string> paths;
  for (size_t i = 1; i < argc; i++) {
    if (fs::path p(argv[i]); fs::exists(p)) {
      std::unordered_set<std::string> _paths;
      if (fs::is_regular_file(p) && (p.extension() == ".hpp" || p.extension() == ".cpp"))
        _paths.emplace(p.string());
      else if (fs::is_directory(p)) {
        for (const auto &entry: fs::recursive_directory_iterator(p)) {
          if (fs::is_regular_file(entry) && (entry.path().extension() == ".hpp" || entry.path().extension() == ".cpp"))
            _paths.insert(entry.path().string());
        }
      }
      paths.insert(_paths.begin(), _paths.end());
    } else {
      std::cerr << "Path does not exist: " << p << "\n";
    }
  }

  // Combine all files that need parsing in a single file
  bool hasData = false;
  std::ofstream allInOne(ALL_IN_ONE_CPP);
  for (const auto &path: paths) {
    const std::filesystem::path filePath(path);
    auto ftime = std::filesystem::last_write_time(filePath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
      ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now()
    );
    std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
    std::string lastWriteTime = std::asctime(std::localtime(&cftime));
    if (lastWriteTime.ends_with('\n'))
      lastWriteTime.pop_back();

    if (overrideProcessingCheck || lastWriteTime != fileProcessingTimes[path]) {
      allInOne << "#include \"" << path << "\"\n";
      fileProcessingTimes[path] = lastWriteTime;
      hasData = true;
    }
  }
  allInOne.close();

  lastProcessingTimes.close();
  lastProcessingTimes.open(LAST_PROCESSING_TIMES_TXT, std::ios::out | std::ios::trunc);

  bool first = true;
  for (const auto &[file, time]: fileProcessingTimes) {
    if (!first)
      lastProcessingTimes << "\n";
    lastProcessingTimes << file << " " << time;
    first = false;
  }

  lastProcessingTimes.flush();
  lastProcessingTimes.close();

  if (!hasData) {
    std::remove(ALL_IN_ONE_CPP);
    return 1;
  }

  // Preprocess the file (faster than the clang tool doing it)
  std::system(PREPROCESS_CMD);

  // Parse the preprocessed file
  std::vector<Engine::Reflection::Record> records;
  Engine::Reflection::Parser::ParseHeader(ALL_IN_ONE_I, records);

  // Write all the data and remove the generated files
  Engine::Reflection::Generator::GenerateRecordFiles(records);
  std::remove(ALL_IN_ONE_CPP);
  std::remove(ALL_IN_ONE_I);
}
