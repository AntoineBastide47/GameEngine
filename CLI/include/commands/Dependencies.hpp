//
// Dependencies.hpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#ifndef DEPENDENCIES_HPP
#define DEPENDENCIES_HPP

#include "Command.hpp"

class Dependencies final : public Command {
  public:
    Dependencies();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //DEPENDENCIES_HPP
