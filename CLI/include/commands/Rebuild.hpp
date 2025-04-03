//
// Rebuild.hpp
// Author: Antoine Bastide
// Date: 03/04/2025
//

#ifndef REBUILD_HPP
#define REBUILD_HPP

#include "Command.hpp"

class Rebuild final : public Command {
  public:
    Rebuild();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};


#endif //REBUILD_HPP
