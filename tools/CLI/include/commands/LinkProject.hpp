//
// LinkProject.hpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#ifndef LINK_PROJECT_HPP
#define LINK_PROJECT_HPP

#include "Command.hpp"

class LinkProject final : public Command {
  public:
    LinkProject();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //LINK_PROJECT_HPP
