//
// Version.hpp
// Author: Antoine Bastide
// Date: 28/03/2025
//

#ifndef VERSION_HPP
#define VERSION_HPP

#include "Command.hpp"

class Version final : public Command {
  public:
    Version();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //VERSION_HPP
