//
// LinkProject.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include "commands/LinkProject.hpp"

LinkProject::LinkProject()
    : Command("Links an existing project to the engine", "{path}", 1, "~/Projects/Test") {}

static std::string trim(const std::string& s) {
    const size_t start = s.find_first_not_of(" \t");
    const size_t end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

static std::string toForwardSlashes(std::string path) {
    std::ranges::replace(path, '\\', '/');
    return path;
}

static std::string getRelativePath(const fs::path& from, const fs::path& to) {
    fs::path relPath;
    auto fromIter = from.begin();
    auto toIter = to.begin();

    while (fromIter != from.end() && toIter != to.end() && *fromIter == *toIter) {
        ++fromIter;
        ++toIter;
    }

    for (; fromIter != from.end(); ++fromIter)
        relPath /= "..";

    for (; toIter != to.end(); ++toIter)
        relPath /= *toIter;

    return relPath.string();
}

static void updateCMakeFile(const fs::path& projectDir, const std::string& location) {
    fs::path cmakeFile = projectDir / "CMakeLists.txt";

    if (!fs::exists(cmakeFile)) {
        std::cerr << RED << "ERROR: CMakeLists.txt not found in " << projectDir << RESET;
        return;
    }

    std::ifstream in(cmakeFile);
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    std::regex pattern(R"(set\(ENGINE_LOCATION\s+.*\))");
    std::string replacement = "set(ENGINE_LOCATION " + location + ")";
    content = std::regex_replace(content, pattern, replacement);

    std::ofstream out(cmakeFile);
    out << content;
    out.close();

    std::cout << GREEN << "Linked project successfully." << RESET;
}

void LinkProject::Run(
    const std::vector<std::string>& args,
    const OrderedMap<std::string, std::unique_ptr<Command>>& commands
) {
    std::string inputPath = trim(args[0]);
    inputPath = toForwardSlashes(inputPath);

    fs::path projectPath(inputPath);
    if (inputPath.starts_with("~"))
        if (const char* home = std::getenv("HOME"))
            projectPath = fs::path(home) / inputPath.substr(2);

    if (!fs::exists(projectPath) || !fs::is_directory(projectPath)) {
        std::cerr << RED << "ERROR: Either '" << inputPath << "' does not exist or is not a directory." << RESET;
        return;
    }

    const fs::path engineDir = fs::canonical(fs::current_path());
    const fs::path targetDir = fs::canonical(projectPath);
    const std::string relativePath = getRelativePath(targetDir, engineDir);

    updateCMakeFile(targetDir, relativePath);

    fs::copy_file("./engine-cli", targetDir / "engine-cli", fs::copy_options::overwrite_existing);
}
