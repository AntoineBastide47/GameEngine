//
// RunProject.hpp
// Author: Antoine Bastide
// Date: 31/03/2025
//

#ifndef RUN_PROJECT_HPP
#define RUN_PROJECT_HPP
#include "Command.hpp"

class RunProject final : public Command {
  public:
    RunProject();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //RUN_PROJECT_HPP
