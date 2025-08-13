#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <set>

#include "Generator.hpp"
#include "Parser.hpp"
#include "../../Engine/include/Engine/Macros/PlatformDetection.hpp"

#define ALL_IN_ONE_CPP "allInOne.cpp"
#define ALL_IN_ONE_II "allInOne.ii"
#define LAST_PROCESSING_TIMES_TXT "./Engine/lastProcessingTimes.txt"

#ifdef ENGINE_MACOS
#define OS_INCLUDES "-I/opt/homebrew/include"
#endif

#define PREPROCESS_CMD_START "clang++ -std=c++20 -E -DHEADER_FORGE_ENABLE_ANNOTATIONS "
#define PREPROCESS_CMD_END " " OS_INCLUDES " " ALL_IN_ONE_CPP " -o " ALL_IN_ONE_II

namespace fs = std::filesystem;

static std::string join(const std::vector<std::string> &args) {
  std::string result;
  bool first = true;
  for (const auto &arg: args) {
    if (!first)
      result.push_back(' ');
    result += arg;
    first = false;
  }
  return result;
}

static std::string normalize_path(const std::string &in) {
  try {
    return fs::weakly_canonical(fs::path(in)).lexically_normal().string();
  } catch (...) {
    return fs::path(in).lexically_normal().string();
  }
}

int main(const int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Error: at least one file or directory must be specified.\n";
    std::cerr << "Usage: " << argv[0] << " --parse <file|directory>... --compilerArgs <args>...\n";
    return -1;
  }

  if (!fs::exists("./Engine"))
    fs::create_directory("./Engine");

  std::unordered_map<std::string, std::string> fileProcessingTimes;
  if (fs::exists(LAST_PROCESSING_TIMES_TXT)) {
    std::fstream lastProcessingTimes(LAST_PROCESSING_TIMES_TXT, std::ios::in);
    std::string line;
    while (std::getline(lastProcessingTimes, line)) {
      const size_t splitPoint = line.find_first_of(' ');
      if (splitPoint == std::string::npos)
        continue;

      const std::string pathPart = line.substr(0, splitPoint);
      const std::string timePart = line.substr(splitPoint + 1);
      fileProcessingTimes[normalize_path(pathPart)] = timePart;
    }
    lastProcessingTimes.close();
  }

  // Gather each file path recursively
  std::set<std::string> filesystemPaths;
  std::vector<std::string> compilerArgs;
  bool parsingPaths = false;

  for (size_t i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "--parse") {
      parsingPaths = true;
      continue;
    }
    if (arg == "--compilerArgs") {
      parsingPaths = false;
      continue;
    }

    if (parsingPaths) {
      if (fs::path p(arg); fs::exists(p)) {
        if (fs::is_regular_file(p) && p.string().ends_with(".hpp") && !p.string().ends_with(".gen.hpp"))
          filesystemPaths.insert(normalize_path(p.string()));
        else if (fs::is_directory(p))
          for (const auto &entry: fs::recursive_directory_iterator(p))
            if (fs::is_regular_file(entry) && entry.path().string().ends_with(".hpp") &&
                !entry.path().string().ends_with(".gen.hpp"))
              filesystemPaths.insert(normalize_path(entry.path().string()));
      } else
        std::cerr << "Path does not exist: " << p << "\n";
    } else
      compilerArgs.push_back(arg);
  }

  // Check which files need processing
  std::unordered_map<std::string, std::string> filesToProcess;
  std::unordered_map<std::string, std::string> newTimestamps;

  for (const auto &path: filesystemPaths) {
    const std::filesystem::path filePath(path);
    auto ftime = std::filesystem::last_write_time(filePath);
    auto duration = ftime.time_since_epoch();
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    std::string currentTimeStr = std::to_string(nanos);

    newTimestamps[path] = currentTimeStr;

    bool needsProcessing = false;
    if (const auto it = fileProcessingTimes.find(path); it == fileProcessingTimes.end() || it->second != currentTimeStr)
      needsProcessing = true;

    if (needsProcessing)
      filesToProcess[path] = currentTimeStr;
  }

  // If no files need processing, exit early
  if (filesToProcess.empty()) {
    return 0;
  }

  // Create the all-in-one file with only files that need processing
  std::ofstream allInOne(ALL_IN_ONE_CPP);
  for (const auto &path: filesToProcess | std::views::keys) {
    allInOne << "#include \"" << path << "\"\n";
  }
  allInOne.close();

  // Preprocess the file (faster than the clang tool doing it)
  if (const int preprocessResult = std::system(
    (PREPROCESS_CMD_START + join(compilerArgs) + PREPROCESS_CMD_END).c_str()
  ); preprocessResult != 0) {
    std::cerr << "Preprocessing failed\n";
    std::remove(ALL_IN_ONE_CPP);
    return -1;
  }

  // Parse the preprocessed file
  std::vector<Engine::Reflection::Record> records;
  try {
    Engine::Reflection::Parser::ParseHeader(ALL_IN_ONE_II, records);
  } catch (const std::exception &e) {
    std::cerr << "Parsing failed: " << e.what() << "\n";
    std::remove(ALL_IN_ONE_CPP);
    std::remove(ALL_IN_ONE_II);
    return -1;
  }

  // Write all the data
  try {
    Engine::Reflection::Generator::GenerateRecordFiles(records);
  } catch (const std::exception &e) {
    std::cerr << "Generation failed: " << e.what() << "\n";
    std::remove(ALL_IN_ONE_CPP);
    std::remove(ALL_IN_ONE_II);
    return -1;
  }

  // Only update timestamps after successful processing
  for (const auto &[path, newTimestamp]: filesToProcess) {
    fileProcessingTimes[normalize_path(path)] = newTimestamp;
  }

  // Write updated timestamps to file
  std::ofstream lastProcessingTimes(LAST_PROCESSING_TIMES_TXT, std::ios::out | std::ios::trunc);
  bool first = true;
  for (const auto &[file, time]: fileProcessingTimes) {
    if (!first)
      lastProcessingTimes << "\n";
    lastProcessingTimes << file << " " << time;
    first = false;
  }
  lastProcessingTimes.flush();
  lastProcessingTimes.close();

  std::remove(ALL_IN_ONE_CPP);
  std::remove(ALL_IN_ONE_II);
}
