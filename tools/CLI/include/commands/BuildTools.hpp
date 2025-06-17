//
// BuildTools.hpp
// Author: Antoine Bastide
// Date: 17.06.2025
//

#ifndef BUILDTOOLS_HPP
#define BUILDTOOLS_HPP

#include "Command.hpp"

class BuildTools final : public Command {
  public:
    BuildTools();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //BUILDTOOLS_HPP
