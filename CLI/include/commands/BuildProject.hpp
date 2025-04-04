//
// BuildProject.hpp
// Author: Antoine Bastide
// Date: 31/03/2025
//

#ifndef BUILD_PROJECT_HPP
#define BUILD_PROJECT_HPP

#include "Command.hpp"

class BuildProject final : public Command {
  public:
    BuildProject();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //BUILD_PROJECT_HPP
