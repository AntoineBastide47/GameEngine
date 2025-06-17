//
// CreateProject.hpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#ifndef CREATE_PROJECT_HPP
#define CREATE_PROJECT_HPP

#include "Command.hpp"

class CreateProject final : public Command {
  public:
    CreateProject();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //CREATE_PROJECT_HPP
