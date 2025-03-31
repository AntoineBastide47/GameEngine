//
// CreateProject.cpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#include "commands/CreateProject.hpp"

CreateProject::CreateProject() : Command("Walks the user through the creation of a new project") {}

static std::string trim(const std::string &s) {
  const size_t start = s.find_first_not_of(" \t");
  const size_t end = s.find_last_not_of(" \t");
  return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

static std::string getRelativePath(const fs::path &from, const fs::path &to) {
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

static void replaceInFile(const fs::path &file, const std::string &projectName, const std::string &engineLocation) {
  std::ifstream in(file);
  if (!in)
    return;

  std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  in.close();

  size_t pos;
  while ((pos = content.find("{{PROJECT_NAME}}")) != std::string::npos)
    content.replace(pos, std::string("{{PROJECT_NAME}}").length(), projectName);

  while ((pos = content.find("{{ENGINE_LOCATION}}")) != std::string::npos)
    content.replace(pos, std::string("{{ENGINE_LOCATION}}").length(), engineLocation);

  std::ofstream out(file);
  out << content;
}

void CreateProject::Run(
  const std::vector<std::string> &args,
  const OrderedMap<std::string, std::unique_ptr<Command>> &commands
) {
  std::string type;
  while (true) {
    std::string availableTemplates;
    for (const auto &entry: fs::directory_iterator("./Templates"))
      if (fs::is_directory(entry)) {
        if (!availableTemplates.empty())
          availableTemplates += ", ";
        availableTemplates += entry.path().filename().string();
      }
    std::cout << "Project Type (" << availableTemplates << "): ";

    std::getline(std::cin, type);
    type = trim(type);
    if (fs::is_directory("./Templates/" + type))
      break;
    std::cout << RED << "ERROR: Template for '" << type << "' does not exist.\nCurrently supported templates: "
      << availableTemplates << RESET << "\n";
  }

  std::string location;
  while (true) {
    std::cout << "Project Location: ";
    std::getline(std::cin, location);
    location = trim(location);
    std::ranges::replace(location, '\\', '/');
    if (location.empty())
      std::cout << RED << "ERROR: Location cannot be empty\n" << RESET;
    else if (fs::exists(location) && fs::is_directory(location))
      break;
    else
      std::cout << RED << "ERROR: Either '" << location << "' does not exist or it is not a folder\n" << RESET;
  }

  std::string name;
  while (true) {
    std::cout << "Project Name: ";
    std::getline(std::cin, name);
    std::erase(name, ' ');
    if (name.empty())
      std::cout << "ERROR: Name cannot be empty\n";
    else if (fs::exists(location + "/" + name))
      std::cout << RED << "ERROR: A directory with the name '" << name << "' already exists at the project location\n"
        << RESET;
    else
      break;
  }

  fs::path projectPath = fs::path(location) / name;
  fs::create_directories(projectPath);
  fs::copy("./Templates/" + type, projectPath, fs::copy_options::recursive);
  fs::copy("./include/Shaders", projectPath / "Engine/Shaders", fs::copy_options::recursive);

  fs::path from = fs::canonical(projectPath);
  fs::path to = fs::canonical(fs::current_path());
  std::string relPath = getRelativePath(from, to);

  for (const auto &entry: fs::recursive_directory_iterator(projectPath))
    if (fs::is_regular_file(entry.path()))
      replaceInFile(entry.path(), name, relPath);

  fs::rename(projectPath / "Game/src/{{PROJECT_NAME}}.cpp", projectPath / ("Game/src/" + name + ".cpp"));
  fs::rename(projectPath / "Game/include/{{PROJECT_NAME}}.hpp", projectPath / ("Game/include/" + name + ".hpp"));

  fs::copy_file("./engine-cli", projectPath / "engine-cli", fs::copy_options::overwrite_existing);

  std::cout << GREEN << "Project '" << name << "' of type '" << type << "' created at '" << projectPath << "'" << RESET;
}
